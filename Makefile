CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L

TARGET_ARENA = arena
SRCS_ARENA = arena.c

all: $(TARGET_ARENA)

$(TARGET_ARENA): $(SRCS_ARENA) arena.h
	$(CC) $(CFLAGS) $(SRCS_ARENA) -o $(TARGET_ARENA)

clean:
	rm $(TARGET_ARENA)

.PHONY: all clean
