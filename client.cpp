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

char name[1024] = { 0 };

void sig_child(int signal) {
    pid_t pid;
    int stat;

    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    {
        cout << "Child " << pid << " terminated.\n";
    }
}

void processTCP() {
    SOCKADDR_IN saddr;

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5000);
    saddr.sin_addr.s_addr = inet_addr("172.19.175.255");

    while (true)
    {
        int tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (!connect(tcpSocket, (SOCKADDR*)&saddr, sizeof(saddr)))
        {
            while (true)
            {
                char response[1024] = { 0 };
                int r = recv(tcpSocket, response, sizeof(response) - 1, 0);

                if (r > 0)
                {
                    while (response[strlen(response) - 1] == '\n' || response[strlen(response) - 1] == '\r')
                    {
                        response[strlen(response) - 1] = 0;
                    }
                    cout << response << endl;
                }
                else { break; }
            }

            close(tcpSocket);
            cout << "Clients updated\n";
        }
        else {
            cout << "Failed to connect\n";
        }

        sleep(5);
    }
}

void processUDP() {
    int udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    SOCKADDR_IN toAddr;

    toAddr.sin_family = AF_INET;
    toAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
    toAddr.sin_port = htons(6000);

    int enable = 1;
    setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, &enable, sizeof(int));

    while (true)
    {
        sendto(udpSocket, name, strlen(name), 0, (SOCKADDR*)&toAddr, sizeof(toAddr));
        sleep(5);
    }
}

int main() {
    signal(SIGCHLD, sig_child);

    cin >> name;

    if (fork())
    {
        processTCP();
    }
    else {
        processUDP();
    }

    return 0;
}