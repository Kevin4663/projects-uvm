#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define DEFAULT_PORT 8080
#define MAX_CLIENTS 30
#define BUFFER_SIZE 1024

typedef struct {
    int sock_fd;
    int is_registered;
    char username[20];
} Client;

void broadcast(Client clients[], int sender_fd, const char *msg) {
    for (int j = 0; j < MAX_CLIENTS; j++) {
        // Send to everyone connected and registered EXCEPT the person who sent it
        if (clients[j].sock_fd != -1 && clients[j].is_registered == 1 && clients[j].sock_fd != sender_fd) {
            send(clients[j].sock_fd, msg, strlen(msg), 0);
        }
    }
}

int validate_username(const char *name, Client clients[], char *err_buf, size_t err_size) {
    size_t len = strlen(name);

    // Rule 1: No leading or trailing spaces (checked first in case they typed spaces)
    if (len > 0) {
        if (isspace((unsigned char)name[0]) || isspace((unsigned char)name[len - 1])) {
            snprintf(err_buf, err_size, "[ERROR] No leading or trailing spaces allowed. Try again: ");
            return 0;
        }
    }

    // Rule 2: Not empty, and 3 to 16 chars long
    if (len < 3 || len > 16) {
        snprintf(err_buf, err_size, "[ERROR] Username must be 3-16 characters. Try again: ");
        return 0;
    }

    // Rule 3: Only letters, numbers, '_', or '-'
    for (size_t i = 0; i < len; i++) {
        if (!isalnum((unsigned char)name[i]) && name[i] != '_' && name[i] != '-') {
            snprintf(err_buf, err_size, "[ERROR] Invalid characters used. Try again: ");
            return 0;
        }
    }

    // Rule 4: Not a reserved word (strcasecmp ignores uppercase/lowercase)
    const char *reserved[] = {"Server", "Admin", "System", "All"};
    for (int i = 0; i < 4; i++) {
        if (strcasecmp(name, reserved[i]) == 0) {
            snprintf(err_buf, err_size, "[ERROR] That name is reserved. Try again: ");
            return 0;
        }
    }

    // Rule 5: Not already taken by another active registered client
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].sock_fd != -1 && clients[i].is_registered == 1) {
            if (strcasecmp(name, clients[i].username) == 0) {
                snprintf(err_buf, err_size, "[ERROR] Username already taken. Try again: ");
                return 0;
            }
        }
    }

    return 1; // All checks passed!
}

int main(int argc, char *argv[]) {
    // port auto assigned to 8080
    int port = DEFAULT_PORT;

    // if port provided  
    if (argc > 1) {
        port = atoi(argv[1]);
    }

    // try to create the server socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket Failed.");
        exit(EXIT_FAILURE);
    }

    // try to enable address reuse for server_socket
    int optval = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR,
                   &optval, sizeof(optval)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // try to bind the socket to the port
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind Failed.");
        exit(EXIT_FAILURE);
    }

    // listen on socket 
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Listen Failed.");
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %d...\n", port);

    // Initialize all client slots to empty
    Client clients[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].sock_fd = -1;
        clients[i].is_registered = 0;
        memset(clients[i].username, 0, sizeof(clients[i].username));
    }

    fd_set read_fds;
    char buffer[BUFFER_SIZE];

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(server_socket, &read_fds);
        int max_fd = server_socket;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].sock_fd != -1) {
                FD_SET(clients[i].sock_fd, &read_fds); // Watch each connected client
                if (clients[i].sock_fd > max_fd) {
                    max_fd = clients[i].sock_fd;
                }
            }
        }

        // Wait here until a client connects or sends data
        select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        // Check if a new client is connecting
        if (FD_ISSET(server_socket, &read_fds)) {
            int new_sock = accept(server_socket, NULL, NULL);
            if (new_sock >= 0) {
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (clients[i].sock_fd == -1) {
                        clients[i].sock_fd = new_sock;
                        clients[i].is_registered = 0;
                        memset(clients[i].username, 0, sizeof(clients[i].username));

                        printf("New client connected on socket %d!\n", new_sock);
                        const char *prompt = "Enter a username: ";
                        send(new_sock, prompt, strlen(prompt), 0);
                        break;
                    }
                }
            }
        }

        // Check existing clients for messages or disconnects
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = clients[i].sock_fd;
            if (fd != -1 && FD_ISSET(fd, &read_fds)) {
                memset(buffer, 0, sizeof(buffer));
                ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

                if (bytes <= 0) {
                    // bytes <= 0 means the client disconnected!
                    printf("Socket %d disconnected.\n", fd);

                    if (clients[i].is_registered == 1) {
                        char leave_msg[BUFFER_SIZE];
                        snprintf(leave_msg, sizeof(leave_msg), "%s has left the chatroom.\n", clients[i].username);
                        printf("%s", leave_msg);
                        broadcast(clients, fd, leave_msg);
                    }

                    close(fd);
                    clients[i].sock_fd = -1;
                    clients[i].is_registered = 0;
                    memset(clients[i].username, 0, sizeof(clients[i].username));
                } else {
                    // Strip the newline character from the end of the input
                    buffer[strcspn(buffer, "\r\n")] = '\0';

                    // If not registered yet, treat message as a username attempt
                    if (clients[i].is_registered == 0) {
                        char err_msg[256];
                        if (validate_username(buffer, clients, err_msg, sizeof(err_msg)) == 1) {
                            strcpy(clients[i].username, buffer);
                            clients[i].is_registered = 1;

                            char welcome_msg[256];
                            snprintf(welcome_msg, sizeof(welcome_msg), "[SERVER] Registered as %s!\n", clients[i].username);
                            send(fd, welcome_msg, strlen(welcome_msg), 0);
                        } else {
                            send(fd, err_msg, strlen(err_msg), 0);
                        }
                    } else {
                        // Registered client sending a normal chat message
                        if (strlen(buffer) > 0) {
                            char chat_msg[BUFFER_SIZE + 32];
                            snprintf(chat_msg, sizeof(chat_msg), "<%s>: %s\n", clients[i].username, buffer);
                            broadcast(clients, fd, chat_msg);
                        }
                    }
                }
            }
        }
    }

    // close socket properly
    close(server_socket);
    return 0;
}