CC = gcc
CFLAGS = -Wall -Wextra -pthread -g
LDLIBS = -lssl -lcrypto
TARGET = https_client
SRC = https_client.c
BIN_DIR ?= ../bin

HOSTS ?= www.example.com www.google.com www.wikipedia.org www.kernel.org www.openssl.org
ARGS ?= -c $(HOSTS)

.PHONY: all run run-seq run-conc clean

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(SRC)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(SRC) $(LDLIBS)

run: $(BIN_DIR)/$(TARGET)
	$(BIN_DIR)/$(TARGET) $(ARGS)

run-seq: $(BIN_DIR)/$(TARGET)
	$(BIN_DIR)/$(TARGET) -s $(HOSTS)

run-conc: $(BIN_DIR)/$(TARGET)
	$(BIN_DIR)/$(TARGET) -c $(HOSTS)

clean:
	rm -f $(BIN_DIR)/$(TARGET)