/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:24:42 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/08 13:32:05 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "pipex_bonus.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int	wait_children(int fd[2], pid_t pid1, pid_t pid2)
{
	int	status;
	int	exit_code;

	exit_code = EXIT_SUCCESS;
	if (waitpid(pid1, &status, 0) == -1)
		close_and_quit(fd, errno);
	if (waitpid(pid2, &status, 0) == -1)
		close_and_quit(fd, errno);
	if (WIFEXITED(status))
		exit_code = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		exit_code = 128 + WTERMSIG(status);
	if (exit_code == EXIT_SUCCESS && WIFEXITED(status))
		exit_code = WEXITSTATUS(status);
	else if (exit_code == EXIT_SUCCESS && WIFSIGNALED(status))
		exit_code = 128 + WTERMSIG(status);
	return (exit_code);
}

/* int	parse_redirect_execute(t_data *data, int fd[2]) */
/* { */
/* 	char	*path; */
/* 	char	**args; */
/* 	char	*cmd; */
/**/
/* 	path = NULL; */
/* 	if (data->pos == 0 && data->cmd1 != NULL) */
/* 		cmd = data->cmd1; */
/* 	else if (data->pos == 1 && data->cmd2 != NULL) */
/* 		cmd = data->cmd2; */
/* 	args = ft_split(cmd, ' '); */
/* 	redirect_fd(data, fd, path, args); */
/* 	if (!*args) */
/* 		error_cmd_not_found(fd, args, NULL, NULL); */
/* 	if (is_a_path(args[0])) */
/* 		path = args[0]; */
/* 	else */
/* 	{ */
/* 		path = get_binary(args[0], data->envp); */
/* 		if (!path) */
/* 			error_cmd_not_found(fd, args, NULL, NULL); */
/* 	} */
/* 	execute(path, args, data->envp); */
/* 	return (0); */
/* } */

/* int	redirect_fd(t_data *data, int fd[2], char *path, char **args) */
/* { */
/* 	int	file; */
/**/
/* 	if (data->pos == 0) */
/* 	{ */
/* 		close(fd[0]); */
/* 		file = open(data->infile, O_RDONLY); */
/* 		if (file == -1) */
/* 			open_error(fd[1], data->infile, path, args); */
/* 		dup_and_close(fd[1], file, fd[1]); */
/* 	} */
/* 	else */
/* 	{ */
/* 		close(fd[1]); */
/* 		file = open(data->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644); */
/* 		if (file == -1) */
/* 			open_error(fd[0], data->outfile, path, args); */
/* 		dup_and_close(file, fd[0], fd[0]); */
/* 	} */
/* 	return (0); */
/* } */

int	execute(char *binary, char **args, char **envp)
{
	if (!binary || access(binary, F_OK) != 0)
		error_cmd_not_found(NULL, args, NULL, NULL);
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
