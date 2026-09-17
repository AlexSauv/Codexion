CC = cc
CFLAGS = -Wall -Wextra -Werror -MMD -MP -g3
NAME = codexion
SRCS = parsing.c time_handler.c codex.c codexion.c
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