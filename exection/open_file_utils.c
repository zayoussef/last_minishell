/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open_file_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 17:20:21 by elchakir          #+#    #+#             */
/*   Updated: 2024/08/08 23:21:05 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	check_permissions(t_data *data, Command *cmd, TokenType type)
{
	int	access_result;

	if (type == TOKEN_REDIRECT_IN)
		access_result = access(cmd->redirection->filename, R_OK);
	else
		access_result = access(cmd->redirection->filename, W_OK);
	if (access_result == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd->redirection->filename, 2);
		ft_putstr_fd(": Permission denied\n", 2);
		data->exit_status = 1;
		cmd->redir_errors = -1;
		return ;
	}
}

void	handle_redirect_in(t_data *data, Command *cmd, Redirection *redir)
{
	if (access(redir->filename, F_OK) == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(redir->filename, 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		data->exit_status = 1;
		cmd->redir_errors = -1;
		return ;
	}
}
