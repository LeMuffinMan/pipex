/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:18:50 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/08 13:24:25 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"
#include <unistd.h>
#include "libft.h"
#include <stdlib.h>
#include <stdio.h>

int	open_error(int fd, char *file, char *path, char **args)
{
	write(2, "pipex: ", 7);
	write(2, file, ft_strlen(file));
	write(2, ": ", 2);
	perror("");
	free(path);
	free_array(args);
	close(fd);
	exit(1);
}

int	error_cmd_not_found(int fd[2], char **args, char *path, char *binary)
{
	write(2, "pipex: ", 7);
	write(2, "command not found: ", 19);
	if (args[0])
		write(2, args[0], ft_strlen(args[0]));
	write(2, "\n", 1);
	if (binary)
		free(binary);
	if (path)
		free(path);
	free_array(args);
	if (close(fd[0]) == -1)
		exit(127);
	if (close(fd[1]) == -1)
		exit(127);
	exit(127);
}

int	error_permission_denied(char **args, char *binary)
{
	ft_putstr_fd("pipex: permission denied: ", 2);
	perror("");
	free_array(args);
	free(binary);
	exit(1);
}
