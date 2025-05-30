/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ryabuki <ryabuki@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:11:50 by myokono           #+#    #+#             */
/*   Updated: 2025/04/17 17:33:13 by ryabuki          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	is_valid_identifier(char *key)
{
	int	i;

	if (!key || !*key)
		return (false);
	if (!ft_isalpha(key[0]) && key[0] != '_')
		return (false);
	i = 1;
	while (key[i])
	{
		if (!ft_isalnum(key[i]) && key[i] != '_')
			return (false);
		i++;
	}
	return (true);
}

int	builtin_unset(t_command *cmd, t_shell *shell)
{
	int		i;
	int		status;
	char	*msg;

	i = 1;
	status = 0;
	while (cmd->args[i])
	{
		if (is_valid_identifier(cmd->args[i]) == false)
		{
			msg = ft_strjoin(cmd->args[i], ": not a valid identifier");
			if (msg == NULL)
				return (ERROR);
			print_error("unset", msg);
			free(msg);
			status = 1;
		}
		else
			remove_env_node(&shell->env_list, cmd->args[i]);
		i++;
	}
	update_env_array(shell);
	if (!shell->env_array)
		return (system_error("update_env_array"), ERROR);
	return (status);
}
