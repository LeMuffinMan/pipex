/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:18:50 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/15 17:41:56 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"
#include <unistd.h>
#include "libft.h"
#include <stdlib.h>
#include <stdio.h>

int error_permission_denied(t_data **data, t_strs *strs)
{
	ft_putstr_fd("pipex: permission denied: ", 2);
	perror("");
	free_array(strs->args);
	free(strs->path);
	close((*tmp)->fd[0]);
	close((*tmp)->prev->fd[0]);
	free_data(data);
	return (0);
}

//code d'erreur ?
int error_cmd_not_found(t_data **data, t_data **tmp, t_strs *strs)
{
	ft_putstr_fd("pipex: command not found: ", 2);
	if ((*tmp)->cmd)
		ft_putstr_fd((*tmp)->cmd, 2); // a verifier ! donne la bonne cmd en avancant le ptr ?
	ft_putstr_fd("\n", 2); // PAS SUUUUUR !
	if (strs->args)
		free_array(strs->args);
	if (strs->path)
		free(strs->path);
	close((*tmp)->fd[0]);
	close((*tmp)->prev->fd[0]);
	free_data(data);
	exit(127);
}

int open_error(t_data **data, t_strs *strs)
{
	ft_putstr_fd("pipex: open error: ", 2);
	perror("");
	free_array(strs->args);
	free(strs->path);
	close((*tmp)->fd[0]);
	close((*tmp)->prev->fd[0]);
	free_data(data);
	exit(1); 
}
