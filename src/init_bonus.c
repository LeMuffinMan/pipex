/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 15:18:38 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/15 17:27:57 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>  // open
#include "libft.h"

int free_data(t_data **data)
{
	t_data *tmp;
	t_data *next_node;

	tmp = *data;
	if (!*data)
		return (1);
	while(tmp)
	{
		next_node = tmp->next;
		free(tmp);
		tmp = next_node;
	}
	*data = NULL;
	return (0);
}

int add_first_node(t_data **data, char *cmd, char **env, char *infile)
{
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

int add_node(t_data **data, char *cmd, char **env, char *last_arg)
{
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

int here_doc(char **av)
{
  int file;
  char *line;
	
	file = open(av[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (file == -1)
	{
		ft_putstr_fd("pipex: open error: ", 2);
		perror("");
	}
	while (1)
	{
		ft_putstr_fd("> ", STDOUT_FILENO);
		line = get_next_line(STDIN_FILENO);
		if (!line)
		{
			ft_putstr_fd("\n", STDOUT_FILENO);
			break ;
		}
		if (ft_strncmp(line, av[2], ft_strlen(av[2])) == 0 && line[ft_strlen(av[2])] == '\n')
		{
			free(line);
			break ;
		}
		ft_putstr_fd(line, file);
		free(line);
	}
	close(file);
	return (0);
}

int init_data(t_data **data, char **av, char **env)
{
	int i;

  /* if (ft_strncmp(av[1], "here_doc", 8) == 0) */
  /* { */
    //creer un fichier temp / le supprimer avec unlink
    //infile devient ce fichier temporaire
     //on attend qu'il ait finit de parler, puis ont suit la meme procedure
    //av[2] : le delimiter : tout decaller du coup !
    //bien mettre en append la derniere redir
  /* } */
  if (ft_strncmp(av[1], "here_doc", 8) == 0)
  {
		here_doc(av);
  	add_first_node(data, av[3], env, av[1]);
  	i = 4;
  }
  else
	{
		add_first_node(data, av[2], env, av[1]);
		i = 3;
	}
	while (av[i + 1])
	{
		add_node(data, av[i], env, av[i + 1]);
		i++;
	}
	return (0);
}

char *get_last_arg(char **av)
{
	int i;

	i = 0;
	while (av[i])
		i++;
	return (av[i - 1]);
}
