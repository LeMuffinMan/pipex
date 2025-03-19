/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:24:42 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/19 16:24:13 by oelleaum         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "pipex_bonus.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

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

//gerer si on me donne PATH et pas d'env
//free toute la liste !
//il faut lui filer data pour qu'il puisse la free !
int	execute(t_strs *strs, t_data **data)
{
	int exit_code;
	/* printf("executing cmd : %s\n", (*data)->cmd); */
	exit_code = access(strs->path, F_OK);
	if (!strs->path || exit_code != 0)
		print_errors(data, NULL, "command not found: ", 127);
	exit_code = access(strs->path, X_OK);
	if (exit_code != 0)
		print_errors(data, strs, "permission denied: ", 126);
	/* dprintf(2, "tmp->cmd = %s\n", (*data)->cmd); */
	/* dprintf(2, "tmp->fd[0]: %d\n", (*data)->fd[0]); */
	/* dprintf(2, "tmp->fd[1]: %d\n", (*data)->fd[1]); */
	exit_code = execve(strs->path, strs->args, (*data)->env);
	if (exit_code != 0)
		print_errors(data, strs, "execve: ", 0);
	return (0);
}

//voir tous les tests chiants et securiser 
//il faut free toute la liste !!
void parse_redirect_execute(t_data **data, t_data **tmp, char **av)
{
	t_strs strs;

	//cas 1 : ls 
		//cmd 1 ok 
		//cmd 2 ok
	//cas 2 : ls -l
		//cmd 1 ok 
		//cmd 2 ok 
	//cas 3 : /usr/bin/ls
		//cmd 1 ok 
		//cmd 2 ok 
	//cas 4 : /usr/bin/ls -l
		//cmd 1 ok 
		//cmd 2 ok 
	//cas 5 : env -i : path line not found ou cmd not found et on s'arrete ?
	//cas 5 : no env et /usr/bin/ls
		//cmd1 cmd2 ok 
	//cas 5 : no PATH et /usr/bin/ls
	//cas 5 : PATH empty et /usr/bin/ls
	strs.path = NULL;
	if ((*tmp)->cmd)
	{
		strs.args = ft_split((*tmp)->cmd, ' ');
		if (!strs.args)
			malloc_error(data);	
		if (!strs.args[0])	
			free_array(strs.args);
	/* if (!strs.args[0]) // empeche un segfautl pour une cmd "" ? */
		/* strs.args = NULL; */
	}
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
	execute(&strs, data);
}

