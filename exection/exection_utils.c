/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exection_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 15:42:06 by yozainan          #+#    #+#             */
/*   Updated: 2024/07/16 15:07:10 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*build_executable_path(char *path, char *cmd)
{
	char	*tmp_path;
	char	*exec_path;

	tmp_path = ft_strjoin(path, "/");
	exec_path = ft_strjoin(tmp_path, cmd);
	free(tmp_path);
	if (access(exec_path, F_OK | X_OK) == 0)
		return (exec_path);
	free(exec_path);
	return (NULL);
}

char	*check_cmd(char *cmd)
{
	if (access(cmd, F_OK | X_OK) == 0)
		return (cmd);
	else if (access(cmd, X_OK) != 0)
	{
		ft_putstr_fd("minishell: permission denied: ", 2); //have test like : minishell> /builtins
		ft_putstr_fd(cmd, 2);
		ft_putchar_fd('\n', 2);
		exit(126);
	}
	else
	{
		ft_putstr_fd("minishell: no such file or directory: ", 2);
		ft_putstr_fd(cmd, 2);
		ft_putchar_fd('\n', 2);
		exit(127);
	}
}

void	clear_tab(char **tab)
{
	int	i;

	i = 0;
	while (tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
}

char    *find_path(char *cmd, char **env)
{
    char    **path;
    char    *exec_path;
    int        i;

    i = 0;
    if (ft_strchr(cmd, '/'))
        return (check_cmd(cmd));
    while (env[i])
    {
        if (ft_strnstr(env[i], "PATH=", 5))
            break ;
        i++;
    }
    if (env[i] == NULL)
        path = ft_split(PATH_ENV, ':');
    else
        path = ft_split(env[i] + 5, ':');
    i = 0;
    while (path[i])
    {
        exec_path = build_executable_path(path[i], cmd);
        if (exec_path != NULL)
            return (clear_tab(path), exec_path);
        i++;
    }
    return (clear_tab(path), NULL);
}

char **list_to_char(t_env_node *env_list)
{
    t_env_node  *cur;
    char        **env_array;
    char        *temp;
    int         count = 0;
    int         i = 0;

    // Count the number of environment variables
    cur = env_list;
    while (cur)
    {
        count++;
        cur = cur->next;
    }
    // Allocate memory for the environment array
    env_array = (char **)malloc(sizeof(char *) * (count + 1));
    if (!env_array)
        return (NULL);
    // Build the environment array
    cur = env_list;
    while (cur)
    {
        temp = ft_strjoin(cur->name, "="); // Create the key=value string
        if (!temp)
        {  // Check if the memory allocation failed
            // Free previously allocated strings and the array
            while (i > 0)
                free(env_array[--i]);
            free(env_array);
            return NULL;
        }
        env_array[i] = ft_strjoin(temp, cur->value);  // Append the value
        free(temp);  // Free the temporary string
        if (!env_array[i])
        {  // Check if the memory allocation failed
            // Free previously allocated strings and the array
            while (i > 0)
                free(env_array[--i]);
            free(env_array);
             return NULL;
        }
        i++;
        cur = cur->next;
    }
    env_array[i] = NULL;  // Null-terminate the array
    return env_array;
}

void run_execution(t_data *data)
{
	char	*exec_path;
    struct stat path_stat;

	data->env = list_to_char(data->env_list);
    exec_path = find_path(data->cmd->argv[0], data->env);
	if (exec_path == NULL)
	{
		ft_putstr_fd("minishell: command not found: ", 2);
		ft_putstr_fd(data->cmd->argv[0], 2);
		ft_putchar_fd('\n', 2);
        free(data->cmd);
		free(exec_path);
		exit(127);
	}
	else
	{
		if (stat(exec_path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode) != 0) 
        {
            ft_putstr_fd("minishell: ", 2);
            ft_putstr_fd(exec_path, 2);
            ft_putstr_fd(": Is a directory\n", 2);
            free(exec_path);
            exit(126);
        }
        execve(exec_path, data->cmd->argv, data->env);
		perror("minishell");
		exit(127);
	}
}
