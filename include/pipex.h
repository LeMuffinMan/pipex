/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 16:29:17 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/01 16:29:17 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

typedef struct s_data
{
	char			*file;
	int fd_input;  // pour dup & close : NULL si stdin
	int fd_output; // pour dup & close : NULL si stdout
	char **paths;  // extract paths : si no paths : exit
	char *binary;  // le chemin du binary
	char **args;   // la commande et ses args
	struct s_data	*next;
}					t_data;

#endif
