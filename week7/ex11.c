#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct sockaddr SOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;

int g_client[1024], g_count, pid = 0, s;

void sig_handler(int signal) {
    printf("signal received %d", signal);
    exit(0);
}

int main() {
    pid = getpid();

    SOCKADDR_IN caddr, saddr;
    int clen = sizeof(saddr);

    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8888);
    saddr.sin_addr.s_addr = 0;

    bind(s, (SOCKADDR*)&saddr, sizeof(saddr));
    listen(s, 10);

    while (1)
    {
        int c = accept(s, (SOCKADDR*)&caddr, &clen);
        
        g_client[g_count++] = c;
        printf("A client connection is accepted\n");

        if (!fork())
        while (1)
        {
            char buffer[1024] = {0};
            printf("Waiting\n");

            int r = recv(c, buffer, sizeof(buffer) - 1, 0);
            printf("%s (Received %d bytes from socket %d)\n", buffer, r, c);
            if (r <= 0)
            {
                printf("A client has disconnected\n");
                break;
            } else {
                // for (int i = 0; i < g_count; i++)
                // {
                //     send(g_client[i], buffer, strlen(buffer), 0);
                // }
                FILE* f = fopen("data.txt", "rt");
                // while (!feof(f))
                // {
                //     int tmp = 0;
                //     fscanf(f, "%d", &tmp);
                //     if (c != tmp)
                //     {
                //         send(tmp, buffer, strlen(buffer) - 1, 0);
                //     }
                // }
                fprintf(f, "%s", buffer);
                fclose(f);
                kill(pid, SIGUSR1);
            }
        }
    }

    return 0;
}