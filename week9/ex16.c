#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 2048

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
int clients[MAX_CLIENTS];
int n_clients = 0;

void send_message(char* message, int curr_client) {
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < n_clients; i++) {
        if (clients[i] != curr_client) {
            if (send(clients[i], message, strlen(message), 0) < 0) {
                perror("Send failed");
                exit(EXIT_FAILURE);
            }
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

void* handle_client(void* arg) {
    char buffer[BUFFER_SIZE];
    int curr_client = *((int*)arg);

    memset(buffer, 0, BUFFER_SIZE);
    while (recv(curr_client, buffer, BUFFER_SIZE, 0) > 0) {
        send_message(buffer, curr_client);
        memset(buffer, 0, BUFFER_SIZE);
    }

    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < n_clients; i++) {
        if (clients[i] == curr_client) {
            for (int j = i; j < n_clients - 1; j++) {
                clients[j] = clients[j + 1];
            }
            break;
        }
    }
    n_clients--;

    pthread_mutex_unlock(&clients_mutex);
}

int main(int argc, char** argv) {
    int server_sock, client_sock, c;
    struct sockaddr_in server, client;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Could not create socket");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    if (bind(server_sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        return 1;
    }

    listen(server_sock, 3);

    printf("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);

    while ((client_sock = accept(server_sock, (struct sockaddr*)&client, (socklen_t*)&c))) {
        pthread_t sniffer_thread;

        pthread_mutex_lock(&clients_mutex);

        clients[n_clients++] = client_sock;

        pthread_mutex_unlock(&clients_mutex);

        if (pthread_create(&sniffer_thread, NULL, handle_client, (void*)&client_sock) < 0) {
            perror("Could not create thread");
            return 1;
        }

        printf("Client connected\n");
    }

    if (client_sock < 0) {
        perror("Accept failed");
        return 1;
    }

    return 0;
}
