// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 6666

void send_file(FILE *fp, int sockfd) {
    char data[1024] = {0};

    while (fgets(data, 1023, fp) != NULL) {
        if (send(sockfd, data, sizeof(data), 0) == -1) {
            perror("Error in sending file.");
            exit(1);
        }
        memset(data, 0, 1024);
    }
}

int main() {
    char *filename = "a.txt";
    int sockfd, new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    FILE *fp;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error in socket");
        exit(1);
    }
    printf("Server socket created successfully.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error in bind");
        exit(1);
    }
    printf("Bind to port %d\n", PORT);

    if (listen(sockfd, 10) == 0) {
        printf("Listening...\n");
    } else {
        perror("Error in listening");
        exit(1);
    }

    addr_size = sizeof(client_addr);
    new_sock = accept(sockfd, (struct sockaddr*)&client_addr, &addr_size);

    fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error in reading file.");
        exit(1);
    }

    send_file(fp, new_sock);
    printf("File data sent successfully.\n");

    close(new_sock);
    printf("Connection closed.\n");

    return 0;
}
