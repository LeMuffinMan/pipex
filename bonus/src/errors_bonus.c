/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:18:50 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/19 16:51:32 by oelleaum         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "pipex_bonus.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int	close_pipe_free_exit(t_data **data, t_strs *strs, int exit_code)
{
	if (strs && strs->args)
		free_array(strs->args);
	if (strs && strs->path)
		free(strs->path);
	if ((*data)->fd[0])
		close((*data)->fd[0]);
	if ((*data)->prev && (*data)->prev->fd[1] && (*data)->prev->fd[1] > 2)
		close((*data)->prev->fd[1]);
	if (*data && (*data)->fd[1])
		close((*data)->fd[1]);
	free_data(data);
	if (exit_code == 0)
		exit(errno);
	else
		exit(exit_code);
	return (0);
}

void	print_errors(t_data **data, char *file_name, int error_code,
		int fd_to_close)
{
	if (fd_to_close > 2)
		close(fd_to_close);
	ft_putstr_fd("pipex: ", 2);
	ft_putstr_fd(file_name, 2);
	ft_putstr_fd(": ", 2);
	perror("");
	close_pipe_free_exit(data, NULL, error_code);
}

void	error_cmd_not_found(t_data **data, t_strs *strs, t_data **tmp)
{
	ft_putstr_fd("pipex: command not found: ", 2);
	if ((*tmp)->cmd)
		ft_putstr_fd((*tmp)->cmd, 2);
	ft_putstr_fd("\n", 2);
	close_pipe_free_exit(data, strs, 127);
}

void	malloc_error(t_data **data)
{
	ft_putstr_fd("pipex: malloc error: ", 2);
	perror("");
	close_pipe_free_exit(data, NULL, errno);
}
