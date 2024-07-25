#include "../minishell.h"

void add_token(Token *tokens, int *num_tokens, TokenType type, char *value)
{
    tokens[*num_tokens].type = type;
    tokens[*num_tokens].value = ft_strdup(value);
    (*num_tokens)++;
}

void skip_whitespace(const char **p) 
{
    while (ft_isspace(**p))
        (*p)++;
}

void next_type(Redirection **redir, Command **current)
{
    Redirection *temp;
    if ((*redir)->type == TOKEN_REDIRECT_IN || (*redir)->type == TOKEN_APPEND_OUT 
        || (*redir)->type == TOKEN_REDIRECT_OUT) 
    {
        if ((*current)->redirection == NULL)
            (*current)->redirection = (*redir);
        else
        {
            temp = (*current)->redirection;
            while (temp->next)
                temp = temp->next;
            temp->next = (*redir);
        }
    }
    else if ((*redir)->type == TOKEN_HERE_DOC) 
    {
        if ((*current)->heredoc == NULL)
            (*current)->heredoc = (*redir);
        else
        {
            temp = (*current)->heredoc;
            while (temp->next)
                temp = temp->next;
            temp->next = (*redir);
        }
    }
}

void handle_redirection(Token *tokens, int *i, Command **current) 
{
    Redirection *redir;

    if (*current == NULL || !current)
    {
        *current = create_command();
    }
    redir = (Redirection *)malloc(sizeof(Redirection));
    if (!redir)
        return;
    redir->type = tokens[*i].type;
    redir->filename = tokens[++(*i)].value;
    redir->next = NULL;
    next_type(&redir, current);
}

void finalize_command(Command **current, int *argc) 
{
    if (*current) 
    {
        (*current)->argv[*argc] = NULL; // Terminate argv array
        *argc = 0;
    }
}
