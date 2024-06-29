// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 6666

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error in socket");
        exit(1);
    }
    printf("Client socket created successfully.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error in connect");
        exit(1);
    }
    printf("Connected to Server.\n");

    printf("Receiving file...\n");
    FILE *fp = fopen("b.txt", "w");
    if (fp == NULL) {
        perror("Error in creating file.");
        exit(1);
    }
    int n;
    while ((n = recv(sockfd, buffer, 1023, 0)) > 0) {
        if (fwrite(buffer, sizeof(char), n, fp) != n) {
            perror("Error in writing file.");
            exit(1);
        }
        memset(buffer, 0, 1023);
    }
    printf("File received successfully.\n");

    fclose(fp);
    close(sockfd);
    printf("Disconnected from server.\n");

    return 0;
}
