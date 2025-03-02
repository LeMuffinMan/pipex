/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 16:29:17 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/01 16:29:17 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>   // fork, pipe, dup2, execve, access, close, read, write
#include <stdlib.h>   // exit
#include <stdio.h>    // perror
#include <string.h>   // strerror
#include <fcntl.h>    // open
#include <sys/wait.h> // wait, waitpid
#include <errno.h>    // errno
#include "libft.h"
#include "pipex.h"

//gerer les arguments des commandes
//gerer la stdout de la 1ere commande : pas dans le fd 0, utiliser dup2
//D'ABORD : on verifie si file1 exist et est accessible
//Puis, on le prend comme fd / stdin
//on execute la commande avec ce fd 
//on redirige la stdout de cmd1 vers stdin de cmd2
//on verifie si file2 exists et est accessible, on le touch si il existe pas ?
//on execute cmd2 avec en stdin la stdout de cmd1, et on la stdout de cmd2 est redirigee vers file2
//on close tous les fd
//on attend les children

///access(const char *pathname, int mode)
///- F_OK : file exists
///- R_OK : readable
///- W_OK : writable
///- X_OK : executable
///
///errno : variable globale qui stock le dernier code d'erreur de l'appel systeme echoue
///strerror a la place ?

int main(int ac, char **av)
{
  int fd;
  if (ac != 5)
  {
  //Si av[1] != file || av[2] != cmd1 || av[3] != cmd2 || av[4] != file2
   //error
    ft_printf("Usage : ./pipex file1 cmd1 cmd2 file2\n");
    //mettre en canal d'erreur plutot
    exit (1);
  }
  fd = open(av[1], O_RDONLY); 
  if (fd == -1)
  {
    ft_printf("%s: %s: ", av[0], av[1]);
    perror("");
    exit (1); // ou 0 ?
  }
  /* fd = open(av[1], O_RDONLY); // RDONLY ? */
  /* if (fd < 0) */
  /* { */
  /*   ft_printf("Error while opening %s", av[1]); */
  /*   //perror ? */
  /*   return (-1); */
  /* } */
  /* pipe(fd); // voir comment utiliser   */
  /* if (fork() == 0) // 0 = child */
  /* { */
  /*   //close les fd inutiles ? */
  /*   //split av[2] les args de la commande : path + args */
  /*   if (execve(path, args, envp ?)) //voir les parametres pour execve */
  /*   { */
  /*       */
  /*   } */
  /*   //close tous les fd */
  /* } */
  /* else //parent */
  /* { */
  /*   //close les fd inutiles */
  /*   //split av[3] les args de la commande : path + args */
  /*   if (execve(path, args, envp ?)) //voir les parametres pour execve */
  /*   { */
  /**/
  /*   } */
  /*   wait(); */
  /*   //close tous les fd  */
  /* } */
  return (0);
}
