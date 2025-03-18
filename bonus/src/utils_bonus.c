/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:12:54 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/15 17:42:05 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <sys/wait.h>
#include <unistd.h> 
#include <stdlib.h>
#include <errno.h>
#include "pipex_bonus.h"
#include "libft.h"
#include <stdio.h>  // perror

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

int close_pipe_free_exit(t_data **data, t_strs *strs, int exit_code)
{
	if (strs->args)
		free_array(strs->args);
	if (strs->path)
		free(strs->path);
	if ((*data)->fd[0])
		close((*data)->fd[0]);
	if ((*data)->prev && (*data)->prev->fd[1] && (*data)->prev->fd[1])
		close((*data)->prev->fd[1]);
	free_data(data);
	if (exit_code == 0)
		exit(errno);
	else
		exit(exit_code);
	return (0);
}

