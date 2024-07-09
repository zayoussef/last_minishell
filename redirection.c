#include "minishell.h"

void add_token(Token *tokens, int *num_tokens, TokenType type, char *value) 
{
    tokens[*num_tokens].type = type;
    tokens[*num_tokens].value = ft_strdup(value);
    (*num_tokens)++;
}

void skip_whitespace(const char **p) 
{
    while (ft_isspace(**p)) (*p)++;
}

void next_type(Redirection **redir,Command **current)
{
    if ((*redir)->type == TOKEN_REDIRECT_IN) 
    {
        (*redir)->next = (*current)->input;
        (*current)->input = (*redir);
    }
     else if ((*redir)->type == TOKEN_REDIRECT_OUT)
    {
        (*redir)->next = (*current)->output;
        (*current)->output = (*redir);
    } 
    else if ((*redir)->type == TOKEN_APPEND_OUT) 
    {
        (*redir)->next = (*current)->append_output;
        (*current)->append_output = (*redir);
    } 
    else if ((*redir)->type == TOKEN_HERE_DOC) 
    {
        (*redir)->next = (*current)->heredoc;
        (*current)->heredoc = (*redir);
    }
}

void handle_redirection(Token *tokens, int *i, Command **current) 
{
    if (*current == NULL||!current) 
    {
        *current = create_command();
    }
    Redirection *redir ;
    redir = (Redirection *)malloc(sizeof(Redirection));
    if (!redir)
        return; // Handle malloc failure
    redir->type = tokens[*i].type;
    redir->filename = tokens[++(*i)].value;
    redir->next = NULL;
    next_type(&redir,current);
}

void finalize_command(Command **current, int *argc) 
{
    if (*current) 
    {
        (*current)->argv[*argc] = NULL; // Terminate argv array
        *argc = 0;
    }
}