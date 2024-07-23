/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/11 21:36:21 by yozainan          #+#    #+#             */
/*   Updated: 2024/07/23 13:33:15 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int check_ambiguous_unexpected_tokens(Token *tokens,int i)
{

 if( tokens[i + 1].type != TOKEN_END && 
        (tokens[i].type == TOKEN_AMBIGUOUS || tokens[i + 1].type == TOKEN_AMBIGUOUS))
        {
            return (printf("minishell: $: ambiguous error\n"),1);
        }
        if((!ft_strcmp(tokens[0].value ,"&&")) || (!ft_strcmp(tokens[0].value ,"||")))
        {
            return (printf("Syntax error: unexpected token 1 '&'\n"),1);
        }
        if((tokens[i].type == TOKEN_APPEND_OUT || tokens[i].type == TOKEN_REDIRECT_OUT || tokens[i].type == TOKEN_REDIRECT_IN) 
        && !ft_strcmp(tokens[i + 1].value,"#"))
        {
            return (printf("minishell: $: ambigios error\n"),1);
        }
        if((tokens[i].type == TOKEN_HERE_DOC && tokens[i + 1].type != TOKEN_WORD) || 
            (tokens[i].type == TOKEN_REDIRECT_IN && tokens[i + 1].type != TOKEN_WORD ) || 
            (tokens[i].type == TOKEN_REDIRECT_OUT && tokens[i + 1].type != TOKEN_WORD) ||
            (tokens[i].type == TOKEN_APPEND_OUT && tokens[i + 1].type != TOKEN_WORD))
        {
            return (printf("Syntax error: near unexpected token 2 '%s'\n", tokens[i].value),1);
        }   
        return 0;
}

int  check_unexpected_tokens_pipe_and_background(Token *tokens,int *i)
{
    if (tokens[*i].type == TOKEN_BACKGROUND ||( tokens[*i].type == TOKEN_AND && tokens[*i + 1].type != TOKEN_WORD)
        || (tokens[*i].type == TOKEN_AND && tokens[*i + 1].type == TOKEN_END))
        {
            // Single '&' is not allowed
            return (printf("Syntax error: unexpected token 3 '&'\n"), 1);
        }
        if((tokens[*i].type == TOKEN_AND && tokens[*i + 1].type != TOKEN_WORD)
            || (tokens[*i].type == TOKEN_BACKGROUND) || (tokens[*i].type == TOKEN_BACKGROUND && tokens[*i].type != TOKEN_WORD))
        {
            return (printf("Syntax error: unexpected token  4 '&'\n"), 1);
        }
        if (tokens[*i].type != TOKEN_WORD && tokens[*i].type != TOKEN_PIPE 
            && tokens[*i].type != TOKEN_AND && tokens[*i].type != TOKEN_END)
        {
            if ((tokens[*i].type == tokens[*i + 1].type && tokens[*i].type != TOKEN_END))
            {
                return (printf("Syntax error: unexpected token 5'%s'\n", tokens[*i].value), 1);
            }
            (*i)++;
        }
    return 0;
}

int check_syntaxe(Token *tokens,int nb_tokens)
{
    int i;
    i = 0;

    while (i < nb_tokens && tokens[i].type != TOKEN_END)
    {
        if(check_ambiguous_unexpected_tokens(tokens,i) == 1 
        || check_unexpected_tokens_pipe_and_background(tokens,&i) == 1)
        {
            return 1;  
    }
        else if (tokens[i].type != TOKEN_END && tokens[i].type == tokens[i + 1].type 
                 && tokens[i].type != TOKEN_WORD)
        {
            return (printf("Syntax error: unexpected token 6 '%s'\n", tokens[i].value), 1);
        }
        else if ((tokens[i].type == TOKEN_BACKGROUND || tokens[i].type == TOKEN_PIPE || tokens[i].type == TOKEN_AND) 
                 && (tokens[i + 1].type == TOKEN_END || tokens[i + 1].type == tokens[i].type))
        {
            return (printf("Syntax error: unexpected token 7 '%s'\n", tokens[i].value), 1);
        }
        i++;
    }
    return 0;
}

Command* parse(Token *tokens) 
{
    Command *head = NULL;
    Command *current;
    int flag;
    int argc;
    int i;
    current = create_command();
    if (!current) return NULL; // Handle memory allocation failure
    flag = 0;
    argc = 0;
    i = 0;
    while (tokens[i].type != TOKEN_END) 
    {
        if (tokens[i].type == TOKEN_WORD) {
            handle_word(tokens, &i, &current, &argc);
        } else if (tokens[i].type == TOKEN_PIPE || tokens[i].type == TOKEN_AND) 
        {
            finalize_command(&current, &argc);
            add_command_to_list(&head, current);
            current = create_command(); // Start a new command
            if (!current) return head; // Handle memory allocation failure
        } else if (tokens[i].type == TOKEN_REDIRECT_IN || tokens[i].type == TOKEN_REDIRECT_OUT 
                || tokens[i].type == TOKEN_APPEND_OUT || tokens[i].type == TOKEN_HERE_DOC) {
            handle_redirection(tokens, &i, &current);
            flag++;
        } else if ((tokens[i].type == TOKEN_LPR && tokens[i + 1].type == TOKEN_WORD )
        || (tokens[i].type == TOKEN_RPR && tokens[i - 1].type == TOKEN_WORD)) {
            flag++;
        } else {
            write(2, "minishell: syntax error near unexpected token  newline\n", 54);
            return (free_all_resources(current), NULL);
        }
        i++;
    }
    if (current && (argc > 0 || flag > 0)) { // Check if there's anything in the current command
        finalize_command(&current, &argc);
        add_command_to_list(&head, current);
    }
    return head;
}

int ft_size(char **argv)
{
    int i;

    if (argv == NULL)
        return 0;
    i = 0;
    while(argv[i] != NULL)
        i++;
    return i;
}

