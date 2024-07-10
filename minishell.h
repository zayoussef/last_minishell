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
    int pipe_erros;
    int fd;
    struct Redirection *next;
} Redirection;

typedef struct Command 
{
    TokenType *type;
    char **argv;                // Arguments array
    Redirection *input;         // Input redirection
    Redirection *output;        // Output redirection
    Redirection *append_output; // Append output redirection
    Redirection *heredoc;       // Here-doc redirection
    struct Command *next;       // Pointer to next command
} Command;

typedef struct s_data
{
    t_env_node  *env_list;
    pid_t	    pid;
	pid_t	    pid_last;
    Command     *cmd;
    char        **av;
    char        **env;
    int         i;
    int         flag;
    int         ac;
    int		    in_file;
	int		    out_file;
    int         fd[2];
    int         fdin;
    int         fdout;
	int			tmp_in;
	int			tmp_out;
    int			size_cmds;
    int         pipe_errors;
    int         exit_status;
    int         exit_signal;
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

void input_redirection(t_data *data);
void output_redirection(t_data *data);
void open_pipe(t_data *data);
void setup_redirections(t_data *data);
void wating_processes(t_data *data, int *status);
void output_fd(t_data *data);
void close_if_not_standard_fd(int fd, int standard_fd);
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
int open_redirections(t_data *data);
int redirection_in_out(t_data *data) ;
/********************parsing_tools********************/
void ft_strncpy(char *dest, const char *src, int n);
/*****************builtins_utils*****************/
void        append_to_existing_env(t_env_node *current, char *name, char *value);
void        update_or_add_env(t_env_node **env_list, char *name, char *value);
void        existing_env(t_env_node **env_list, char *name, char *value);
void        update_env_var(t_env_node **env_list, char *name, char *new);
char        *get_env_value(t_env_node *env, const char *name);
void        change_directory(t_data *data, const char *path);
void        handle_new_env(t_env_node **env_list, char *arg);
void        handle_export(t_env_node **env_list, char *arg);
char        *ft_getenv(t_env_node *env, const char *name);
t_env_node  *find_env(t_env_node *env, const char *name);
void        print_oldpwd_and_pwd(t_env_node *env_list);
void        ft_swap_env(t_env_node *a, t_env_node *b);
void        print_sorted_env(t_env_node *env);
int         validation_home(t_env_node *env);
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
void ft_init_minishell(int ac, char **av, char **env, t_data *data);
char *find_command_path(const char *command, t_env_node *env_list);
void free_env_list(t_env_node *env_list);
t_env_node *create_env_list(char **s_env);
/***********************builtin*******************/
void    build_export(t_data *data);
void    build_unset(t_data *data);
void    build_env(t_data *data);
void    build_pwd(t_data *data);
void    build_cd(t_data *data);
void    build_echo(char **av);
void    build_exit(char **av);

#endif