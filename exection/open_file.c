/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open_file.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 17:20:21 by elchakir          #+#    #+#             */
/*   Updated: 2024/07/13 18:36:32 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void redirection_in_out(t_data *data)
{
    Redirection *redir;

    redir = data->cmd->input;
    while (redir != NULL)
    {
        //close
        data->cmd->fdin = open(redir->filename, O_RDONLY);
        if (data->cmd->fdin == -1)
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
    redir = data->cmd->output;
    while (redir != NULL)
    {
        data->cmd->fdout = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (data->cmd->fdout == -1)
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
    redir = data->cmd->append_output;
    while (redir != NULL)
    {
        data->cmd->fdout = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (data->cmd->fdout == -1)
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
        redirection_in_out(data);
        current_cmd = current_cmd->next;
    }
}

// int handle_redirection_and_errors(t_data *data)
// {
//     int fd;

//     fd = STDOUT_FILENO;
//     if (data->cmd->input && (fd = data->cmd->input->fd_in) == -1)
//         data->redir_erros = -1;
//     if (data->cmd->output && (fd = data->cmd->output->fd_out) == -1)
//         data->redir_erros = -1;
//     if (data->cmd->append_output && (fd = data->cmd->append_output->fd_out) == -1)
//         data->redir_erros = -1;
//     return fd;
// }
