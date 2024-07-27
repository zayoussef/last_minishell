/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bulild_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 15:42:06 by yozainan          #+#    #+#             */
/*   Updated: 2024/07/27 17:48:42 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void build_env(t_data *data)
{
    t_env_node  *curr;
    int         fd;

    if (data->cmd->next)
        fd = 1;
    else
        fd = data->cmd->fdout;
    if (fd == -1)
        return ;
    curr = data->env_list;
    if (data->ac == 1)
    {
        while (curr)
        {
            ft_putstr_fd(curr->name, fd);
            ft_putstr_fd("=", fd);
            ft_putstr_fd(curr->value, fd);
            ft_putstr_fd("\n", fd);
            curr = curr->next;
        }
    }
    else
    {
        ft_putstr_fd("env: ‘", fd);
        ft_putstr_fd(data->av[1], fd);
        ft_putstr_fd("’: No such file or directory\n", fd);
        data->exit_status = EXIT_FAILURE;
    }
}
