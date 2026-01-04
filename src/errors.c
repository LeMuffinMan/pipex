/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:18:50 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/08 13:24:25 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "pipex.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_errors(t_data **data, char *file_name, int error_code,
                  int fd_to_close) {
  char *error_msg;

  error_msg = ft_strjoin("pipex: ", file_name);
  if (fd_to_close > 2)
    close(fd_to_close);
  perror(error_msg);
  free(error_msg);
  close_pipe_free_exit(data, NULL, error_code);
}

void error_cmd_not_found(t_data **data, t_strs *strs, t_data **tmp) {
  char *error_msg;

  error_msg = ft_strjoin("pipex: ", (*tmp)->cmd);
  ft_putstr_fd(error_msg, 2);
  ft_putstr_fd(": command not found\n", 2);
  free(error_msg);
  close_pipe_free_exit(data, strs, 127);
}

void malloc_error(t_data **data) {
  perror("pipex: malloc error");
  close_pipe_free_exit(data, NULL, errno);
}

int close_pipe_free_exit(t_data **data, t_strs *strs, int exit_code) {
  if (strs && strs->args)
    free_array(strs->args);
  if (strs && strs->path)
    free(strs->path);
  if ((*data)->fd[0] && (*data)->fd[0] > 2)
    close((*data)->fd[0]);
  if ((*data)->prev && (*data)->prev->fd[1] && (*data)->prev->fd[1] > 2)
    close((*data)->prev->fd[1]);
  if (*data && (*data)->fd[1] && (*data)->fd[1] > 2)
    close((*data)->fd[1]);
  free_data(data);
  if (exit_code == 0)
    exit(errno);
  else
    exit(exit_code);
  return (0);
}
