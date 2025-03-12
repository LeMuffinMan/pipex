/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 16:29:17 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/08 15:28:27 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

// mettre un max fd dans l'include ?
// dup2 et close a proteger ?

#include "libft.h"
#include "pipex_bonus.h"
#include <errno.h>    // errno
#include <fcntl.h>    // open
#include <stdio.h>    // perror
#include <stdlib.h>   // exit
#include <string.h>   // strerror
#include <sys/wait.h> // wait, waitpid
#include <unistd.h>   // fork, pipe, dup2, execve, access, close, read, write

/// access(const char *pathname, int mode)
///- F_OK : file exists
///- R_OK : readable
///- W_OK : writable
///- X_OK : executable
///
/// errno : variable globale qui stock le dernier code d'erreur de l'appel
/// systeme echoue
/// strerror a la place ?
///
/// REVOIR LIBFT COmpile et virer le header en trop

int free_data(t_data **data)
{
	t_data *tmp;
	t_data *next_node;

	tmp = *data;
	if (!*data)
		return (1);
	while(tmp->next)
	{
		next_node = tmp->next;
		free(tmp);
		tmp = next_node;
	}
	*data = NULL;
	return (0);
}

//securiser et free toute la liste !
int add_first_node(t_data **data, char *cmd, char **env, char *infile)
{
	t_data *node;

	node = malloc(sizeof(t_data));
	if (node == NULL)
	{
		free_data(data);
		perror("malloc");
		exit(1); //choisir 1 ou exit_failure
	}
	*data = node;
	node->file = infile;
	node->cmd = cmd;
	node->env = env;
	node->next = NULL;
	return (0);
}

//securiser et free toute la liste !
int add_node(t_data **data, char *cmd, char **env, char *last_arg)
{
	t_data *node;
	t_data *tmp;

	node = malloc(sizeof(t_data));
	if (node == NULL)
	{
		free_data(data);
		perror("malloc");
		exit(1); //choisir 1 ou exit_failure
	}
	tmp = *data;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = node;
	if (*last_arg) // syntax ?
		node->file = last_arg; //bon compte ?
	else
		node->file = NULL;
	node->cmd = cmd;
	node->env = env;
	node->next = NULL;
	return (0);
}

int init_data(t_data **data, char **av, char **env)
{
	int i;

	add_first_node(data, av[2], env, av[1]);
	i = 3;
	while (av[i + 1])
	{
		add_node(data, av[i], env, av[i + 1]);
		i++;
	}
	return (0);
}

/* int print_lst(t_data *data) */
/* { */
/* 	t_data *tmp; */
/**/
/* 	tmp = data; */
/* 	while (tmp->next) */
/* 	{ */
/* 		printf("%s\n", tmp->cmd); */
/* 		tmp = tmp->next; */
/* 	} */
/* 	printf("%s\n", tmp->cmd); */
/* 	return (0); */
/* } */


int close_pipeline_free_exit(t_data **data)
{
	t_data *tmp;

	tmp = *data;
	while (tmp->next)
	{
		if (tmp->fd_in != -1)
		{
			if (close(tmp->fd_in) == -1)
			{
				free_data(data);
				perror("close"); // a mettre partout 
				exit(1); 
			}
			if (close(tmp->fd_out) == -1)
			{
				free_data(data);
				exit(1);
			}
		}
		tmp = tmp->next;
	}
	free_data(data);
	return (0);
}

//securiser et free toute la liste !
int connect_nodes(t_data *node1, t_data *node2)
{
	int fd[2];
	
	if (pipe(fd) == -1)
	{
		perror("pipe");
		exit (EXIT_FAILURE);
	}
	node1->fd_out = fd[1];
	node2->fd_in = fd[0];
	/* close(fd[0]); */
	/* close(fd[1]); */
	/* printf("%s fd_out will write to %s fd_in\n", node1->cmd, node2->cmd); */
	return (0);
}

