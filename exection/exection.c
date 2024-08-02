/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exection.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 15:42:06 by yozainan          #+#    #+#             */
/*   Updated: 2024/08/02 19:02:01 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	handle_sigint(int sig)
{
	(void)sig;
	ft_putstr_fd("\n", STDOUT_FILENO);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	g_data.exit_status = 130;
}

t_data	*get_global_data(void)
{
	g_data.exit_status = 0;
	return (&g_data);
}

int	ft_strlnode(Command *cmd)
{
	int	i;

	i = 0;
	while (cmd)
	{
		i++;
		cmd = cmd->next;
	}
	return (i);
}

void	init_execution(t_data *data, int *status)
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

void	execution(t_data *data)
{
	int	status;

	status = 0;
	fill_cmd(data);
	open_check_redirections(data);
	data->is_pipeline = (data->size_cmds > 1);
	init_execution(data, &status);
	wating_processes(data, &status);
}
