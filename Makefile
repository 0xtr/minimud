CC = gcc
CFLAGS = -Wall -Wextra -g
//CFLAGS = -w
LDFLAGS = -lm
LDLIBS = -lbsd -isystem src/crypto/ -lsqlite3

all:
	$(CC) $(LDLIBS) $(LDFLAGS) server.c -o server $(CFLAGS) -std=c11 