//securiser et free toute la liste !
int setup_pipeline(t_data **data)
{
	int pipe_begin[2];
	t_data *tmp;

	if (pipe(pipe_begin) == -1)
	{
		free_data(data);
		perror("pipe");
		exit (EXIT_FAILURE);
	}
	(*data)->fd_in = -1;
	(*data)->fd_out = pipe_begin[1];
	/* printf("%s fd_in is infile | fd_out is %s\n", (*data)->cmd, (*data)->next->cmd); */
	tmp = (*data)->next;
	tmp->fd_in = pipe_begin[0];
	/* printf("%s fd_in is %s\n", tmp->cmd, (*data)->cmd); */
	while (tmp->next)
	{
		connect_nodes(tmp, tmp->next);
		tmp = tmp->next;
	}
	/* printf("%s will write in outfile\n", tmp->cmd); */
	tmp->fd_out = -2;
	return (0);
}

//free toute la liste !!!
//code d'erreur ?
int error_cmd_not_found(t_data **data, t_data **tmp, t_strs *strs)
{
	ft_putstr_fd("pipex: command not found: ", 2);
	if ((*tmp)->cmd)
		ft_putstr_fd((*tmp)->cmd, 2); // a verifier ! donne la bonne cmd en avancant le ptr ?
	ft_putstr_fd("\n", 2); // PAS SUUUUUR !
	if (strs->args)
		free_array(strs->args);
	if (strs->path)
		free(strs->path);
	close_pipeline_free_exit(data);
	exit(127);
}

/* int dup_input_output(int fd_out, int fd_in) */
/* { */
/* 	if (dup2(fd_out, STDOUT_FILENO) == -1) */
/* 		exit(errno); */
/* 	if (dup2(fd_in, STDIN_FILENO) == -1) */
/* 		exit(errno); */
/* 	return (0); */
/* } */

int open_error(t_data **data, t_strs *strs)
{
	ft_putstr_fd("pipex: open error: ", 2);
	perror("");
	free_array(strs->args);
	free(strs->path);
	close_pipeline_free_exit(data);
	exit(1); //exit 1 ?
}

//free toute la liste
//gerer open error 
int redirect_stdin_stdout(t_data **tmp, t_data **data, t_strs *strs, char **av)
{
	int file;
	
	file = -1;
	if ((*tmp)->file == av[1])
	{
		file = open(av[1], O_RDONLY);
		if (file == -1)
			open_error(data, strs);
    if (dup2(file, STDIN_FILENO) == -1)
    {
    	close (file);
    	open_error(data, strs);
    }
    close(file);
    if (dup2((*tmp)->fd_out, STDOUT_FILENO) == -1)
    {
    	close((*tmp)->fd_out);
    	open_error(data, strs);
    }
    close((*tmp)->fd_out);
	}
	else if ((*tmp)->file)
	{
		file = open((*tmp)->file, O_WRONLY | O_CREAT | O_TRUNC, 0644); // APPEND ?
		if (file == -1)
			open_error(data, strs);
		if (dup2(file, STDOUT_FILENO) == -1)
		{
			close(file);
			open_error(data, strs);
		}
    close(file);
    if (dup2((*tmp)->fd_in, STDIN_FILENO) == -1)
    {
    	close((*tmp)->fd_in);
    	open_error(data, strs);
    }
    close((*tmp)->fd_in);
	}
	else 
	{
    if (dup2((*tmp)->fd_out, STDOUT_FILENO) == -1)
    {
    	close((*tmp)->fd_out);
    	open_error(data, strs);
    }
    close((*tmp)->fd_out);
    if (dup2((*tmp)->fd_in, STDIN_FILENO) == -1)
    {
    	close((*tmp)->fd_in);
    	open_error(data, strs);
    }
    close((*tmp)->fd_in);
	}
	return (0);
}

int error_permission_denied(t_data **data, t_strs *strs)
{
	ft_putstr_fd("pipex: permission denied: ", 2);
	perror("");
	close_pipeline_free_exit(data);
	free_array(strs->args);
	free(strs->path);
	return (0);
}

//gerer si on me donne PATH et pas d'env
//free toute la liste !
//il faut lui filer data pour qu'il puisse la free !
int	execute(t_strs *strs, t_data **data)
{
	/* printf("executing cmd : %s\n", (*data)->cmd); */
	if (!strs->path || access(strs->path, F_OK) != 0)
		error_cmd_not_found(data, NULL, NULL);
	if (access(strs->path, X_OK) != 0)
		error_permission_denied(data, strs);
	if (execve(strs->path, strs->args, (*data)->env) != 0)
	{
		free_array(strs->args);
		free(strs->path);
		close_pipeline_free_exit(data);
		perror("execve error");
		exit(errno);
	}
	return (0);
}

