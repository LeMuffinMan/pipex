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

// mettre un max fd dans l'include ?
// dup2 et close a proteger ?

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

void	free_array(char **s)
{
	int	i;

	i = 0;
	while (s[i])
	{
		free(s[i]);
		s[i] = NULL;
		i++;
	}
	free(s);
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

  binary = malloc(sizeof(char) * ft_strlen(path) + ft_strlen(cmd) + 2);
  if (!binary)
      return (NULL); //A gerer autrement pour differencier malloc error et path null
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

char *which_cmd(char **paths, char *cmd)
{
  char *binary;
  int i;

  binary = NULL;
  i = 0;
  while (paths[i])
  {
    binary = join_full_path(binary, cmd, paths[i]);
    /* printf("binary = %s\n", binary); */
    if (access(binary, X_OK) == 0)
      return (binary);
    free(binary);
    i++;
  }
  binary = NULL;
  return (binary);
}

char **get_paths(char **envp)
{
  char *path_line;
  char **paths = NULL;

  path_line = get_path_line(envp);
  if (!*path_line) // si on vide le PATH , estce que le 6eme bite de la ligne PATH= est null ?
  {
    perror("path_line error");
    return (NULL);
  }
  /* //bien renvoyer null si ca merde */
  paths = ft_split(path_line, ':');
  return (paths);
}

char *get_binary(char *cmd, char **envp)
{
  char **paths;
  char **args;
  char *binary;

  paths = get_paths(envp);
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

void free_cmd(t_cmd cmd)
{
  if (cmd.binary)
    free(cmd.binary);
  if (cmd.args)
    free_array(cmd.args);
  cmd.binary = NULL;
  cmd.args = NULL;
}

int add_cmds(char **av, char **envp, t_cmd *cmd1, t_cmd *cmd2)
{
  //gerer l'erreur de malloc : on quite tout !
  //differencier d'un path inexistant : on retourne NULL et on continue dans ce cas 
  if (is_a_path(av[2]))
    cmd1->binary = av[2];
  else
    cmd1->binary = get_binary(av[2], envp);
  cmd1->args = ft_split(av[2], ' ');
  if (is_a_path(av[2]))
    cmd2->binary = av[2];
  else
    cmd2->binary = get_binary(av[3], envp);
  cmd2->args = ft_split(av[3], ' ');
  return (0); // gerer le return (1) ?
}

void fct_error(t_cmd cmd)
{
  perror("");
  free_cmd(cmd);
  exit(1);
}

int open_error(char *file, int fd, t_cmd cmd)
{
  write(2, "pipex: ", 7);
  write(2, file, ft_strlen(file));
  write(2, ": ", 2);
  perror("");
  close (fd);
  free_cmd(cmd);
  exit (1);
}

int dup_and_close(int fd_to_close, int fd_to_use, int file, char mode)
{
  //revoir les fonctions d'erreur pour tout free et tout close 
    if (close (fd_to_close) == -1)
      fct_error(cmd);
    if (mode == 'r') 
	    file = open(file, O_RDONLY);
	  else if (mode == 'w')
	    file = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	  if (infile == -1)
	    open_error(file, fd_to_use, cmd);
    if (dup2(fd_to_use, STDOUT_FILENO) == -1)
      fct_error(cmd);
	  if (dup2(file, STDIN_FILENO) == -1)
	    fct_error(cmd);
    if (close (fd_to_use) == -1)
      fct_error(cmd);
    if (close (infile) == -1)
      fct_error(cmd);
}

int execute(t_cmd cmd)
{
  if (!cmd.binary || access(cmd.binary, F_OK) != 0)
  {
    write(2, "pipex: ", 7);
    write(2, "command not found: ", 19);
    write(2, cmd.args[0], ft_strlen(cmd.args[0]));
    write(2, "\n", 1);
    free_cmd(cmd);
    exit (126);
  }
  if (access(cmd.binary, X_OK) != 0)
  {
    write(2, "pipex: ", 7);
    write(2, "permission denied: ", 19);
    perror("");
    free_cmd(cmd);
    exit (127);
  }
  if (execve(cmd.binary, cmd.args, envp) != 0)
  {
    free_cmd(cmd);
    perror("execve error");
    exit (1);
  }
}

/* a renommer ? */
int from_input_to_output(char *file, int fd[2], t_cmd cmd, char **envp, int order)
{
  int infile;
  int outfile;
  //order = 0 : 1er pipe // order = 2 : pipe du milieu // order = 3 : pipe de fin
  if (order == 0)
  {
    dup_and_close(fd[0], fd[1], infile, 'r');
    execute(cmd);
  }
  else if (order == 1)
  {
    //bonus
  }
  else if (order == 2)
  {
    dup_and_close(fd[1], fd[0], outfile, 'w');
    execute(cmd);
  }
}

int fork_error(int fd[2], t_cmd cmd1, t_cmd cmd2)
{
  free_cmd(cmd1);
  free_cmd(cmd2);
  close (fd[1]);
  close (fd[0]);
  exit(1);
}

int init(int ac, char **av, char **envp, int fd[2])
{
  if (ac != 5 || !*envp)
    exit (1);
  if (pipe(fd) == -1)
  {
    perror("pipe fct failed");
    exit (1);
  }
}

int exit_end(t_cmd cmd1, t_cmd cmd2, int fd[2])
{
  free_cmd(cmd1);
  free_cmd(cmd2);
  close (fd[1]);
  close (fd[0]);
  exit (0);
}

int main (int ac, char **av, char **envp)
{
  int fd[2];
  int pid;
  int status;
  t_cmd cmd1;
  t_cmd cmd2;

  init(ac, av, envp, fd);
  add_cmds(av, envp, &cmd1, &cmd2);
  pid = fork();
  if (pid == -1)
    fork_error(fd, cmd1, cmd2);
  if (pid == 0)
  {
    free_cmd(cmd2);
    from_input_to_output(av[1], fd, cmd1, envp, 0);
  }
  pid = fork();
  if (pid == -1)
  {
    waitpid(pid, &status, 0);
    fork_error(fd, cmd1, cmd2);
  }
  if (pid == 0)
  {
    free_cmd(cmd1);
    from_pipe_to_file(av[4], fd, cmd2, envp, 2);
  }
  waitpid(pid, &status, 0);
  exit_end(cmd1, cmd2, fd);
}

///sleep 5 : verfier que tout fonctionne en mm temps ( sleep 5 | sleep 5 )
///infile cat | cat | ls outfile
////bin/ls comme cmd
///proteger is infile ou outfile est /dev/urandom
///here_doc : limiter = EOF et pas EOFa
///
///

  /* int i = 0; */
  /* printf("cmd1.binary = %s\n", cmd1.binary); */
  /* while (cmd1.args[i]) */
  /* { */
  /*   printf("cmd1.args = %s\n", cmd1.args[i]); */
  /*   i++; */
  /* } */
  /* printf("cmd2.binary = %s\n", cmd2.binary); */
  /* i = 0; */
  /* while (cmd2.args[i]) */
  /* { */
  /*   printf("cmd2.args = %s\n", cmd2.args[i]); */
  /*   i++; */
  /* } */

/* int from_file_to_pipe(char *file, int fd[2], t_cmd cmd, char **envp) */
/* { */
/*   int infile; */
/**/
/*   if (close (fd[0]) == -1) */
/*     fct_error(cmd); */
/* 	infile = open(file, O_RDONLY); */
/* 	if (infile == -1) */
/*   { */
/*     write(2, "pipex: ", 7); */
/*     write(2, file, ft_strlen(file)); */
/*     write(2, ": ", 2); */
/*     perror(""); */
/*     close (fd[1]); */
/*     free_cmd(cmd); */
/*     exit (1); */
/*   } */
/*   if (dup2(fd[1], STDOUT_FILENO) == -1) */
/*     fct_error(cmd); */
/* 	if (dup2(infile, STDIN_FILENO) == -1) */
/* 	  fct_error(cmd); */
/*   if (close (fd[1]) == -1) // j'en ai plus besoin ? */
/*     fct_error(cmd); */
/*   if (close (infile) == -1) */
/*     fct_error(cmd); */
/*   if (!cmd.binary || access(cmd.binary, F_OK) != 0) */
/*   { */
/*     write(2, "pipex: ", 7); */
/*     write(2, "command not found: ", 19); */
/*     write(2, cmd.args[0], ft_strlen(cmd.args[0])); */
/*     write(2, "\n", 1); */
/*     free_cmd(cmd); */
/*     exit (126); */
/*   } */
/*   if (access(cmd.binary, X_OK) != 0) */
/*   { */
/*     write(2, "pipex: ", 7); */
/*     write(2, "permission denied: ", 19); */
/*     perror(""); */
/*     free_cmd(cmd); */
/*     exit (127); */
/*   } */
/*   if (execve(cmd.binary, cmd.args, envp) != 0) */
/*   { */
/*     free_cmd(cmd); */
/*     perror("execve error"); */
/*     exit (1); */
/*   } */
/*   exit (0); */
/* } */
/**/
/* int from_pipe_to_file(char *file, int fd[2], t_cmd cmd, char **envp) */
/* { */
/*   int outfile; */
/**/
/*   close (fd[1]); */
/*   //si il existe pas faut le creer ! */
/*   //et si ya pas de permissions ? */
/* 	outfile = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644); */
/* 	if (outfile == -1) */
/*   { */
/*     write(2, "pipex: ", 7); */
/*     write(2, file, ft_strlen(file)); */
/*     write(2, ": ", 2); */
/*     perror(""); */
/*     close (fd[0]); */
/*     free_cmd(cmd); */
/*     exit (1); */
/*   } */
/* 	dup2(fd[0], STDIN_FILENO); */
/* 	dup2(outfile, STDOUT_FILENO); */
/*   close (fd[0]); */
/*   close (outfile); */
/*   if (!cmd.binary || access(cmd.binary, F_OK) != 0) */
/*   { */
/*     write(2, "pipex: ", 7); */
/*     write(2, "command not found: ", 19); */
/*     write(2, cmd.args[0], ft_strlen(cmd.args[0])); */
/*     write(2, "\n", 1); */
/*     free_cmd(cmd); */
/*     exit (126); */
/*   } */
/*   if (access(cmd.binary, X_OK) != 0) */
/*   { */
/*     write(2, "pipex: ", 7); */
/*     write(2, "permission denied: ", 19); */
/*     perror(""); */
/*     free_cmd(cmd); */
/*     exit (127); */
/*   } */
/*   if (execve(cmd.binary, cmd.args, envp) != 0) */
/*   { */
/*     free_cmd(cmd); */
/*     perror("execve error"); */
/*     exit (1); */
/*   } */
/*   exit (0); */
/* } */

