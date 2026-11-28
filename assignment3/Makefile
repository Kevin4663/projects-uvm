CC = gcc
CFLAGS = -W -pthread -g
TARGET = asn3
SRC = main.c
BIN_DIR ?= ../bin

.PHONY: all run clean

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(SRC)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(SRC)

run: $(BIN_DIR)/$(TARGET)
	$(BIN_DIR)/$(TARGET)

clean:
	rm -f $(BIN_DIR)/$(TARGET)