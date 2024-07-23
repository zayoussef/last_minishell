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
        lex(line, tokens, &nb_token,data->env_list);
        if (check_syntaxe(tokens, nb_token))
        {
            free(line);
            continue ;
        }
        cmd = parse(tokens);
        if (cmd)
        {
            data->cmd = cmd;
            data->ac = ft_size(cmd->argv);
            data->av = cmd->argv;
            data->size_cmds = ft_strlnode(data->cmd);
            execution(data);
            free(line);
            free_all_resources(cmd);
        }
        // exit_status = data->exit_status;
        // free_env_list(data->env_list);
        // free(data);
    }
    return (data->exit_status);
}
