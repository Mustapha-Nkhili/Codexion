CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -I.
SRCS = src/main.c src/parser.c src/priority_queue.c src/init.c \
	   src/dongles.c src/logger.c  src/simulation.c  src/coder.c \
	   src/monitor.c src/sim_state.c \
	   utils/parser_utils.c utils/priority_queue_utils.c \
	   utils/dongles_utils.c utils/dongles_utils2.c
OBJS = $(SRCS:.c=.o)
NAME = codexion
HEADER = codexion.h

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
