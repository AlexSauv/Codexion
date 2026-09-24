CC = cc
CFLAGS = -Wall -Wextra -Werror -MMD -MP -g3 -pthread
NAME = codexion
SRCS = parsing.c \
	utils.c\
	time_handling.c \
	checker_state.c \
	init_codex.c \
	codexion.c \
	dongle_handling.c \
	event_handling.c \
	heap_handling.c \
	heap_init.c
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)
.PHONY: all clean fclean re

all: $(NAME)

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

-include $(DEPS)

clean:
	rm -f $(OBJS) $(DEPS)

fclean: clean
	rm -f $(NAME)

re: fclean all