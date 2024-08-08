/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open_file.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 17:20:21 by elchakir          #+#    #+#             */
/*   Updated: 2024/08/08 23:08:33 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	add_redirection(Redirection **redir_list, Redirection *redir)
{
	Redirection	*temp;

	temp = *redir_list;
	if (temp == NULL)
		*redir_list = redir;
	else
	{
		while (temp->next)
			temp = temp->next;
		temp->next = redir;
	}
}

void	open_file(t_data *data, Command *cmd, Redirection *redir)
{
	int	fd;

	fd = -1;
	if (!ft_strcmp(redir->filename, "\"\"") || !ft_strcmp(redir->filename,
			"\'\'"))
		return ;
	if (redir->type == TOKEN_REDIRECT_IN)
		fd = open(redir->filename, O_RDONLY);
	else if (redir->type == TOKEN_REDIRECT_OUT)
		fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (redir->type == TOKEN_APPEND_OUT)
		fd = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(redir->filename, 2);
		perror(" ");
		data->exit_status = 1;
		cmd->redir_errors = -1;
		return ;
	}
	if (redir->type == TOKEN_REDIRECT_IN)
		cmd->fdin = fd;
	else
		cmd->fdout = fd;
}

void	handle_other_redirects(t_data *data, Command *cmd, Redirection *redir)
{
	if (redir->type != TOKEN_HERE_DOC && redir->type != TOKEN_HERE_DOC_NO)
	{
		open_file(data, cmd, redir);
		if (cmd->redir_errors == -1)
			return ;
	}
}

void	redirection_in_out(t_data *data, Command *cmd)
{
	Redirection	*redir;

	redir = cmd->redirection;
	while (redir != NULL)
	{
		if (redir->type == TOKEN_REDIRECT_IN)
		{
			handle_redirect_in(data, cmd, redir);
			if (cmd->redir_errors == -1)
				return ;
		}
		check_permissions(data, cmd, redir->type);
		if (cmd->redir_errors == -1)
			return ;
		handle_other_redirects(data, cmd, redir);
		if (cmd->redir_errors == -1)
			return ;
		redir = redir->next;
	}
}

void	open_check_redirections(t_data *data)
{
	Command	*current_cmd;

	current_cmd = data->cmd;
	while (current_cmd != NULL)
	{
		redirection_in_out(data, current_cmd);
		current_cmd = current_cmd->next;
	}
}
