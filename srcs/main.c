/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: myokono <myokono@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 19:12:06 by myokono           #+#    #+#             */
/*   Updated: 2025/04/18 16:56:23 by myokono          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		g_signal_status = 0;

static t_shell	*init_shell(char **envp)
{
	t_shell	*shell;

	shell = malloc(sizeof(t_shell));
	if (!shell)
		return (NULL);
	shell->tokens = NULL;
	shell->commands = NULL;
	shell->env_list = init_env_list(envp);
	shell->env_array = env_list_to_array(shell->env_list);
	if (!shell->env_list || !shell->env_array)
	{
		free_shell(shell);
		return (NULL);
	}
	shell->exit_status = 0;
	shell->running = 1;
	return (shell);
}

static char	*get_last_argument(t_command *commands)
{
	t_command	*cmd;
	char		**args;
	int			i;

	cmd = commands;
	if (!cmd)
		return (NULL);
	while (cmd && cmd->next)
		cmd = cmd->next;
	args = cmd->args;
	if (!args || !args[0])
		return (NULL);
	i = 0;
	while (args[i + 1])
		i++;
	return (ft_strdup(args[i]));
}

int	process_input(char *input, t_shell *shell)
{
	char	*last_arg;

	if (!input || ft_strlen(input) == 0)
		return (SUCCESS);
	add_history(input);
	shell->tokens = tokenize(input, shell);
	if (shell->tokens == NULL && shell->exit_status == NO_TOKEN)
		return (SUCCESS);
	if (shell->tokens == NULL || parse(shell) != SUCCESS)
		return (ERROR);
	shell->exit_status = execute_commands(shell);
	last_arg = get_last_argument(shell->commands);
	if (last_arg)
	{
		add_env_node(&shell->env_list, "_", last_arg);
		update_env_array(shell);
		free(last_arg);
	}
	return (shell->exit_status);
}

static int	shell_loop(t_shell *shell)
{
	char	*input;
	int		status;

	status = 0;
	while (shell->running)
	{
		setup_signals();
		if (g_signal_status != 0)
		{
			shell->exit_status = g_signal_status + 128;
			g_signal_status = 0;
			continue ;
		}
		input = readline("minishell$ ");
		if (input == NULL)
		{
			printf("exit\n");
			break ;
		}
		shell->exit_status = process_input(input, shell);
		status = shell->exit_status;
		free_tokens(shell->tokens);
		shell->tokens = NULL;
		free_commands(shell->commands);
		shell->commands = NULL;
		free(input);
	}
	return (status);
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	*shell;
	int		status;

	setup_signals();
	shell = init_shell(envp);
	if (!shell)
		return (ERROR);
	if (argc >= 3 && ft_strcmp(argv[1], "-c") == 0)
		status = process_input(ft_strdup(argv[2]), shell);
	else
		status = shell_loop(shell);
	free_shell(shell);
	rl_clear_history();
	return (status);
}
