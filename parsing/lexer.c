/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 17:20:21 by elchakir          #+#    #+#             */
/*   Updated: 2024/08/08 14:53:25 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void handle_quotes_and_wordsv2(const char **p, QuoteWordParserState **state)
{
	if ((*state)->in_quotes)
	{
		if (**p == (*state)->quote_char)
		{
			(*state)->in_quotes = 0;
			(*state)->quote_char = '\0';
		}
	}
	else
	{
		(*state)->in_quotes = 1;
		(*state)->quote_char = **p;
	}
}

char *expand_variable(const char *start, int length, t_env_node *env)
{
	char varname[length + 1];
	char *value;

	ft_strncpy(varname, start, length);
	varname[length] = '\0';
	value = ft_getenv(env, varname);
	if (value)
		return (ft_strdup(value));
	else
		return (ft_strdup("")); // Return empty string if variable is not found
}

int is_ambiguous(const char *expanded_value)
{
	// Check if the expanded value contains spaces or is empty
	return (ft_strrch_finder((char *)expanded_value, ' ') != NULL || expanded_value[0] == '\0' || !expanded_value);
}

char *alloc_token(const char *s, int *r, int i)
{
	char *buf;
	int size;
	int j;
	int rem;

	j = 0;
	size = 0;
	rem = i;
	while (s[i] && s[i] != 32 && !(s[i] >= 9 && s[i] <= 13))
	{
		size++;
		i++;
	}
	buf = malloc(size + 1);
	if (!buf)
		return (NULL);
	while (j < size)
	{
		buf[j] = s[rem];
		j++;
		rem++;
	}
	buf[j] = '\0';
	*r = rem;
	return (buf);
}

void ft_buffer_split(Token *tokens, int *nb, const char *str)
{
	char *token_value;
	int i;
	int j;

	i = 0;
	j = 0;
	while (str[i])
	{
		if (!ft_isspace((unsigned char)str[i]))
		{
			token_value = alloc_token(str, &j, i);
			if (token_value)
				add_token(tokens, nb, TOKEN_WORD, token_value);
			i = j;
		}
		else
			i++;
	}
}

int check_space(char *s)
{
	int i;

	i = 0;
	while (s[i])
	{
		if (s[i] == 32 || (s[i] >= 9 && s[i] <= 13))
			return (1);
		i++;
	}
	return (0);
}

int is_token(const char *p)
{
	char *start;
	start = (char *)p;
	while (*start && (ft_isspace(*start) || *start != '\'' || *start != '"'))
		start++;
	if (*start == '\0' || *start == '|')
		return 1;
	return 0;
}
int handel_quotes(const char **p, QuoteWordParserState *state)
{
	if (**p == '\\' && *(*p + 1) != '\0')
	{
		state->buffer[state->buffer_index++] = *(*p + 1);
		(*p) += 2;
		return (1);
	}
	else if ((**p == '"' || **p == '\'') && (state->in_quotes == 0 || **p == state->quote_char))
	{
		if (**p == '"')
		{
			state->double_quotes = 1;
		}
		handle_quotes_and_wordsv2(p, &state);
		(*p)++;
		return (1);
	}
	return (0);
}

int dollar(const char **p, QuoteWordParserState *state)
{
	char *pid_str;
	int i;

	i = 0;
	if (state->start[0] == '$')
	{
		pid_str = ft_itoa(g_data.pid);
		while (pid_str[i])
			state->buffer[state->buffer_index++] = pid_str[i++];
		(*p)++;
		return (free(pid_str), 1);
	}
	else if (state->start[0] == '?')
	{
		state->expanded_value = ft_itoa(g_data.exit_status);
		while (state->expanded_value[i])
			state->buffer[state->buffer_index++] = state->expanded_value[i++];
		free(state->expanded_value);
		state->expanded_value = NULL;
		g_data.exit_status = 0;
		(*p)++;
		state->expandit = 1;
	}
	return (0);
}

int handel_dollar_expand(const char **p, QuoteWordParserState *state)
{
	if (dollar(p, state))
		return (1);
	return (0);
}

int check_ambiguous(Token *tokens, int *num_tokens, QuoteWordParserState *state)
{
	if (is_ambiguous(state->expanded_value) && *num_tokens > 0 && (tokens[*num_tokens - 1].type == TOKEN_REDIRECT_OUT || tokens[*num_tokens - 1].type == TOKEN_APPEND_OUT || tokens[*num_tokens - 1].type == TOKEN_REDIRECT_IN))
	{
		free(state->expanded_value);
		add_token(tokens, num_tokens, TOKEN_AMBIGUOUS, "?");
		return (1);
	}
	return (0);
}

int end_dollar(const char **p, QuoteWordParserState *state)
{
	if (handel_dollar_expand(p, state))
		return (1);
	return (0);
}

int add_to(const char **p, Token *tokens, int *num_tokens,
		   QuoteWordParserState *state)
{
	int i;

	i = 0;
	while (ft_isalnum(**p) || **p == '_')
		(*p)++;
	if (*p > state->start)
	{
		state->expanded_value = expand_variable(state->start, *p - state->start,
												state->env);
		if (check_ambiguous(tokens, num_tokens, state))
			return (1);
		else if (state->expandit == 0)
		{
			if (state->double_quotes || check_space(state->expanded_value) == 0)
			{
				while (state->expanded_value[i] && state->start[0] != '?')
					state->buffer[state->buffer_index++] = state->expanded_value[i++];
				state->buffer[state->buffer_index] = '\0';
				free(state->expanded_value);
			}
			else if (!state->double_quotes)
			{
				ft_buffer_split(tokens, num_tokens, state->expanded_value);
				return (free(state->expanded_value), 1);
			}
		}
	}
	else
		state->buffer[state->buffer_index++] = '$';
	return 0;
}

void handle_quotes_and_words(const char **p, Token *tokens, int *num_tokens,
							 QuoteWordParserState *state)
{
	state->buffer_index = 0;
	while (ft_isspace(**p) && !state->in_quotes)
		(*p)++;
    while (**p != '\0' && (state->in_quotes || (!ft_isspace(**p) && !ft_strchr("<>|&()",**p))))
	{
		if (handel_quotes(p, state) == 1)
			continue;
		if (**p == '$' && *(*p + 1) != '\0' && state->quote_char != '\'')
		{
			(*p)++;
			state->start = *p;
			if (end_dollar(p, state))
				continue;
			if (add_to(p, tokens, num_tokens, state))
				return;
		}
		else
			state->buffer[state->buffer_index++] = *(*p)++;
	}
	if (state->buffer_index == 0 && state->in_quotes)
	    state->buffer[0] = '\0';
	state->buffer[state->buffer_index] = '\0';

	if (state->buffer_index >= 0 && state->flag == 0)
		// Only add token if buffer is not empty
		add_token(tokens, num_tokens, TOKEN_WORD, state->buffer);
}

void hanv3(const char **p, char *special, TokenType *type)
{
	if (**p == '&')
	{
		if (*(*p + 1) == '&')
		{
			*type = TOKEN_AND;
			special[0] = '&';
			special[1] = '&';
			special[2] = '\0';
			(*p)++;
		}
		else
		{
			special[0] = '&';
			special[1] = '\0';
			*type = TOKEN_BACKGROUND;
		}
	}
}

void handle_v2(const char **p, char *special, TokenType *type)
{
	if (**p == '>')
	{
		if (*(*p + 1) == '>')
		{
			special[0] = '>';
			*type = TOKEN_APPEND_OUT;
			(*p)++;
		}
		else if (**p == '>' && *(*p + 1) != '>' && *(*p + 1) != '#')
			*type = TOKEN_REDIRECT_OUT;
		else if (**p == '>' && *(*p + 1) == '#')
			*type = TOKEN_HASHTAG;
	}
}

int check_here_doc_expand(const char **p)
{
	const char *str;
	str = (*p + 2);
	while (ft_isspace(*str))
		str++;
	if ((*str) == '"' || (*str) == '\'')
		return 1;
	return 0;
}
void doc_or_in(const char **p, TokenType *type, char *special)
{
	if (*(*p + 1) == '<')
	{
		special[0] = '<';
		special[1] = '<';
		special[2] = '\0';

		if (check_here_doc_expand(p))
			*type = TOKEN_HERE_DOC_NO;
		else
			*type = TOKEN_HERE_DOC;
		(*p)++;
	}
	else if (**p == '<' && *(*p + 1) != '<' && *(*p + 1) != '#')
	{
		*type = TOKEN_REDIRECT_IN;
	}
	else if (**p == '<' && *(*p + 1) == '#')
		*type = TOKEN_HASHTAG;
}

void handel_q(const char **p, TokenType type, Token *tokens, int *num_tokens)
{
	char *sp;

	if (**p == '(')
	{
		type = TOKEN_LPR;
		sp = " ( ";
		(*p)++;
		add_token(tokens, num_tokens, type, sp);
		return;
	}
	else if (**p == ')')
	{
		type = TOKEN_RPR;
		sp = " ) ";
		(*p)++;
		add_token(tokens, num_tokens, type, sp);
		return;
	}
}

void handle_special_characters(const char **p, Token *tokens,
							   int *num_tokens)
{
	TokenType type;
	char special[4];

	type = TOKEN_WORD;
	if (**p == '|' || **p == '<' || **p == '>' || **p == '&')
	{
		special[0] = **p;
		special[1] = '\0';
		type = TOKEN_WORD;
		if (**p == '|' && *(*p + 1) != '|')
			type = TOKEN_PIPE;
		else if (**p == '|' && *(*p + 1) == '|')
			type = TOKEN_OR;
		else if (**p == '&')
			hanv3(p, special, &type);
		else if (**p == '<')
			doc_or_in(p, &type, special);
		else if (**p == '>')
			handle_v2(p, special, &type);
		add_token(tokens, num_tokens, type, special);
		(*p)++;
		return;
	}
	else
		handel_q(p, type, tokens, num_tokens);
}

int lex(const char *input, Token *tokens, int *num_tokens, t_env_node *env)
{
	const char *p;
	QuoteWordParserState state;

	p = input;
	state.in_quotes = 0;
	state.quote_char = '\0';
	state.status = true;
	state.buffer_index = 0;
	state.expanded_value = NULL;
	state.double_quotes = 0;
	state.expandit = 0;
	state.buffer_index = 0;
	state.flag = 0;
	state.start = NULL;
	state.env = env;
	ft_memset(state.buffer, 0, sizeof(state.buffer));
	while (*p)
	{
		skip_whitespace(&p);
		handle_special_characters(&p, tokens, num_tokens);
		handle_quotes_and_words(&p, tokens, num_tokens, &state);
	}
	// Check if quotes are not closed
	if (state.in_quotes)
	{
		printf("Syntax error: unclosed quote '%c'\n", state.quote_char);
		return state.in_quotes;
	}
	add_token(tokens, num_tokens, TOKEN_END, "");
	return 0;
}