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

// gerer les arguments des commandes
// gerer la stdout de la 1ere commande : pas dans le fd 0, utiliser dup2
// D'ABORD : on verifie si file1 exist et est accessible
// Puis, on le prend comme fd / stdin
// on execute la commande avec ce fd
// on redirige la stdout de cmd1 vers stdin de cmd2
// on verifie si file2 exists et est accessible, on le touch si il existe pas ?
// on execute cmd2 avec en stdin la stdout de cmd1,
/* et on la stdout de cmd2 est redirigee vers file2 */
// on close tous les fd
// on attend les children

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

/* int parse_redirect_execute(char *av, int fd[2], char **envp, pid_t pid) */
/* { */
/* 	char *path; */
/* 	char **args; */
/**/
/* 	path = NULL; */
/* 	args = ft_split(av, ' '); */
/* 	if (!*args) */
/* 		error_no_cmd(); */
/* 	if (is_a_path(args[0])) */
/* 		path = args[0]; */
/* 	else */
/* 	{ */
/* 		path = get_binary(args[0], envp); */
/* 		if (!path) */
/* 			error_no_cmd(); */
/* 	} */
/* 	execute(path, args, envp); */
/* 	return (0); */
/* } */
/**/

/* int setup_pipeline(t_data **data, int fd_in, int fd_out) */
/* { */
/* 	node->fd_in = fd_in; */
/* 	node->fd_out = fd_out; */
/* 	return (0); */
/* } */

int add_first_node(t_data **data, char *cmd, char **env)
{
	t_data *node;

	node = malloc(sizeof(t_data));
	*data = node;
	node->cmd = cmd;
	node->env = env;
	node->next = NULL;
	return (0);
}

int add_node(t_data **data, char *cmd, char **env)
{
	t_data *node;
	t_data *tmp;

	node = malloc(sizeof(t_data));
	if (node == NULL)
	{
		//free_data(data);
		//exit (1);
	}
	tmp = *data;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = node;
	node->cmd = cmd;
	node->env = env;
	node->next = NULL;
	return (0);
}

int init_data(t_data **data, char **av, char **env)
{
	int i;

	add_first_node(data, av[2], env);
	i = 3;
	while (av[i + 1])
	{
		add_node(data, av[i], env);
		i++;
	}
	return (0);
}

int print_lst(t_data *data)
{
	t_data *tmp;

	tmp = data;
	while (tmp->next)
	{
		printf("%s\n", tmp->cmd);
		tmp = tmp->next;
	}
	printf("%s\n", tmp->cmd);
	return (0);
}

/* int pipe_children(t_data **data, int fd_end[2]) */
/* { */
/* 	t_data *tmp; */
/* 	int fd[2]; */
/**/
/* 	// une condition pour setup le dernier pipe */
/* 	tmp = (*data)->next; */
/* 	if (pipe(fd) == -1) */
/* 	{ */
		/* error  */
/* 	} */
/* 	tmp->fd_in = fd[0]; */
/* 	 */
/**/
/**/
/* } */


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
	printf("%s fd_out will write to %s fd_in\n", node1->cmd, node2->cmd);
	return (0);
}

int setup_pipeline(t_data **data, int infile, int outfile)
{
	int pipe_begin[2];
	t_data *tmp;

	if (pipe(pipe_begin) == -1)
	{
		perror("pipe");
		exit (EXIT_FAILURE);
	}
	(*data)->fd_in = infile;
	(*data)->fd_out = pipe_begin[1];
	printf("%s fd_in is infile | fd_out is %s\n", (*data)->cmd, (*data)->next->cmd);
	tmp = (*data)->next;
	tmp->fd_in = pipe_begin[0];
	printf("%s fd_in is %s\n", tmp->cmd, (*data)->cmd);
	while (tmp->next)
	{
		connect_nodes(tmp, tmp->next);
		tmp = tmp->next;
	}
	printf("%s will write in outfile\n", tmp->cmd);
	tmp->fd_out = outfile;
	return (0);
}

