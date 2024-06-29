#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace std;

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

int main() {
    int off = 0, on = 1, s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    SOCKADDR_IN baddr, caddr, saddr;

    saddr.sin_port = htons(8000);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = 0;

    baddr.sin_family = AF_INET;
    baddr.sin_port = htons(8000);
    baddr.sin_addr.s_addr = inet_addr("172.19.175.255");

    if (!bind(s, (SOCKADDR*)&saddr, sizeof(saddr))) {
        while (true)
        {
            char buffer[1024] = {0};
            int clen = sizeof(caddr);
            int r = recvfrom(s, buffer, sizeof(buffer) - 1, 0, (SOCKADDR*)&caddr, (socklen_t*)&clen);

            cout << "Receive " << r << " bytes from " << inet_ntoa(caddr.sin_addr) << endl;

            setsockopt(s, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
            sendto(s, buffer, strlen(buffer), 0, (SOCKADDR*)&caddr, sizeof(caddr));
            setsockopt(s, SOL_SOCKET, SO_BROADCAST, &off, sizeof(off));
        }
    } else {
        cout << "Faield to bind\n";
    }

    return 0;
}