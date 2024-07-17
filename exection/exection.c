/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exection.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 15:42:06 by yozainan          #+#    #+#             */
/*   Updated: 2024/07/17 11:25:15 by yozainan         ###   ########.fr       */
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
    g_data.exit_signal = 0;
    g_data.redir_erros = 0;
    return &g_data;
}

int ft_strlnode(Command *cmd)
{
    int i = 0;

    while (cmd)
    {
        i++;
        cmd = cmd->next;
    }
    return i;
}

void wating_processes(t_data *data, int *status)
{
    int i;

    i = 0;
    if (waitpid(data->pid, status, 0) == -1)
        return ;
    while (++i < data->size_cmds)
        wait(NULL);
    if (WIFEXITED(*status))
        data->exit_status = WEXITSTATUS(*status);
    else if (WIFSIGNALED(*status))
    {
        if (WTERMSIG(*status) == SIGQUIT)
            ft_putstr_fd("Quit (core dumped)\n", 2);
        else if (WTERMSIG(*status) == SIGINT)
            ft_putstr_fd("\n", 2);
        data->exit_status = 128 + (*status);
    }
}

void init_execution(t_data *data, int *status)
{
    if (data->size_cmds == 1)
        singel_cmd(data, status); // done from where 1 command all test done !
    else
    {
        first_cmd(data, status);
        data->cmd = data->cmd->next;
        while (data->cmd->next != NULL)
        {
            middel_cmd(data, status);
            data->cmd = data->cmd->next;
        }
        last_cmd(data, status);
    }
}

void execution(t_data *data)
{
    int status;

    status  = 0;
    open_check_redirections(data);
    if (data->redir_erros == -1)
        return ;
    init_execution(data, &status);
    wating_processes(data, &status);
}

