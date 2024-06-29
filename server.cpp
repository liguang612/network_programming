#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

typedef struct sockaddr SOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;

void processTCP() {
    int tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN saddr;

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = 0;
    saddr.sin_port = htons(5000);

    bind(tcpSocket, (SOCKADDR*)&saddr, sizeof(saddr));
    listen(tcpSocket, 10);

    while (true)
    {
        SOCKADDR_IN caddr;
        unsigned int clen = sizeof(caddr);
        int c = accept(tcpSocket, (SOCKADDR*)&caddr, &clen);
        FILE* f = fopen("clients.txt", "rt");

        cout << "A new TCP client has connected.\n";

        while (!feof(f))
        {
            char response[1024] = { 0 };

            fgets(response, sizeof(response) - 1, f);
            if (strlen(response) > 0)
            {
                send(c, response, strlen(response), 0);
            }
        }

        fclose(f);
        close(c);
    }
}

void processUDP() {
    int udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    SOCKADDR_IN saddr;

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = 0;
    saddr.sin_port = htons(6000);

    bind(udpSocket, (SOCKADDR*)&saddr, sizeof(saddr));

    while (true)
    {
        char response[1024] = { 0 };
        SOCKADDR_IN caddr;
        unsigned int clen = sizeof(caddr);

        int r = recvfrom(udpSocket, response, sizeof(response) - 1, 0, (SOCKADDR*)&caddr, &clen);

        if (r > 0)
        {
            while (response[strlen(response) - 1] == '\r' || response[strlen(response) - 1] == '\n')
            {
                response[strlen(response) - 1] = 0;
            }

            FILE* f = fopen("clients.txt", "a+t");
            fprintf(f, "%s %s\n", response, inet_ntoa(caddr.sin_addr));

            fclose(f);
            cout << response << ' ' << inet_ntoa(caddr.sin_addr);
        }

    }

}

void sig_child(int signal) {
    pid_t pid;
    int stat;

    while ((pid = waitpid(-1, &stat, WNOHANG) > 0))
    {
        cout << "Child " << pid << " has terminated\n";
    }
}

int main() {
    FILE* f = fopen("clients.txt", "wt");
    fclose(f);

    signal(SIGCHLD, sig_child);

    if (fork())
    {
        processTCP();
    }
    else {
        processUDP();
    }

    return 0;
}