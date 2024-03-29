# Compiler
CC = gcc

# Source and object directories
SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = bin

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Executable name
EXECUTABLE = $(BIN_DIR)/least

# Main target
all: $(BIN_DIR) $(EXECUTABLE)

# Rule to create the bin directory
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Rule to create the obj directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Rule to build object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -std=c17 -O2 -c $< -o $@

# Rule to build the executable
$(EXECUTABLE): $(OBJ_FILES)
	$(CC) $^ -lncurses -o $@

# Clean target
clean:
	rm -f $(OBJ_DIR)/*.o $(EXECUTABLE)

.PHONY: all clean
