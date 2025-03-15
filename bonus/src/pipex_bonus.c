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
#include "pipex_bonus.h"
#include <stdlib.h>   // exit
#include <unistd.h>

/// access(const char *pathname, int mode)
///- F_OK : file exists
///- R_OK : readable
///- W_OK : writable
///- X_OK : executable
///
/// errno : variable globale qui stock le dernier code d'erreur de l'appel
/// systeme echoue
/// strerror a la place ?
///
/// REVOIR LIBFT COmpile et virer le header en trop

// TODO
// Faire le here doc 
// 		pas oublier de changer l'option append !!
// 		revoir les options des open
// pour toute erreur : verifier si je free tout data
// verifier les protection des fonctions qui foirent 
// verifier les segfault 
// mettre un max fd dans l'include ?

/// proteger is infile ou outfile est /dev/urandom
/// here_doc : limiter = EOF et pas EOFa
////bin/ls comme cmd
// si on supprime que la ligne PATH ?
// env -i / unset PATH ?
//seg fault si cmd 3 est ""
//!! un fd qui reste open pour 3 cmds !!
//!! pour 4 cmds : bad file descriptor
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
        get_pipe(&tmp);
      tmp->pid = fork();
      if (tmp->pid == -1)
          close_pipeline_free_exit(&data);
      if (tmp->pid == 0)
          parse_redirect_execute(&data, &tmp, av);
      else
      {
        	if (tmp && tmp->fd[1] > 2)
        		close(tmp->fd[1]);
        	if (tmp->prev && tmp->fd[0] > 2)
            close(tmp->prev->fd[0]);
      }
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

