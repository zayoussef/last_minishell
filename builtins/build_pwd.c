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
    int     fd;

    if (data->cmd->next)
        fd = 1;
    else
        fd = data->cmd->fdout;
    if (fd == -1)
        return ;
    if (getcwd(pwd, PATH_MAX))
    {
        ft_putendl_fd(pwd, fd);
        data->exit_status = EXIT_SUCCESS;
    }
    else 
    {
        oldpwd = ft_getenv(data->env_list, "PWD");
        if (oldpwd)
        {
            ft_putendl_fd(oldpwd, fd);
            data->exit_status = EXIT_SUCCESS;
        }
        else 
        {
            ft_putendl_fd("minishell: pwd: error retrieving current directory: getcwd: cannot access parent directories: No such file or directory", fd);
            data->exit_status = EXIT_FAILURE;
        }
    }
}
