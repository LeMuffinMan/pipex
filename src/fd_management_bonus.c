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
#include <errno.h>

// free toute la liste
// gerer open error
int open_dup_close_input_redirection(t_data **data, t_data **tmp, t_strs *strs, char **av)
{
    int file;

		close((*tmp)->next->fd[0]);
    file = open(av[1], O_RDONLY);
  	if (file < 0) 
  	{
  		close((*tmp)->fd[1]);
  		//ici open error et plus loin print_errors ?
  		open_error(data, strs, av[1]);
  	}
    if (dup2(file, STDIN_FILENO) == -1)
    {
	    close(file);
	    print_errors(data, strs, "dup2: ", -1);
    }
		/* dprintf(2, "%s (%d) will be input for 1st cmd\n", av[1], file); */
  	if ((*tmp)->fd[0] > 2) // a generaliser ?
    	close((*tmp)->fd[0]);
    (*tmp)->fd[0] = file;
    close(file);
    if (dup2((*tmp)->fd[1], STDOUT_FILENO) == -1)
    {
	    close((*tmp)->fd[1]);
	    print_errors(data, strs, "dup2: ", -1);
    }
    close((*tmp)->fd[1]);
    return (0);
}

//gerer open error !
int open_dup_close_output_redirection(t_data **data, t_data **tmp, t_strs *strs, char **av)
{
    int file;

	close((*tmp)->prev->fd[1]); //checker si un fd open sur le 2nc child
	if (ft_strncmp(av[1], "here_doc", 8) == 0)
		file = open(get_last_arg(av), O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		file = open(get_last_arg(av), O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (file < 0) 
  {
		close((*tmp)->fd[0]);
  	print_errors(data, strs, "open: ", errno);
  }
	// coder le no such file directory infile + executer la seconde normalement 
	// verifier les perms 
	if (dup2(file, STDOUT_FILENO) == -1)
	{
		close(file);
		print_errors(data, strs, "dup2: ", -1);
	}
	/* dprintf(2, "%s (%d) will recieve output from last cmd\n", av[4], file); */
	(*tmp)->fd[1] = file;
	close(file);
	if (ft_strncmp(av[1], "here_doc", 8) == 0)
	{
		if (unlink(av[1]) != 0)
			print_errors(data, strs, "unlink: ", errno);
	}
	if (dup2((*tmp)->fd[0], STDIN_FILENO) == -1)
	{
		close((*tmp)->fd[0]);
		print_errors(data, strs, "dup2: ", -1);
	}
	close((*tmp)->fd[0]);
	return (0);
}

int open_dup_close_pipe_to_pipe(t_data **data, t_data **tmp, t_strs *strs)
{
	if (dup2((*tmp)->fd[1], STDOUT_FILENO) == -1)
	{
		close((*tmp)->fd[1]);
		print_errors(data, strs, "dup2: ", -1);
	}
	close((*tmp)->fd[1]);
	if (dup2((*tmp)->fd[0], STDIN_FILENO) == -1)
	{
		close((*tmp)->fd[0]);
		print_errors(data, strs, "dup2: ", -1);
	}
	close((*tmp)->fd[0]);
	return (0);
}

int	redirect_stdin_stdout(t_data **tmp, t_data **data, t_strs *strs, char **av)
{
	if ((*tmp)->file && ft_strncmp((*tmp)->file, av[1], ft_strlen(av[1])) == 0)
	    open_dup_close_input_redirection(data, tmp, strs, av);
	else if ((*tmp)->file && ft_strncmp((*tmp)->file, get_last_arg(av),
			ft_strlen((*tmp)->file)) == 0)
		open_dup_close_output_redirection(data, tmp, strs, av);
	else
	    open_dup_close_pipe_to_pipe(data, tmp, strs);
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
	/* dprintf(2, "new pipe : in = %d out = %d\n", fd[1], fd[0]); */
	// au premier node (cmd1) : on a set la sortie de cmd1 et l'entree de cmd2
	if (node && !(*node)->prev)
	{
		(*node)->fd[1] = fd[1];
		(*node)->fd[0] = 3;
		(*node)->next->fd[0] = 3;
		/* dprintf(2, "cmd1 : %s write in fd %d\n", (*node)->cmd, (*node)->fd[1]); */
		/* dprintf(2, "cmd2 : %s read from fd %d\n\n", (*node)->next->cmd, (*node)->next->fd[0]); */
	}
	// si on est PAS ENCORE sur la derniere
	else if (node && (*node)->next)
	{
		(*node)->fd[1] = fd[1];
		(*node)->next->fd[0] = fd[0];
		/* dprintf(2, "cmd1 : %s write in fd %d\n", (*node)->cmd, (*node)->fd[1]); */
		/* dprintf(2, "cmd2 : %s read from fd %d\n\n", (*node)->next->cmd, (*node)->next->fd[0]); */
	}
	return (0);
}
