# Chat Client

This directory contains the client-side C program, `chat_client.c`.

## Requirements

- A C compiler such as GCC or Clang
- GNU Make
- A running chat server

## Build

From this directory, run:

```sh
make
```

This builds the `chat_client` executable in the repository's `bin/` directory. To remove the executable, run:

```sh
make clean
```

You can override the compiler or build flags, for example:

```sh
make CC=gcc CFLAGS="-Wall -Wextra -std=c11 -O2"
```

## Run

Provide the server's IPv4 address and port:

```sh
../../bin/chat_client <Server_IP> <Port>
```

For example:

```sh
../../bin/chat_client 127.0.0.1 8080
```

You can also build and run in one step with Make:

```sh
make run ARGS="127.0.0.1 8080"
```

Enter messages and press Enter to send them. Enter `/exit` or `/quit` to close the client.
