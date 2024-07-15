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

int execute_builtin(t_data *data)
{
    if (!ft_strcmp(data->cmd->argv[0], "exit"))
        build_exit(data->cmd->argv);
    else if (!ft_strcmp(data->cmd->argv[0], "env"))
        build_env(data);
    else if (!ft_strcmp(data->cmd->argv[0], "pwd"))
        build_pwd(data);
    else if (!ft_strcmp(data->cmd->argv[0], "cd"))
        build_cd(data);
    else if (!ft_strcmp(data->cmd->argv[0], "export"))
        build_export(data);
    else if (!ft_strcmp(data->cmd->argv[0], "unset"))
        build_unset(data);
    else if (!ft_strcmp(data->cmd->argv[0], "echo"))
        build_echo(data);
    return (data->exit_status);
}

void run_builtin(t_data *data, int *status)
{
    if (check_is_builtin(*data) == 1)
    {
        *status = execute_builtin(data);
    }
    else
        *status = 1;
    data->exit_status = *status;
}

void singel_cmd(t_data *data, int *status)
{
    //output in stdout;
    if (check_is_builtin(*data) == 1)
        run_builtin(data, status);
    else
    {
        data->singel_pid = fork();
        if (data->singel_pid  == -1)
        {
            perror("fork");
            return ;
        }
        else if (data->singel_pid == 0)
        {
            data->env = list_to_char(data->env_list);
            run_execution(data);
        }
        else
            wating_processes(data, status);
    }
}

void first_cmd(t_data *data, int *status)
{
    int pipe_fd[2];

    // Create a pipe for inter-process communication
    if (pipe(pipe_fd) == -1)
    {
        perror("pipe");
        *status = 1;
        return;
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        *status = 1;
        return;
    }
    else if (pid == 0)
    {
        // Child process
        // Redirect STDOUT to the write end of the pipe
        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
        {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(pipe_fd[0]);
        close(pipe_fd[1]);

        // Check if the command is a built-in command and execute it
        if (check_is_builtin(*data) == 1)
        {
            execute_builtin(data);
            exit(data->exit_status);  // Ensure to exit with the status set by the built-in command
        }
        else
        {
            // Execute the command if it's not a built-in
            data->env = list_to_char(data->env_list);
            run_execution(data);
            exit(EXIT_FAILURE);  // run_execution should handle exit
        }
    }
    else
    {
        // Parent process
        // Close the write end of the pipe, parent does not use it
        close(pipe_fd[1]);
        // Store the read end of the pipe to pass it to the next command
        data->fd[0] = pipe_fd[0];
    }
}

int middel_cmd(t_data *data, int *status)
{
    int pipe_fd[2];

    if (pipe(pipe_fd) == -1)
    {
        perror("pipe");
        *status = 1;
        return -1;
    }
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        *status = 1;
        return -1;
    }
    else if (pid == 0)
    {
        // Child process
        if (dup2(data->fd[0], STDIN_FILENO) == -1 || dup2(pipe_fd[1], STDOUT_FILENO) == -1)
        {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        close(data->fd[0]);

        if (check_is_builtin(*data) == 1)
        {
            execute_builtin(data);
            exit(data->exit_status);
        }
        else
        {
            data->env = list_to_char(data->env_list);
            run_execution(data);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        // Parent process
        close(data->fd[0]);
        close(pipe_fd[1]);
        data->fd[0] = pipe_fd[0];
    }
    return 0;
}

int last_cmd(t_data *data, int *status)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        *status = 1;
        return -1;
    }
    else if (pid == 0)
    {
        // Child process
        if (dup2(data->fd[0], STDIN_FILENO) == -1)
        {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(data->fd[0]);

        if (check_is_builtin(*data) == 1)
        {
            execute_builtin(data);
            exit(data->exit_status);
        }
        else
        {
            data->env = list_to_char(data->env_list);
            run_execution(data);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        // Parent process
        close(data->fd[0]);
    }
    return 0;
}

void init_execution(t_data *data, int *status)
{
    if (data->size_cmds == 1)
        singel_cmd(data, status);// done from where 1 command all test done !
    else if (data->size_cmds > 1 )
    {
        first_cmd(data, status);
        data->cmd = data->cmd->next;
        while (data->cmd->next != NULL)
        {
            middel_cmd(data, status);
            data->cmd = data->cmd->next;
        }
        last_cmd(data, status);
    }
}

void close_if_not_standard_fd(int fd, int standard_fd)
{
    if (fd != standard_fd)
        close(fd);
}

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
    if (check_is_builtin(*data) == 1)
    {
        *status = execute_builtin(data);
        data->exit_status = *status;
        exit(data->exit_status);
    }
    else if (data)
    {
        data->env = list_to_char(data->env_list);
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
        while (tmp)
        {
            if (data->exit_signal == -1)
                exit(130);
            tmp = tmp->next;
        }
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

void execution(t_data *data)
{
    int status;

    status  = 0;
    open_check_redirections(data);
    if (data->redir_erros == -1)
        return ;
    init_execution(data, &status);
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
    g_data.exit_signal = 0;
    g_data.redir_erros = 0;
    return &g_data;
}

int ft_strlnode(Command *cmd)
{
    int i = 0;

    while (cmd)
    {
        i++;
        cmd = cmd->next;
    }
    return i;
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
        line = readline("\033[32mminishell> \033[0m");
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
            data->size_cmds = ft_strlnode(data->cmd);
            data->redir_erros = 0;
            data->cmd->fdin = 0;
            data->cmd->fdout= 1;
            execution(data);
            free(line);
            free_all_resources(cmd);
        }
        // exit_status = data->exit_status;
        // free_env_list(data->env_list);
        // free(data);
    }
    return (data->exit_status);
}