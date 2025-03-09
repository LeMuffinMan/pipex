/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 15:23:06 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/08 15:27:56 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"
#include "libft.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

char	*get_path_line(char **envp)
{
	char	*path_line;
	int		i;

	i = 0;
	path_line = NULL;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0 && ft_strlen(envp[i]) > 5)
		{
			path_line = envp[i] + 5;
			break ;
		}
		i++;
	}
	return (path_line);
}

char	*join_full_path(char *binary, char *cmd, char *path)
{
	int	i;
	int	j;

	binary = malloc(sizeof(char) * ft_strlen(path) + ft_strlen(cmd) + 2);
	if (!binary)
		return (NULL);
	i = 0;
	while (path[i])
	{
		binary[i] = path[i];
		i++;
	}
	binary[i++] = '/';
	j = 0;
	while (cmd[j])
		binary[i++] = cmd[j++];
	binary[i] = '\0';
	return (binary);
}

char	*which_cmd(char **paths, char *cmd)
{
	char	*binary;
	int		i;

	binary = NULL;
	i = 0;
	while (paths[i])
	{
		binary = join_full_path(binary, cmd, paths[i]);
		if (access(binary, X_OK) == 0)
			return (binary);
		free(binary);
		i++;
	}
	binary = NULL;
	return (binary);
}

char	**get_paths(char **envp)
{
	char	*path_line;
	char	**paths;

	paths = NULL;
	path_line = get_path_line(envp);
	if (!path_line)
	{
		write(2, "path_line not found\n", 20);
		exit (1);
	}
	paths = ft_split(path_line, ':');
	return (paths);
}

char	*get_binary(char *cmd, char **envp)
{
	char	**paths;
	char	**args;
	char	*binary;

	paths = get_paths(envp);
	if (!paths)
	{
		perror("get paths");
		return (NULL);
	}
	args = ft_split(cmd, ' ');
	if (!args)
	{
		free_array(paths);
		perror("args");
		return (NULL);
	}
	binary = which_cmd(paths, args[0]);
	free_array(paths);
	free_array(args);
	return (binary);
}
