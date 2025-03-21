/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 16:29:17 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/08 15:28:27 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "pipex.h"
#include <errno.h>    
#include <fcntl.h>    
#include <stdio.h>    
#include <stdlib.h>   
#include <string.h>  
#include <sys/wait.h> 
#include <unistd.h>   

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
			if ((*tmp) && (*tmp)->fd[1] > 2)
				close((*tmp)->fd[1]);
			if ((*tmp)->prev && (*tmp)->prev->fd[0] > 2)
				close((*tmp)->prev->fd[0]);
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
	if (ac == 5)
	{
		init_data(&data, av, env);
		tmp = data;
		fork_management(&data, &tmp, av);
	}
	else
		ft_putstr_fd("Usage : ./pipex infile cmd1 cmd2 outfile\n", 2);
	return (0);
}
