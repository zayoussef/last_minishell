/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open_file.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 17:20:21 by elchakir          #+#    #+#             */
/*   Updated: 2024/07/11 10:35:16 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int redirection_in_out(t_data *data) 
{
    Redirection *redir;

    redir = data->cmd->input;
    while (redir != NULL) 
    {
        redir->fd = open(redir->filename, O_RDONLY);
        if (redir->fd == -1)
        {
            ft_putstr_fd("minishell: ", 2);
            ft_putstr_fd(redir->filename, 2);
            perror(" ");
            return -1;
        }
        redir = redir->next;
    }

    redir = data->cmd->output;
    while (redir != NULL) 
    {
        redir->fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (redir->fd == -1) 
        {
            ft_putstr_fd("minishell: ", 2);
            ft_putstr_fd(redir->filename, 2);
            perror(" ");
            return -1;
        }
        redir = redir->next;
    }
    return 0;
}

int open_redirections(t_data *data)
{
    Command *current_cmd;
    Redirection *redir;
    
    current_cmd = data->cmd;
    while (current_cmd != NULL)
    {
        // redirection_in_out(data);
        redir = data->cmd->append_output;
        while (redir != NULL)
        {
            redir->fd = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (redir->fd == -1)
            {
                ft_putstr_fd("minishell: ", 2);
                ft_putstr_fd(redir->filename, 2);
                perror(" ");
                return -1;
            }
            redir = redir->next;
        }

        current_cmd = current_cmd->next;
    }
    return 0;
}
