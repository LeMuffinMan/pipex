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
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
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
		/* printf("binary = %s\n", binary); */
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
		perror("get paths error");
		return (NULL);
	}
	args = ft_split(cmd, ' ');
	if (!args)
	{
		free_array(paths);
		perror("args error");
		return (NULL);
	}
	binary = which_cmd(paths, args[0]);
	free_array(paths);
	free_array(args);
	return (binary);
}

void	free_data(t_data **data)
{
	while (1)
	{
		if ((*data)->binary)
			free((*data)->binary);
		if ((*data)->paths)
			free_array((*data)->paths);
		if ((*data)->args)
			free_array((*data)->args);
		if (!(*data)->next)
			break ;
		*data = (*data)->next;
	}
}



void	fct_error(t_data **data)
{
	//il faut aussi fermer le fd ici !
	perror("");
	free_data(data);
	exit(1);
}

int	open_error(t_data **data, int fd)
{
	write(2, "pipex: ", 7);
	write(2, (*data)->file, ft_strlen((*data)->file));
	write(2, ": ", 2);
	perror("");
	close(fd);
	free_data(data);
	exit(1);
}

//si je dois lire dans le pipe 
int stdin_to_fd(t_data **data)
{
	int file;

	//en cas d'erreur revoir la fermeture des fd 
	if (close((*data)->pipe_as_output) == -1)
		fct_error(data);
	file = open((*data)->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (file == -1)
		open_error(data, (*data)->pipe_as_input);
	if (dup2((*data)->file, STDOUT_FILENO) == -1)
		fct_error(data);
	if (dup2((*data)->pipe_as_input, STDIN_FILENO) == -1)
		fct_error(data);
	if (close((*data)->pipe_as_input) == -1)
		fct_error(data);
	if (close((*data)->file) == -1)
		fct_error(data);
	return (0);
}

//si je dois ecrire dans le pipe
int stdout_to_fd(t_data **data)
{
	int file;

	if (close((*data)->pipe_as_input) == -1)
		fct_error(data);
	file = open((*data)->file, O_RDONLY);
	if (file == -1)
		open_error(data, (*data)->pipe_as_output);
	if (dup2((*data)->file, STDIN_FILENO) == -1)
		fct_error(data);
	if (dup2((*data)->pipe_as_output, STDOUT_FILENO) == -1)
		fct_error(data);
	if (close((*data)->pipe_as_output) == -1)
		fct_error(data);
	if (close((*data)->file) == -1)
		fct_error(data);
	return (0);
}

int dup_and_close(t_data **data)
{
	if ((*data)->order == 0)
		stdout_to_fd(data);
	else if ((*data)->order == 1)
		stdin_to_fd(data);
	return (0);
}

/* int	dup_and_close(t_data **data, char mode) */
/* { */
/* 	int file; */
/* 	// revoir les fonctions d'erreur pour tout free et tout close */
/* 	if (close(data->fd_to_close) == -1) */
/* 		fct_error(data); */
/* 	if (mode == 'r') */
/* 		file = open(data->file, O_RDONLY); */
/* 	else if (mode == 'w') */
/* 		file = open(data->file, O_WRONLY | O_CREAT | O_TRUNC, 0644); */
/* 	if (file == -1) */
/* 		open_error(data, data->fd[1]); */
/* 	if (!data->fd_input) */
/* 	{ */
/* 	if (dup2(data->fd, STDOUT_FILENO) == -1) */
/* 		fct_error(cmd); */
/* 	if (dup2(file, STDIN_FILENO) == -1) */
/* 		fct_error(cmd); */
/* 	if (close(fd_to_use) == -1) */
/* 		fct_error(cmd); */
/* 	if (close(infile) == -1) */
/* 		fct_error(cmd); */
/* } */

int	execute(t_data **data, char **envp)
{
	if (!(*data)->binary || access((*data)->binary, F_OK) != 0)
	{
		write(2, "pipex: ", 7);
		write(2, "command not found: ", 19);
		write(2, (*data)->args[0], ft_strlen((*data)->args[0]));
		write(2, "\n", 1);
		free_data(data);
		exit(126);
	}
	if (access((*data)->binary, X_OK) != 0)
	{
		write(2, "pipex: ", 7);
		write(2, "permission denied: ", 19);
		perror("");
		free_data(data);
		exit(127);
	}
	if (execve((*data)->binary, (*data)->args, envp) != 0)
	{
		free_data(data);
		perror("execve error");
		exit(1);
	}
	return (0);
}

// a renommer ?
// pour tout free en cas d'urgence, donner le ** ?
int	from_input_to_output(t_data **data, char **envp, int order)
{
	// order = 0 : 1er pipe // order = 2 : pipe du milieu
	// order = 3 : pipe de fin
	while ((*data)->order != order)
		*data = (*data)->next;
	if ((*data)->order == 0)
	{
		/* free_data(data); */
		dup_and_close(data);
		execute(data, envp);
	}
	/* else if (data->order == 1) */
	/* { */
	/* 	// free tous les nodes non necessaires */
	/* 	// bonus */
	/* } */
	else if ((*data)->order == 1)
	{
		// free tous les nodes non necessaires
		dup_and_close(data);
		execute(data, envp);
	}
	return (0);
}
/**/

int	fork_error(t_data **data)
{
	//pb a close un fd deja close ?
	free_data(data);
	close((*data)->pipe_as_input);
	close((*data)->pipe_as_output);
	exit(1);
}

int	is_a_path(char *s)
{
	int	i;

	i = 0;
	while (s[i])
	{
		if (s[i] == '/')
			return (1);
		i++;
	}
	return (0);
}

int	add_first_node(char **av, char **envp, int fd[2], t_data **data)
{
	t_data	*new_node;

	new_node = malloc(sizeof(t_data));
	if (new_node == NULL)
		return (1); // rien a free ? on exit ?
	new_node->file = av[1];
	new_node->pipe_as_input= fd[0];
	new_node->pipe_as_output = fd[1];
	new_node->paths = get_paths(envp);
	if (is_a_path(av[2]))
		new_node->binary = av[2];
	else
		new_node->binary = get_binary(av[2], envp);
	new_node->args = ft_split(av[2], ' ');
	new_node->order = 0;
	new_node->next = NULL;
	*data = new_node;
	return (0);
}

int	add_last_node(char **av, char **envp, int fd[2], t_data **data)
{
	t_data	*new_node;
	t_data	*tmp;

	new_node = malloc(sizeof(t_data));
	if (new_node == NULL)
		return (1); // faut free data !
	tmp = *data;
	while (tmp->next != NULL)
		tmp = tmp->next;
	tmp->next = new_node;
	new_node->file = av[4];
	new_node->pipe_as_input= fd[0];
	new_node->pipe_as_output = fd[1];
	new_node->paths = get_paths(envp);
	// si pas de path on exit et on free tout
	if (is_a_path(av[3]))
		new_node->binary = av[3];
	else
		new_node->binary = get_binary(av[3], envp);
	new_node->args = ft_split(av[3], ' ');
	new_node->order = 1;
	new_node->next = NULL;
	return (0);
}

t_data	*add_data(char **av, char **envp, int fd[2])
{
	t_data	*data;

	data = NULL;
	add_first_node(av, envp, fd, &data);
	add_last_node(av, envp, fd, &data);
	if (data == NULL)
		return (NULL); // a gerer !
	return (data);
}
int	init(char **av, char **envp, t_data **data)
{
	int fd[2];

	if (pipe(fd) == -1)
	{
		perror("pipe");
		exit(1);
	}
	*data = add_data(av, envp, fd);
	if (!*data)
	{
		perror("add_data failed");
		exit(1);
	}
	// fork ici ?
	return (0);
}

int	exit_end(t_data **data)
{
	free_data(data);
	close((*data)->pipe_as_output);
	close((*data)->pipe_as_input);
	exit(0);
}

int	main(int ac, char **av, char **envp)
{
	t_data	*data;
	int		pid;
	int		status;

	data = NULL;
	if (ac != 5 || !*envp)
		exit(1);
	init(av, envp, &data);
	pid = fork();
	if (pid == -1)
		fork_error(&data);
	if (pid == 0)
		from_input_to_output(&data, envp, 0);
	pid = fork();
	if (pid == -1)
	{
		waitpid(pid, &status, 0);
		fork_error(&data);
	}
	/* rm_first_node(&data); */
		//il faut que je check qqpart quel node exec !
	if (pid == 0)
		from_input_to_output(&data, envp, 1);
	waitpid(pid, &status, 0);
	exit_end(&data);
}

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

/* int from_file_to_pipe(char *file, int fd[2], t_cmd cmd, char **envp) */
/* { */
/*   int infile; */
/**/
/*   if (close (fd[0]) == -1) */
/*     fct_error(cmd); */
/* 	infile = open(file, O_RDONLY); */
/* 	if (infile == -1) */
/*   { */
/*     write(2, "pipex: ", 7); */
/*     write(2, file, ft_strlen(file)); */
/*     write(2, ": ", 2); */
/*     perror(""); */
/*     close (fd[1]); */
/*     free_cmd(cmd); */
/*     exit (1); */
/*   } */
/*   if (dup2(fd[1], STDOUT_FILENO) == -1) */
/*     fct_error(cmd); */
/* 	if (dup2(infile, STDIN_FILENO) == -1) */
/* 		fct_error(cmd); */
/*   if (close (fd[1]) == -1) // j'en ai plus besoin ? */
/*     fct_error(cmd); */
/*   if (close (infile) == -1) */
/*     fct_error(cmd); */
/*   if (!cmd.binary || access(cmd.binary, F_OK) != 0) */
/*   { */
/*     write(2, "pipex: ", 7); */
/*     write(2, "command not found: ", 19); */
/*     write(2, cmd.args[0], ft_strlen(cmd.args[0])); */
/*     write(2, "\n", 1); */
/*     free_cmd(cmd); */
/*     exit (126); */
/*   } */
/*   if (access(cmd.binary, X_OK) != 0) */
/*   { */
/*     write(2, "pipex: ", 7); */
/*     write(2, "permission denied: ", 19); */
/*     perror(""); */
/*     free_cmd(cmd); */
/*     exit (127); */
/*   } */
/*   if (execve(cmd.binary, cmd.args, envp) != 0) */
/*   { */
/*     free_cmd(cmd); */
/*     perror("execve error"); */
/*     exit (1); */
/*   } */
/*   exit (0); */
/* } */
/**/
/* int from_pipe_to_file(char *file, int fd[2], t_cmd cmd, char **envp) */
/* { */
/*   int outfile; */
/**/
/*   close (fd[1]); */
/*   //si il existe pas faut le creer ! */
/*   //et si ya pas de permissions ? */
/* 	outfile = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644); */
/* 	if (outfile == -1) */
/*   { */
/*     write(2, "pipex: ", 7); */
/*     write(2, file, ft_strlen(file)); */
/*     write(2, ": ", 2); */
/*     perror(""); */
/*     close (fd[0]); */
/*     free_cmd(cmd); */
/*     exit (1); */
/*   } */
/* 	dup2(fd[0], STDIN_FILENO); */
/* 	dup2(outfile, STDOUT_FILENO); */
/*   close (fd[0]); */
/*   close (outfile); */
/*   if (!cmd.binary || access(cmd.binary, F_OK) != 0) */
/*   { */
/*     write(2, "pipex: ", 7); */
/*     write(2, "command not found: ", 19); */
/*     write(2, cmd.args[0], ft_strlen(cmd.args[0])); */
/*     write(2, "\n", 1); */
/*     free_cmd(cmd); */
/*     exit (126); */
/*   } */
/*   if (access(cmd.binary, X_OK) != 0) */
/*   { */
/*     write(2, "pipex: ", 7); */
/*     write(2, "permission denied: ", 19); */
/*     perror(""); */
/*     free_cmd(cmd); */
/*     exit (127); */
/*   } */
/*   if (execve(cmd.binary, cmd.args, envp) != 0) */
/*   { */
/*     free_cmd(cmd); */
/*     perror("execve error"); */
/*     exit (1); */
/*   } */
/*   exit (0); */
/* } */
