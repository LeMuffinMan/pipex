/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:24:42 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/08 13:32:05 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "pipex.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int	wait_children(t_data **data)
{
	int		status;
	int		exit_code;
	t_data	*tmp;

	tmp = *data;
	exit_code = EXIT_SUCCESS;
	while (tmp)
	{
		waitpid(tmp->pid, &status, 0);
		if (WIFEXITED(status))
			exit_code = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			exit_code = 128 + WTERMSIG(status);
		if (exit_code == EXIT_SUCCESS && WIFEXITED(status))
			exit_code = WEXITSTATUS(status);
		else if (exit_code == EXIT_SUCCESS && WIFSIGNALED(status))
			exit_code = 128 + WTERMSIG(status);
		tmp = tmp->next;
	}
	free_data(data);
	return (exit_code);
}

int	execute(t_strs *strs, t_data **data)
{
	char	*error_msg;

	if (access(strs->path, F_OK != 0) || !strs->path)
		print_errors(data, "command not found: ", 127, -1);
	if (access(strs->path, X_OK != 0))
	{
		error_msg = ft_strjoin("pipex: ", strs->args[0]);
		perror(error_msg);
		free(error_msg);
		free_data(data);
		free(strs->path);
		free(strs->args);
		exit(errno);
	}
	if (execve(strs->path, strs->args, (*data)->env) != 0)
		print_errors(data, "execve: ", 0, -1);
	return (0);
}

void	parse_redirect_execute(t_data **data, t_data **tmp, char **av)
{
	t_strs	strs;

	if ((*tmp)->cmd)
		redirect_stdin_stdout(tmp, data, av);
	strs.path = NULL;
	if ((*tmp)->cmd)
	{
		strs.args = ft_split((*tmp)->cmd, ' ');
		if (!strs.args)
			malloc_error(data);
		if (!strs.args[0])
			error_cmd_not_found(data, &strs, tmp);
	}
	if (is_a_path(strs.args[0]))
		strs.path = strs.args[0];
	else if ((*data)->env)
	{
		strs.path = get_binary(strs.args[0], (*tmp)->env);
		if (!strs.path)
			error_cmd_not_found(data, &strs, tmp);
	}
	else
		error_cmd_not_found(data, &strs, tmp);
	execute(&strs, data);
}
