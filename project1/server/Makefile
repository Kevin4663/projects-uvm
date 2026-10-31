CC ?= cc
CFLAGS ?= -Wall -Wextra -std=c11
LDFLAGS ?=

BIN_DIR ?= ../../bin
TARGET := $(BIN_DIR)/chat_server
SOURCE := chat_server.c

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SOURCE)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

run: $(TARGET)
	$(TARGET) $(ARGS)

clean:
	rm -f $(TARGET)
