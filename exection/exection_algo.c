/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exection_algo.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 15:42:06 by yozainan          #+#    #+#             */
/*   Updated: 2024/07/27 17:26:14 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void singel_cmd(t_data *data, int *status)
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
            if (data->cmd->fdin > 2)
            {
                if (dup2(data->cmd->fdin, STDIN_FILENO) == -1)
                {
                    perror("dup2 single_cmd fdin");
                    exit(EXIT_FAILURE);
                }
                close(data->cmd->fdin);
            }
            if (data->cmd->fdout != STDOUT_FILENO)
            {
                if (dup2(data->cmd->fdout, STDOUT_FILENO) == -1)
                {
                    perror("dup2 single_cmd fdout");
                    exit(EXIT_FAILURE);
                }
                close(data->cmd->fdout);
            }
            run_execution(data);
            exit(EXIT_SUCCESS);
        }
        else
            wating_processes(data, status);
    }
}

// void first_cmd(t_data **data, int *status)
// {
//     int pipe_fd[2];

//     if (pipe(pipe_fd) == -1)
//     {
//         perror("pipe");
//         *status = 1;
//         return;
//     }
//     (*data)->pid = fork();
//     if ((*data)->pid == -1)
//     {
//         perror("fork");
//         close(pipe_fd[0]);
//         close(pipe_fd[1]);
//         *status = 1;
//         return;
//     }
//     else if ((*data)->pid == 0)
//     {
//         if ((*data)->cmd->fdout > 2)
//         {
//             if (dup2((*data)->cmd->fdout, STDOUT_FILENO) == -1)
//             {
//                 perror("dup2 first_command fdout");
//                 exit(EXIT_FAILURE);
//             }
//             close((*data)->cmd->fdout);
//         }
//         else
//         {
//             if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
//             {
//                 perror("dup2 first_command pipe_fd[1]");
//                 exit(EXIT_FAILURE);
//             }
//             close(pipe_fd[1]);
//         }
//         close(pipe_fd[0]);
//         if (check_is_builtin(*(*data)) == 1)
//         {
//             execute_builtin((*data));
//             exit((*data)->exit_status);
//         }
//         else
//             run_execution(*data);
//     }
//     else
//     {
//         close(pipe_fd[1]);
//         (*data)->fd[0] = pipe_fd[0];
//     }
// }

// int middel_cmd(t_data **data, int *status)
// {
//     int pipe_fd[2];

//     if (pipe(pipe_fd) == -1)
//     {
//         perror("pipe");
//         *status = 1;
//         return -1;
//     }
//     (*data)->pid = fork();
//     if ((*data)->pid == -1)
//     {
//         perror("fork");
//         close(pipe_fd[0]);
//         close(pipe_fd[1]);
//         *status = 1;
//         return -1;
//     }
//     else if ((*data)->pid == 0)
//     {
//         if ((*data)->cmd->fdin > 2)
//         {
//             if (dup2((*data)->cmd->fdin, STDIN_FILENO) == -1)
//             {
//                 perror("dup2 middel_command fdin");
//                 exit(EXIT_FAILURE);
//             }
//             close((*data)->cmd->fdin);
//         }
//         else
//         {
//             if (dup2((*data)->fd[0], STDIN_FILENO) == -1)
//             {
//                 perror("dup2 middel_command data->fd[0]");
//                 exit(EXIT_FAILURE);
//             }
//             close((*data)->fd[0]);
//         }
//         if ((*data)->cmd->fdout > 2)
//         {
//             if (dup2((*data)->cmd->fdout, STDOUT_FILENO) == -1)
//             {
//                 perror("dup2 middel_command fdout");
//                 exit(EXIT_FAILURE);
//             }
//             close((*data)->cmd->fdout);
//         }
//         else
//         {
//             if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
//             {
//                 perror("dup2 middel_command pipe_fd[1]");
//                 exit(EXIT_FAILURE);
//             }
//             close(pipe_fd[1]);
//         }
//         close(pipe_fd[0]);
//         if (check_is_builtin(*(*data)) == 1)
//         {
//             execute_builtin((*data));
//             exit((*data)->exit_status);
//         }
//         else
//             run_execution(*data);
//     }
//     else
//     {
//         close((*data)->fd[0]);
//         close(pipe_fd[1]);
//         (*data)->fd[0] = pipe_fd[0];
//     }
//     return 0;
// }

// int last_cmd(t_data **data, int *status)
// {
//     (*data)->pid = fork();
//     if ((*data)->pid == -1)
//     {
//         perror("fork");
//         *status = 1;
//         return -1;
//     }
//     else if ((*data)->pid == 0)
//     {
//         if ((*data)->cmd->fdin > 2)
//         {
//             if (dup2((*data)->cmd->fdin, STDIN_FILENO) == -1)
//             {
//                 perror("dup2 last_cmd fdin");
//                 exit(EXIT_FAILURE);
//             }
//             close((*data)->cmd->fdin);
//         }
//         else
//         {
//             if (dup2((*data)->fd[0], STDIN_FILENO) == -1)
//             {
//                 perror("dup2 last_cmd data->fd[0]");
//                 exit(EXIT_FAILURE);
//             }
//             close((*data)->fd[0]);
//         }
//         if ((*data)->cmd->fdout > 2)
//         {
//             if (dup2((*data)->cmd->fdout, STDOUT_FILENO) == -1)
//             {
//                 perror("dup2 last_cmd fdout");
//                 exit(EXIT_FAILURE);
//             }
//             close((*data)->cmd->fdout);
//         }
//         if (check_is_builtin(*(*data)) == 1)
//         {
//             execute_builtin((*data));
//             exit((*data)->exit_status);
//         }
//         else
//             run_execution(*data);
//     }
//     else
//     {
//         close((*data)->fd[0]);
//         wating_processes(*data, status);
//     }
//     return 0;
// }


void multiple_cmd(t_data **data, int *status)
{
    int pipe_fd[2];

    // Create a pipe for inter-process communication
    if (pipe(pipe_fd) == -1)
    {
        perror("pipe");
        *status = 1;
        return;
    }

    // Fork the process
    (*data)->pid = fork();
    if ((*data)->pid == -1)
    {
        perror("fork");
        *status = 1;
        return;
    }
    else if ((*data)->pid == 0)
    {
        // Child process

        // Handle input redirection
        if ((*data)->cmd->fdin > 2)
        {
            if (dup2((*data)->cmd->fdin, STDIN_FILENO) == -1)
            {
                perror("dup2 multiple_cmd fdin");
                exit(EXIT_FAILURE);
            }
            close((*data)->cmd->fdin);
        }
        else if ((*data)->fd[0] > 2)
        {
            if (dup2((*data)->fd[0], STDIN_FILENO) == -1)
            {
                perror("dup2 multiple_cmd data->fd[0]");
                exit(EXIT_FAILURE);
            }
            close((*data)->fd[0]);
        }
        // Handle output redirection
        if ((*data)->cmd->fdout > 2)
        {
            if (dup2((*data)->cmd->fdout, STDOUT_FILENO) == -1)
            {
                perror("dup2 multiple_cmd fdout");
                exit(EXIT_FAILURE);
            }
            close((*data)->cmd->fdout);
        }
        else if ((*data)->cmd->next)
        {
            if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
            {
                perror("dup2 multiple_cmd pipe_fd[1]");
                exit(EXIT_FAILURE);
            }
            close(pipe_fd[1]);
        }
        else
        {
            close(pipe_fd[1]);
        }
        close(pipe_fd[0]);
        // Execute the command
        if (check_is_builtin(*(*data)) == 1)
        {
            execute_builtin((*data));
            exit((*data)->exit_status);
        }
        else
        {
            run_execution(*data);
        }
        exit(EXIT_SUCCESS);
    }
    else
    {
        // Parent process
        close(pipe_fd[1]);
        if ((*data)->fd[0] > 2)
        {
            close((*data)->fd[0]);
        }

        // Pass the read end of the pipe to the next command
        (*data)->fd[0] = pipe_fd[0];
    }

    // Wait for processes
    wating_processes((*data), status);
}