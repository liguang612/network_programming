#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <fstream>
#include <iostream>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

int client_sockets[MAX_CLIENTS];
int num_clients = 0;

void handle_sigchld(int sig) {
    while (waitpid((pid_t)(-1), 0, WNOHANG) > 0) {}
}

void handle_client(int client_socket) {
    char filename[BUFFER_SIZE];
    recv(client_socket, filename, BUFFER_SIZE - 1, 0);
    FILE* file = fopen(filename, "wb");
    char buffer[BUFFER_SIZE];
    while (true) {
        ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            break;
        }
        fwrite(buffer, 1, bytes_received, file);
    }
    fclose(file);
}

int main() {
    signal(SIGCHLD, handle_sigchld);

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(5000);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    listen(server_socket, MAX_CLIENTS);

    while (true) {
        struct sockaddr_in client_address;
        socklen_t client_len = sizeof(client_address);

        int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_len);
        client_sockets[num_clients++] = client_socket;
        if (fork() == 0) {
            close(server_socket);
            handle_client(client_socket);
            exit(0);
        }
    }
    return 0;
}
