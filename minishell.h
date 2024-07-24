/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 15:03:14 by yozainan         #+#    #+#             */
/*   Updated: 2024/05/13 11:30:15 by yozainan        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "libft/libft.h"

#define PATH_ENV "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"

typedef struct s_env_node
{
    char        *name;
    char        *value;
    struct s_env_node *next;
} t_env_node;

/**************************************parsing*********************/

#define MAX_TOKENS 100000

typedef enum 
{
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_AMBIGUOUS,
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_APPEND_OUT,
    TOKEN_HERE_DOC,
    TOKEN_BACKGROUND,
    TOKEN_AND,
    TOKEN_LPR,
    TOKEN_RPR,
    TOKEN_DQUOTE,
    TOKEN_SQUOTE,
    TOKEN_END
} TokenType;


typedef struct 
{
    TokenType type;
    char *value;
} Token;

typedef struct Redirection 
{
    TokenType type;
    char *filename;
    struct Redirection *next;
} Redirection; // fiblaste matkoune stuct deyal kola type fi redirectio n
// khase ukoune pointer wahed, buche utejem3o ou yethato binefse tartipe li aselane dakhel fi line men terminal !
// khadek tsepisifi liya newe3e deyal redirection wache < wela > || >> || << 
// > /fffff <kjdf

// output -> output ....etc // inp
typedef struct Command 
{
    TokenType   *type;
    //Redirection *input;         // Input redirection
    Redirection *redirection;        // Output redirection
   // Redirection *append_output; // Append output redirection
    Redirection *heredoc;       // Here-doc redirection
    char        **argv;         // Arguments array
    int         fdin;           //
    int         fdout;          //
    struct Command *next;       // Pointer to next command
} Command;

typedef struct s_data
{
    t_env_node  *env_list;
    pid_t	    singel_pid;
	pid_t	    first_pid;
    pid_t       middel_pid;
	pid_t	    last_pid;
    pid_t pid;
    Command     *cmd;
    char        **av;
    char        **env;
    int         ac;
    int         fd[2];
    int			size_cmds;
    int         exit_status;
    int         exit_signal;
    int         redir_erros;
} t_data;

extern t_data g_data;

typedef struct 
{
    int in_quotes;
    char quote_char;
    char buffer[1024];
    int buffer_index;
    t_env_node *env;
} QuoteWordParserState;


void handle_word(Token *tokens, int *i, Command **current, int *argc) ;
void add_command_to_list(Command **head, Command *current) ;
Command* create_command() ;
void init_command(Command *cmd) ;
Command* parse(Token *tokens);

/********************lex**************************/
void lex(const char *input, Token *tokens, int *num_tokens,t_env_node *env);
void handle_special_characters(const char **p, Token *tokens, int *num_tokens);
void handle_v2(const char **p,char *special,TokenType *type);
void handle_quotes_and_words(const char **p, Token *tokens, int *num_tokens, QuoteWordParserState *state) ;
int check_syntaxe(Token *tokens,int nb_tokens);
int ft_size(char **argv);

/**********************redirection_parisng***********************/
void add_token(Token *tokens, int *num_tokens, TokenType type, char *value) ;
void skip_whitespace(const char **p) ;
void handle_redirection(Token *tokens, int *i, Command **current) ;
void finalize_command(Command **current, int *argc) ;

/********************************free_parsing************************************/
void free_all_resources(Command *head);
void free_redirection(Redirection *redir) ;
void free_all_v2(Command *current);

/*****************************REIRECTION_OPEN_FILE*****************************************/
void redirection_in_out(t_data **data, Command *cmd);
void open_check_redirections(t_data **data);

/********************parsing_tools********************/
void ft_strncpy(char *dest, const char *src, int n);

/*****************builtins_utils*****************/
void        append_to_existing_env(t_env_node *current, char *name, char *value);
void        update_or_add_env(t_env_node **env_list, char *name, char *value);
void        existing_env(t_env_node **env_list, char *name, char *value);
void        update_env_var(t_env_node **env_list, char *name, char *new);
void ft_init_minishell(int ac, char **av, char **env, t_data *data);
char *find_command_path(const char *command, t_env_node *env_list);
char        *get_env_value(t_env_node *env, const char *name);
void        change_directory(t_data *data, const char *path);
void        handle_new_env(t_env_node **env_list, char *arg);
void        handle_export(t_env_node **env_list, char *arg);
char        *ft_getenv(t_env_node *env, const char *name);
t_env_node  *find_env(t_env_node *env, const char *name);
int         handle_redirection_and_errors(t_data *data);
void        print_oldpwd_and_pwd(t_env_node *env_list);
void        ft_swap_env(t_env_node *a, t_env_node *b);
void        free_env_list(t_env_node *env_list);
void        print_sorted_env(t_env_node *env);
int         validation_home(t_env_node *env);
t_env_node  *create_env_list(char **s_env);
void        sort_env(t_env_node *tmp_env);
void        swap_pwd_oldpwd(t_data *data);
int         is_valid_identifier(char *s);
void        set_oldpwd(t_env_node *env);
void        print_av(char **av, int n);
void        set_pwd(t_env_node *env);
void        check_option(char **av);
void        go_home(t_data *data);
int         check_number(char *s);
int         check_n(char *str);
int         check_is_builtin(t_data data);
void        init_execution(t_data *data, int *status);
void        run_execution(t_data *data);
void        singel_cmd(t_data *data, int *status);
void        first_cmd(t_data **data, int *status);
int         middel_cmd(t_data **data, int *status);
int         last_cmd(t_data **data, int *status);
void        handle_sigint(int sig);
t_data *get_global_data(void);
int ft_strlnode(Command *cmd);
void wating_processes(t_data *data, int *status);
void init_execution(t_data *data, int *status);
void execution(t_data *data);
char	*build_executable_path(char *path, char *cmd);
char	*check_cmd(char *cmd);
void	clear_tab(char **tab);
char    *find_path(char *cmd, char **env);
char **list_to_char(t_env_node *env_list);
void run_execution(t_data *data);
int check_is_builtin(t_data data);
int execute_builtin(t_data *data);
void run_builtin(t_data *data, int *status);
void run_builtin(t_data *data, int *status);



/***********************builtin*******************/
void    build_export(t_data *data);
void    build_unset(t_data *data);
void    build_echo(t_data *data);
void    build_env(t_data *data);
void    build_pwd(t_data *data);
void    build_cd(t_data *data);
void    build_exit(char **av);

#endif
