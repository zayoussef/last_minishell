/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exection.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 15:42:06 by yozainan          #+#    #+#             */
/*   Updated: 2024/08/09 14:21:55 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void init_execution(t_data *data, int *status)
{
    if (data->size_cmds == 1)
    {
        if (data->cmd->redir_errors == 0)
            singel_cmd(data, status);
    }
    else
    {
        while (data->cmd != NULL)
        {
            multiple_cmd(data, status);
            data->cmd = data->cmd->next;
        }
    }
}

void handle_invalid_redirection(t_data *data, Command *cmd,
                                Redirection *redir)
{
    if (!ft_strcmp(redir->filename, ""))
    {
        ft_putstr_fd("minishell: ", 2);
        ft_putstr_fd(": No such file or directory handle_invalid_redirection \n", 2);
        data->exit_status = 1;
        cmd->redir_errors = 1;
        cmd->dup = 1;
    }
}

void check_invalid_redirections(t_data *data)
{
    Command *current_cmd;
    Redirection *redir;

    current_cmd = data->cmd;
    while (current_cmd != NULL)
    {
        if (current_cmd->type == TOKEN_AMBIGUOUS)
        {
            ft_putstr_fd("minishell: ambiguous redirect\n", 2);
            data->exit_status = 1;
            data->cmd->redir_errors = 1;
            data->cmd->dup = 1;
            return ;
        }
        if (current_cmd->type == TOKEN_HASHTAG)
        {
            data->exit_status = 1;
            data->cmd->dup = 1;
            return;
        }
        redir = current_cmd->redirection;
        while (redir != NULL)
        {
            handle_invalid_redirection(data, current_cmd, redir);
            if (current_cmd->dup == 1)
                break;
            redir = redir->next;
        }
        current_cmd = current_cmd->next;
    }
}

void handle_all_heredocs(t_data *data)
{
    Command *current_cmd;
    Redirection *redir;

    current_cmd = data->cmd;
    while (current_cmd)
    {
        redir = current_cmd->heredoc;
        while (redir)
        {
            handle_heredoc(data, redir, current_cmd);
            redir = redir->next;
        }
        current_cmd = current_cmd->next;
    }
}

void execution(t_data *data)
{
    int status;

    status = 0;
    fill_cmd(data);
    print_command_structure(data->cmd);
    handle_all_heredocs(data);
    if (data->her_errors)
        return ;
    check_invalid_redirections(data);
    if (data->cmd->dup != 1)
        open_check_redirections(data);
    data->is_pipeline = (data->size_cmds > 1);
    if (!data->cmd)
        return ;
    init_execution(data, &status);
    wating_processes(data, &status);
}
