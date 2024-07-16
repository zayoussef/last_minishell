/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_cd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 15:03:14 by yozainan          #+#    #+#             */
/*   Updated: 2024/07/16 09:56:51 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int validation_home(t_env_node *env)
{
    while (env)
    {
        if (!ft_strncmp(env->name, "HOME", 4))
        {
            if (env->name[4] == '=' || env->name[4] == '\0')
                return (1);
        }
        env = env->next;
    }
    return (0);
}

void    go_home(t_data *data)
{
    char    *path;
    int     i;

    dprintf(2, "hello \n");
    if (!validation_home(data->env_list))
    {
        ft_putstr_fd("minishell: cd : HOME not set\n",  2);
        data->exit_status = EXIT_FAILURE;
        dprintf(2, "hello 1\n");
    }
    else
    {
        if (data->exit_status == EXIT_FAILURE)
            return ;
        else
        {
            dprintf(2, "hello 2\n");
            set_oldpwd(data->env_list);
            path = ft_getenv(data->env_list, "HOME");
            i = chdir(path);
            if (i)
                printf("minishell: cd: %s: No such file or directory\n", path);
            set_pwd(data->env_list);
        }
    }
}

void change_directory(t_data *data, const char *path) 
{
    char    *current_pwd;
    char    cwd[PATH_MAX];

    current_pwd = ft_getenv(data->env_list, "PWD");
    if (chdir(path) == 0)
    {
        update_env_var(&data->env_list, "OLDPWD", current_pwd);
        if (getcwd(cwd, sizeof(cwd)))
            update_env_var(&data->env_list, "PWD", cwd);
    } 
    else
    {
        printf("minishell: cd: %s: No such file or directory\n", path);
        data->exit_status = EXIT_FAILURE;
    }
}

void build_cd(t_data *data)
{
    if (data->ac > 2)
    {
        ft_putstr_fd("minishell: cd: too many arguments\n", 2);
        data->exit_status = EXIT_FAILURE;
        return ;
    }
    if (!data->av[1] || !ft_strcmp(data->av[1], "--") 
        || !ft_strcmp(data->av[1], "~"))
        go_home(data);
    else if (!ft_strcmp(data->av[1], "-"))
        swap_pwd_oldpwd(data);
    else
        change_directory(data, data->av[1]);

}
