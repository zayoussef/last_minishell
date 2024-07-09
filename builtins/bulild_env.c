/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bulild_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 15:42:06 by yozainan          #+#    #+#             */
/*   Updated: 2024/07/09 09:13:47 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void build_env(t_data *data)
{
    t_env_node  *curr;
   
    curr  = data->env_list;
    if (data->ac > 0)
    {
        while (curr)
        {
            printf("%s=%s\n", curr->name, curr->value);
            curr = curr->next;
        }
    }
    else
    {
        printf("minishell: env: Too many arguments.\n");
        g_data.exit_status = EXIT_FAILURE;
    }
}
