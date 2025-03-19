/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:18:50 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/15 17:41:56 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"
#include <unistd.h>
#include "libft.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

void print_errors(t_data **data, t_strs *strs, char *message, int error_code)
{
	ft_putstr_fd("pipex: ", 2);
	ft_putstr_fd(message, 2);
	perror("");
	close_pipe_free_exit(data, strs, error_code);
}

void error_cmd_not_found(t_data **data, t_data **tmp, t_strs *strs)
{
	ft_putstr_fd("pipex: command not found: ", 2);
	if ((*tmp)->cmd)
		ft_putstr_fd((*tmp)->cmd, 2);
	ft_putstr_fd("\n", 2);
	close_pipe_free_exit(data, strs, 127); //127 ou errno ?
}

void malloc_error(t_data **data)
{
	ft_putstr_fd("pipex: malloc error: ", 2);
	perror("");
	free_data(data);
	exit(errno);
}

//env -i ./pipex_bonus infile "cat" "cat"  outfile
void open_error(t_data **data, t_strs *strs, char *file)
{
	ft_putstr_fd("pipex: ", 2);
	ft_putstr_fd(file, 2);
	ft_putstr_fd(": ", 2);
	perror("");
	close_pipe_free_exit(data, strs, errno);
}

int close_pipe_free_exit(t_data **data, t_strs *strs, int exit_code)
{
	/* dprintf(2, "fd to close : %d\n", (*data)->fd[1]); */
	/* dprintf(2, "data->fd[0] = %d\n data->fd[1] = %d\n", (*data)->fd[0], (*data)->fd[1]); */
	if ((*data)->prev)
		/* dprintf(2, "data->prev->fd[0] = %d\ndata->prev->fd[1] = %d\n", (*data)->prev->fd[0], (*data)->prev->fd[1]); */
	if (strs->args)
		free_array(strs->args);
	if (strs->path)
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

