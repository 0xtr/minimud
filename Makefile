CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm
LDLIBS = -lbsd -isystem src/crypto/ -lsqlite3

all:
	$(CC) $(LDLIBS) $(LDFLAGS) server.c -o server $(CFLAGS) -std=c11
