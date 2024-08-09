/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_her_doc.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 17:20:21 by elchakir          #+#    #+#             */
/*   Updated: 2024/08/08 23:17:27 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	handle_heredoc_error(int pipe_fd[2], char *line)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(HER_DOC, 2);
	close(pipe_fd[1]);
	free(line);
	exit(0);
}

void	process_heredoc_line(char *line, Redirection *redir, t_data *data,
		int pipe_fd[2])
{
	char	*expanded_line;

	if (redir->type == TOKEN_HERE_DOC)
	{
		find_when_start(line, data, &expanded_line);
		write(pipe_fd[1], expanded_line, strlen(expanded_line));
		write(pipe_fd[1], "\n", 1);
		free(expanded_line);
	}
	else
	{
		write(pipe_fd[1], line, strlen(line));
		write(pipe_fd[1], "\n", 1);
	}
}

void	handle_child_process_her_doc(int pipe_fd[2], Redirection *redir,
		t_data *data)
{
	char	*line;

	close(pipe_fd[0]);
	signal(SIGINT, herd_sig);
	remove_quotes(redir->filename);
	while (1)
	{
		line = readline("> ");
		if (!line)
			handle_heredoc_error(pipe_fd, line);
		if (ft_strcmp(line, redir->filename) == 0)
		{
			free(line);
			close(pipe_fd[1]);
			exit(0);
		}
		process_heredoc_line(line, redir, data, pipe_fd);
		free(line);
	}
}

void	handle_parent_process(t_data *data, int pipe_fd[2])
{
	int	status;

	close(pipe_fd[1]);
	data->cmd->fdin = pipe_fd[0];
	waitpid(data->pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
	{
		data->exit_status = 130;
		data->her_errors = 1;
	}
}

void	handle_heredoc(t_data *data, Redirection *redir)
{
	int	pipe_fd[2];

	if (data->her_errors)
		return ;
	if (pipe(pipe_fd) == -1)
	{
		perror("pipe");
		data->cmd->redir_errors = -1;
		return ;
	}
	check_limit_her_doc(redir);
	data->pid = fork();
	if (data->pid == -1)
	{
		perror("fork");
		data->cmd->redir_errors = -1;
		return ;
	}
	if (data->pid == 0)
		handle_child_process_her_doc(pipe_fd, redir, data);
	else
		handle_parent_process(data, pipe_fd);
}
