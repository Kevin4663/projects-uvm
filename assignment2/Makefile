CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = getaddr
SRC = getaddr.c
BIN_DIR ?= ../bin

.PHONY: all clean

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(SRC)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(SRC)

clean:
	rm -f $(BIN_DIR)/$(TARGET)
