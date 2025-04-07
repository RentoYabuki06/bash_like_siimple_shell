/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: myokono <myokono@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 20:34:01 by myokono           #+#    #+#             */
/*   Updated: 2025/04/07 20:59:30 by myokono          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	execute_external_forked(t_command *cmd, t_shell *shell)
{
	char	*exec_path;
	int		code;

	exec_path = find_executable(cmd->args[0], shell->env_list);
	if (!exec_path)
	{
		command_error(cmd->args[0], "command not found");
		return (127);
	}
	execve(exec_path, cmd->args, shell->env_array);
	system_error(exec_path);
	if (access(exec_path, F_OK) != 0)
		code = 127;
	else if (access(exec_path, X_OK) != 0)
		code = 126;
	else
		code = 1;
	free(exec_path);
	exit(code);
}

void	cleanup_pipes(t_command *commands)
{
	t_command	*cmd;

	cmd = commands;
	while (cmd)
	{
		if (cmd->input_fd != STDIN_FILENO)
		{
			close(cmd->input_fd);
			cmd->input_fd = STDIN_FILENO;
		}
		if (cmd->output_fd != STDOUT_FILENO)
		{
			close(cmd->output_fd);
			cmd->output_fd = STDOUT_FILENO;
		}
		cmd = cmd->next;
	}
}

int	setup_pipes(t_command *commands)
{
	t_command	*current;
	t_command	*next;
	int			pipe_fd[2];

	current = commands;
	while (current && current->next)
	{
		next = current->next;
		if (pipe(pipe_fd) == -1)
		{
			system_error("pipe");
			cleanup_pipes(commands);
			return (ERROR);
		}
		if (current->output_fd == STDOUT_FILENO)
			current->output_fd = pipe_fd[1];
		else
			close(pipe_fd[1]);
		if (next->input_fd == STDIN_FILENO)
			next->input_fd = pipe_fd[0];
		else
			close(pipe_fd[0]);
		current = next;
	}
	return (SUCCESS);
}

void	setup_child_io(t_command *cmd)
{
	default_signals();
	if (cmd->input_fd != STDIN_FILENO)
	{
		if (dup2(cmd->input_fd, STDIN_FILENO) == -1)
		{
			system_error("dup2");
			close(cmd->input_fd);
			exit(EXIT_FAILURE);
		}
		if (cmd->input_fd != cmd->output_fd)
			close(cmd->input_fd);
	}
	if (cmd->output_fd != STDOUT_FILENO)
	{
		if (dup2(cmd->output_fd, STDOUT_FILENO) == -1)
		{
			system_error("dup2");
			close(cmd->output_fd);
			exit(EXIT_FAILURE);
		}
		close(cmd->output_fd);
	}
}
