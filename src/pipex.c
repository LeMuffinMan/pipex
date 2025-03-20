/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 16:29:17 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/08 15:28:27 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

// mettre un max fd dans l'include ?
// dup2 et close a proteger ?

#include "libft.h"
#include "pipex.h"
#include <errno.h>    // errno
#include <fcntl.h>    // open
#include <stdio.h>    // perror
#include <stdlib.h>   // exit
#include <string.h>   // strerror
#include <sys/wait.h> // wait, waitpid
#include <unistd.h>   // fork, pipe, dup2, execve, access, close, read, write

// gerer les arguments des commandes
// gerer la stdout de la 1ere commande : pas dans le fd 0, utiliser dup2
// D'ABORD : on verifie si file1 exist et est accessible
// Puis, on le prend comme fd / stdin
// on execute la commande avec ce fd
// on redirige la stdout de cmd1 vers stdin de cmd2
// on verifie si file2 exists et est accessible, on le touch si il existe pas ?
// on execute cmd2 avec en stdin la stdout de cmd1,
/* et on la stdout de cmd2 est redirigee vers file2 */
// on close tous les fd
// on attend les children

/// access(const char *pathname, int mode)
///- F_OK : file exists
///- R_OK : readable
///- W_OK : writable
///- X_OK : executable
///
/// errno : variable globale qui stock le dernier code d'erreur de l'appel
/// systeme echoue
/// strerror a la place ?
///
/// REVOIR LIBFT COmpile et virer le header en trop

// utiliser putstrfd

int	fork_management(t_data **data, t_data **tmp, char **av)
{
	while (*tmp)
	{
		if ((*tmp) && (*tmp)->next)
			get_pipe(tmp, data);
		(*tmp)->pid = fork();
		if ((*tmp)->pid == -1)
			print_errors(data, "fork: ", -1, -1);
		if ((*tmp)->pid == 0)
			parse_redirect_execute(data, tmp, av);
		else
		{
			if ((*tmp) && (*tmp)->fd[1] > 2)
				close((*tmp)->fd[1]);
			if ((*tmp)->prev && (*tmp)->prev->fd[0] > 2)
				close((*tmp)->prev->fd[0]);
		}
		if (!(*tmp)->next)
			close((*tmp)->fd[0]);
		*tmp = (*tmp)->next;
	}
	exit(wait_children(data));
}

int	main(int ac, char **av, char **env)
{
	t_data	*data;
	t_data	*tmp;

	data = NULL;
	if (ac == 5)
	{
		init_data(&data, av, env);
		tmp = data;
		fork_management(&data, &tmp, av);
	}
	else
		ft_putstr_fd("Usage : ./pipex infile cmd1 cmd2 outfile", 2);
	return (0);
}

// si on supprime que la ligne PATH ?
// env -i / unset PATH ?
//
// tout proteger
// 	- verifier si on free tout en sortant

/// sleep 5 : verfier que tout fonctionne en mm temps ( sleep 5 | sleep 5 )
/// infile cat | cat | ls outfile
////bin/ls comme cmd
/// proteger is infile ou outfile est /dev/urandom
/// here_doc : limiter = EOF et pas EOFa
///
///

/* int i = 0; */
/* printf("cmd1->binary = %s\n", data->binary); */
/* while (data->args[i]) */
/* { */
/*   printf("cmd1->args = %s\n", data->args[i]); */
/*   i++; */
/* } */
/* printf("cmd2->binary = %s\n", data->next->binary); */
/* i = 0; */
/* while (data->next->args[i]) */
/* { */
/*   printf("cmd2->args = %s\n", data->next->args[i]); */
/*   i++; */
/* } */
