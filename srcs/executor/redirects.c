/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirects.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ryabuki <ryabuki@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 12:54:49 by myokono           #+#    #+#             */
/*   Updated: 2025/04/12 21:06:21 by ryabuki          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	setup_redirect_in(t_command *cmd, char *filename)
{
	int	fd;

	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		system_error(filename);
		return (ERROR);
	}
	if (cmd->input_fd != STDIN_FILENO)
		close(cmd->input_fd);
	cmd->input_fd = fd;
	return (SUCCESS);
}

static int	setup_redir_out(t_command *cmd, char *filename, t_token_type type)
{
	int	fd;
	int	flags;

	flags = O_WRONLY | O_CREAT;
	if (type == TOKEN_REDIRECT_OUT)
		flags |= O_TRUNC;
	else
		flags |= O_APPEND;
	fd = open(filename, flags, 0644);
	if (fd == -1)
	{
		system_error(filename);
		return (ERROR);
	}
	if (cmd->output_fd != STDOUT_FILENO)
		close(cmd->output_fd);
	cmd->output_fd = fd;
	return (SUCCESS);
}

static int	setup_redir(t_command *cmd, t_token_type type, char *filename)
{
	if (type == TOKEN_REDIRECT_IN)
		return (setup_redirect_in(cmd, filename));
	else if (type == TOKEN_REDIRECT_OUT || type == TOKEN_APPEND)
		return (setup_redir_out(cmd, filename, type));
	else if (type == TOKEN_HEREDOC)
		return (setup_redirect_heredoc(cmd, filename));
	return (ERROR);
}

// int	setup_redirects(t_command *cmd)
// {
// 	t_token	*token;
// 	t_token	*next;

// 	token = cmd->redirects;
// 	while (token)
// 	{
// 		next = token->next;
// 		if (!next)
// 		{
// 			error_message("Missing filename for redirection");
// 			return (ERROR);
// 		}
// 		if (setup_redir(cmd, token->type, next->value) == ERROR)
// 			return (ERROR);
// 		token = next->next;
// 	}
// 	return (SUCCESS);
// }



// int setup_redirects(t_command *cmd)
// {
// 	t_token *token;
// 	t_token *next;
	
// 	// ステップ1: 入力リダイレクト（<）を処理
// 	token = cmd->redirects;
// 	while (token)
// 	{
// 		next = token->next;
// 		if (!next)
// 		{
// 			error_message("Missing filename for redirection");
// 			return (ERROR);
// 		}
// 		if (token->type == TOKEN_REDIRECT_IN)
// 		{
// 			if (setup_redir(cmd, token->type, next->value) == ERROR)
// 				return (ERROR);
// 		}
// 		token = next->next;
// 	}
	
// 	// ステップ2: 最後のヒアドキュメント（<<）を処理
// 	t_token *last_heredoc = NULL;
// 	t_token *last_heredoc_value = NULL;
// 	token = cmd->redirects;
// 	while (token)
// 	{
// 		next = token->next;
// 		if (!next)
// 			break;
// 		if (token->type == TOKEN_HEREDOC)
// 		{
// 			last_heredoc = token;
// 			last_heredoc_value = next;
// 		}
// 		token = next->next;
// 	}
// 	if (last_heredoc && last_heredoc_value)
// 	{
// 		if (setup_redir(cmd, last_heredoc->type, last_heredoc_value->value) == ERROR)
// 			return (ERROR);
// 	}

// 	// ステップ3: 出力リダイレクト（>、>>）を処理
// 	token = cmd->redirects;
// 	while (token)
// 	{
// 		next = token->next;
// 		if (!next)
// 			break;
		
// 		if (token->type == TOKEN_REDIRECT_OUT || token->type == TOKEN_APPEND)
// 		{
// 			if (setup_redir(cmd, token->type, next->value) == ERROR)
// 				return (ERROR);
// 		}
		
// 		token = next->next;
// 	}
	
// 	return (SUCCESS);
// }

int setup_redirects(t_command *cmd)
{
	t_token *token;
	t_token *next;

	// 1. ヒアドキュメント（<<）を前から処理
	int last_heredoc_fd = -1;

	token = cmd->redirects;
	while (token)
	{
		next = token->next;
		if (!next)
			break;
		if (token->type == TOKEN_HEREDOC)
		{
			int heredoc_fd = setup_redir_return_fd(token->type, next->value);
			if (heredoc_fd == -1)
				return (ERROR);
			if (last_heredoc_fd != -1)
				close(last_heredoc_fd);  // 前の heredoc fd は閉じる
			last_heredoc_fd = heredoc_fd;
		}
		token = next->next;
	}
	
	// 最後の heredoc の fd を input_fd に設定
	if (last_heredoc_fd != -1)
	{
		if (cmd->input_fd != STDIN_FILENO)
			close(cmd->input_fd);
		cmd->input_fd = last_heredoc_fd;
	}
	

	// 2. 出力（>、>>）
	token = cmd->redirects;
	while (token)
	{
		next = token->next;
		if (!next) break;
		if (token->type == TOKEN_REDIRECT_OUT || token->type == TOKEN_APPEND)
		{
			if (setup_redir(cmd, token->type, next->value) == ERROR)
				return (ERROR);
		}
		token = next->next;
	}

	// 3. 入力（<）※HEREDOC より後に処理されると上書きしてしまうので注意
	token = cmd->redirects;
	while (token)
	{
		next = token->next;
		if (!next)
		{
			error_message("Missing filename for redirection");
			return (ERROR);
		}
		if (token->type == TOKEN_REDIRECT_IN)
		{
			if (setup_redir(cmd, token->type, next->value) == ERROR)
				return (ERROR);
		}
		token = next->next;
	}

	return (SUCCESS);
}
