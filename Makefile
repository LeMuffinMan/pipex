# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/01 16:29:17 by oelleaum          #+#    #+#              #
#    Updated: 2025/03/08 15:24:43 by oelleaum         ###   ########lyon.fr    #
#                                                                              #
# **************************************************************************** #

NAME=pipex
BONUS_NAME=pipex_bonus

CC=cc
CFLAGS=-Wall -Werror -Wextra
INC=-I include
INC_LIBFT=-I libft/include
INC_BONUS=-I bonus/include

SRC_DIR=src
OBJ_DIR=obj

LIBFT_SRC_DIR = libft/src
LIBFT_OBJ_DIR = libft/obj
BONUS_SRC_DIR = bonus/src
BONUS_OBJ_DIR = bonus/obj

# Pour staisfaire le meilleur product manager du monde : TODO
# Faire une variable pour le chemin 
# Kenzo : faire la compil de la libft dans le makefile de la libft + y mettre la abs le printf et le gnl
LIBFT_SRC_FILES = \
    $(LIBFT_SRC_DIR)/ft_atoi.c \
    $(LIBFT_SRC_DIR)/ft_bzero.c \
    $(LIBFT_SRC_DIR)/ft_calloc.c \
    $(LIBFT_SRC_DIR)/ft_isalnum.c \
    $(LIBFT_SRC_DIR)/ft_isalpha.c \
    $(LIBFT_SRC_DIR)/ft_isascii.c \
    $(LIBFT_SRC_DIR)/ft_isdigit.c \
    $(LIBFT_SRC_DIR)/ft_isprint.c \
    $(LIBFT_SRC_DIR)/ft_itoa.c \
    $(LIBFT_SRC_DIR)/ft_lstadd_back_bonus.c \
    $(LIBFT_SRC_DIR)/ft_lstadd_front_bonus.c \
    $(LIBFT_SRC_DIR)/ft_lstclear_bonus.c \
    $(LIBFT_SRC_DIR)/ft_lstdelone_bonus.c \
    $(LIBFT_SRC_DIR)/ft_lstiter_bonus.c \
    $(LIBFT_SRC_DIR)/ft_lstlast_bonus.c \
    $(LIBFT_SRC_DIR)/ft_lstnew_bonus.c \
    $(LIBFT_SRC_DIR)/ft_lstsize_bonus.c \
    $(LIBFT_SRC_DIR)/ft_memchr.c \
    $(LIBFT_SRC_DIR)/ft_memcmp.c \
    $(LIBFT_SRC_DIR)/ft_memcpy.c \
    $(LIBFT_SRC_DIR)/ft_memmove.c \
    $(LIBFT_SRC_DIR)/ft_memset.c \
    $(LIBFT_SRC_DIR)/ft_putchar_fd.c \
    $(LIBFT_SRC_DIR)/ft_putendl_fd.c \
    $(LIBFT_SRC_DIR)/ft_putnbr_fd.c \
    $(LIBFT_SRC_DIR)/ft_putstr_fd.c \
    $(LIBFT_SRC_DIR)/ft_split.c \
    $(LIBFT_SRC_DIR)/ft_strdup.c \
    $(LIBFT_SRC_DIR)/ft_strchr.c \
    $(LIBFT_SRC_DIR)/ft_striteri.c \
    $(LIBFT_SRC_DIR)/ft_strjoin.c \
    $(LIBFT_SRC_DIR)/ft_strlcat.c \
    $(LIBFT_SRC_DIR)/ft_strlcpy.c \
    $(LIBFT_SRC_DIR)/ft_strmapi.c \
    $(LIBFT_SRC_DIR)/ft_strnstr.c \
    $(LIBFT_SRC_DIR)/ft_strrchr.c \
    $(LIBFT_SRC_DIR)/ft_strtrim.c \
    $(LIBFT_SRC_DIR)/ft_substr.c \
    $(LIBFT_SRC_DIR)/ft_strlen.c \
    $(LIBFT_SRC_DIR)/ft_strncmp.c \
    $(LIBFT_SRC_DIR)/ft_tolower.c \
    $(LIBFT_SRC_DIR)/ft_toupper.c \
    $(LIBFT_SRC_DIR)/ft_split.c \
    $(LIBFT_SRC_DIR)/get_next_line.c \
    $(LIBFT_SRC_DIR)/get_next_line_utils.c \
    $(LIBFT_SRC_DIR)/ft_printf.c \
    $(LIBFT_SRC_DIR)/hexa.c \
    $(LIBFT_SRC_DIR)/int.c \
    $(LIBFT_SRC_DIR)/ptr.c

LIBFT_OBJ_FILES = $(LIBFT_SRC_FILES:.c=.o)

BONUS_SRC_FILES = \
    bonus/src/errors_bonus.c \
    bonus/src/exec_bonus.c \
    bonus/src/init_bonus.c \
    bonus/src/parsing_bonus.c \
    bonus/src/pipex_bonus.c \
    bonus/src/utils_bonus.c

BONUS_OBJ_FILES = $(addprefix $(BONUS_OBJ_DIR)/, $(subst bonus/src/, , $(BONUS_SRC_FILES:.c=.o)))

SRC_FILES = \
    src/pipex.c \
    src/utils.c \
    src/errors.c \
    src/exec.c \
    src/init.c \
    src/parsing.c

OBJ_FILES = $(SRC_FILES:.c=.o)
OBJ = $(addprefix $(OBJ_DIR)/,$(OBJ_FILES))
BONUS = $(addprefix $(BONUS_OBJ_DIR)/,$(BONUS_OBJ_FILES))

LIBFT_A = libft/libft.a

GREEN=\033[32m
RED=\033[31m
RESET=\033[0m

all: $(NAME)

$(NAME): $(OBJ) $(LIBFT_A) Makefile libft/Makefile libft/include/libft.h
	$(CC) $(CFLAGS) $(OBJ) $(LIBFT_A) $(LIBFT_FLAGS) -o $(NAME)
	@echo 
	@echo "$(GREEN)compilation successful ✅ $(NAME)$(RESET)"
	@echo 

$(LIBFT_A): $(LIBFT_SRC_FILES) FORCE
	@$(MAKE) --no-print-directory -C libft

$(OBJ_DIR)/%.o: %.c Makefile ./include/pipex.h
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) $(INC_LIBFT) -I . -c $< -o $@

bonus: $(BONUS_NAME)

$(BONUS_OBJ_DIR)/%.o: bonus/src/%.c ./bonus/include/pipex_bonus.h Makefile libft/Makefile
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC_BONUS) $(INC_LIBFT) -c $< -o $@

$(BONUS_NAME): $(BONUS_OBJ_FILES) $(LIBFT_A) ./bonus/include/pipex_bonus.h
	$(CC) $(CFLAGS) $(BONUS_OBJ_FILES) $(LIBFT_A) $(LIBFT_FLAGS) -o $(BONUS_NAME)
	@echo
	@echo "$(GREEN)compilation successful ✅ $(BONUS_NAME)$(RESET)"
	@echo

clean:
	rm -rf $(OBJ_DIR)/*

fclean: clean
	rm -f $(NAME) $(BONUS_NAME)
	rm -f $(LIBFT_A)

re: fclean all

FORCE:
.PHONY: all re clean fclean bonus
