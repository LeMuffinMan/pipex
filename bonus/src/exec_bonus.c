/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:24:42 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/15 17:00:28 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "pipex_bonus.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int close_pipeline_free_exit(t_data **data)
{
	t_data *tmp;

	tmp = *data;
	while (tmp)
	{
		if (close(tmp->fd[0]) == -1 || close(tmp->fd[1]) == -1)
		{
			free_data(data);
			perror("close"); // a mettre partout 
			exit(1); 
		}
		tmp = tmp->next;
	}
	free_data(data);
	return (0);
}

//revoir la doc !
int wait_children(t_data **data)
{
	int status;
	int exit_code;
	t_data *tmp;

	tmp = *data;
	exit_code = EXIT_SUCCESS;
	while (tmp->next)
	{
		if (waitpid(tmp->pid, &status, 0) == -1)
			close_pipeline_free_exit(data);
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

//gerer si on me donne PATH et pas d'env
//free toute la liste !
//il faut lui filer data pour qu'il puisse la free !
int	execute(t_strs *strs, t_data **data)
{
	/* printf("executing cmd : %s\n", (*data)->cmd); */
	if (!strs->path || access(strs->path, F_OK) != 0)
		error_cmd_not_found(data, NULL, NULL);
	if (access(strs->path, X_OK) != 0)
		error_permission_denied(data, strs);
	dprintf(2, "tmp->cmd = %s\n", (*data)->cmd);
	dprintf(2, "tmp->fd[0]: %d\n", (*data)->fd[0]);
	dprintf(2, "tmp->fd[1]: %d\n", (*data)->fd[1]);
	if (execve(strs->path, strs->args, (*data)->env) != 0)
	{
		free_array(strs->args);
		free(strs->path);
		close_pipeline_free_exit(data);
		perror("execve error");
		exit(errno);
	}
	return (0);
}

//voir tous les tests chiants et securiser 
//il faut free toute la liste !!
int parse_redirect_execute(t_data **data, t_data **tmp, char **av)
{
	t_strs strs;

	//cas 1 : ls
	//cas 2 : ls -l
	//cas 3 : /usr/bin/ls
	//cas 4 : /usr/bin/ls -l
	//cas 5 : no env et /usr/bin/ls
	//cas 5 : no PATH et /usr/bin/ls
	//cas 5 : PATH empty et /usr/bin/ls
	strs.path = NULL;
	strs.args = ft_split((*tmp)->cmd, ' ');
	if (!strs.args[0]) // empeche un segfautl pour une cmd "" ?
		error_cmd_not_found(data, tmp, &strs); //ajouter strs pour tout free
	redirect_stdin_stdout(tmp, data, &strs, av); //en cas d'erreur args a free !
	if (is_a_path(strs.args[0]))
		strs.path = strs.args[0];
	else if ((*data)->env) // voir les cas possibles ici
	{
		strs.path = get_binary(strs.args[0], (*tmp)->env);
		if (!strs.path) // avec ou sans * ? 
			error_cmd_not_found(data, tmp, &strs);
	}
	else
		error_cmd_not_found(data, tmp, &strs);
	/* dprintf(2, "tmp->cmd = %s\n", (*tmp)->cmd); */
	/* dprintf(2, "tmp->fd[0]: %d\n", (*tmp)->fd[0]); */
	/* dprintf(2, "tmp->fd[1]: %d\n", (*tmp)->fd[1]); */
	execute(&strs, tmp);
	exit(0);
}

