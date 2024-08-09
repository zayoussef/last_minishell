/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   her_doc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yozainan <yozainan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 17:20:21 by elchakir          #+#    #+#             */
/*   Updated: 2024/08/08 22:48:43 by yozainan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	remove_quotes(char *str)
{
	int	i;
	int	j;

	if (!str)
		return ;
	i = 0;
	j = 0;
	while (str[i])
	{
		if (str[i] != '\'' && str[i] != '"')
		{
			str[j] = str[i];
			j++;
		}
		i++;
	}
	str[j] = '\0';
}

void	expand_variable_in_str(char **temp_result, char **p, t_data *data)
{
	char	*start;
	char	*expanded;
	char	*tmp;

	start = *p;
	if (**p == '$')
	{
		tmp = ft_itoa(data->pid);
		ft_strncpy(*temp_result + ft_strlen(*temp_result), tmp, ft_strlen(tmp));
		(*p)++;
		free(tmp);
	}
	else
	{
		while (ft_isalnum(**p) || **p == '_')
			(*p)++;
		expanded = expand_variable(start, *p - start, data->env_list);
		ft_strncpy(*temp_result + ft_strlen(*temp_result), expanded,
			ft_strlen(expanded));
		free(expanded);
	}
}

void	find_when_start(char *str, t_data *data, char **result)
{
	char	*p;
	char	*temp_result;

	temp_result = malloc(strlen(str) * 2);
	if (!temp_result)
		return ;
	temp_result[0] = '\0';
	p = ft_strrch_finder(str, '$');
	while (p != NULL)
	{
		ft_strncpy(temp_result + strlen(temp_result), str, p - str);
		p++;
		expand_variable_in_str(&temp_result, &p, data);
		str = p;
		p = ft_strrch_finder(str, '$');
	}
	ft_strncpy(temp_result + ft_strlen(temp_result), str, ft_strlen(str));
	*result = temp_result;
}

void	check_limit_her_doc(Redirection *redir)
{
	Redirection	*tmp_redir;
	int			counter;

	tmp_redir = redir;
	counter = 0;
	while (tmp_redir)
	{
		if (tmp_redir->type == TOKEN_HERE_DOC
			|| tmp_redir->type == TOKEN_HERE_DOC_NO)
			counter++;
		tmp_redir = tmp_redir->next;
	}
	if (counter > 16)
	{
		ft_putstr_fd("minishell: maximum here-document count exceeded\n",
			STDERR_FILENO);
		exit(2);
	}
}
