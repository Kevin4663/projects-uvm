#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <errno.h>
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

int main(int argc, char *argv[]){
    printf("Hello world");
}