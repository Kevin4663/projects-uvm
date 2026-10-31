# Assignment 1: Simple TCP Client/Server

This project demonstrates basic socket programming in C using the TCP protocol. It contains a simple client and a simple server that communicate over a network connection.

## Files

- `client.c` — connects to a remote server and sends one message.
- `server.c` — listens for incoming connections, receives a message, and echoes it back.
- `Makefile` — builds both programs.

## What the server does

`server.c`:

- Reads a port number from the command line.
- Uses `getaddrinfo()` with `AI_PASSIVE` to prepare the local address information.
- Creates a listening socket with `socket()`.
- Binds the socket to the specified port with `bind()`.
- Starts listening for incoming connections with `listen()`.
- Accepts a client connection with `accept()`.
- Resolves the client's IP address using `getnameinfo()`.
- Receives a message from the client with `recv()`.
- Sends the same message back to the client with `send()`.
- Closes the sockets when done.

This acts as a minimal echo server: it receives data and returns it to the sender.

## What the client does

`client.c`:

- Expects a server IP address and port as command-line arguments.
- Uses `getaddrinfo()` to resolve the server address.
- Creates a socket for the connection.
- Connects to the server using `connect()`.
- Prompts the user to type a message.
- Sends the message to the server with `send()`.
- Receives the server's response with `recv()`.
- Prints the echoed message.
- Closes the socket.

This acts as a simple client that sends a text message to a server and displays the server's reply.

## Build

From the `assignment1` directory, run:

```bash
make
```

This will create two executables in the repository's `bin/` directory:

- `../bin/client`
- `../bin/server`

## Run the programs

Start the server in one terminal:

```bash
../bin/server 8080
```

Then run the client in another terminal:

```bash
../bin/client 127.0.0.1 8080
```

When prompted, enter a message. The server will receive it and send it back to the client.

## Notes

- The programs use TCP sockets (`SOCK_STREAM`).
- They are intentionally simple and demonstrate the basic flow of a network client/server application.
- The server accepts only one client connection in this version, then exits.