//voir tous les tests chiants et securiser 
//il faut free toute la liste !!
int parse_redirect_execute(t_data **data, t_data **tmp, char **av)
{
	t_strs strs;

	//cas 1 : ls
	//cas 2 : ls -l
	//cas 3 : /usr/bin/ls
	//cas 4 : /usr/bin/ls -l
	//cas 5 : no env et /usr/bin/ls
	//cas 5 : no PATH et /usr/bin/ls
	//cas 5 : PATH empty et /usr/bin/ls
	if ((*tmp)->file)
		printf("file = %s\n", (*tmp)->file);
	printf("cmd = %s\n", (*tmp)->cmd);
	strs.path = NULL;
	strs.args = ft_split((*tmp)->cmd, ' ');
	printf("args[0] = %s\n", strs.args[0]);
	if (!strs.args[0]) // empeche un segfautl pour une cmd "" ?
		error_cmd_not_found(data, tmp, &strs); //ajouter strs pour tout free
	redirect_stdin_stdout(tmp, data, &strs, av); //en cas d'erreur args a free !
	if (is_a_path(strs.args[0]))
		strs.path = strs.args[0];
	else if ((*data)->env) // voir les cas possibles ici
	{
		strs.path = get_binary(strs.args[0], (*tmp)->env);
		if (!strs.path) // avec ou sans * ? 
			error_cmd_not_found(data, tmp, &strs);
	}
	else
		error_cmd_not_found(data, tmp, &strs);
	execute(&strs, tmp);
	exit(0);
}

//revoir la doc !
int wait_children(t_data **data)
{
	int status;
	int exit_code;

	exit_code = EXIT_SUCCESS;
	while ((*data)->next)
	{
		if (waitpid((*data)->pid, &status, 0) == -1)
			close_pipeline_free_exit(data);
		if (WIFEXITED(status))
			exit_code = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			exit_code = 128 + WTERMSIG(status);
		if (exit_code == EXIT_SUCCESS && WIFEXITED(status))
			exit_code = WEXITSTATUS(status);
		else if (exit_code == EXIT_SUCCESS && WIFSIGNALED(status))
			exit_code = 128 + WTERMSIG(status);
		*data = (*data)->next;
	}
	return (exit_code);
}


//seg fault si cmd 3 est ""
//❯ ./pipex_bonus infile "ls -l" "wc -l" outfile
//n'ecrit rien dans outfile MAIS il le cree
int main (int ac, char **av, char **env)
{
	t_data *data;
	t_data *tmp;
	
	if (ac >= 5)
	{
		init_data(&data, av, env);	
		setup_pipeline(&data); // verifier ac - 1
		tmp = data;
		tmp->pid = fork();
		if (data->pid == -1)
			close_pipeline_free_exit(&data);
		if (data->pid == 0)
			parse_redirect_execute(&data, &tmp, av); // faire une copie du noeud et free la liste dans le child ?
		tmp = tmp->next;
		while (tmp->next)
		{
			tmp->pid = fork();
			if (tmp->pid == -1)
				close_pipeline_free_exit(&data);
			if (tmp->pid == 0)
				parse_redirect_execute(&data, &tmp, av);
			tmp = tmp->next;
		}
		tmp->pid = fork();
		if (tmp->pid == -1)
			close_pipeline_free_exit(&data);
		if (tmp->pid == 0)
			parse_redirect_execute(&data, &tmp, av);
		exit(wait_children(&data)); 
	}
}

// si on supprime que la ligne PATH ?
// env -i / unset PATH ?
//
// tout proteger e
// 	- verifier si on free tout en sortant

/// sleep 5 : verfier que tout fonctionne en mm temps ( sleep 5 | sleep 5 )
/// infile cat | cat | ls outfile
////bin/ls comme cmd
/// proteger is infile ou outfile est /dev/urandom
/// here_doc : limiter = EOF et pas EOFa
///
///

