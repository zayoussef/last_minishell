/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_export.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 18:37:48 by yozainan          #+#    #+#             */
/*   Updated: 2024/06/28 23:19:42 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void sort_env(t_env_node *tmp_env)
{
    t_env_node *tmp;

    tmp = tmp_env;
    if (!tmp_env)
        return ;
    while (tmp->next)
    {
        if (ft_strcmp(tmp->name, tmp->next->name) > 0)
        {
            ft_swap_env(tmp, tmp->next);
            tmp = tmp_env;
        }
        else
            tmp = tmp->next;
    }
}

void print_sorted_env(t_env_node *env)
{
    t_env_node *tmp_env;

    tmp_env = env;
    sort_env(tmp_env);

    while (tmp_env)
    {
        if (ft_strcmp(tmp_env->name, "_") != 0) // Skip printing the "_" variable
        {
            printf("declare -x %s", tmp_env->name);
            if (tmp_env->value)
                printf("=\"%s\"", tmp_env->value);
            printf("\n");
        }
        tmp_env = tmp_env->next;
    }
}

int is_valid_identifier(char *s)
{
    int i;

    i = 1;
    if (!s || (!ft_isalpha(s[0]) && s[0] != '_'))
        return 0;
    while (s[i] && s[i] != '=' && !(s[i] == '+' && s[i + 1] == '='))
    {
        if (!ft_isalnum(s[i]) && s[i] != '_')
            return 0;
        i++;
    }
    return 1;
}

void handle_export(t_env_node **env_list, char *arg)
{
    char *delm_pos = ft_strrch_finder(arg, '=');
    if (delm_pos)
    {
        char *name;
        char *value;
        if (*(delm_pos - 1) == '+')
        {
            name = ft_strndup(arg, delm_pos - arg - 1);
            value = ft_strdup(delm_pos + 1);
           existing_env(env_list, name, value);
        }
        else
        {
            name = ft_strndup(arg, delm_pos - arg);
            value = ft_strdup(delm_pos + 1);
            update_or_add_env(env_list, name, value);
        }
    }
    else
        handle_new_env(env_list, arg);
}

void build_export(t_data *data)
{
    int i = 1;

    if (data->ac == 1)
    {
        print_sorted_env(data->env_list);
        return ;
    }
    while (data->av[i])
    {
        if (!is_valid_identifier(data->av[i]))
        {
            printf("minishell: export: `%s`: not a valid identifier\n", data->av[i]);
            data->exit_status = EXIT_FAILURE;
        }
        else
            handle_export(&(data->env_list), data->av[i]);
        i++;
    }
    data->exit_status = EXIT_SUCCESS;
}
        