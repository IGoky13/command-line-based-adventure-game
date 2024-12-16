# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Source and target files
SRC = main.c
TARGET = game

# Build rules
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Clean up build files
clean:
	rm -f $(TARGET)

# Run the game
run: $(TARGET)
	./$(TARGET)

# Phony targets
.PHONY: all clean run

