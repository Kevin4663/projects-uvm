#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <Server_IP> <Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *server_ip = argv[1];
    int port = atoi(argv[2]);

    // try to create the client socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket Failed.");
        exit(EXIT_FAILURE);
    }

    // configure server address
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid IP address.\n");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // connect to the server
    if (connect(client_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connect Failed.");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // allocate local buffers
    char *send_buf = malloc(BUFFER_SIZE);
    char *recv_buf = malloc(BUFFER_SIZE);
    if (send_buf == NULL || recv_buf == NULL) {
        perror("Malloc Failed.");
        free(send_buf);
        free(recv_buf);
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    fd_set read_fds;
    int exit_code = EXIT_SUCCESS;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds); // Watch keyboard input
        FD_SET(client_socket, &read_fds); // Watch server socket

        int max_fd = client_socket;
        if (STDIN_FILENO > max_fd) {
            max_fd = STDIN_FILENO;
        }

        // Wait for either keyboard input or a message from the server
        select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        // 1. Check if the server sent a message
        if (FD_ISSET(client_socket, &read_fds)) {
            memset(recv_buf, 0, BUFFER_SIZE);
            ssize_t bytes = recv(client_socket, recv_buf, BUFFER_SIZE - 1, 0);

            if (bytes <= 0) {
                fprintf(stderr, "\n[ERROR] Connection lost to server. Exiting...\n");
                exit_code = EXIT_FAILURE;
                break;
            }

            printf("%s", recv_buf);
            fflush(stdout);
        }

        // 2. Check if the user typed something and pressed Enter
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            memset(send_buf, 0, BUFFER_SIZE);
            if (fgets(send_buf, BUFFER_SIZE, stdin) == NULL) {
                break;
            }

            // Strip newline character
            send_buf[strcspn(send_buf, "\r\n")] = '\0';

            // Check for exit command
            if (strcmp(send_buf, "/exit") == 0 || strcmp(send_buf, "/quit") == 0) {
                exit_code = EXIT_SUCCESS;
                break;
            }

            // Send input to the server
            if (strlen(send_buf) == 0) {
                send(client_socket, "\n", 1, 0);
            } else {
                send(client_socket, send_buf, strlen(send_buf), 0);
            }
        }
    }

    // Free memory and close socket cleanly
    free(send_buf);
    free(recv_buf);
    close(client_socket);
    return exit_code;
}