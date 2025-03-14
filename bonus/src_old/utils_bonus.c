/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:12:54 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/08 13:13:48 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <sys/wait.h>
#include <unistd.h> 
#include <stdlib.h>
#include <errno.h>

int	dup_and_close(int fd_out, int fd_in, int fd_to_close)
{
	if (dup2(fd_out, STDOUT_FILENO) == -1)
		exit(errno);
	if (dup2(fd_in, STDIN_FILENO) == -1)
		exit(errno);
	if (close(fd_to_close) == -1)
		exit(errno);
	return (0);
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

/* int	close_and_quit(int fd[2], int error_code) */
/* { */
/* 	if (close(fd[0]) == -1) */
/* 		exit(errno); */
/* 	if (close(fd[1]) == -1) */
/* 		exit(errno); */
/* 	exit(error_code); */
/* } */

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
