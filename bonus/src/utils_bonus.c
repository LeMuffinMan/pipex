/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:12:54 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/19 16:24:54 by oelleaum         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "pipex_bonus.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int is_a_path(char *s) {
  int i;

  i = 0;
  while (s[i]) {
    if (s[i] == '/')
      return (1);
    i++;
  }
  return (0);
}

void free_array(char **s) {
  int i;

  i = 0;
  while (s && s[i]) {
    free(s[i]);
    s[i] = NULL;
    i++;
  }
  free(s);
  s = NULL;
}

char *get_last_arg(char **av) {
  int i;

  i = 0;
  while (av && av[i])
    i++;
  return (av[i - 1]);
}

int free_data(t_data **data) {
  t_data *tmp;
  t_data *next_node;

  tmp = *data;
  if (!*data)
    return (1);
  while (tmp) {
    next_node = tmp->next;
    free(tmp);
    tmp = next_node;
  }
  *data = NULL;
  return (0);
}
