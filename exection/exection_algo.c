/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exection_algo.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 15:42:06 by yozainan          #+#    #+#             */
/*   Updated: 2024/08/08 22:50:25 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	handle_child_process(t_data *data, int pipe_fd[2])
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	setup_input(data);
	setup_output(data, pipe_fd);
	if (data->cmd->dup == 1)
	{
		data->cmd->dup = 0;
		exit(1);
	}
	if (check_is_builtin(*data))
		execute_builtin(data);
	else
		run_execution(data);
	exit(data->exit_status);
}

void	singel_cmd(t_data *data, int *status)
{
	if (check_is_builtin(*data) == 1)
		run_builtin(data, status);
	else
	{
		data->pid = fork();
		if (data->pid == -1)
		{
			perror("fork");
			*status = 1;
			return ;
		}
		else if (data->pid == 0)
		{
			signal(SIGINT, SIG_DFL);
			signal(SIGQUIT, SIG_DFL);
			if (data->cmd->fdin > 2)
				ft_dup_in(data);
			if (data->cmd->fdout != STDOUT_FILENO)
				ft_dup_out(data, 0);
			run_execution(data);
			exit(EXIT_SUCCESS);
		}
	}
}

void	multiple_cmd(t_data *data, int *status)
{
	int	pipe_fd[2];

	pipe_fd[0] = -1;
	pipe_fd[1] = -1;
	if (data->cmd->next && pipe(pipe_fd) == -1)
	{
		perror("pipe");
		*status = 1;
		return ;
	}
	data->pid = fork();
	if (data->pid == 0)
		handle_child_process(data, pipe_fd);
	else
	{
		if (pipe_fd[1] >= 3)
			close(pipe_fd[1]);
		if (data->fd[0] >= 3)
			close(data->fd[0]);
		data->fd[0] = pipe_fd[0];
	}
}
