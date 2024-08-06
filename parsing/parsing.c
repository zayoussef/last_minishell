/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/11 21:36:21 by yozainan          #+#    #+#             */
/*   Updated: 2024/08/06 20:05:11 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	check_ambiguous_unexpected_tokens(Token *tokens, int i)
{
	if (((tokens[i].type == TOKEN_HERE_DOC && tokens[i + 1].type != TOKEN_WORD)
			|| (tokens[i].type == TOKEN_REDIRECT_IN && tokens[i
				+ 1].type != TOKEN_WORD)
			|| (tokens[i].type == TOKEN_REDIRECT_OUT && tokens[i
				+ 1].type != TOKEN_WORD) || (tokens[i].type == TOKEN_APPEND_OUT
				&& tokens[i + 1].type != TOKEN_WORD)) && (tokens[i
			+ 1].type != TOKEN_AMBIGUOUS))
	{
		return (printf("Syntax error: near unexpected token 2 '%s'\n",
				tokens[i].value), 1);
	}
	return (0);
}

bool	is_token_invalid(Token *tokens, int i, int nb_tokens);
bool	is_double_token_error(Token *tokens, int i);
bool	is_initial_token_error(Token *tokens, int i);
bool	is_final_token_error(Token *tokens, int i, int nb_tokens);
bool	is_invalid_sequence(Token *tokens, int i);

int	check_syntaxe(Token *tokens, int nb_tokens)
{
	int	i;

	i = 0;
	while (i < nb_tokens && tokens[i].type != TOKEN_END)
	{
		if (tokens[i].type == NOT_DEFINE)
			return (1);
		if (tokens[i].type == TOKEN_OR || tokens[i].type == TOKEN_AND
			|| tokens[i].type == TOKEN_LPR || tokens[i].type == TOKEN_BACKGROUND
			|| tokens[i].type == TOKEN_RPR)
			return (printf("Syntax error: undefined token '%s'\n",
					tokens[i].value), 1);
		if (tokens[i].type == TOKEN_PIPE && tokens[i
			- 1].type != TOKEN_AMBIGUOUS && tokens[i + 1].type == TOKEN_END)
			return (printf("Syntax error: undefined token 111 '%s'\n",
					tokens[i].value), 1);
		if (check_ambiguous_unexpected_tokens(tokens, i))
			return (1);
		if (is_double_token_error(tokens, i))
			return (1);
		if (is_initial_token_error(tokens, i))
			return (1);
		if (is_final_token_error(tokens, i, nb_tokens))
			return (1);
		if (is_invalid_sequence(tokens, i))
			return (1);
		i++;
	}
	return (0);
}

bool	is_double_token_error(Token *tokens, int i)
{
	if (tokens[i].type == tokens[i + 1].type && tokens[i].type != TOKEN_WORD
		&& tokens[i].type != TOKEN_END)
	{
		return (printf("Syntax error: unexpected token '%s'\n",
				tokens[i].value), true);
	}
	return (false);
}

bool	is_initial_token_error(Token *tokens, int i)
{
	if (i == 0 && (tokens[i].type == TOKEN_AND || tokens[i].type == TOKEN_OR
			|| tokens[i].type == TOKEN_PIPE
			|| tokens[i].type == TOKEN_BACKGROUND))
	{
		return (printf("Syntax error: unexpected token '%s'\n",
				tokens[i].value), true);
	}
	return (false);
}

bool	is_final_token_error(Token *tokens, int i, int nb_tokens)
{
	if (i == nb_tokens - 1 && (tokens[i].type == TOKEN_AND
			|| tokens[i].type == TOKEN_OR || tokens[i].type == TOKEN_PIPE
			|| tokens[i].type == TOKEN_BACKGROUND))
	{
		return (printf("Syntax error: unexpected token '%s'\n",
				tokens[i].value), true);
	}
	return (false);
}

bool	is_invalid_sequence(Token *tokens, int i)
{
	if ((tokens[i].type == TOKEN_AND && tokens[i + 1].type == TOKEN_AND)
		|| (tokens[i].type == TOKEN_OR && tokens[i + 1].type == TOKEN_OR)
		|| (tokens[i].type == TOKEN_BACKGROUND && tokens[i
			+ 1].type == TOKEN_BACKGROUND))
	{
		return printf("Syntax error: unexpected token '%s'\n", tokens[i].value),
			true;
	}
	return false;
}

bool	is_operator(Token token)
{
	return token.type == TOKEN_PIPE || token.type == TOKEN_AND
		|| token.type == TOKEN_OR;
}

void	process_operator(Token *tokens, int *i, Command **current,Command **head,t_cmd **cmd_head)
{
	add_cmd(&(*current)->cmd_lst,*cmd_head);
	add_command_to_list(head, *current);
	*current = create_command();
	if (!*current)
		return ;
	(*current)->type = tokens[*i].type;
}

bool	is_redirection(Token token)
{
	return token.type == TOKEN_REDIRECT_IN || token.type == TOKEN_REDIRECT_OUT
		|| token.type == TOKEN_APPEND_OUT || token.type == TOKEN_HERE_DOC||token.type == TOKEN_HERE_DOC_NO;
}

bool	is_parenthesis(Token *tokens, int i)
{
	return (tokens[i].type == TOKEN_LPR && tokens[i + 1].type == TOKEN_WORD)
		|| (tokens[i].type == TOKEN_RPR && tokens[i - 1].type == TOKEN_WORD
		&& i > 1);
}

void	handle_syntax_error(void)
{
	g_data.cmd->redir_erros = -1;
	write(2, "minishell: syntax error near unexpected token newline\n", 55);
}

void	process_token(Token *tokens, t_value *value, Command **current,
		Command **head,t_cmd **cmd_head)
{
	if (tokens[value->i].type == TOKEN_WORD)
	{
		handle_word(tokens, &value->i, current);
	}
	else if (is_operator(tokens[value->i]))
		process_operator(tokens, &value->i, current, head,cmd_head);
	else if (is_redirection(tokens[value->i]) && tokens[value->i + 1].type != TOKEN_AMBIGUOUS)
	{
		handle_redirection(tokens, &value->i, current);
		value->flag++;
	}
	else if (is_redirection(tokens[value->i]) && tokens[value->i + 1].type == TOKEN_AMBIGUOUS)
	{
		(*current)->type = TOKEN_AMBIGUOUS;
	}
	else if (is_parenthesis(tokens, value->i))
		value->flag++;
	else if (tokens[value->i].type != TOKEN_AMBIGUOUS)
		handle_syntax_error();
	value->i++;
}

Command	*parse(Token *tokens)
{
	Command	*head;
	t_cmd *cmd_head;
	Command	*current;
	t_value	value;

	value.flag = 0;
	value.i = 0;
	head = NULL;
	cmd_head = NULL;
	current = create_command();
	if (!current)
		return NULL;
	while (tokens[value.i].type != TOKEN_END)
	{
		process_token(tokens, &value, &current, &head,&cmd_head);
	}
	if (current && (ft_lst_size(current->cmd_lst) > 0|| value.flag > 0))
	{
		add_cmd(&current->cmd_lst,cmd_head);	
		add_command_to_list(&head, current);
	}
	return head;
}

int	ft_size(char **argv)
{
	int	i;

	if (argv == NULL)
		return 0;
	i = 0;
	while (argv[i] != NULL)
		i++;
	return i;
}