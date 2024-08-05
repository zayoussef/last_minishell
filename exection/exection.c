/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exection.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 15:42:06 by yozainan          #+#    #+#             */
/*   Updated: 2024/08/05 00:56:57 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void handle_sigint(int sig)
{
    (void)sig;
    ft_putstr_fd("\n", STDOUT_FILENO);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
    g_data.exit_status = 130;
}

t_data *get_global_data(void)
{
    g_data.exit_status = 0;
    return (&g_data);
}

int ft_strlnode(Command *cmd)
{
    int i;

    i = 0;
    while (cmd)
    {
        i++;
        cmd = cmd->next;
    }
    return (i);
}

void init_execution(t_data *data, int *status)
{
    if (data->size_cmds == 1)
        singel_cmd(data, status);
    else
    {
        while (data->cmd != NULL)
        {
            if (data->cmd->redir_erros == -1)
                data->cmd = data->cmd->next;
            multiple_cmd(data, status);
            data->cmd = data->cmd->next;
        }
    }
}

void check_invalid_redirections(t_data *data)
{
    Command *current_cmd;
    Redirection *redir;

    current_cmd = data->cmd;
    while (current_cmd != NULL)
    {
        redir = current_cmd->redirection;
        while (redir != NULL)
        {
            if (!ft_strcmp(redir->filename, "\"\"") || !ft_strcmp(redir->filename, "\'\'"))
            {
                ft_putstr_fd("minishell: ", 2);
                if (current_cmd->argv[0])
                    ft_putstr_fd(current_cmd->argv[0], 2);
                ft_putstr_fd(": No such file or directory\n", 2);
                data->exit_status = 1;
                current_cmd->redir_erros = -1;
                break;
            }
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

    check_invalid_redirections(data);
    // Skip commands with redirection errors
    while (data->cmd && data->cmd->redir_erros == -1)
        data->cmd = data->cmd->next;
    if (!data->cmd)
        return;

    // Open and check redirections
    open_check_redirections(data);

    // Skip commands with redirection errors
    while (data->cmd && data->cmd->redir_erros == -1)
        data->cmd = data->cmd->next;
    if (!data->cmd)
        return;
    data->is_pipeline = (data->size_cmds > 1);
    init_execution(data, &status);
    wating_processes(data, &status);
}
