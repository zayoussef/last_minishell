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
		ft_putstr_fd("minishell: permission denied: ", 2);
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

char	*find_path(char *cmd, char **env)
{
	char	**path;
	char	*exec_path;
	int		i;

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
		return (cmd);
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
    int         count;
    int         i;

    cur = env_list;
    count = 0;
    i = 0;
    while (cur)
    {
        count++;
        cur = cur->next;
    }
    env_array = (char **)malloc(sizeof(char *) * (count + 1));
    if (!env_array)
        return (NULL);
    cur = env_list;
    while (cur)
    {
        env_array[i] = ft_strjoin(cur->name, "=");
        env_array[i] = ft_strjoin(env_array[i], cur->value);
        i++;
        cur = cur->next;
    }
    env_array[i] = NULL;
    return (env_array);
}

void	run_exe(Command *cmd, t_data *data)
{
	char	*exec_path;

	data->env = list_to_char(data->env_list);
	exec_path = find_path(cmd->argv[0], data->env);
	if (exec_path == NULL)
	{
		ft_putstr_fd("minishell: command not found: ", 2);
		ft_putstr_fd(cmd->argv[0], 2);
		ft_putchar_fd('\n', 2);
		clear_tab(cmd->argv);
		free(exec_path);
		exit(127);
	}
	else
	{
		execve(exec_path, cmd->argv, data->env);
		perror("minishell");
		exit(127);
	}
}


void	check_file_cmd1(Command *cmd)
{
	if (cmd->input->fd < 0)
	{
		if (access(cmd->argv[1], F_OK) == -1)
		{
			ft_putstr_fd("minishell: no such file or directory ", 2);
			ft_putstr_fd(cmd->argv[1], 2);
			ft_putstr_fd("\n", 2);
			exit(EXIT_FAILURE);
		}
		else
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmd->argv[1], 2);
			ft_putstr_fd(": ", 2);
			ft_putstr_fd("permission denied", 2);
			ft_putstr_fd("\n", 2);
			exit(EXIT_FAILURE);
		}
	}
	if (ft_strlen(cmd->argv[2]) == 0)
	{
		ft_putstr_fd("pipex: permission denied:\n", 2);
		exit(126);
	}
}


int check_is_builtin(t_data data)
{
    const char *builtins[] = {"exit", "env", "pwd", "cd", "export", "unset", "echo", NULL};
    int i;

    if (data.cmd == NULL || data.cmd->argv == NULL || data.cmd->argv[0] == NULL)
        return 0;
    i = 0;
    while (builtins[i])
    {
        if (ft_strcmp(data.cmd->argv[0], builtins[i]) == 0)
            return 1;
        i++;
    }
    return 0;  
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
        build_echo(cmd->argv);
    return (0);
}

