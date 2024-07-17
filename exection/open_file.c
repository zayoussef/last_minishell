/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open_file.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 17:20:21 by elchakir          #+#    #+#             */
/*   Updated: 2024/07/17 17:56:42 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void redirection_in_out(t_data *data, Command *cmd)
{
    Redirection *redir;

    redir = cmd->input;
    while (redir != NULL)
    {
        //close
        cmd->fdin = open(redir->filename, O_RDONLY);
        if (cmd->fdin == -1)
        {
            ft_putstr_fd("minishell: ", 2);
            ft_putstr_fd(redir->filename, 2);
            perror(" ");
            data->exit_status = 1;
            data->redir_erros = -1;
            return ;
        }
        redir = redir->next;
    }
    redir = cmd->output;
    while (redir != NULL)
    {
        cmd->fdout = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (cmd->fdout == -1)
        {
            ft_putstr_fd("minishell: ", 2);
            ft_putstr_fd(redir->filename, 2);
            perror(" ");
            data->exit_status = 1;
            data->redir_erros = -1;
            return ;
        }
        redir = redir->next;
    }
    redir = cmd->append_output;
    while (redir != NULL)
    {
        cmd->fdout = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (cmd->fdout == -1)
        {
            ft_putstr_fd("minishell: ", 2);
            ft_putstr_fd(redir->filename, 2);
            perror(" ");
            data->exit_status = 1;
            data->redir_erros = -1;
            return ;
        }
        redir = redir->next;
    }
    return ;
}

void open_check_redirections(t_data *data)
{
    Command *current_cmd;

    current_cmd = data->cmd;
    while (current_cmd != NULL)
    {
        redirection_in_out(data, current_cmd);
        current_cmd = current_cmd->next;
    }
}

