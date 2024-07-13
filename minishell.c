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

// void	check_file_cmd1(Command *cmd)
// {
// 	if (cmd->input->fd < 0)
// 	{
// 		if (access(cmd->argv[1], F_OK) == -1)
// 		{
// 			ft_putstr_fd("minishell: no such file or directory ", 2);
// 			ft_putstr_fd(cmd->argv[1], 2);
// 			ft_putstr_fd("\n", 2);
// 			exit(EXIT_FAILURE);
// 		}
// 		else
// 		{
// 			ft_putstr_fd("minishell: ", 2);
// 			ft_putstr_fd(cmd->argv[1], 2);
// 			ft_putstr_fd(": ", 2);
// 			ft_putstr_fd("permission denied", 2);
// 			ft_putstr_fd("\n", 2);
// 			exit(EXIT_FAILURE);
// 		}
// 	}
// 	if (ft_strlen(cmd->argv[2]) == 0)
// 	{
// 		ft_putstr_fd("pipex: permission denied:\n", 2);
// 		exit(126);
// 	}
// }


int check_is_builtin(t_data data)
{
    char    **builtins;
    char    *builtins_str;
    int     i;

    builtins_str = "exit env pwd cd export unset echo";
    if (data.cmd == NULL || data.cmd->argv == NULL || data.cmd->argv[0] == NULL)
        return (0);
    builtins = ft_split(builtins_str, ' ');
    if (!builtins)
        return (0);
    i = 0;
    while (builtins[i])
    {
        if (ft_strcmp(data.cmd->argv[0], builtins[i]) == 0)
        {
            while (builtins[i])
            {
                free(builtins[i]);
                i++;
            }
            return (free(builtins), 1);
        }
        i++;
    }
    return (clear_tab(builtins), 0);
}

int execute_builtin(Command *cmd, t_data *data, int *flag)
{
    *flag = 1;
    if (!ft_strcmp(cmd->argv[0], "exit"))
        build_exit(cmd->argv);
    else if (!ft_strcmp(cmd->argv[0], "env"))
        build_env(data);
    else if (!ft_strcmp(cmd->argv[0], "pwd"))
        build_pwd(data);
    else if (!ft_strcmp(cmd->argv[0], "cd"))
        build_cd(data);
    else if (!ft_strcmp(cmd->argv[0], "export"))
        build_export(data);
    else if (!ft_strcmp(cmd->argv[0], "unset"))
        build_unset(data);
    else if (!ft_strcmp(cmd->argv[0], "echo"))
        build_echo(data);
    return (data->exit_status);
}

void run_builtin(Command *cmd, t_data *data, int *status)
{
    if (data->cmd->next == NULL && check_is_builtin(*data) == 1)
    {
        // output_fd(data);
        // if (data->cmd->output)
        // {
        //     redirection_in_out(data);
            
        // 
        *status = execute_builtin(cmd, data, &data->flag);
        else
        {
            *status = 1;
            data->flag = -1;
        }
        data->exit_status = *status;
        data->i = 1;
    }

}

void init_execution(Command *cmd, t_data *data, int *status)
{
    run_builtin(cmd, data, status);
    // Set signal handlers
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
}

// void output_fd(t_data *data)
// {
//     if (data->cmd->output && data->cmd->output->fd_out > 1)
//         data->fdout = data->cmd->output->fd_out;
//     else
//         data->fdout = dup(data->tmp_out);
// }

// void open_pipe(t_data *data)
// {
//     if (pipe(data->fd) == -1)
//     {
//         perror("pipe");
//         exit(EXIT_FAILURE);
//     }
//     if (data->out_file > 1)
//     {
//         data->fdout = data->out_file;
//         data->out_file = data->fd[1];
//         if (dup2(data->out_file, STDOUT_FILENO) == -1)
//         {
//             perror("dup2");
//             exit(EXIT_FAILURE);
//         }
//         close_if_not_standard_fd(data->out_file, STDOUT_FILENO);
//     }
//     else
//         data->fdout = data->fd[1];
// }

void close_if_not_standard_fd(int fd, int standard_fd)
{
    if (fd != standard_fd)
        close(fd);
}


// void setup_pipe_redirection(t_data *data)
// {
//     if (data->fdin != data->in_file)
//     {
//         if (data->in_file > 0)
//         {
//             close_if_not_standard_fd(data->in_file, STDIN_FILENO);
//             data->fdin = data->in_file;
//         }
//     }
//     if (dup2(data->fdin, STDIN_FILENO) == -1)
//     {
//         perror("dup2");
//         exit(EXIT_FAILURE);
//     }
//     close_if_not_standard_fd(data->fdin, STDIN_FILENO);
//     data->fdin = data->fd[0];
//     if (dup2(data->fdout, STDOUT_FILENO) == -1)
//     {
//         perror("dup2");
//         exit(EXIT_FAILURE);
//     }
//     close_if_not_standard_fd(data->fdout, STDOUT_FILENO);
// }

