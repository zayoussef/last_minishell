/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exection_alogo.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 15:42:06 by yozainan          #+#    #+#             */
/*   Updated: 2024/07/17 18:17:11 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void singel_cmd(t_data *data, int *status)
{
    //output in stdout;
    if (check_is_builtin(*data) == 1)
        run_builtin(data, status);
    else
    {
        data->singel_pid = fork();
        if (data->singel_pid  == -1)
        {
            perror("fork");
            return ;
        }
        else if (data->singel_pid == 0)
        {
            if (data->cmd->fdout != STDOUT_FILENO)
            {
                dup2(data->cmd->fdout, STDOUT_FILENO);
                close(data->cmd->fdout);
            }
            run_execution(data);
        }
        else
            wating_processes(data, status);
    }
}

void first_cmd(t_data *data, int *status)
{
    int pipe_fd[2];

    // Create a pipe for inter-process communication
    if (pipe(pipe_fd) == -1)
    {
        perror("pipe");
        *status = 1;
        return;
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        *status = 1;
        return;
    }
    else if (pid == 0)
    {
        // Child process
        // Redirect STDOUT to the write end of the pipe
        if(data->cmd->fdout > 2)
        {
            close (pipe_fd[1]);
            pipe_fd[1] = data->cmd->fdout;
        }
        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
        {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(pipe_fd[0]);
        close(pipe_fd[1]);

        // Check if the command is a built-in command and execute it
        if (check_is_builtin(*data) == 1)
        {
            execute_builtin(data);
            exit(data->exit_status);  // Ensure to exit with the status set by the built-in command
        }
        else
            run_execution(data);
            // Execute the command if it's not a built-in
    }
    else
    {
        // Parent process
        // Close the write end of the pipe, parent does not use it
        close(pipe_fd[1]);
        // Store the read end of the pipe to pass it to the next command
        data->fd[0] = pipe_fd[0];
    }
}

int middel_cmd(t_data *data, int *status)
{
    int pipe_fd[2];

    if (pipe(pipe_fd) == -1)
    {
        perror("pipe");
        *status = 1;
        return -1;
    }
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        *status = 1;
        return -1;
    }
    else if (pid == 0)
    {
        // Child process

        if(data->cmd->fdin > 2)
        {
            close (data->fd[0]);
            data->fd[0] = data->cmd->fdin;
        }
        if(data->cmd->fdout > 2)
        {
            close (pipe_fd[1]);
            pipe_fd[1] = data->cmd->fdout;
        }
        dprintf(2, "[\tin :%d\t]\n", data->cmd->fdin);
        dprintf(2, "[\tout :%d\t]\n", data->cmd->fdout);   
        if (dup2(data->fd[0], STDIN_FILENO) == -1 || dup2(pipe_fd[1], STDOUT_FILENO) == -1)
        {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        close(data->fd[0]);

        if (check_is_builtin(*data) == 1)
        {
            execute_builtin(data);
            exit(data->exit_status);
        }
        else
        {
            run_execution(data);
        }
    }
    else
    {
        // Parent process
        close(data->fd[0]);
        close(pipe_fd[1]);
        data->cmd->fdout = pipe_fd[0];
        data->fd[0] = pipe_fd[0];
    }
    return 0;
}

int last_cmd(t_data *data, int *status)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        *status = 1;
        return -1;
    }
    else if (pid == 0)
    {
        // Child process
        if(data->cmd->fdin > 2)
        {
            close (data->fd[0]);
            data->fd[0] = data->cmd->fdin;
        }
        if (dup2(data->fd[0], STDIN_FILENO) == -1)
        {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(data->fd[0]);
        if (check_is_builtin(*data) == 1)
        {
            execute_builtin(data);
            exit(data->exit_status);
        }
        else
            run_execution(data);
    }
    else
    {
        // Parent process
        close(data->fd[0]);
    }
    return 0;
}

