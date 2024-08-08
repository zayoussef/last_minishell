/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 15:03:14 by yozainan         #+#    #+#             */
/*   Updated: 2024/07/24 13:57:50 by elchakir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/libft.h"
# include <ctype.h>
# include <linux/limits.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <unistd.h>

/******  Constants ********/
# define PWD_ERROR_MSG "minishell: pwd: error retrieving current directory: getcwd: cannot access parent directories: No such file or directory"
# define HER_DOC "warning: here-document delimited by end-of-file (wanted `')\n"
# define MAX_TOKENS 100000

/********** Enums *******/
typedef enum
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_OR,
	TOKEN_HASHTAG,
	NOT_DEFINE,
	TOKEN_AMBIGUOUS,
	TOKEN_REDIRECT_IN,
	TOKEN_REDIRECT_OUT,
	TOKEN_APPEND_OUT,
	TOKEN_HERE_DOC,
	TOKEN_HERE_DOC_NO,
	TOKEN_BACKGROUND,
	TOKEN_AND,
	TOKEN_LPR,
	TOKEN_RPR,
	TOKEN_DQUOTE,
	TOKEN_SQUOTE,
	TOKEN_END
}						TokenType;

/****************  Structures **********/
typedef struct s_env_node
{
	char				*name;
	char				*value;
	struct s_env_node	*next;
}						t_env_node;

typedef struct
{
	TokenType			type;
	char				*value;
}						Token;

typedef struct Redirection
{
	TokenType			type;
	char				*filename;
	struct Redirection	*next;
}						Redirection;

typedef struct s_cmd
{
	char				*value;
	struct s_cmd		*next;
}						t_cmd;

typedef struct Command
{
	TokenType			type;
	Redirection			*redirection;
	Redirection			*heredoc;
	t_cmd				*cmd_lst;
	char				**av;
	int					fdin;
	int					fdout;
	int					redir_errors;
	int					dup;
	struct Command		*next;
}						Command;

typedef struct s_data
{
	t_env_node			*env_list;
	Command				*cmd;
	pid_t				pid;
	char				**env;
	char				**av;
	int					ac;
	int					fd[2];
	int					size_cmds;
	int					exit_status;
	int					is_pipeline;
	int					her_errors;
}						t_data;

extern t_data			g_data;

typedef struct
{
	int					i;
	int					flag;
}						t_value;

typedef struct
{
	int					in_quotes;
	char				quote_char;
	char				buffer[100001];
	bool				status;
	const char			*start;
	int					expandit;
	int					double_quotes;
	int					flag;
	char				*expanded_value;
	int					buffer_index;
	t_env_node			*env;
}						QuoteWordParserState;

/********** Function Prototypes ********************/

/******************** Utility Functions ********************/
void					ft_strncpy(char *dest, const char *src, int n);
size_t					ft_lst_size(t_cmd *cmd);
char					**ft_list_to_char(t_cmd *cmd, int size);
void					add_cmd(t_cmd **head, t_cmd *cmd);
void					clear_tab(char **tab);
void					remove_quotes(char *str);

/******************** Parsing Functions ********************/
QuoteWordParserState	init_lexer(t_env_node *env);
int						check_syntaxe(Token *tokens, int nb_tokens);
int						ft_strlnode(Command *cmd);
void					handle_special_characters(const char **p, Token *tokens,
							int *num_tokens);
void					handle_quotes_and_words(const char **p, Token *tokens,
							int *num_tokens, QuoteWordParserState *state);
void					handle_v2(const char **p, char *special,
							TokenType *type);
void					handle_redirection(Token *tokens, int *i,
							Command **current);
void					finalize_command(Command **current);
void					process_token(Token *tokens, t_value *value,
							Command **current, Command **head,
							t_cmd **cmd_head);
Command					*parse(Token *tokens);
int						lex(const char *input, Token *tokens, int *num_tokens,
							t_env_node *env);
int						check_syntax(Token *tokens, int nb_tokens);
void					free_all_resources(Command *head);
void					free_redirection(Redirection *redir);
void					free_all_v2(Command *current);
void					add_token(Token *tokens, int *num_tokens,
							TokenType type, char *value);
