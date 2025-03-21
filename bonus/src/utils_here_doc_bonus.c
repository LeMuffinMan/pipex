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

int	generate_key(char *key, int key_length)
{
	int	random_fd;

	random_fd = open("/dev/urandom", O_RDONLY);
	if (random_fd == -1)
	{
		perror("open");
		exit(1);
	}
	if (read(random_fd, key, key_length) != key_length)
	{
		perror("read");
		close(random_fd);
		exit(1);
	}
	close(random_fd);
	return (0);
}

int	encrypt(char *message, char *key, int key_length, char mode)
{
	size_t	i;

	i = 0;
	while (i < ft_strlen(message))
	{
		if (mode == 'e')
			message[i] = (message[i] + key[i % key_length]) % 256;
		else if (mode == 'd')
			message[i] = (message[i] - key[i % key_length] + 256) % 256;
		i++;
	}
	return (0);
}

int	decrypt(char *message, char *key, int key_length)
{
	size_t	i;

	i = 0;
	while (i < ft_strlen(message))
	{
		message[i] = (message[i] - key[i % key_length] + 256) % 256;
		i++;
	}
	return (0);
}
