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
	char			*file;
	int				fd[2];
	char			*cmd;
	pid_t			pid;
	char			**env;
	struct s_data	*next;
	struct s_data	*prev;
}					t_data;

typedef struct s_strs
{
	char			**args;
	char			*path;
	char			*av;
}					t_strs;

// a ranger :

// exec.c
int					execute(t_strs *strs, t_data **data);
int					wait_children(t_data **data);
int					close_pipeline_free_exit(t_data **data);
void				parse_redirect_execute(t_data **data, t_data **tmp,
						char **av);

// errors.c
void				error_permission_denied(t_data **data);
void				error_cmd_not_found(t_data **data, t_data **tmp);
void				open_error(t_data **data, char *file);
void				print_errors(t_data **data, char *message, int error_code);
void				malloc_error(t_data **data);

// init.c
int					free_data(t_data **data);
int					add_first_node(t_data **data, char *cmd, char **env,
						char *infile);
int					add_node(t_data **data, char *cmd, char **env,
						char *last_arg);
int					init_data(t_data **data, char **av, char **env);
char				*get_last_arg(char **av);

// fd_management.c
int					get_pipe(t_data **node, t_data **data);
int					open_dup_close_pipe_to_pipe(t_data **data, t_data **tmp);
int					open_dup_close_output_redirection(t_data **data,
						t_data **tmp, char **av);
int					open_dup_close_input_redirection(t_data **data,
						t_data **tmp, char **av);
int					redirect_stdin_stdout(t_data **tmp, t_data **data,
						char **av);

// parsing.c
char				*get_binary(char *cmd, char **envp);
char				**get_paths(char **envp);
char				*which_cmd(char **paths, char *cmd);
char				*join_full_path(char *binary, char *cmd, char *path);
char				*get_path_line(char **envp);

// utils.c
void				free_array(char **s);
int					is_a_path(char *s);
int					close_pipe_free_exit(t_data **data, t_strs *strs,
						int exit_code);

#endif
