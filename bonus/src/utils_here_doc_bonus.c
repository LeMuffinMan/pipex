/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_here_doc_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 15:04:30 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/20 15:12:35 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "pipex_bonus.h"
#include <errno.h>
#include <fcntl.h> // open
#include <unistd.h>

int	open_outfile(char *file_name, char *here_doc)
{
	if (ft_strncmp(here_doc, "here_doc", 8) == 0)
		return (open(file_name, O_WRONLY | O_CREAT | O_APPEND, 0644));
	else
		return (open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644));
}

int	unlink_here_doc(t_data **data)
{
	if (unlink("/tmp/here_doc") != 0)
		print_errors(data, "unlink: ", errno, -1);
	return (0);
}
