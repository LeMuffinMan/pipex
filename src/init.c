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

int add_first_node(t_data **data, char *cmd, char **env, char *infile) {
  t_data *node;

  node = malloc(sizeof(t_data));
  if (node == NULL)
    malloc_error(data);
  *data = node;
  node->file = infile;
  node->cmd = cmd;
  node->env = env;
  node->next = NULL;
  node->prev = NULL;
  node->fd[0] = -1;
  node->fd[1] = -1;
  return (0);
}

int add_node(t_data **data, char *cmd, char **env, char *last_arg) {
  t_data *node;
  t_data *tmp;

  node = NULL;
  node = malloc(sizeof(t_data));
  if (node == NULL)
    malloc_error(data);
  tmp = *data;
  while (tmp->next)
    tmp = tmp->next;
  tmp->next = node;
  node->prev = tmp;
  if (*last_arg)
    node->file = last_arg;
  else
    node->file = NULL;
  node->cmd = cmd;
  node->env = env;
  node->next = NULL;
  node->fd[0] = -1;
  node->fd[1] = -1;
  return (0);
}

int init_data(t_data **data, char **av, char **env) {
  int i;

  add_first_node(data, av[2], env, av[1]);
  i = 3;
  while (av[i + 1]) {
    add_node(data, av[i], env, av[i + 1]);
    i++;
  }
  return (0);
}