void run_builtin(Command *cmd, t_data *data, int *status)
{
    if (data->size_cmds == 1 && check_is_builtin(*data) == 1)
    {
        output_fd(data);
        dup2(data->fdout, STDOUT_FILENO);
        close(data->fdout);
        if (!data->pipe_errors)
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
    Command *cur;
    int nb_cmd;

    // Check for NULL pointers
    if (!cmd || !data)
        return ;
    // Initialize cur and nb_cmd
    cur = cmd;
    nb_cmd = 0;
    // Set in_file and out_file
    if (cmd->input)
        data->in_file = cmd->input->fd;
    if (cmd->output)
        data->out_file = cmd->output->fd;

    // Count the number of commands
    while (cur)
    {
        nb_cmd++;
        cur = cur->next;
    }
    data->size_cmds = nb_cmd;

    // Duplicate standard input and output
    if ((data->tmp_in = dup(STDIN_FILENO)) == -1) 
    {
        perror("dup");
        return;
    }
    if ((data->tmp_out = dup(STDOUT_FILENO)) == -1) 
    {
        perror("dup");
        close(data->tmp_in);
        return;
    }

    data->fdin = 0;
    if (data->cmd->input && data->cmd->input->fd > 0)
        data->fdin = data->cmd->input->fd;
    else if ((data->fdin = dup(data->tmp_in)) == -1) 
    {
        perror("dup");
        close(data->tmp_in);
        close(data->tmp_out);
        return;
    }

    // Check if the command is a single built-in command
    run_builtin(cmd, data, status);

    // Set signal handlers
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
}

void output_fd(t_data *data)
{
    if (data->cmd->output && data->cmd->output->fd > 1)
        data->fdout = data->cmd->output->fd;
    else
        data->fdout = dup(data->tmp_out);
}

void open_pipe(t_data *data)
{
    if (pipe(data->fd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    if (data->out_file > 1)
    {
        data->fdout = data->out_file;
        data->out_file = data->fd[1];
        if (dup2(data->out_file, STDOUT_FILENO) == -1)
        {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close_if_not_standard_fd(data->out_file, STDOUT_FILENO);
    }
    else
        data->fdout = data->fd[1];
}

void close_if_not_standard_fd(int fd, int standard_fd)
{
    if (fd != standard_fd)
        close(fd);
}


void setup_pipe_redirection(t_data *data)
{
    if (data->fdin != data->in_file)
    {
        if (data->in_file > 0)
        {
            close_if_not_standard_fd(data->in_file, STDIN_FILENO);
            data->fdin = data->in_file;
        }
    }
    if (dup2(data->fdin, STDIN_FILENO) == -1)
    {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    close_if_not_standard_fd(data->fdin, STDIN_FILENO);
    data->fdin = data->fd[0];
    if (dup2(data->fdout, STDOUT_FILENO) == -1)
    {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    close_if_not_standard_fd(data->fdout, STDOUT_FILENO);
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
        g_data.test = ("hello test!");
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

    if (data->size_cmds == 1 && check_is_builtin(*data))
    {
        *status = execute_builtin(cmd, data, &data->flag);
        data->exit_status = *status;
        exit(data->exit_status);
    }
    else if (data->flag != -1)
    {
        close_if_not_standard_fd(data->fdin, 0);
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
    while (data->cmd && data->i < data->size_cmds)
    { 
        data->cmd = cmds;
        if (cmds->input)
            data->in_file = cmds->input->fd;
        else
            data->in_file = STDIN_FILENO;

        if (cmds->output)
            data->out_file = cmds->output->fd;
        else
            data->out_file = STDOUT_FILENO;

        // Check for Pipe Errors or Empty Commands:
        if ((cmds->append_output && cmds->append_output->pipe_erros == -1) || !cmds)
        {
            data->i++;
            cmds = cmds->next;
            continue;
        }
        data->flag = 0;
        if (data->i == data->size_cmds - 1)
            output_fd(data);
        else
            open_pipe(data);
        setup_pipe_redirection(data);
        execute_fork(data, status);
        data->i++;
        cmds = cmds->next;
    }
}

void restore_and_cleanup(t_data *data)
{
    // Restore original standard input and output
    if (dup2(data->tmp_in, STDIN_FILENO) == -1)
    {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    if (dup2(data->tmp_out, STDOUT_FILENO) == -1)
    {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    close_if_not_standard_fd(data->tmp_in, STDIN_FILENO);
    close_if_not_standard_fd(data->tmp_out, STDOUT_FILENO);
    close_if_not_standard_fd(data->fdin, STDIN_FILENO);
}


void wating_processes(t_data *data, int *status)
{
    int i = 0;

    if (waitpid(data->pid, status, 0) == -1)
        return ;
    while (++i < data->size_cmds)
        wait(NULL);
    if (WIFEXITED(*status))
        data->exit_status = WEXITSTATUS(*status);
    else if (WIFSIGNALED(*status))
    {
        int sg = WTERMSIG(*status);
        if (sg == SIGQUIT)
            ft_putstr_fd("Quit: 3\n", 2);
        else if (sg == SIGINT)
            ft_putstr_fd("\n", 2);
        data->exit_status = 128 + sg;
    }
}

void execution(Command *cmd, t_data *data)
{
    int status;

    data->i = 0;
    init_execution(cmd, data, &status);
    execute_cmd(cmd, data, &status);
    restore_and_cleanup(data);
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
    return &g_data;
}

int main(int argc, char **argv, char **envp)
{
    Token   tokens[MAX_TOKENS];
    Command *cmd;
    t_data  *data;
    char    *line;
    int     nb_token;
    int     exit_status;

    // data = malloc(sizeof(t_data *));
    // if (!data)
    // {
    //     perror("malloc");
    //     return EXIT_FAILURE;
    // }
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
        lex(line, tokens, &nb_token);
        if (check_syntaxe(tokens, nb_token))
        {
            // printf("Syntax error\n");
            free(line);
            continue;
        }
        cmd = parse(tokens);
        data->cmd = cmd;
        data->ac = ft_size(cmd->argv);
        data->av = cmd->argv;
        if (!data->cmd)
        {
            free(line);
            continue;
        }
        open_redirections(data->cmd);
        while (data->cmd && data->cmd->argv[0] == NULL)
            data->cmd = data->cmd->next;
        if (data->cmd)
            execution(data->cmd, data);
        free(line);
        free_all_resources(cmd);
    }
    exit_status = data->exit_status;
    free_env_list(data->env_list);
    // free(data);
    return (exit_status);
}
