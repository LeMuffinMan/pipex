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
#include "pipex_bonus.h"
#include <fcntl.h>  // open
#include <stdio.h>  // perror
#include <stdlib.h> // exit
#include <unistd.h> // fork, pipe, dup2, execve, access, close, read, write

// free toute la liste
// gerer open error
int open_dup_close_input_redirection(t_data **data, t_data **tmp, t_strs *strs, char **av)
{
    int file;

    file = open(av[1], O_RDONLY);
    if (dup2(file, STDIN_FILENO) == -1)
    {
	    close(file);
	    open_error(data, strs);
    }
  	if ((*tmp)->fd[0] > 2)
    	close((*tmp)->fd[0]);
    (*tmp)->fd[0] = file;
    close(file);
    if (dup2((*tmp)->fd[1], STDOUT_FILENO) == -1)
    {
	    close((*tmp)->fd[1]);
	    open_error(data, strs);
    }
    close((*tmp)->fd[1]);
    return (0);
}

//gerer open error !
int open_dup_close_output_redirection(t_data **data, t_data **tmp, t_strs *strs, char **av)
{
    int file;

	if (ft_strncmp(av[1], "here_doc", 8) == 0)
		file = open(get_last_arg(av), O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		file = open(get_last_arg(av), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (dup2(file, STDOUT_FILENO) == -1)
	{
		close(file);
		open_error(data, strs);
	}
	(*tmp)->fd[1] = file;
	close(file);
	if (ft_strncmp(av[1], "here_doc", 8) == 0)
		unlink(av[1]);
	if (dup2((*tmp)->fd[0], STDIN_FILENO) == -1)
	{
		close((*tmp)->fd[0]);
		open_error(data, strs);
	}
	close((*tmp)->fd[0]);
	return (0);
}

int open_dup_close_pipe_to_pipe(t_data **data, t_data **tmp, t_strs *strs)
{
	if (dup2((*tmp)->fd[1], STDOUT_FILENO) == -1)
	{
		close((*tmp)->fd[1]);
		open_error(data, strs);
	}
	close((*tmp)->fd[1]);
	if (dup2((*tmp)->fd[0], STDIN_FILENO) == -1)
	{
		close((*tmp)->fd[0]);
		open_error(data, strs);
	}
	close((*tmp)->fd[0]);
	return (0);
}

int	redirect_stdin_stdout(t_data **tmp, t_data **data, t_strs *strs, char **av)
{
	if (ft_strncmp((*tmp)->file, av[1], ft_strlen(av[1])) == 0)
	    open_dup_close_input_redirection(data, tmp, strs, av);
	else if (ft_strncmp((*tmp)->file, get_last_arg(av),
			ft_strlen((*tmp)->file)) == 0)
		open_dup_close_output_redirection(data, tmp, strs, av);
	else
	    open_dup_close_pipe_to_pipe(data, tmp, strs);
	return (0);
}

// Free la liste !
int	get_pipe(t_data **node)
{
	int	fd[2];

	if (pipe(fd) == -1)
	{
		perror("pipe");
		//free la liste !
		exit(EXIT_FAILURE);
	}
	dprintf(2, "new pipe : in = %d out = %d\n", fd[1], fd[0]);
	// au premier node (cmd1) : on a set la sortie de cmd1 et l'entree de cmd2
	if (node && !(*node)->prev)
	{
		(*node)->fd[1] = fd[1];
		(*node)->fd[0] = -1;
		(*node)->next->fd[0] = fd[0];
		dprintf(2, "cmd1 : %s will write in fd %d\n", (*node)->cmd, (*node)->fd[1]);
		dprintf(2, "cmd2 : %s wait input from fd %d\n\n", (*node)->next->cmd, (*node)->next->fd[0]);
	}
	// si on est PAS ENCORE sur la derniere
	else if (node && (*node)->next)
	{
		(*node)->fd[1] = fd[1];
		(*node)->next->fd[0] = fd[0];
		dprintf(2, "cmd : %s will write in fd %d\n", (*node)->cmd, (*node)->fd[1]);
		dprintf(2, "cmd : %s wait input from fd %d\n\n", (*node)->next->cmd, (*node)->next->fd[0]);
	}
	return (0);
}
