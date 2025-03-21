# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: oelleaum <oelleaum@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/01 16:29:17 by oelleaum          #+#    #+#              #
#    Updated: 2025/03/15 16:07:15 by oelleaum         ###   ########lyon.fr    #
#                                                                              #
# **************************************************************************** #

NAME = pipex

CC = cc
CFLAGS = -Wall -Werror -Wextra -g3
INC = -I include
INC_LIBFT = -I libft/include
INC_BONUS = -I bonus/include

SRC_DIR = src
OBJ_DIR = obj

LIBFT_SRC_DIR = libft/src
LIBFT_OBJ_DIR = libft/obj
BONUS_SRC_DIR = bonus/src
BONUS_OBJ_DIR = bonus/obj

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
    bonus/src/fd_management_bonus.c \
    bonus/src/utils_bonus.c \
    bonus/src/utils_here_doc_bonus.c

BONUS_OBJ_FILES = $(addprefix $(BONUS_OBJ_DIR)/, $(subst bonus/src/, , $(BONUS_SRC_FILES:.c=.o)))

SRC_FILES = \
    src/pipex.c \
    src/utils.c \
    src/errors.c \
    src/exec.c \
    src/init.c \
    src/parsing.c \
    src/fd_management.c

OBJ_FILES = $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))

LIBFT_A = libft/libft.a

GREEN = \033[32m
RED = \033[31m
RESET = \033[0m

# Variable to determine if we are compiling the bonus version
BONUS = 0

ifeq ($(BONUS), 1)
    SRC_FILES = $(BONUS_SRC_FILES)
    OBJ_FILES = $(BONUS_OBJ_FILES)
    INC += $(INC_BONUS)
endif

all: $(NAME)

$(NAME): $(OBJ_FILES) $(LIBFT_A) Makefile libft/Makefile libft/include/libft.h
	$(CC) $(CFLAGS) $(OBJ_FILES) $(LIBFT_A) $(LIBFT_FLAGS) -o $(NAME)
	@echo 
	@echo "$(GREEN)compilation successful ✅ $(NAME)$(RESET)"
	@echo 

$(LIBFT_A): $(LIBFT_SRC_FILES) FORCE
	@$(MAKE) --no-print-directory -C libft

$(OBJ_DIR)/%.o: %.c Makefile ./include/pipex.h
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) $(INC_LIBFT) -I . -c $< -o $@

$(BONUS_OBJ_DIR)/%.o: bonus/src/%.c ./bonus/include/pipex_bonus.h Makefile libft/Makefile
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC_BONUS) $(INC_LIBFT) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BONUS_OBJ_DIR)
	rm -rf $(LIBFT_OBJ_DIR)

fclean: clean
	rm -f $(NAME)
	rm -f $(LIBFT_A)

re: fclean all

bonus:
	@$(MAKE) BONUS=1

FORCE:
.PHONY: all re clean fclean bonus
