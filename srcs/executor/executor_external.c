/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_external.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: myokono <myokono@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 19:48:16 by myokono           #+#    #+#             */
/*   Updated: 2025/04/07 19:50:42 by myokono          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	handle_execve_error(char *exec_path)
{
	int	code;

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

static void	handle_child_process(t_command *cmd, t_shell *shell, \
		char *exec_path)
{
	if (cmd->input_fd != STDIN_FILENO)
	{
		if (dup2(cmd->input_fd, STDIN_FILENO) == -1)
			system_error("dup2");
		close(cmd->input_fd);
	}
	if (cmd->output_fd != STDOUT_FILENO)
	{
		if (dup2(cmd->output_fd, STDOUT_FILENO) == -1)
			system_error("dup2");
		close(cmd->output_fd);
	}
	default_signals();
	execve(exec_path, cmd->args, shell->env_array);
	handle_execve_error(exec_path);
}

static int	wait_child_status(pid_t pid)
{
	int	status;

	if (waitpid(pid, &status, 0) == -1)
		return (system_error("waitpid"), ERROR);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGINT)
			write(STDOUT_FILENO, "\n", 1);
		return (128 + WTERMSIG(status));
	}
	return (ERROR);
}

int	execute_external_standalone(t_command *cmd, t_shell *shell)
{
	pid_t	pid;
	char	*exec_path;

	exec_path = find_executable(cmd->args[0], shell->env_list);
	if (!exec_path)
	{
		command_error(cmd->args[0], "command not found");
		return (127);
	}
	ignore_signals();
	pid = fork();
	if (pid == -1)
	{
		free(exec_path);
		system_error("fork");
		return (ERROR);
	}
	if (pid == 0)
		handle_child_process(cmd, shell, exec_path);
	free(exec_path);
	return (wait_child_status(pid));
}
