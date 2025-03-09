/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 15:18:38 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/08 15:27:59 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

t_data	add_first_node(int ac, char **av, int fd[2], char **envp)
{
	t_data	node;
	pid_t	pid;
	int		fd[2];

	if (pipe(fd) == -1)
	{
		perror("pipe");
		exit(errno);
	}
	close (fd[1]); // ou l'autre ?
	node = malloc(sizeof(t_data));
	if (node == NULL)
	{
		// exit proprement
	}
	node->fd_in = NULL;
	node->fd_out = fd[0]; // fd[0] ou fd[1]
	node->envp = envp;
	node->av = av;
	node->pid = pid;
	node->next = NULL;
	return (node);
}

t_data	*init(int ac, char **av, int fd[2], char **envp)
{
	int		i;
	t_data	*data;

	// revoir ac != 5
	if (ac != 5 || !*(data)->envp)
		exit(errno);
	data = add_first_node(ac, av, fd, envp);
	// d'abord on pipe :
	// fd_in
	// fd_out
	// add first node :
	// infile = av[1]
	// cmd = av[2];
	// outfile = NULL;
	// envp = data->envp;
	// pipe_fd = faire un premier pipe hors boucle
	// next = NULL;
	// boucle
	// tmp->next = New_node
	// infile = NULL
	// cmd = av[i]
	// outfile = NULL
	// envp = data->envp
	// faire un pipe
	// next = NULL
	// fin de boucle :
	// tmp->next = new_node
	// infile = NULL
	// cmd = av[i];
	// outfile = av[i + 1]
		//attention au segfault si il oublie de me mettre un outfile
	// envp = data->envp
	// pipe_fd : faire un pipe
	// next = NULL;
	//
	data->infile = av[1];
	i = 2;
	while (av[i + 2])
	{
		data->cmd = av[i];
		if (pipe(data->pipe_fd) == -1)
		{
			perror("pipe");
			exit(errno);
		}
		i++;
	}
	data->outfile = av[i];
	return (0);
}
