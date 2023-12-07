CC = gcc
CFLAGS = -Wall -Wextra -std=c11
DFLAGS = -g -DDEBUG -Wall -Wextra -Werror -std=c11
SRC_DIR = .
BUILD_DIR = build
EXE_NAME = compiler

all: $(EXE_NAME) clean

debug:
	$(CC) $(DFLAGS) -o $(EXE_NAME)_debug $(SRC_DIR)/*.c

# Get all .c files in the source directory
SRCS := $(wildcard $(SRC_DIR)/*.c)

# Derive object file names from source files
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Target: build the executable
$(EXE_NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Target: build object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Target: create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

zip: dos2unix clean
	zip -r xsalta01.zip *.c *.h Makefile rozdeleni dokumentace.pdf rozsireni

dos2unix:
	dos2unix *.c *.h Makefile rozdeleni rozsireni

# Target: clean up
clean:
	rm -rf $(BUILD_DIR)

# .PHONY rule to prevent conflicts with file names
.PHONY: clean dos2unix zip debug

