/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 16:29:17 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/08 15:28:08 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include <sys/wait.h>

typedef struct s_data
{
	char *file;
	int 			fd_in;
	int 			fd_out;
	char * 		cmd;
	pid_t			pid;
	char			**env;
	struct s_data	*next;
}					t_data;

typedef struct s_strs
{
	char **args;
	char *path;
	char *av;
} t_strs;

// a ranger :
char				*get_binary(char *cmd, char **envp);

// exec.c
// int					wait_children(int fd[2], pid_t pid1, pid_t pid2);
// int					parse_redirect_execute(t_data *data, int fd[2]);
// int					redirect_fd(t_data *data, int fd[2], char *path,
// 						char **args);
// int					execute(char *binary, char **args, char **envp);

// errors.c
// int					open_error(int fd, char *file, char *path, char **args);
// int					error_cmd_not_found(int fd[2], char **args, char *path,
// 						char *binary);
// int					error_permission_denied(char **args, char *binary);

// utils.c
int					dup_and_close(int fd_out, int fd_in, int fd_to_close);
int					is_a_path(char *s);
int close_and_quit(int infile, int outfile, t_data *data);
void				free_array(char **s);

// init.c
int					init(t_data *data, int ac, char **av, int fd[2]);

// parsing.c
char				*get_path_line(char **envp);
char				*join_full_path(char *binary, char *cmd, char *path);
char				*which_cmd(char **paths, char *cmd);
char				**get_paths(char **envp);
char				*get_binary(char *cmd, char **envp);

#endif
