CC = gcc
CFLAGS = -Wall -Wextra -Werror --std=c99

arena: arena.c
	$(CC) $(CFLAGS) -o $@ $<
