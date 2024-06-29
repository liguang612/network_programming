#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CONNECTIONS 100

// Mutex for file writing
pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_connection(void *arg) {
    int sockfd = *(int *)arg;
    free(arg);

    // Handle the connection...
    // If writing to the file, lock the mutex first
    pthread_mutex_lock(&file_mutex);
    // Write to the file
    FILE *file = fopen("output.txt", "a");
    if (file != NULL) {
        fputs("Some data\n", file);
        fclose(file);
    }
    pthread_mutex_unlock(&file_mutex);

    close(sockfd);
    return NULL;
}

int main() {
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(6000);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_sockfd, MAX_CONNECTIONS);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addrlen = sizeof(client_addr);
        int *client_sockfd = malloc(sizeof(int));
        *client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &client_addrlen);

        pthread_t thread;
        pthread_create(&thread, NULL, handle_connection, client_sockfd);
        pthread_detach(thread);
    }

    close(server_sockfd);
    return 0;
}
