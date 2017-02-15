CC = gcc
LD = -lm -lbsd -isystem src/crypto/ -lsqlite3
CFLAGS = -Wall -Wextra -g -O2 -std=c11

OBJ_DIR = obj
BINARY_NAME = minimud
OBJS = players utilities sqlite mapping commands printing .

OBJLIBS = $(foreach obj, $(OBJS),$(obj).o)
SOURCES = src $(foreach obj, $(OBJS),$(wildcard src/$(obj)/*.c))
LINKED = $(patsubst %.c,%.o, $(SOURCES))

.PHONY: all clean # stops file name conflicts

$(OBJ_DIR)/%.o: %.c # match via wildcard %
	@echo "Compiling $<"
	$(CC) $(LD) $(CFLAGS) -c $< -o $@

$(BINARY_NAME): $(LINKED)
	$(CC) $(LD) $(CFLAGS) $(LINKED) -o $@

clean:
	@echo "Cleaning $(BINARY_NAME)"
	/bin/rm $(BINARY_NAME)

