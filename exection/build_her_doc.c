/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_her_doc.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 17:20:21 by elchakir          #+#    #+#             */
/*   Updated: 2024/08/08 22:26:42 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void remove_quotes(char *str)
{
    int i;
    int j;

    if (!str)
        return;
    i = 0;
    j = 0;
    while (str[i])
    {
        if (str[i] != '\'' && str[i] != '"')
        {
            str[j] = str[i];
            j++;
        }
        i++;
    }
    str[j] = '\0';
}

void expand_variable_in_str(char **temp_result, char **p, t_data *data)
{
    char *start;
    char *expanded;
    char *tmp;

    start = *p;
    if (**p == '$')
    {
        tmp = ft_itoa(data->pid);
        ft_strncpy(*temp_result + ft_strlen(*temp_result), tmp, ft_strlen(tmp));
        (*p)++;
        free(tmp);
    }
    else
    {
        while (ft_isalnum(**p) || **p == '_')
            (*p)++;
        expanded = expand_variable(start, *p - start, data->env_list);
        ft_strncpy(*temp_result + ft_strlen(*temp_result), expanded, ft_strlen(expanded));
        free(expanded);
    }
}

void find_when_start(char *str, t_data *data, char **result)
{
    char *p;
    char *temp_result;

    temp_result = malloc(strlen(str) * 2);
    if (!temp_result)
        return;
    temp_result[0] = '\0';
    while ((p = ft_strrch_finder(str, '$')) != NULL)
    {
        ft_strncpy(temp_result + strlen(temp_result), str, p - str);
        p++;
        expand_variable_in_str(&temp_result, &p, data);
        str = p;
    }
    ft_strncpy(temp_result + ft_strlen(temp_result), str, ft_strlen(str));
    *result = temp_result;
}

void check_limit_her_doc(Redirection *redir)
{
    Redirection *tmp_redir = redir;
    int counter = 0;

    while (tmp_redir)
    {
        if (tmp_redir->type == TOKEN_HERE_DOC || tmp_redir->type == TOKEN_HERE_DOC_NO)
            counter++;
        tmp_redir = tmp_redir->next;
    }
    if (counter > 16)
    {
        ft_putstr_fd("minishell: maximum here-document count exceeded\n", STDERR_FILENO);
        exit(2);
    }
}

void handle_heredoc_error(int pipe_fd[2], char *line)
{
    ft_putstr_fd("minishell: ", 2);
    ft_putstr_fd("warning: here-document delimited by end-of-file (wanted `')\n", 2);
    close(pipe_fd[1]);
    free(line);
    exit(0);
}

void process_heredoc_line(char *line, Redirection *redir, t_data *data, int pipe_fd[2])
{
    char *expanded_line;

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

void handle_child_process_her_doc(int pipe_fd[2], Redirection *redir, t_data *data)
{
    char *line;

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

void handle_parent_process(t_data *data, int pipe_fd[2])
{
    int status;

    close(pipe_fd[1]);
    data->cmd->fdin = pipe_fd[0];
    waitpid(data->pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
    {
        data->exit_status = 130;
        data->her_erros = 1;
    }
}

void handle_heredoc(t_data *data, Redirection *redir)
{
    int pipe_fd[2];

    if (data->her_erros)
        return;
    if (pipe(pipe_fd) == -1)
    {
        perror("pipe");
        data->cmd->redir_erros = -1;
        return;
    }
    check_limit_her_doc(redir);
    data->pid = fork();
    if (data->pid == -1)
    {
        perror("fork");
        data->cmd->redir_erros = -1;
        return;
    }
    if (data->pid == 0)
        handle_child_process_her_doc(pipe_fd, redir, data);
    else
        handle_parent_process(data, pipe_fd);
}
