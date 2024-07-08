/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open_file.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 17:20:21 by elchakir          #+#    #+#             */
/*   Updated: 2024/07/08 16:52:27 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void redirection_in_out(Command *cmd)
{
    while(cmd->input != NULL) 
    {
        cmd->input->fd = open(cmd->input->filename, O_RDONLY);
        if (cmd->input->fd == -1)
        {
            perror("Failed to open input file");
            break;
        }
        cmd->input = cmd->input->next;
    }
    while(cmd->output != NULL) 
    {
        cmd->output->fd = open(cmd->output->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (cmd->output->fd == -1)
        {
            perror("Failed to open output file");
            break;
        }
        cmd->output = cmd->output->next;
    }
}

void open_redirections(Command *cmd) 
{
    while(cmd != NULL)
    {
        redirection_in_out(cmd);
        while(cmd->append_output != NULL) 
        {
            cmd->append_output->fd = open(cmd->append_output->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (cmd->append_output->fd == -1)
        {
            perror("Failed to open append output file");
            break;
        }
        cmd->append_output = cmd->append_output->next;
        }
        cmd = cmd->next;
        }
}
