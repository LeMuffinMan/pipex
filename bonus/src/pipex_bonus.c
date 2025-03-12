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

//securiser et free toute la liste !
int add_first_node(t_data **data, char *cmd, char **env, char *infile)
{
	t_data *node;

	node = malloc(sizeof(t_data));
	if (node == NULL)
	{
		//free !
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
		//free_data(data); a ajouter PARTOUT !
		//exit (1);
	}
	tmp = *data;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = node;
	if (!*last_arg) // syntax ?
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
int error_cmd_not_found(t_data *data, char **args)
{
	ft_putstr_fd("pipex: command not found: ", 2);
	ft_putstr_fd(data->cmd, 2); // a verifier ! donne la bonne cmd en avancant le ptr ?
	if (args)
		free_array(args);
	if (close(data->fd_in) == -1)
		exit(127);
	if (close(data->fd_out) == -1)
		exit(127);
	exit(127);
}

int dup_input_output(int fd_out, int fd_in)
{
	if (dup2(fd_out, STDOUT_FILENO) == -1)
		exit(errno);
	if (dup2(fd_in, STDIN_FILENO) == -1)
		exit(errno);
	return (0);
}

//free toute la liste
int redirect_stdin_stdout(t_data *data, char *path, char **args, char **av)
{
	int file;
	
	(void)path;
	(void)args; // a free
	if (data->file == av[1])
	{
		/* close(fd[0]); */
		//a proteger
		file = open(av[1], O_RDONLY);
		if (file == -1)
			/* open_error(); */
		dup_input_output(data->fd_out, file);
	}
	else if (data->file)
	{
		/* close(fd[1]); // pas sur ? */
		// a proteger
		file = open(data->file, O_WRONLY | O_CREAT | O_TRUNC, 0644); // APPEND ?
		if (file == -1)
			/* open_error(); */
		dup_input_output(data->fd_in, file);
	}
	else 
	{
		/* close(fd[1]); */
		//a proteger
		/* close(fd[0]); */
		//a proteger
		dup_input_output(data->fd_out, data->fd_in);
	}
	return (0);
}

//gerer si on me donne PATH et pas d'env
//free toute la liste !
//il faut lui filer data pour qu'il puisse la free !
int	execute(char *binary, char **args, char **envp)
{
	if (!binary || access(binary, F_OK) != 0)
		/* error_cmd_not_found(data, NULL); */
	if (access(binary, X_OK) != 0)
		error_permission_denied(args, binary);
	if (execve(binary, args, envp) != 0)
	{
		free_array(args);
		free(binary);
		perror("execve error");
		exit(errno);
	}
	return (0);
}

//voir tous les tests chiants et securiser 
//il faut free toute la liste !!
int parse_redirect_execute(t_data *data, char **av)
{
	char **args;
	char *path;

	//cas 1 : ls
	//cas 2 : ls -l
	//cas 3 : /usr/bin/ls
	//cas 4 : /usr/bin/ls -l
	//cas 5 : no env et /usr/bin/ls
	//cas 5 : no PATH et /usr/bin/ls
	//cas 5 : PATH empty et /usr/bin/ls
	path = NULL;
	args = ft_split(data->cmd, ' ');
	if (!*args)
		error_cmd_not_found(data, NULL);
	redirect_stdin_stdout(data, path, args, av); //en cas d'erreur args a free !
	if (is_a_path(args[0]))
		path = args[0];
	else
	{
		path = get_binary(args[0], data->env);
		if (!path)
			error_cmd_not_found(data, NULL);
	}
	execute(path, args, data->env);
	return(1);
}

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

//revoir la doc !
int wait_children(t_data *data)
{
	int status;
	int exit_code;

	exit_code = EXIT_SUCCESS;
	while (data->next)
	{
		if (waitpid(data->pid, &status, 0) == -1)
			close_pipeline_free_exit(&data);
		if (WIFEXITED(status))
			exit_code = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			exit_code = 128 + WTERMSIG(status);
		if (exit_code == EXIT_SUCCESS && WIFEXITED(status))
			exit_code = WEXITSTATUS(status);
		else if (exit_code == EXIT_SUCCESS && WIFSIGNALED(status))
			exit_code = 128 + WTERMSIG(status);
		data = data->next;
	}
	return (exit_code);
}


int main (int ac, char **av, char **env)
{
	t_data *data;
	
	if (ac >= 5)
	{
		init_data(&data, av, env);	
		setup_pipeline(&data); // verifier ac - 1
		data->pid = fork();
		if (data->pid == -1)
			close_pipeline_free_exit(&data);
		if (data->pid == 0)
			parse_redirect_execute(data, av); // faire une copie du noeud et free la liste dans le child ?
		data = data->next;
		while (data->next)
		{
			data->pid = fork();
			if (data->pid == -1)
				close_pipeline_free_exit(&data);
			if (data->pid == 0)
				parse_redirect_execute(data, av);
			data = data->next;
		}
		data->pid = fork();
		if (data->pid == -1)
			close_pipeline_free_exit(&data);
		if (data->pid == 0)
			parse_redirect_execute(data, av);
		exit(wait_children(data)); 
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

