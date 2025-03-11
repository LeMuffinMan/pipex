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

int parse_redirect_execute(char *av, int fd[2], char **envp, pid_t pid)
{
	char *path;
	char **args;

	path = NULL;
	args = ft_split(av, ' ');
	if (!*args)
		error_no_cmd();
	if (is_a_path(args[0]))
		path = args[0];
	else
	{
		path = get_binary(args[0], envp);
		if (!path)
			error_no_cmd();
	}
	execute(path, args, envp);
	return (0);
}

int	main(int ac, char **av, char **envp)
{
	int		fd[2];
	pid_t pid;
	int i;

	i = 2;
	while (av[i + 2])
	{
  	if (pipe(fd) == -1) {
      	perror("pipe");
      	exit(EXIT_FAILURE);
  	}
  	i++;
  	pid = fork();
  	if (pid == -1)
  		close_and_quit(fd, errno); //revoir
  	if (pid == 0)
  	{
  		if (i == 2)
  		{
  			fd[0] = open(av[1], O_RDONLY);
  			if (fd[0] == -1)
  				open_error(fd[0], av[1])
  			//close ici ?
  			if (dup2(fd[1], STDOUT_FILENO) == -1)
  			 exit(errno); // a proteger mieux ?
  		}
  		else if (i == ac - 1)
  		{
  			fd[1] = open(av[ac - 1],  O_WRONLY | O_CREAT | O_TRUNC, 0644);
  			if (fd[1] == -1)
  				open_error(fd[0], av[1])
  			if (dup2(fd[0], STDIN_FILENO) == -1)
  			 exit (errno);
  		}
  		else
  		{
				if (dup2(fd[1], STDOUT_FILENO) == -1)
					exit(errno);
				if (dup2(fd[0], STDIN_FILENO) == -1)
					exit(errno);
  		}
  		parse_redirect_execute(av[i], fd, envp, pid);
  	}
  	else 
  	{
  		close (fd[1]);
  		close (fd[0]);
  	}
  }
	i = 2;
	while (i < ac - 1)
	{
		wait(NULL);
		i++
	}
	return (0);
 }
	
// si on supprime que la ligne PATH ?
// env -i / unset PATH ?
//
// tout proteger e
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
