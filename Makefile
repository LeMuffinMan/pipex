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
#BONUS_NAME=pipex_bonus

CC=cc
CFLAGS=-Wall -Werror -Wextra
INC=-I include
INC_LIBFT=-I libft/include
#INC_BONUS=-I bonus/include

SRC_DIR=src
OBJ_DIR=obj

LIBFT_SRC_DIR = libft/src
LIBFT_OBJ_DIR = libft/obj
#BONUS_SRC_DIR = bonus/src

# Pour staisfaire le meilleur product manager du monde : TODO
# Faire une variable pour le chemin 
# Kenzo : faire la compil de la libft dans le makefile de la libft + y mettre la abs le printf et le gnl
LIBFT_SRC_FILES = \
    libft/src/ft_atoi.c \
    libft/src/ft_bzero.c \
    libft/src/ft_calloc.c \
    libft/src/ft_isalnum.c \
    libft/src/ft_isalpha.c \
    libft/src/ft_isascii.c \
    libft/src/ft_isdigit.c \
    libft/src/ft_isprint.c \
    libft/src/ft_itoa.c \
    libft/src/ft_lstadd_back_bonus.c \
    libft/src/ft_lstadd_front_bonus.c \
    libft/src/ft_lstclear_bonus.c \
    libft/src/ft_lstdelone_bonus.c \
    libft/src/ft_lstiter_bonus.c \
    libft/src/ft_lstlast_bonus.c \
    libft/src/ft_lstnew_bonus.c \
    libft/src/ft_lstsize_bonus.c \
    libft/src/ft_memchr.c \
    libft/src/ft_memcmp.c \
    libft/src/ft_memcpy.c \
    libft/src/ft_memmove.c \
    libft/src/ft_memset.c \
    libft/src/ft_putchar_fd.c \
    libft/src/ft_putendl_fd.c \
    libft/src/ft_putnbr_fd.c \
    libft/src/ft_putstr_fd.c \
    libft/src/ft_split.c \
    libft/src/ft_strdup.c \
    libft/src/ft_strchr.c \
    libft/src/ft_striteri.c \
    libft/src/ft_strjoin.c \
    libft/src/ft_strlcat.c \
    libft/src/ft_strlcpy.c \
    libft/src/ft_strmapi.c \
    libft/src/ft_strnstr.c \
    libft/src/ft_strrchr.c \
    libft/src/ft_strtrim.c \
    libft/src/ft_substr.c \
    libft/src/ft_strlen.c \
    libft/src/ft_strncmp.c \
    libft/src/ft_tolower.c \
    libft/src/ft_toupper.c \
    libft/src/ft_split.c \
    libft/src/get_next_line.c \
    libft/src/get_next_line_utils.c \
    libft/src/ft_printf.c \
    libft/src/hexa.c \
    libft/src/int.c \
    libft/src/ptr.c

LIBFT_OBJ_FILES = $(LIBFT_SRC_FILES:.c=.o)

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

clean:
	rm -rf $(OBJ_DIR)/*

fclean: clean
	rm -f $(NAME) $(BONUS_NAME)
	rm -f $(LIBFT_A)

re: fclean all

FORCE:
.PHONY: all re clean fclean bonus
