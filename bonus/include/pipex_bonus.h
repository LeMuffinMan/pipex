/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 16:29:17 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/20 15:20:46 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_BONUS_H
# define PIPEX_BONUS_H

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
int					wait_children(t_data **data);
int					execute(t_strs *strs, t_data **data);
void				parse_redirect_execute(t_data **data, t_data **tmp,
						char **av);

// errors.c
void				print_errors(t_data **data, char *message, int error_code,
						int fd_to_close);
void				error_cmd_not_found(t_data **data, t_strs *strs,
						t_data **tmp);
void				malloc_error(t_data **data);

// init.c
int					add_first_node(t_data **data, char *cmd, char **env,
						char *infile);
int					add_node(t_data **data, char *cmd, char **env,
						char *last_arg);
int					init_data(t_data **data, char **av, char **env);

// fd_management.c
int					open_dup_close_input_redirection(t_data **data,
						t_data **tmp, char **av);
int					open_dup_close_output_redirection(t_data **data,
						t_data **tmp, char **av);
int					open_dup_close_pipe_to_pipe(t_data **data, t_data **tmp);
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
int					free_data(t_data **data);
char				*get_last_arg(char **av);

// here_doc_utils.c
int					open_outfile(char *file_name, char *here_doc);
int					unlink_here_doc(t_data **data);



#endif
