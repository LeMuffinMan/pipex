/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 16:29:17 by oelleaum          #+#    #+#             */
/*   Updated: 2025/03/15 17:27:33 by oelleaum         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */


#include "libft.h"
#include "pipex.h"
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

/// REVOIR LIBFT COmpile et virer le header en trop

/// proteger is infile ou outfile est /dev/urandom
int main(int ac, char **av, char **env)
{
  t_data *data;
  t_data *tmp;

	data = NULL;
  if (ac >= 5)
  {
    init_data(&data, av, env);    
    tmp = data;
    while(tmp)
    {
      if (tmp && tmp->next)
        get_pipe(&tmp, &data);
      tmp->pid = fork();
      if (tmp->pid == -1)
        print_errors(&data, NULL, "fork: ", -1); // voir si errno marche partout comme on veut 
      if (tmp->pid == 0)
          parse_redirect_execute(&data, &tmp, av);
      else
      {
        if (tmp && tmp->fd[1] > 2)
        	close(tmp->fd[1]);
        if (tmp->prev && tmp->prev->fd[0] > 2)
          close(tmp->prev->fd[0]);
      }
      if (!tmp->next)
        close(tmp->fd[0]);
      tmp = tmp->next;
    }
    exit(wait_children(&data)); 
  }
  else 
  {
    ft_putstr_fd("Usage : ./pipex infile cmd1 cmd2 outfile\n", 1);
    free_data(&data);
    exit(1);
  }
}

//le tip coralie pour PATH ?

//checker toutes les leaks des cas chiants 
//Pas de leak sur infile cat ls outfile 
//pipex: infile: pipex: command not found: lgs==43201== Warning: invalid file descriptor -1 in syscall close()

/* oelleaum@z2r5p6:~/GitPerso/pipex$ ./pipex_bonus infile "echo 'hello'" "sudo apt update" outfile */
/* new pipe : in = 4 out = 3 */
/* cmd1 : echo 'hello' will write in fd 4 */
/* cmd2 : sudo apt update wait input from fd 3 */
/**/
/* pipex: open error: Bad file descriptor */
/* pipex: command not found: sudo apt update */
/* oelleaum@z2r5p6:~/GitPerso/pipex$ echo $? */
/* 139 */



