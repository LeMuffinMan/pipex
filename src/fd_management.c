/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd_management_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 16:01:38 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/15 17:39:45 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "pipex.h"
#include <fcntl.h>  // open
#include <stdio.h>  // perror
#include <stdlib.h> // exit
#include <unistd.h> // fork, pipe, dup2, execve, access, close, read, write
#include <errno.h>

int open_dup_close_input_redirection(t_data **data, t_data **tmp, char **av)
{
    int file;

		if ((*tmp)->next)
			close((*tmp)->next->fd[0]);
    file = open(av[1], O_RDONLY);
  	if (file < 0) 
	    print_errors(data, "open : ", -1, (*tmp)->fd[1]);
    if (dup2(file, STDIN_FILENO) == -1)
	    print_errors(data, "dup2: ", -1, file);
  	if ((*tmp)->fd[0] > 2)
    	close((*tmp)->fd[0]);
    (*tmp)->fd[0] = file;
    close(file);
    if (dup2((*tmp)->fd[1], STDOUT_FILENO) == -1)
	    print_errors(data, "dup2: ", -1, (*tmp)->fd[1]);
    close((*tmp)->fd[1]);
    return (0);
}

//gerer open error !
int open_dup_close_output_redirection(t_data **data, t_data **tmp, char **av)
{
    int file;

	if ((*tmp)->prev)
		close((*tmp)->prev->fd[1]);
	file = open(get_last_arg(av), O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (file < 0) 
  {
  	if (access(av[4], W_OK) != 0)
  		print_errors(data, "open: ", 1, (*tmp)->fd[0]);
  	print_errors(data, "open: ", errno, (*tmp)->fd[0]);
  }
	if (dup2(file, STDOUT_FILENO) == -1)
		print_errors(data, "dup2: ", -1, file);
	(*tmp)->fd[1] = file;
	close(file);
	if (dup2((*tmp)->fd[0], STDIN_FILENO) == -1)
		print_errors(data, "dup2: ", -1, (*tmp)->fd[0]);
	close((*tmp)->fd[0]);
	return (0);
}

int open_dup_close_pipe_to_pipe(t_data **data, t_data **tmp)
{
	if (dup2((*tmp)->fd[1], STDOUT_FILENO) == -1)
		print_errors(data, "dup2: ", -1, (*tmp)->fd[1]);
	close((*tmp)->fd[1]);
	if (dup2((*tmp)->fd[0], STDIN_FILENO) == -1)
		print_errors(data, "dup2: ", -1, (*tmp)->fd[0]);
	close((*tmp)->fd[0]);
	return (0);
}

int	redirect_stdin_stdout(t_data **tmp, t_data **data, char **av)
{
	if ((*tmp)->file && ft_strncmp((*tmp)->file, av[1], ft_strlen(av[1])) == 0)
	    open_dup_close_input_redirection(data, tmp, av);
	else if ((*tmp)->file && ft_strncmp((*tmp)->file, get_last_arg(av),
			ft_strlen((*tmp)->file)) == 0)
		open_dup_close_output_redirection(data, tmp, av);
	else
	    open_dup_close_pipe_to_pipe(data, tmp);
	return (0);
}

int	get_pipe(t_data **node, t_data **data)
{
	int	fd[2];

	if (pipe(fd) == -1)
	{	
		ft_putstr_fd("pipex: pipe error: ", 2);
		perror("");
		ft_putstr_fd("\n", 2);
		free_data(data);
		exit(errno);
	}
	if (node && !(*node)->prev)
	{
		(*node)->fd[1] = fd[1];
		(*node)->fd[0] = 3;
		(*node)->next->fd[0] = 3;
	}
	else if (node && (*node)->next)
	{
		(*node)->fd[1] = fd[1];
		(*node)->next->fd[0] = fd[0];
	}
	return (0);
}
