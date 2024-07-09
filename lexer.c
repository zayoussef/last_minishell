#include "minishell.h"
#include <unistd.h> // For getpid()

void handle_quotes_and_wordsv2(const char **p,QuoteWordParserState **state)
{
            if ((*state)->in_quotes) 
            {
                if (**p == (*state)->quote_char) 
                    (*state)->in_quotes = 0;
                else 
                    (*state)->buffer[(*state)->buffer_index++] = **p;
            } 
            else 
            {
                (*state)->in_quotes = 1;
                (*state)->quote_char = **p;
            }
}

char* expand_variable(const char *start, int length,t_env_node *env) 
{
    char varname[length + 1];
    char *value;
    
    ft_strncpy(varname, start, length);
    varname[length] = '\0';
    value = ft_getenv(env,varname);
    if(value) return ft_strdup(value);
    else
        return(ft_strdup("")); // Return empty string if variable is not found
}

int is_ambiguous(const char *expanded_value) {
    // Check if the expanded value contains spaces or is empty
    return (strchr(expanded_value, ' ') != NULL || expanded_value[0] == '\0');
}

void handle_quotes_and_words(const char **p, Token *tokens, int *num_tokens, QuoteWordParserState *state) 
{
    char *expanded_value;
    const char *start;
    int pid;
    int flag;
    int i;
    char *pid_str;
    flag = 0;
    state->buffer_index = 0;
    while (**p != '\0' && (state->in_quotes || (!ft_isspace(**p) && **p != '|' 
    && **p != '<' && **p != '>' && **p != '&' && **p != '(' && **p != ')')))
    {
        if (**p == '\\' && *(*p + 1) != '\0') 
        {
            state->buffer[state->buffer_index++] = *(*p + 1);
            (*p) += 2;
        } 
        else if ((**p == '"' || **p == '\'') && (state->in_quotes == 0 
            || **p == state->quote_char)) 
        {
           handle_quotes_and_wordsv2(p,&state);
            (*p)++;
        } 
         else if (**p == '$') 
        {
            i = 0;
            (*p)++;
            start = *p;
            if (start[0] == '$') 
            {
                pid = getpid();
                pid_str = ft_itoa(pid);
                //add_token(tokens, num_tokens, TOKEN_WORD, pid_str);
                while(pid_str[i])
                state->buffer[state->buffer_index++] = pid_str[i++];
                free(pid_str);
                (*p)++;
                continue;
            }           
            while (ft_isalnum(**p) || **p == '_') (*p)++;
            if (*p > start) {
                expanded_value = expand_variable(start, *p - start,state->env);
                //add_token(tokens, num_tokens, TOKEN_WORD, expanded_value);
                if (is_ambiguous(expanded_value) && *num_tokens > 0 && (tokens[*num_tokens - 1].type == TOKEN_REDIRECT_OUT 
                ||tokens[*num_tokens - 1].type == TOKEN_APPEND_OUT || tokens[*num_tokens - 1].type == TOKEN_REDIRECT_IN)) {
                    free(expanded_value);
                    flag = 1;
                }
                else
                {
                while(expanded_value[i])
                state->buffer[state->buffer_index++] = expanded_value[i++];
                free(expanded_value);
                }
            }
            else
                state->buffer[state->buffer_index++] = '$';
        }
       else 
           state->buffer[state->buffer_index++] = *(*p)++;
    }
    state->buffer[state->buffer_index] = '\0';
    if(flag == 1)
        add_token(tokens, num_tokens, TOKEN_AMBIGUOUS, "#");
    else if (state->buffer_index > 0 && flag == 0) // Only add token if buffer is not empty
        add_token(tokens, num_tokens, TOKEN_WORD, state->buffer);
}

void handle_v2(const char **p,char *special,TokenType *type)
{
       if (**p == '>') 
        {
            if (*(*p + 1) == '>') 
            {
                special[1] = '>';
                *type = TOKEN_APPEND_OUT;
                (*p)++;
            }
            else
                *type = TOKEN_REDIRECT_OUT;
        }
        else if (**p == '&') 
        {
            (*p)++;
            if (**p == '&' && *(*p + 1) != '&')
            {
                *type = TOKEN_AND;
                special[1] = '&';
                (*p)++;
            }
            else
            {
                *type = TOKEN_BACKGROUND;
                (*p)++;
            }
        }
}

void doc_or_in(const char **p,TokenType *type,char *special)
{
            if (*(*p + 1) == '<') 
            {
                special[1] = '<';
                *type = TOKEN_HERE_DOC;
                (*p)++;
            } 
            else
                *type = TOKEN_REDIRECT_IN;
}

void handle_special_characters(const char **p, Token *tokens, int *num_tokens)
{
    TokenType type;
    char special[3];
    char *sp;
    if (**p == '|' || **p == '<' || **p == '>' 
    || **p == '&')
    {
        special[0] = **p;
        special[1] = '\0';
        type = TOKEN_WORD;
        if (**p == '|') 
            type = TOKEN_PIPE;
        else if (**p == '<')
           doc_or_in(p,&type,special);
        else
            handle_v2(p,special,&type);
        add_token(tokens, num_tokens, type, special);
        (*p)++;
    }
    else if(**p == '(')
        {
            type = TOKEN_LPR;
            sp = " ( ";
        (*p)++;
        add_token(tokens, num_tokens, type, sp);
    }
    else if(**p == ')')
        {
        type = TOKEN_RPR;
        sp = " ) ";
        (*p)++;
        add_token(tokens, num_tokens, type, sp);
    }
}

void lex(const char *input, Token *tokens, int *num_tokens,t_env_node *env)
{
    const char *p ;
    p = input;
    QuoteWordParserState state;
    state.in_quotes = 0;
    state.quote_char = '\0';
    state.buffer_index = 0;
    state.env = env;
    ft_memset(state.buffer, 0, sizeof(state.buffer));

    while (*p) 
    {
        skip_whitespace(&p);
        handle_special_characters(&p, tokens, num_tokens);
        handle_quotes_and_words(&p, tokens, num_tokens, &state);
    }

    // Check if quotes are not closed
    if (state.in_quotes) {
        printf("Syntax error: unclosed quote '%c'\n", state.quote_char);
    }

    add_token(tokens, num_tokens, TOKEN_END, "");
}