int main (int ac, char **av, char **env)
{
	t_data *data;
	int infile;
	int outfile;
	
	if (ac >= 5)
	{
		init_data(&data, av, env);	
		infile = open(av[1], O_RDONLY);
		outfile = open(av[ac], O_WRONLY | O_CREAT | O_TRUNC, 0644);
		setup_pipeline(&data, infile, outfile);
		return (0);
		data->pid = fork();
		if (data->pid == -1)
		{
			//pipe error 
		}
		if (data->pid == 0)
		{
			//close ?
			/* parse_redirect_execute() */
		}
		else 
		{
			//le parent doit faire quelque chose ?
		}
		while (data->next)
		{
			data->pid = fork();
			if (data->pid == -1)
			{
				//pipe error 
			}
			if (data->pid == 0)
			{
				//close ?
				/* parse_redirect_execute() */
			}
			else 
			{
				//parent doit faire qqchose ?
			}
			data = data->next;
		}
		data->pid = fork();
		if (data->pid == -1)
		{
			//pipe error 
		}
		if (data->pid == 0)
		{
			//close ?
			/* parse_redirect_execute() */
		}
		else 
		{
			//le parent doit faire quelque chose ?
		}
		//wait les childs
		//exit 
	}
}

/* int	main(int ac, char **av, char **envp) */
/* { */
/* 	int		fd[2]; */
/* 	pid_t pid; */
/* 	int i; */
/**/
/* 	i = 2; */
/* 	while (av[i + 2]) */
/* 	{ */
/*   	if (pipe(fd) == -1) { */
/*       	perror("pipe"); */
/*       	exit(EXIT_FAILURE); */
/*   	} */
/*   	i++; */
/*   	pid = fork(); */
/*   	if (pid == -1) */
/*   		close_and_quit(fd, errno); //revoir */
/*   	if (pid == 0) */
/*   	{ */
/*   		if (i == 2) */
/*   		{ */
/*   			fd[0] = open(av[1], O_RDONLY); */
/*   			if (fd[0] == -1) */
/*   				open_error(fd[0], av[1]) */
/*   			//close ici ? */
/*   			if (dup2(fd[1], STDOUT_FILENO) == -1) */
/*   			 exit(errno); // a proteger mieux ? */
/*   		} */
/*   		else if (i == ac - 1) */
/*   		{ */
/*   			fd[1] = open(av[ac - 1],  O_WRONLY | O_CREAT | O_TRUNC, 0644); */
/*   			if (fd[1] == -1) */
/*   				open_error(fd[0], av[1]) */
/*   			if (dup2(fd[0], STDIN_FILENO) == -1) */
/*   			 exit (errno); */
/*   		} */
/*   		else */
/*   		{ */
/* 				if (dup2(fd[1], STDOUT_FILENO) == -1) */
/* 					exit(errno); */
/* 				if (dup2(fd[0], STDIN_FILENO) == -1) */
/* 					exit(errno); */
/*   		} */
/*   		parse_redirect_execute(av[i], fd, envp, pid); */
/*   	} */
/*   	else  */
/*   	{ */
/*   		close (fd[1]); */
/*   		close (fd[0]); */
/*   	} */
/*   } */
/* 	i = 2; */
/* 	while (i < ac - 1) */
/* 	{ */
/* 		wait(NULL); */
/* 		i++ */
/* 	} */
/* 	return (0); */
/*  } */
	
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

/* int i = 0; */
/* printf("cmd1->binary = %s\n", data->binary); */
/* while (data->args[i]) */
/* { */
/*   printf("cmd1->args = %s\n", data->args[i]); */
/*   i++; */
/* } */
/* printf("cmd2->binary = %s\n", data->next->binary); */
/* i = 0; */
/* while (data->next->args[i]) */
/* { */
/*   printf("cmd2->args = %s\n", data->next->args[i]); */
/*   i++; */
/* } */
