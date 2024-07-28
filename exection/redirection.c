/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 17:20:21 by elchakir          #+#    #+#             */
/*   Updated: 2024/07/28 23:24:57 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	add_token(Token *tokens, int *num_tokens, TokenType type, char *value)
{
	tokens[*num_tokens].type = type;
	tokens[*num_tokens].value = ft_strdup(value);
	(*num_tokens)++;
}

void	skip_whitespace(const char **p)
{
	while (ft_isspace(**p))
		(*p)++;
}

void	add_redirection(Redirection **redir_list, Redirection *redir)
{
	Redirection	*temp;

	temp = *redir_list;
	if (temp == NULL)
		*redir_list = redir;
	else
	{
		while (temp->next)
			temp = temp->next;
		temp->next = redir;
	}
}

void	next_type(Redirection **redir, Command **current)
{
	if ((*redir)->type == TOKEN_REDIRECT_IN
		|| (*redir)->type == TOKEN_APPEND_OUT
		|| (*redir)->type == TOKEN_REDIRECT_OUT)
		add_redirection(&(*current)->redirection, *redir);
	else if ((*redir)->type == TOKEN_HERE_DOC)
		add_redirection(&(*current)->heredoc, *redir);
}

void	handle_redirection(Token *tokens, int *i, Command **current)
{
	Redirection	*redir;

	if (*current == NULL || !current)
		*current = create_command();
	redir = (Redirection *)malloc(sizeof(Redirection));
	if (!redir)
		return ;
	redir->type = tokens[*i].type;
	redir->filename = tokens[++(*i)].value;
	redir->next = NULL;
	next_type(&redir, current);
}

void	finalize_command(Command **current, int *argc)
{
	if (*current)
	{
		(*current)->argv[*argc] = NULL;
		*argc = 0;
	}
}
