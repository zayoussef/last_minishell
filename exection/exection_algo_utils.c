/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exection_algo_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 15:42:06 by yozainan          #+#    #+#             */
/*   Updated: 2024/08/08 22:52:12 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_dup_in(t_data *data)
{
	if (dup2(data->cmd->fdin, STDIN_FILENO) == -1)
	{
		perror("dup2 single_cmd fdin");
		exit(EXIT_FAILURE);
	}
	close(data->cmd->fdin);
}

void	ft_dup_out(t_data *data, int i)
{
	if (i == 1)
	{
		data->cmd->fdin = 0;
		data->cmd->fdout = 1;
	}
	else
	{
		if (dup2(data->cmd->fdout, STDOUT_FILENO) == -1)
		{
			perror("dup2 single_cmd fdout");
			exit(EXIT_FAILURE);
		}
		close(data->cmd->fdout);
	}
}

void	setup_input(t_data *data)
{
	if (data->cmd->fdin >= 3)
		ft_dup_in(data);
	else if (data->fd[0] >= 3)
	{
		dup2(data->fd[0], STDIN_FILENO);
		close(data->fd[0]);
	}
}

void	setup_output(t_data *data, int pipe_fd[2])
{
	if (data->cmd->fdout >= 3)
		ft_dup_out(data, 0);
	else if (data->cmd->next)
		dup2(pipe_fd[1], STDOUT_FILENO);
	if (pipe_fd[1] >= 3)
		close(pipe_fd[1]);
	if (pipe_fd[0] >= 3)
		close(pipe_fd[0]);
	ft_dup_out(data, 1);
}

void	clear_tab(char **tab)
{
	int	i;

	i = 0;
	while (tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
}
