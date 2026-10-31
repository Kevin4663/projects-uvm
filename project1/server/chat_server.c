#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define DEFAULT_PORT 8080
#define MAX_CLIENTS 30
#define BUFFER_SIZE 1024

typedef struct {
    int sock_fd;
    int is_registered;
    char username[20];
} Client;

int main(int argc, char *argv[]){
    // port auto assigned to 8080
    int port = 8080; 

    // if port provided  
    if (argc > 1){
        port = atoi(argv[1]);
    }

    // try to create the server socket
    int master_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (master_sock < 0){
        perror("Socket Failed.");
        exit(EXIT_FAILURE);
    }

    // try to enable adress reuse for master_socket
    int optval = 1;
    if (setsockopt(master_sock, SOL_SOCKET, SO_REUSEADDR,
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

    if (bind(master_sock, (struct sockaddr *)&address, sizeof(address)) < 0){
        perror("Bind Failed.");
        exit(EXIT_FAILURE);
    }

    listen(master_sock, 10);
    printf("Server is alive and listening on port %d...\n", port);

    close(master_sock);
    return 0;
}