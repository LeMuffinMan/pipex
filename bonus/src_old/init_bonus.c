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

#include "pipex_bonus.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

/* t_data	add_first_node(char **av, char **envp) */
/* { */
/* 	t_data	*node; */
/* 	pid_t	pid; */
/* 	int		fd[2]; */
/**/
/* 	if (pipe(fd) == -1) */
/* 	{ */
/* 		perror("pipe"); */
/* 		exit(errno); */
/* 	} */
/* 	close (fd[1]); // ou l'autre ? */
/* 	node = malloc(sizeof(t_data)); */
/* 	if (node == NULL) */
/* 	{ */
/* 		// exit proprement */
/* 	} */
/* 	node->fd_in = NULL; */
/* 	node->fd_out = fd[0]; // fd[0] ou fd[1] */
/* 	node->envp = envp; */
/* 	node->av = av; */
/* 	node->pid = pid; */
/* 	node->next = NULL; */
/* 	return (node); */
/* } */
/**/
/* int add_node(t_data **data, char **av, int i, char **envp) */
/* { */
/* 	t_data *tmp; */
/* 	t_data node; */
/* 	pid_t	pid; */
/* 	int		fd[2]; */
/**/
/* 	if (pipe(fd) == -1) */
/* 	{ */
/* 		perror("pipe"); */
/* 		exit(errno); */
/* 	} */
/* 	node = malloc(sizeof(t_data)); */
/* 	if (node == NULL) */
/* 	{ */
/* 		// exit proprement */
/* 	} */
/* 	tmp = *data; */
/* 	while (tmp->next) */
/* 		tmp = tmp->next; */
/* 	tmp->next = node; */
/* 	node->fd_in = fd[1]; */
/* 	node->fd_out = fd[0]; // fd[0] ou fd[1] */
/* 	node->envp = envp; */
/* 	node->av = av; */
/* 	node->pid = pid; */
/* 	node->next = NULL; */
/* 	if (!av[i + 2]) */
/* 	{ */
/* 		close (fd[0]); */
/* 		node->fd_out = NULL; */
/* 		return (0); */
/* 	} */
/* 	else  */
/* 		return (1); */
/* } */
/**/
/* t_data	*init(int ac, char **av, char **envp) */
/* { */
/* 	int		i; */
/* 	t_data	*data; */
/**/
/* 	// revoir ac != 5 */
/* 	if (ac != 5 || !*(data)->envp) */
/* 		exit(errno); */
/* 	data = add_first_node(av, envp); */
/* 	i = 3; */
/* 	while (add_node(&data, av, i, envp)) */
/* 		i++; */
/* 	return (0); */
/* } */
