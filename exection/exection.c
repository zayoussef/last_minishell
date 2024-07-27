/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exection.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 15:42:06 by yozainan          #+#    #+#             */
/*   Updated: 2024/07/27 18:13:46 by yozainan         ###   ########.fr       */
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
    pid_t pid;
    int wstatus;
    int i;

    i = 0;
    waitpid(data->pid, status, 0);
    while (++i < data->size_cmds)
    {
        pid = wait(&wstatus);
        if (pid == -1)
            continue ;
    }
    if (WIFEXITED(*status))
        data->exit_status = WEXITSTATUS(*status);
    else if (WIFSIGNALED(*status))
    {
        if (WTERMSIG(*status) == SIGQUIT)
            ft_putstr_fd("Quit (core dumped)\n", 2);
        else if (WTERMSIG(*status) == SIGINT)
            ft_putstr_fd("\n", 2);
        data->exit_status = 128 + WTERMSIG(*status);
    }
}

void init_execution(t_data *data, int *status)
{
    if (data->size_cmds == 1)
        singel_cmd(data, status);
    else
    {
        while (data->cmd != NULL)
        {
            multiple_cmd(&data, status);
            data->cmd = data->cmd->next;
        }
    }
}

void execution(t_data *data)
{
    int status;

    status = 0;
    data->redir_erros = 0;
    data->cmd->fdin = 0;
    data->cmd->fdout = 1;
    open_check_redirections(&data);
    if (data->redir_erros == -1)
        return ;
    // print_command_structure(data->cmd);
    init_execution(data, &status);
    wating_processes(data, &status);
}
