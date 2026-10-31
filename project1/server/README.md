# Chat Server

This directory contains the server-side C program, `chat_server.c`.

## Requirements

- A C compiler such as GCC or Clang
- GNU Make

## Build

From this directory, run:

```sh
make
```

This builds the `chat_server` executable in the repository's `bin/` directory. To remove the executable, run:

```sh
make clean
```

You can override the compiler or build flags, for example:

```sh
make CC=gcc CFLAGS="-Wall -Wextra -std=c11 -O2"
```

## Current status

The source is currently a starter stub: running `../../bin/chat_server` prints `Hello world`. Although it defines constants and a client structure for a chat server, it does not yet create a socket, accept clients, or exchange chat messages.

Run the current program with:

```sh
make run
```
