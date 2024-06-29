// server.c
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

#define MAX_CLIENTS 10

int main() {
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN saddr, caddr;
    int clen = sizeof(caddr);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = 0;

    if (bind(s, (SOCKADDR*)&saddr, sizeof(saddr)) == 0) {
        listen(s, MAX_CLIENTS);

        fd_set active_fds, read_fds; // the set of active sockets and the set to be passed to select
        int max_fd; // the maximum file descriptor number

        FD_ZERO(&active_fds);
        FD_SET(s, &active_fds);
        max_fd = s;

        while (1) {
            read_fds = active_fds;
            if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
                perror("Error in select");
                exit(1);
            }

            for (int i = 0; i <= max_fd; ++i) {
                if (FD_ISSET(i, &read_fds)) {
                    if (i == s) {
                        // a new connection request
                        int new_socket = accept(s, (SOCKADDR*)&caddr, &clen);
                        if (new_socket < 0) {
                            perror("Error in accept");
                            exit(1);
                        }
                        FD_SET(new_socket, &active_fds); // add new socket to the set of active sockets
                        if (new_socket > max_fd) {
                            max_fd = new_socket;
                        }
                    }
                    else {
                        // data arriving on an already-connected socket
                        char buffer[1024] = { 0 };
                        int valread = read(i, buffer, 1024);
                        if (valread == 0) {
                            // client disconnected
                            close(i);
                            FD_CLR(i, &active_fds);
                        }
                        else {
                            // process the message
                            printf("Received message: %s\n", buffer);
                        }
                    }
                }
            }
        }
    }
    else {
        printf("Failed to bind!\n");
    }

    close(s);
    return 0;
}
