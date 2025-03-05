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
///
///REVOIR LIBFT COmpile et virer le header en trop


int from_file_to_pipe(char *file, int fd, char *cmd, char **envp)
{
  int infile;

	infile = open(file, O_RDONLY);
	if (infile == -1)
  {
    perror("Open error");
	  exit (1);
  }
	dup2(fd, STDOUT_FILENO);
	dup2(infile, STDIN_FILENO);
  close (fd);
  if (execute(cmd, envp) != 0)
  {
    close (infile);
    perror("execve error");
    exit(1);
  }
  close (infile);
  return (0);
}

int from_pipe_to_file(char *file, int fd, char *cmd, char **envp)
{
  int outfile;

	outfile = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (outfile == -1)
  {
    perror("Open error");
	  exit (1);
  }
	dup2(fd, STDIN_FILENO);
	dup2(outfile, STDOUT_FILENO);
  close (fd);
  if (execute(cmd, envp) != 0)
  {
    close (outfile);
    perror("execve error");
    //Exit ? wait child ?
    exit(1);
  }
  close (outfile);
  return (0);
}

int setup_pipe(int ac, int fd[2])
{
  int pid;

  if (ac != 5)
  {
    ft_printf("Usage : ./pipex file1 cmd1 cmd2 file2\n");
    //mettre en canal d'erreur plutot
    exit (1);
  }
  ///La fonction pipe crée un tube (pipe) qui permet la communication entre deux processus. pipefd est un tableau de deux entiers où pipefd[0] est le descripteur de fichier pour la lecture et pipefd[1] pour l'écriture.
  if (pipe(fd) == -1)
  {
    perror("pipe fct failed");
    exit (1);
  }
  pid = fork();
  if (pid == -1) 
  {
    perror("fork fct failed");
    exit(1);
  }
  return(pid);
}

char *get_path_line(char **envp)
{
  char *path_line;
  int i = 0;

  path_line = NULL;
  while (envp[i]) 
  {
    if (ft_strncmp(envp[i], "PATH=", 5) == 0)
    {
      //pas sur de ca 
      path_line = envp[i] + 5;
      break;
    }
    i++;
  }
//retourne null si pas trouve ?
  return (path_line);
}

char *join_full_path(char *binary, char *cmd, char *path)
{
  int i;
  int j;

  binary = malloc(sizeof(char) * ft_strlen(path[i]) + ft_strlen(cmd) + 2);
  if (!binary)
      return (NULL);
  while (path[i])
      binary[i] = path[i++];
  binary[i++] = '/';
  while (cmd[j])
      binary[i++] = cmd[j++];
  binary[i] = '\0';
  return (binary);
}

char *which_cmd(char **paths, char *cmd)
{
  char *binary;
  int i;

  binary = NULL;
  i = 0;
  while (paths[i])
  {
    if (!join_full_path(binary, cmd, path[i]))
    {
      perror("malloc error joining path");
      return (NULL);
    }
    if (access(binary, X_OK) == 0)
      return (binary);
    i++;
  }
  return (binary);
}

char **get_paths(char **envp)
{
  char *path_line;
  char **paths;

  path_line = get_path_line(envp);
  if (!path_line)
  {
    perror("path_line error");
    return (NULL);
  }
  //bien renvoyer null si ca merde
  paths = ft_split(path_line, ':');
  free(path_line);
  return (paths);
}

char *get_binary(char **envp, char *cmd)
{
  char **paths;
  char **args;
  char *binary;

  paths = get_paths();
  if (!paths)
  {
    perror("get paths error");
    return (NULL);
  }
  args = ft_split(cmd, ' ');
  if (!args)
  {
    free_array(paths);
    perror("args error");
    return (NULL);
  }
  binary = which_cmd(paths, args[0]);
  free_array(paths);
  free_array(args);
  return (binary); 
}

int execute(char *cmd, char **envp)
{
  char *binary;
 
  binary = get_binary(envp, cmd);
  if (!binary)
  {
    perror("cmd_not_found");
    return (1);
  }
  success = execve(binary, args, envp);
  free(binary);
  return (success);
}

int main(int ac, char **av, char **envp)
{
  int fd[2];
  int pid;

  pid = setup_pipe(ac, fd);
  if (pid == 0) // child process
    from_file_to_pipe(av[1], fd[1], av[2], envp);
  else //parent process
    from_pipe_to_file(av[4], fd[0], av[3], envp);
  return (0);
}