void run_execution(t_data *data)
{
	char	*exec_path;
    struct stat path_stat;

	exec_path = find_path(data->cmd->argv[0], data->env);
	if (exec_path == NULL)
	{
		ft_putstr_fd("minishell: command not found: ", 2);
		ft_putstr_fd(data->cmd->argv[0], 2);
		ft_putchar_fd('\n', 2);
        free(data->cmd);
		free(exec_path);
        data->exit_status = 127;
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
            data->exit_status = 126;
            exit(126);
        }
        execve(exec_path, data->cmd->argv, data->env);
		perror("minishell");
        data->exit_status = 127;
		exit(127);
	}
}

void run_child(t_data *data, int *status)
{
    Command *cmd = data->cmd;

    if (check_is_builtin(*data))
    {
        *status = execute_builtin(cmd, data, &data->flag);
        data->exit_status = *status;
        exit(data->exit_status);
    }
    else if (data->flag != -1)
    {
        // close_if_not_standard_fd(data->cmd->fdin, 0);
        data->env = list_to_char(data->env_list);
        // if (cmd->input && cmd->input->fd_in != -1)
        //     handle_redirection_and_errors(data);
        // else if (cmd->output && cmd->output->fd_out != -1)
        //     handle_redirection_and_errors(data);
        run_execution(data);
    }
}

void execute_fork(t_data *data, int *status)
{
    Command *tmp;

    tmp = data->cmd;
    data->pid = fork();
    if (data->pid == -1)
    {
        perror("fork");
        data->exit_status = EXIT_FAILURE;
        exit(data->exit_status);
    }
    else if (data->pid == 0)
    {
        // Child process
        signal(SIGINT, SIG_DFL);  // Reset SIGINT to default behavior
        signal(SIGQUIT, SIG_DFL); // Reset SIGQUIT to default behavior
        dup2(data->cmd->fdin, STDIN_FILENO);
        dup2(data->cmd->fdout, STDOUT_FILENO);
        close_if_not_standard_fd(data->cmd->fdin, STDIN_FILENO);
        close_if_not_standard_fd(data->cmd->fdout, STDOUT_FILENO);
        // nedd check and close 
        while (tmp)
        {
            if (data->exit_signal == -1)
                exit(130);
            tmp = tmp->next;
        }
        // if (open_check_redirections(data) == -1 )
        //     exit(1);
        run_child(data, status);
        data->exit_status = *status;
        exit(data->exit_status); // Ensure child process exits with the correct status
    }
}

void execute_cmd(Command *cmds, t_data *data, int *status)
{
    cmds = data->cmd; 
    while (cmds)
    {
        execute_fork(data, status);
        // data->i++;
        cmds = cmds->next;
    }
}

void restore_and_cleanup(t_data *data)
{
    close_if_not_standard_fd(data->cmd->fdin, STDIN_FILENO);
    close_if_not_standard_fd(data->cmd->fdout, STDOUT_FILENO);
}


void wating_processes(t_data *data, int *status)
{
    int i;

    i = 0;
    if (waitpid(data->pid, status, 0) == -1)
        return ;
    while (++i < data->size_cmds)
        wait(NULL);
    if (WIFEXITED(*status))
        data->exit_status = WEXITSTATUS(*status);
    else if (WIFSIGNALED(*status))
    {
        if (WTERMSIG(*status) == SIGQUIT)
            ft_putstr_fd("Quit (core dumped)\n", 2);
        else if (WTERMSIG(*status) == SIGINT)
            ft_putstr_fd("\n", 2);
        data->exit_status = 128 + (*status);
    }
}

void execution(Command *cmd, t_data *data)
{
    int status;

    data->i = 0;
    init_execution(cmd, data, &status);
    execute_cmd(cmd, data, &status);
    // restore_and_cleanup(data);
    wating_processes(data, &status);
}

void handle_sigint(int sig)
{
    (void)sig;
    ft_putstr_fd("\n", STDOUT_FILENO);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
    g_data.exit_status = 130; // Fix: g_data is not a pointer, so use dot operator instead of arrow operator
}

t_data *get_global_data(void)
{
    g_data.exit_status = 0;
    g_data.redir_erros = 0;
    return &g_data;
}

int main(int argc, char **argv, char **envp)
{
    Token   tokens[MAX_TOKENS];
    Command *cmd;
    t_data  *data;
    char    *line;
    int     nb_token;
    // int     exit_status;

    (void)argc;
    (void)argv;
    data = get_global_data();
    data->env_list = create_env_list(envp);
    while (1)
    {
        signal(SIGINT, handle_sigint);
        signal(SIGQUIT, SIG_IGN);
        line = readline("minishell> ");
        if (!line)
            break;
        if (strlen(line) > 0)
            add_history(line);
        nb_token = 0;
        lex(line, tokens, &nb_token,data->env_list);
        if (check_syntaxe(tokens, nb_token))
        {
            // printf("Syntax error\n");
            free(line);
            continue ;
        }
        cmd = parse(tokens);
        if (cmd)
        {
            data->cmd = cmd;
            data->ac = ft_size(cmd->argv);
            data->av = cmd->argv;
            data->redir_erros = 0;
            data->cmd->fdin = 0;
            data->cmd->fdout= 1;
            open_check_redirections(data);
            execution(data->cmd, data);
            free(line);
            free_all_resources(cmd);
        }
        // exit_status = data->exit_status;
        // free_env_list(data->env_list);
        // free(data);
    }
    return (data->exit_status);
}