# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: myokono <myokono@student.42tokyo.jp>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/01/01 00:00:00 by user              #+#    #+#              #
#    Updated: 2025/04/07 13:20:40 by myokono          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror
INCLUDES = -I./includes -I./libft
LIBS = -L./libft -lft -lreadline

# ソースファイルディレクトリ
SRCS_DIR = srcs
PARSER_DIR = $(SRCS_DIR)/parser
EXECUTOR_DIR = $(SRCS_DIR)/executor
BUILTINS_DIR = $(SRCS_DIR)/builtins
UTILS_DIR = $(SRCS_DIR)/utils
SIGNALS_DIR = $(SRCS_DIR)/signals

# ソースファイル
SRCS = $(SRCS_DIR)/main.c \
	$(PARSER_DIR)/lexer.c \
	$(PARSER_DIR)/parser.c \
	$(PARSER_DIR)/token.c \
	$(PARSER_DIR)/words.c \
	$(PARSER_DIR)/single_quote.c \
	$(PARSER_DIR)/double_quote.c \
	$(PARSER_DIR)/env.c \
	$(PARSER_DIR)/command.c \
	$(EXECUTOR_DIR)/executor.c \
	$(EXECUTOR_DIR)/executor_builtin.c \
	$(EXECUTOR_DIR)/pipe.c \
	$(EXECUTOR_DIR)/redirects.c \
	$(BUILTINS_DIR)/echo.c \
	$(BUILTINS_DIR)/cd.c \
	$(BUILTINS_DIR)/pwd.c \
	$(BUILTINS_DIR)/export.c \
	$(BUILTINS_DIR)/export_sort.c \
	$(BUILTINS_DIR)/unset.c \
	$(BUILTINS_DIR)/env.c \
	$(BUILTINS_DIR)/exit.c \
	$(UTILS_DIR)/env_utils.c \
	$(UTILS_DIR)/env_init.c \
	$(UTILS_DIR)/error.c \
	$(UTILS_DIR)/memory.c \
	$(UTILS_DIR)/utils.c \
	$(SIGNALS_DIR)/signals.c

# オブジェクトファイル
OBJS = $(SRCS:.c=.o)

# libftのパス
LIBFT = ./libft/libft.a

# ディレクトリ作成コマンド
MKDIR = mkdir -p

# ルール定義
all: $(NAME)

# libftをコンパイル
$(LIBFT):
	$(MAKE) -C ./libft

# オブジェクトファイル生成
%.o: %.c
	@$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# 実行ファイル生成
$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(INCLUDES) $(LIBS) -o $(NAME)
	@echo "$(NAME) has been created!"

test: $(NAME)
	@echo "Running tests..."
	@bash ./mytester.sh
	@echo "Tests completed!"

clean:
	$(MAKE) -C ./libft clean
	rm -f $(OBJS)
	@echo "Object files cleaned!"

fclean: clean
	$(MAKE) -C ./libft fclean
	rm -f $(NAME)
	@echo "$(NAME) and object files cleaned!"

re: fclean all

.PHONY: all clean fclean re