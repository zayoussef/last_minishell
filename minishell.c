/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 17:31:07 by yozainan          #+#    #+#             */
/*   Updated: 2024/07/04 17:23:42 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_data g_data;

void free_split(char **arr)
{
    int i = 0;
    if (arr != NULL)
    {
        while (arr[i] != NULL)
        {
            free(arr[i]);
            i++;
        }
        free(arr);
    }
}

void print_redirection(Redirection *redir) {
    while (redir) {
        printf("  Redirection: Type=%d, Filename=%s\n", redir->type, redir->filename);
        redir = redir->next;
    }
}

void print_command_structure(Command *cmd) 
{
    while (cmd) {
        // Print command arguments
        printf("Command: ");
        for (int i = 0; cmd->argv && cmd->argv[i]; i++) 
        {
            printf("%s ", cmd->argv[i]);
        }
        printf("\n");

        // Print file descriptors
        printf("File descriptors:\n");
        printf("  fdin: %d\n", cmd->fdin);
        printf("  fdout: %d\n", cmd->fdout);

        // // Print heredoc redirections
        // if (cmd->heredoc) {
        //     printf("Heredoc redirections:\n");
        //     print_redirection(cmd->heredoc);
        // }

        // Print other redirections
        if (cmd->redirection) {
            printf("Other redirections:\n");
            print_redirection(cmd->redirection);
        }

        // Move to the next command
        cmd = cmd->next;
        printf("\n");
    }
}

int check_parse(Redirection *redir)
{
    Redirection *temp;
    temp = redir;
    while(temp)
    {
        if(!ft_strcmp(temp->filename,"?"))
            return 1;
        else if(!ft_strcmp(temp->filename,"#"))
        return(ft_putstr_fd("syntax error near unexpected token `newline'\n",2),1);
        temp = temp->next;
    }
    return 0;
}

int main(int argc, char **argv, char **envp)
{
    Token   tokens[MAX_TOKENS];
    Command *cmd;
    t_data  *data;
    char    *line;
    int     nb_token;

    (void)argc;
    (void)argv;
    data = get_global_data();
    data->env_list = create_env_list(envp);
    while (1)
    {
        signal(SIGINT, handle_sigint);
        signal(SIGQUIT, SIG_IGN);
        line = readline("\033[32mminishell> \033[0m");
        if (!line)
            break;
        if (strlen(line) > 0)
            add_history(line);
        nb_token = 0;
        
        if (lex(line, tokens, &nb_token,data->env_list) ||(check_syntaxe(tokens, nb_token)))
        {
            free(line);
            continue;
        }
        cmd = parse(tokens);
        if(cmd && check_parse(cmd->redirection) == 1)
            {
                free_redirection(cmd->redirection);
                Command *temp;
                if(cmd->next)
                {
                temp = cmd->next;
                free(cmd);
                cmd = temp;
                }
                else
                    continue;
            }
        
        if (cmd)
        {
            data->cmd = cmd;
            data->ac = ft_size(cmd->argv);
            data->av = cmd->argv;
            data->size_cmds = ft_strlnode(data->cmd);
            execution(data);
            // print_command_structure(cmd);
            if(line)
                free(line);
            free_all_resources(cmd);
        }
        // exit_status = data->exit_status;
        // free_env_list(data->env_list);
        // free(data);
    }
    return (data->exit_status);
}
