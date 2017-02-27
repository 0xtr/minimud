CC = gcc
LD = -lm -lbsd -isystem src/crypto/ -lsqlite3
CFLAGS = -Wall -Wextra -g -O2 -std=c11

BUILD_DIR = ./build
SRC_DIR = ./src
BINARY_NAME = minimud
#EXCLUDES = crypto
EXCLUDES = 

SRCS = $(shell find $(SRC_DIR) -name *.c $(foreach exclude, $(EXCLUDES), -and -not -path "*$(exclude)*"))
OBJS = $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)
INC_DIRS = $(shell find ./src -type d $(foreach exclude, $(EXCLUDES), -and -not -path "*$(exclude)*"))
MKDIR_P = mkdir -p -v

.PHONY: all clean # stops file name conflicts

$(BINARY_NAME): $(OBJS)
	$(CC) $(LD) $(CFLAGS) $(SRCS) -o $@

$(BUILD_DIR)/%.c.o: %.c # match via wildcard %
	@echo "Compiling $<"
	$(MKDIR_P) $(dir $@)
	$(CC) $(LD) $(CFLAGS) -c $< -o $@

clean:
	/bin/rm -v vgcore*
	/bin/rm -rv $(BUILD_DIR)
	/bin/rm -v $(BINARY_NAME)

-include $(DEPS)
