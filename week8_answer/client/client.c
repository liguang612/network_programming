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
#include <stdio.h>
#include <stdbool.h>

typedef struct sockaddr SOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;

void sig_child(int signal) {

}

void processUDP() {
    int udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    SOCKADDR_IN saddr;

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(6000);
    saddr.sin_addr.s_addr = 0;

    bind(udpSocket, (SOCKADDR*)&saddr, sizeof(saddr));

    while (true)
    {
        char buffer[1024] = { 0 };
        SOCKADDR_IN caddr;
        int clen = sizeof(caddr);
        int r = recvfrom(udpSocket, buffer, sizeof(buffer) - 1, 0, (SOCKADDR*)&caddr, &clen);

        if (r > 0)
        {
            FILE* f = fopen("clients.txt", "a+t");

            fprintf(f, "%s %s\n", buffer, inet_ntoa(caddr.sin_addr));
            fclose(f);
        }
    }
}

void processTCP() {
    int tcpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_TCP);
    SOCKADDR_IN saddr;

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5000);
    saddr.sin_addr.s_addr = 0;

    bind(tcpSocket, (SOCKADDR*)&saddr, sizeof(saddr));

    while (true)
    {
        SOCKADDR_IN caddr;
        int clen = sizeof(caddr);
        int c = accept(tcpSocket, (SOCKADDR*)&caddr, &clen);
        FILE* f = fopen("clients.txt", "a+t");
        while (!feof(f))
        {
            char buffer[1024] = { 0 };
            fgets(buffer, sizeof(buffer) - 1, f);
            if (strlen(buffer) > 0)
            {
                send(c, buffer, strlen(buffer), 0);
            }
        }
    }
}

int main() {
    return 0;
}