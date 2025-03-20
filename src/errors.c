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
#include <errno.h>

void print_errors(t_data **data, char *message, int error_code, int fd_to_close)
{
	if (fd_to_close > 2)
		close(fd_to_close);
	ft_putstr_fd("pipex: ", 2);
	ft_putstr_fd(message, 2);
	perror("");
	close_pipe_free_exit(data, NULL, error_code);
}

void error_cmd_not_found(t_data **data, t_strs *strs, t_data **tmp)
{
	ft_putstr_fd("pipex: command not found: ", 2);
	if ((*tmp)->cmd)
		ft_putstr_fd((*tmp)->cmd, 2);
	ft_putstr_fd("\n", 2);
	close_pipe_free_exit(data, strs, 127); 
}

void malloc_error(t_data **data)
{
	ft_putstr_fd("pipex: malloc error: ", 2);
	perror("");
	close_pipe_free_exit(data, NULL, errno); 
}

int close_pipe_free_exit(t_data **data, t_strs *strs, int exit_code)
{
	if (strs && strs->args)
		free_array(strs->args);
	if (strs && strs->path)
		free(strs->path);
	if ((*data)->fd[0] && (*data)->fd[0] > 2)
		close((*data)->fd[0]);
	if ((*data)->prev && (*data)->prev->fd[1] && (*data)->prev->fd[1] > 2)
		close((*data)->prev->fd[1]);
	if (*data && (*data)->fd[1] && (*data)->fd[1] > 2)
		close((*data)->fd[1]);
	free_data(data);
	if (exit_code == 0)
		exit(errno);
	else
		exit(exit_code);
	return (0);
}

