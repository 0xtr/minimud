CC = gcc
LD = -lm -lbsd -lsqlite3
CFLAGS = -Wall -Wextra -g -O2 -std=c11

BUILD_DIR = ./build
SRC_DIR = ./src
BINARY_NAME = minimud
EXCLUDES = 

SRCS = $(shell find $(SRC_DIR) -name *.c $(foreach exclude, $(EXCLUDES), -and -not -path "*$(exclude)*"))
OBJS = $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)
MKDIR_P = mkdir -p -v

.PHONY: all clean # stops file name conflicts

$(BINARY_NAME): $(OBJS)
	$(CC) $(LD) $(CFLAGS) $(SRCS) -o $@

$(BUILD_DIR)/%.c.o: %.c # match via wildcard %
	@echo "Compiling $<"
	$(MKDIR_P) $(dir $@)
	$(CC) $(LD) $(CFLAGS) -c $< -o $@

clean:
	/bin/rm -fv vgcore*
	/bin/rm -frv $(BUILD_DIR)
	/bin/rm -fv $(BINARY_NAME)

-include $(DEPS)
