#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 2048

void* receive_message(void* socket_desc) {
    int sock = *(int*)socket_desc;
    char message[BUFFER_SIZE];
    int size;

    while ((size = recv(sock, message, BUFFER_SIZE, 0)) > 0) {
        message[size] = '\0';
        printf("%s\n", message);
    }

    return 0;
}

int main(int argc, char** argv) {
    int sock;
    struct sockaddr_in server;
    char message[BUFFER_SIZE], server_reply[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket");
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Connect failed. Error");
        return 1;
    }

    pthread_t thread;
    pthread_create(&thread, NULL, receive_message, (void*)&sock);

    while (1) {
        printf("Enter message: ");
        scanf("%s", message);

        if (send(sock, message, strlen(message), 0) < 0) {
            printf("Send failed");
            return 1;
        }
    }

    close(sock);
    return 0;
}
