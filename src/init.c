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

#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include "pipex.h"
#include <stdlib.h>

int	init(t_data *data, int ac, char **av, int fd[2])
{
	if (ac != 5 || !*(data)->envp)
		exit(errno);
	data->infile = av[1];
	data->cmd1 = av[2];
	data->cmd2 = av[3];
	data->outfile = av[4];
	if (pipe(fd) == -1)
	{
		perror("pipe");
		exit(errno);
	}
	data->pos = 0;
	return (0);
}
