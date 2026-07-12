CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
SRCS = $(wildcard *.c)
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
