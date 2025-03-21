/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 16:29:17 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/20 15:21:03 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "pipex_bonus.h"
#include <errno.h>
#include <stdlib.h> 
#include <unistd.h>
#include <stdio.h> 

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
	(*node)->fd[1] = fd[1];
	(*node)->next->fd[0] = fd[0];
	/* dprintf(2, "%s will write in fd %d\n", (*node)->cmd, (*node)->fd[1]); */
	/* if ((*node)->next) */
		/* dprintf(2, "%s will read from fd %d\n", (*node)->cmd, (*node)->next->fd[0]); */
	return (0);
}

int	fork_management(t_data **data, t_data **tmp, char **av)
{
	while (*tmp)
	{
		if ((*tmp) && (*tmp)->next)
			get_pipe(tmp, data);
		(*tmp)->pid = fork();
		if ((*tmp)->pid == -1)
			print_errors(data, "fork: ", -1, -1);
		if ((*tmp)->pid == 0)
			parse_redirect_execute(data, tmp, av);
		else
		{
			if ((*tmp) && (*tmp)->next)
				close((*tmp)->fd[1]);
			if ((*tmp)->prev)
				close((*tmp)->fd[0]);
		}
		if (!(*tmp)->next)
			close((*tmp)->fd[0]);
		*tmp = (*tmp)->next;
	}
	exit(wait_children(data));
}

int	main(int ac, char **av, char **env)
{
	t_data	*data;
	t_data	*tmp;

	data = NULL;
	if (ac >= 5)
	{
		init_data(&data, av, env);
		tmp = data;
		fork_management(&data, &tmp, av);
	}
	else
		ft_putstr_fd("Usage : ./pipex here_doc LIMITER cmd1 cmd2 outfile\n", 1);
	return (0);
}
