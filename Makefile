
TARGET  = minishell
TEST    = test_builtin
CC      = cc
CFLAGS  = -Wall -Wextra -Werror -g

LIBFT_DIR = libft
LIBFT     = $(LIBFT_DIR)/libft.a

INCLUDES  = -I. -I$(LIBFT_DIR)

SRC =   main.c \
		utils.c \
        main_exit.c \
		signal/signals.c \
		signal/signals_heredoc.c \
        input_pharse.c \
        builtin/builtin_dispatch.c \
        builtin/builtin_cd.c \
        builtin/builtin_echo.c \
        builtin/builtin_env.c \
        builtin/builtin_env_utils.c \
        builtin/builtin_exit.c \
        builtin/builtin_export.c \
        builtin/builtin_pwd.c \
        builtin/builtin_unset.c \
        builtin/builtin_utils.c

# test uses the same files minus main.c, plus test_builtin.c
TEST_SRC = test_builtin.c \
           main_exit.c \
		   utils.c \
		   signal/signals.c \
		   signal/signals_heredoc.c \
           builtin/builtin_dispatch.c \
           builtin/builtin_cd.c \
           builtin/builtin_echo.c \
           builtin/builtin_env.c \
           builtin/builtin_env_utils.c \
           builtin/builtin_exit.c \
           builtin/builtin_export.c \
           builtin/builtin_pwd.c \
           builtin/builtin_unset.c \
           builtin/builtin_utils.c

OBJ      = $(SRC:.c=.o)
TEST_OBJ = $(TEST_SRC:.c=.o)

all: $(LIBFT) $(TARGET)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -L$(LIBFT_DIR) -lft -lreadline -o $(TARGET)

# ── test target ──────────────────────────────────────────────────────────────
test: $(LIBFT) $(TEST_OBJ)
	$(CC) $(CFLAGS) $(TEST_OBJ) -L$(LIBFT_DIR) -lft -lreadline -o $(TEST)
	@echo ""
	@echo "Running tests..."
	@echo ""
	./$(TEST)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	rm -f $(OBJ) $(TEST_OBJ)

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean
	rm -f $(TARGET) $(TEST)

re: fclean all

.PHONY: all clean fclean re test