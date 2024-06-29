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

#define BUFFER_SIZE 1024

void handle_server(int server_socket) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    int bytes_received = recv(server_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received <= 0) {
        close(server_socket);
        return;
    }
    std::cout << "Received from server: " << buffer << std::endl;
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(5000);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    if (fork() == 0) {
        handle_server(server_socket);
        exit(0);
    }
    while (true) {
        char filename[BUFFER_SIZE];
        std::cin.getline(filename, BUFFER_SIZE);
        send(server_socket, filename, strlen(filename), 0);
        FILE* file = fopen(filename, "rb");
        char buffer[BUFFER_SIZE];
        while (!feof(file)) {
            size_t bytes_read = fread(buffer, 1, BUFFER_SIZE, file);
            send(server_socket, buffer, bytes_read, 0);
        }
        fclose(file);
    }
    return 0;
}
