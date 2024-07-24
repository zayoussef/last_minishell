/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_cd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 15:03:14 by yozainan          #+#    #+#             */
/*   Updated: 2024/07/24 18:48:34 by yozainan         ###   ########.fr       */
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

void go_home(t_data *data)
{
    char *path;
    int i;

    if (!validation_home(data->env_list))
    {
        ft_putstr_fd("minishell: cd : HOME not set\n", 2);
        data->exit_status = EXIT_FAILURE;
    }
    else
    {
        if (data->exit_status == EXIT_FAILURE)
            return;
        else
        {
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
    char *current_pwd;
    char *new_cwd;
    char cwd[PATH_MAX];

    current_pwd = getcwd(cwd, sizeof(cwd));
    if (chdir(path) == 0)
    {
        update_env_var(&data->env_list, "OLDPWD", current_pwd);
        new_cwd = getcwd(NULL, 0);
        update_env_var(&data->env_list, "PWD", new_cwd);
        free(new_cwd);
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
        return;
    }
    if (!data->av[1] || !ft_strcmp(data->av[1], "--") || !ft_strcmp(data->av[1], "~"))
        go_home(data);
    else if (!ft_strcmp(data->av[1], "-"))
        swap_pwd_oldpwd(data);
    else
        change_directory(data, data->av[1]);
}
