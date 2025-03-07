/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 16:29:17 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/01 16:29:17 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

// mettre un max fd dans l'include ?
// dup2 et close a proteger ?

#include "libft.h"
#include "pipex.h"
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
/// errno : variable globale qui stock le dernier code d'erreur de l'appel systeme echoue
/// strerror a la place ?
///
/// REVOIR LIBFT COmpile et virer le header en trop

void	free_array(char **s)
{
	int	i;

	i = 0;
	while (s[i])
	{
		free(s[i]);
		s[i] = NULL;
		i++;
	}
	free(s);
}

char	*get_path_line(char **envp)
{
	char	*path_line;
	int		i;

	i = 0;
	path_line = NULL;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0 && ft_strlen(envp[i]) > 5) //voir pour le cas ou le PATH est vide
		{
			path_line = envp[i] + 5;
			break ;
		}
		i++;
	}
	// retourne null si pas trouve ?
	return (path_line);
}

char	*join_full_path(char *binary, char *cmd, char *path)
{
	int	i;
	int	j;

	binary = malloc(sizeof(char) * ft_strlen(path) + ft_strlen(cmd) + 2);
	if (!binary)
		return (NULL);
	// A gerer autrement pour differencier malloc error et path null
	i = 0;
	while (path[i])
	{
		binary[i] = path[i];
		i++;
	}
	binary[i++] = '/';
	j = 0;
	while (cmd[j])
		binary[i++] = cmd[j++];
	binary[i] = '\0';
	return (binary);
}

char	*which_cmd(char **paths, char *cmd)
{
	char	*binary;
	int		i;

	binary = NULL;
	i = 0;
	while (paths[i])
	{
		binary = join_full_path(binary, cmd, paths[i]);
		if (access(binary, X_OK) == 0)
			return (binary);
		free(binary);
		i++;
	}
	binary = NULL;
	return (binary);
}

char	**get_paths(char **envp)
{
	char	*path_line;
	char	**paths;

	paths = NULL;
	path_line = get_path_line(envp);
	if (!*path_line) // si on vide le PATH ,
						/* estce que le 6eme bite de la ligne PATH= est null ? */
	{
		perror("path_line error");
		return (NULL);
	}
	/* //bien renvoyer null si ca merde */
	paths = ft_split(path_line, ':');
	return (paths);
}

char	*get_binary(char *cmd, char **envp)
{
	char	**paths;
	char	**args;
	char	*binary;

	paths = get_paths(envp);
	if (!paths)
	{
		perror("get paths");
		return (NULL);
	}
	args = ft_split(cmd, ' ');
	if (!args)
	{
		free_array(paths);
		perror("args");
		return (NULL);
	}
	binary = which_cmd(paths, args[0]);
	free_array(paths);
	free_array(args);
	return (binary);
}

int close_and_quit(int fd[2])
{
	if (close(fd[0]) == -1)
		perror("close");
	if (close(fd[1]) == -1)
		perror("close");
	exit (1);
}

int execute(char *binary, char **args, char **envp)
{
	if (!binary || access(binary, F_OK) != 0)
	{
		//revoir ici
		write(2, "pipex: ", 7);
		write(2, "command not found: ", 19);
		write(2, binary, ft_strlen(binary));
		write(2, "\n", 1);
		free_array(args);
		free(binary);
		exit(126);
	}
	if (access(binary, X_OK) != 0)
	{
		write(2, "pipex: ", 7);
		write(2, "permission denied: ", 19);
		perror("");
		free_array(args);
		free(binary);
		exit(127);
	}
	if (execve(binary, args, envp) != 0)
	{
		free_array(args);
		free(binary);
		perror("execve error");
		exit(1);
	}
	return (0);
}

int is_a_path(char *s)
{
	int i;

	i = 0;
	while (s[i])
	{
		if (s[i] == '/')
			return (1);
		i++;
	}
	return (0);
}

int	open_error(int fd, char *file)
{
	write(2, "pipex: ", 7);
	write(2, file, ft_strlen(file));
	write(2, ": ", 2);
	perror("");
	close(fd);
	exit(1);
}

int redirect_fd(t_data *data, int fd[2])
{
	int file;

	if (data->pos == 0)
	{
		close(fd[0]);
		file = open(data->infile, O_RDONLY);
		if (file == -1)
			open_error(fd[1], data->infile);
		dup2(file, STDIN_FILENO);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
	}
	else
	{
		close(fd[1]);
		file = open(data->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (file == -1)
			open_error(fd[0], data->outfile);
		dup2(file, STDOUT_FILENO);
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);
	}
	return (0);
}

/* int redirect_fd(t_data data, int fd_to_close, int fd_to_use, char *file) */
/* { */
/* 	int file_fd; */
/**/
/* 	close(fd_to_close); */
/* 	if (pos == 0) */
/* 	{ */
/* 		file_fd = open(file, O_RDONLY); */
/* 		if (file_fd == -1) */
/* 			open_error(fd_to_use, file); */
/* 		dup2(file, STDIN_FILENO); */
/* 		dup2(fd[1], STDOUT_FILENO); */
/* 	} */
/* 	else */
/* 	{ */
/* 		file_fd = open (file, O_WRONLY | O_CREAT | O_TRUNC, 0644); */
/* 		if (file_fd == -1) */
/* 			open_error(fd_to_use, file); */
/* 		dup2(file, STDOUT_FILENO); */
/* 		dup2(fd[0], STDIN_FILENO); */
/* 	} */
/* 	close(fd_to_use); */
/* 	return (0); */
/* } */

int parse_redirect_execute(t_data *data, int fd[2])
{
	char *path;
	char **args;
	char *cmd;

	if (data->pos == 0)
		cmd = data->cmd1;
	else
		cmd = data->cmd2;
	args = ft_split(cmd, ' ');
	if (is_a_path(args[0]))
		path = args[0];
	else 
	{
		path = get_binary(args[0], data->envp);
		if (!path) //voir si ca gere tous les cas d'erreurs 
		{
			free(path);
			close_and_quit(fd);
		}
	}
	redirect_fd(data, fd);
	execute(path, args, data->envp);
	return (0);
}

int init(t_data *data, int ac, char **av, int fd[2])
{
	if (ac != 5 || !*(data)->envp) //voir si ca marche
		exit(1);
	data->infile = av[1];
	data->cmd1 = av[2];
	data->cmd2 = av[3];
	data->outfile = av[4];
	if (pipe(fd) == -1)
	{
		perror("pipe");
		exit(1);
	}
	data->pos = 0;
	return (0);
}

int main(int ac, char **av, char **envp)
{
	int pid;
	/* int status; */
	int fd[2];
	t_data data;

	data.envp = envp;
	init(&data, ac, av, fd);
	pid = fork();
	if (pid == -1)
		close_and_quit(fd);
	if (pid == 0)
		parse_redirect_execute(&data, fd);
	data.pos = 1;
	pid = fork();
	if (pid == -1)
		close_and_quit(fd);
	if (pid == 0)
		parse_redirect_execute(&data, fd);
	wait(NULL); // a la place de wait pid ?
	close(fd[0]);
	close(fd[1]);
	//proteger waitpid ?
	exit (0);
}

//file no perm / not existing ko
// path de cmd ko
// args sur une cmd ko
// incorrect cmd2 ko
// si on supprime que la ligne PATH ?
//
//tout proteger


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


