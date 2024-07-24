/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open_file.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 17:20:21 by elchakir          #+#    #+#             */
/*   Updated: 2024/07/24 12:59:12 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void open_file(t_data **data, Command *cmd, TokenType type)
{
    int fd;

    fd = -1;
    if (type == TOKEN_REDIRECT_IN)
        fd = open(cmd->redirection->filename, O_RDONLY);
    if (type == TOKEN_REDIRECT_OUT)
        fd = open(cmd->redirection->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (type == TOKEN_APPEND_OUT)
        fd = open(cmd->redirection->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        ft_putstr_fd("minishell: ", 2);
        ft_putstr_fd(cmd->redirection->filename, 2);
        perror(" ");
        (*data)->exit_status = 1;
        (*data)->redir_erros = -1;
        return ;
    }
    else
    {
        if (type == TOKEN_REDIRECT_IN)
            cmd->fdin = fd;
        else
            cmd->fdout = fd;
    }
}

void check_permissions(t_data **data, Command *cmd, TokenType type)
{
    int access_result;

    if (type == TOKEN_REDIRECT_IN)
        access_result = access(cmd->redirection->filename, R_OK);
    else
        access_result = access(cmd->redirection->filename, W_OK);
    if (access_result == -1)
    {
        ft_putstr_fd("minishell: ", 2);
        ft_putstr_fd(cmd->redirection->filename, 2);
        ft_putstr_fd(": Permission denied\n", 2);
        (*data)->exit_status = 1;
        (*data)->redir_erros = -1;
        return ;
    }
}

void redirection_in_out(t_data **data, Command *cmd)
{
    Redirection *redir = cmd->redirection;

    while (redir != NULL)
    {
        if (access(redir->filename, F_OK) == -1 && redir->type == TOKEN_REDIRECT_IN)
        {
            ft_putstr_fd("minishell: ", 2);
            ft_putstr_fd(redir->filename, 2);
            ft_putstr_fd(": No such file or directory\n", 2);
            (*data)->exit_status = 1;
            (*data)->redir_erros = -1;
            return ;
        }
        else if (access(redir->filename, F_OK) != -1)
        {
            check_permissions(data, cmd, redir->type);
            if ((*data)->redir_erros == -1)
                return ;
        }
        open_file(data, cmd, redir->type);
        if ((*data)->redir_erros == -1)
            return ;
        redir = redir->next;
    }
}

void open_check_redirections(t_data **data)
{
    Command *current_cmd;

    current_cmd = (*data)->cmd;
    while (current_cmd != NULL)
    {
        current_cmd->fdin = 0;
        current_cmd->fdout = 1;
        (*data)->redir_erros = 0;
        redirection_in_out(data, current_cmd);
        if ((*data)->redir_erros == -1)
            return ;
        current_cmd = current_cmd->next;
    }
}
