/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_echo.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 15:42:06 by yozainan          #+#    #+#             */
/*   Updated: 2024/07/24 17:45:12 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void print_av(char **av, int n)
{
    int i;

    i = 0;
    while (av[i])
    {
        printf("%s", av[i]);
        if (av[i + 1])
            printf(" ");
        i++;
    }
    if (i == 0 || n)
        printf("\n");
}

int check_n(char *str)
{
    int i;

    i = 0;
    if (!str[i])
        return (1);
    while (str[i])
    {
        if (str[i] != 'n')
            return (1);
        i++;
    }
    return (0);
}

void check_option(char **av)
{
    int i;

    i = 1;
    while (av[i] && av[i][0] == '-' && !check_n(av[i] + 1))
        i++;
    if (!av[i])
        return ;
    if (av[i][1] == '\0')
        printf("-\n");
    print_av(av + i, 0);
}

// void   build_echo(t_data *data)
// {
//     // av++;
//     if (av[1] == NULL)
//         printf("\n");
//     else if (av[1][0] == '-')
//         check_option(av);
//     else
//         print_av(av + 1, 1);
// }


void build_echo(t_data *data)
{
    int i;
    int newline;
    int fd;

    i = 1;
    newline = 1;
    fd = data->cmd->fdout;
    if (fd == -1)
        return ;
    if (data->cmd->argv[i] && ft_strcmp(data->cmd->argv[i], "-n") == 0)
    {
        newline = 0;
        i++;
    }
    while (data->cmd->argv[i])
    {
        ft_putstr_fd(data->cmd->argv[i], fd);
        if (data->cmd->argv[i + 1])
            ft_putstr_fd(" ", fd);
        i++;
    }
    if (newline)
        ft_putstr_fd("\n", fd);
    data->exit_status = 0;
}