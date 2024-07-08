/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_pwd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 15:03:14 by yozainan         #+#    #+#             */
/*   Updated: 2024/05/13 14:46:54 by yozainan        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void build_pwd(t_data *data)
{
    char    pwd[PATH_MAX];
    char    *oldpwd;

    if (getcwd(pwd, PATH_MAX))
    {
        printf("%s\n", pwd);
        data->exit_status = EXIT_SUCCESS;
    }
    else
    {
        oldpwd = ft_getenv(data->env_list, "PWD");
        if (oldpwd)
        {
            printf("%s\n", oldpwd);
            data->exit_status = EXIT_SUCCESS;
        }
        else
        {
            printf("minishell: pwd: error retrieving current directory: getcwd: cannot access parent directories: No such file or directory\n");
            data->exit_status = EXIT_FAILURE;
        }
    }
}
