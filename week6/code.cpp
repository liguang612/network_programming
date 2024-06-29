#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>

using namespace std;

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

vector<SOCKADDR_IN> clients;

bool exists(SOCKADDR_IN client) {
    for (auto& c : clients)
    {
        if (c.sin_addr.s_addr == client.sin_addr.s_addr && c.sin_port == client.sin_port)
        {
            return true;
        }
    }
    return false;
}

int main() {
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    SOCKADDR_IN caddr, saddr;

    saddr.sin_port = htons(8000);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = 0;

    if (!bind(s, (SOCKADDR*)&saddr, sizeof(saddr))) {
        while (true)
        {
            char buffer[1024] = { 0 };
            int clen = sizeof(caddr);
            int r = recvfrom(s, buffer, sizeof(buffer) - 1, 0, (SOCKADDR*)&caddr, (socklen_t*)&clen);

            cout << "Receive " << r << " bytes from " << inet_ntoa(caddr.sin_addr) << endl;
            sendto(s, buffer, strlen(buffer), 0, (SOCKADDR*)&caddr, sizeof(caddr));

            // Add client to list of clients
            if (!exists(caddr))
            {
                clients.push_back(caddr);
            }

            // Forward message to all added clients
            for (auto& client : clients)
            {
                if (client.sin_addr.s_addr != caddr.sin_addr.s_addr || client.sin_port != caddr.sin_port)
                {
                    sendto(s, buffer, strlen(buffer), 0, (SOCKADDR*)&client, sizeof(client));
                }
            }
        }
    }
    else {
        cout << "Faield to bind\n";
    }

    return 0;
}