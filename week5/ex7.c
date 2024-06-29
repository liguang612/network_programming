#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

void RECV(char* buffer, int len) {
    int receive = 0, tmp;

    while (1)
    {
        if (tmp = recv(c, buffer, len - receive, 0))
        {
            receive += tmp;
            if (receive == len)
            {
                break;
            }
            if (tmp < len - receive)
            {
                break;
            }
            
        } else {
            break;
        }
    }
}

int main() {
    char buffer[1024], *welcome = "Hello world, first TCP server!\n";
    int clen = sizeof(SOCKADDR_IN), s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN caddr, saddr;

    saddr.sin_addr.s_addr = 0; // inet_addr("0.0.0.0")
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999); // Bản chất: ((8888 & 0x00FF) << 2) | ((8888 & 0xFF00) >> 2)

    if (!bind(s, (SOCKADDR*)&saddr, sizeof(saddr))) {
        listen(s, 10);
        int c = accept(s, (SOCKADDR*)&caddr, &clen);

        while (1)
        {
            memset(buffer, 0, sizeof(buffer));
            if (recv(c, buffer, sizeof(buffer) - 1, 0) > 0) {
                if (!strcmp(buffer, "exit"))
                {
                    break;
                }

                while (buffer[strlen(buffer) - 1] == '\n')
                {
                    buffer[strlen(buffer) - 1] = 0;
                }
                sprintf(buffer + strlen(buffer), "> tmp.txt"); // Use sprintf to concatnate string to write command output into a file
                system(buffer);

                FILE* f = fopen("tmp.txt", "rt");
                while (!feof(f))
                {
                    fgets(buffer, sizeof(buffer) - 1, f);
                    send(c, buffer, sizeof(buffer), 0);
                }
            } else {
                printf("A client has disconnected");
                break;
            }            
        }
        close(c);
    } else {
        printf("Failed to bind\n");
    }

    close(s);
}