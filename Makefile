CC = gcc
CFLAGS = -O3 -Wall -Werror
LDFLAGS = -lncurses -o bin/pixel

all:
	$(CC) $(CFLAGS) $(LDFLAGS) pixel.c
