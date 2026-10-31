CC = gcc
CFLAGS = -Wall -Wextra -O2
BIN_DIR ?= ../bin
TARGETS = $(BIN_DIR)/client $(BIN_DIR)/server

.PHONY: all clean

all: $(TARGETS)

$(BIN_DIR)/client: client.c
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/server: server.c
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)