void					skip_whitespace(const char **p);
void					add_command_to_list(Command **head, Command *current);

/******************** Redirection Parsing Functions ********************/
void					redirection_in_out(t_data *data, Command *cmd);
void					open_check_redirections(t_data *data);
void					add_redirection(Redirection **redir_list,
							Redirection *redir);
void					handle_redirect_in(t_data *data, Command *cmd,
							Redirection *redir);
void					open_file(t_data *data, Command *cmd,
							Redirection *redir);
void					handle_other_redirects(t_data *data, Command *cmd,
							Redirection *redir);

/******************** Built-in Command Functions ********************/
void					build_export(t_data *data);
void					build_unset(t_data *data);
void					build_echo(t_data *data);
void					build_env(t_data *data);
void					build_pwd(t_data *data);
void					build_cd(t_data *data);
void					build_exit(char **av);
int						execute_builtin(t_data *data);
void					run_builtin(t_data *data, int *status);
int						check_is_builtin(t_data data);

/******************** Execution Functions ********************/
void					fill_cmd(t_data *data);
void					wating_processes(t_data *data, int *status);
void					init_execution(t_data *data, int *status);
void					execution(t_data *data);
void					run_execution(t_data *data);
void					singel_cmd(t_data *data, int *status);
void					multiple_cmd(t_data *data, int *status);
char					*build_executable_path(char *path, char *cmd);
char					*find_path(char *cmd, char **env);
void					handle_exec_error(char *cmd);
void					handle_is_directory(char *exec_path);
void					handle_child_process(t_data *data, int pipe_fd[2]);
void					handle_parent_process(t_data *data, int pipe_fd[2]);
void					ft_dup_in(t_data *data);
void					ft_dup_out(t_data *data, int i);
void					setup_input(t_data *data);
void					setup_output(t_data *data, int pipe_fd[2]);
void					handle_heredoc(t_data *data, Redirection *redir);
void					handle_all_heredocs(t_data *data);
void					handle_heredoc_error(int pipe_fd[2], char *line);
void					process_heredoc_line(char *line, Redirection *redir,
							t_data *data, int pipe_fd[2]);

/******************** Signal Handling Functions ********************/
void					handle_sigint(int sig);
void					herd_sig(int sig);

/******************** Environment Functions ********************/
void					append_to_existing_env(t_env_node *current, char *name,
							char *value);
void					update_or_add_env(t_env_node **env_list, char *name,
							char *value);
void					existing_env(t_env_node **env_list, char *name,
							char *value);
void					update_env_var(t_env_node **env_list, char *name,
							char *new);
char					*get_env_value(t_env_node *env, const char *name);
void					handle_new_env(t_env_node **env_list, char *arg);
void					handle_export(t_env_node **env_list, char *arg);
char					*ft_getenv(t_env_node *env, const char *name);
t_env_node				*find_env(t_env_node *env, const char *name);
void					free_env_list(t_env_node *env_list);
t_env_node				*create_env_list(char **env);
char					**list_to_char(t_env_node *env_list);
void					sort_env(t_env_node *tmp_env);
void					print_sorted_env(t_env_node *env);
void					print_oldpwd_and_pwd(t_env_node *env_list);
void					swap_pwd_oldpwd(t_data *data);
void					set_oldpwd(t_env_node *env);
void					set_pwd(t_env_node *env);
int						count_env_nodes(t_env_node *env_list);
void					ft_swap_env(t_env_node *a, t_env_node *b);

/********************  Functions ********************/
t_data					*get_global_data(void);
void					find_when_start(char *str, t_data *data, char **result);
void					expand_variable_in_str(char **temp_result, char **p,
							t_data *data);
char					*expand_variable(const char *start, int length,
							t_env_node *env);
void					check_permissions(t_data *data, Command *cmd,
							TokenType type);
void					change_directory(t_data *data, const char *path);
void					check_limit_her_doc(Redirection *redir);
int						validation_home(t_env_node *env);
void					print_av(char **av, int n, int fd);
void					check_option(char **av, int fd);
void					go_home(t_data *data);
int						check_number(char *s);
int						check_n(char *str);
int						check_is_builtin(t_data data);

#endif
