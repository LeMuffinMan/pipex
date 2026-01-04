/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd_management_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 16:01:38 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/20 15:09:53 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "pipex_bonus.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int open_dup_close_input_redirection(t_data **data, t_data **tmp, char **av) {
  int file;

  if (ft_strncmp(av[1], "here_doc", 8) == 0)
    file = open("/tmp/here_doc", O_RDONLY);
  else
    file = open(av[1], O_RDONLY);
  if (file < 0)
    print_errors(data, av[1], -1, (*tmp)->fd[1]);
  if (dup2(file, STDIN_FILENO) == -1)
    print_errors(data, "dup2: ", -1, file);
  (*tmp)->fd[0] = file;
  close(file);
  if (dup2((*tmp)->fd[1], STDOUT_FILENO) == -1)
    print_errors(data, "dup2: ", -1, (*tmp)->fd[1]);
  close((*tmp)->fd[1]);
  if ((*tmp)->next)
    close((*tmp)->next->fd[0]);
  return (0);
}

int open_dup_close_output_redirection(t_data **data, t_data **tmp, char **av) {
  int file;

  file = open_outfile(get_last_arg(av), av[1]);
  if (file < 0) {
    if (access(get_last_arg(av), W_OK) != 0)
      print_errors(data, get_last_arg(av), 1, (*tmp)->fd[0]);
    print_errors(data, get_last_arg(av), errno, (*tmp)->fd[0]);
  }
  if (dup2(file, STDOUT_FILENO) == -1)
    print_errors(data, "dup2: ", -1, file);
  (*tmp)->fd[1] = file;
  close(file);
  if (ft_strncmp(av[1], "here_doc", 8) == 0)
    unlink_here_doc(data);
  if (dup2((*tmp)->fd[0], STDIN_FILENO) == -1)
    print_errors(data, "dup2: ", -1, (*tmp)->fd[0]);
  close((*tmp)->fd[0]);
  close((*tmp)->fd[1]);
  return (0);
}

int open_dup_close_pipe_to_pipe(t_data **data, t_data **tmp) {
  if (dup2((*tmp)->fd[1], STDOUT_FILENO) == -1)
    print_errors(data, "dup2: ", -1, (*tmp)->fd[1]);
  if (dup2((*tmp)->fd[0], STDIN_FILENO) == -1)
    print_errors(data, "dup2: ", -1, (*tmp)->fd[0]);
  close((*tmp)->fd[1]);
  if ((*tmp)->next)
    close((*tmp)->next->fd[0]);
  return (0);
}

int redirect_stdin_stdout(t_data **tmp, t_data **data, char **av) {
  if (!(*tmp)->prev && (*tmp)->next)
    open_dup_close_input_redirection(data, tmp, av);
  else if ((*tmp)->prev && !(*tmp)->next)
    open_dup_close_output_redirection(data, tmp, av);
  else
    open_dup_close_pipe_to_pipe(data, tmp);
  return (0);
}
