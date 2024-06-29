#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

int main() {
    char buffer[1024], *welcome = "Hello world, first TCP server!\n";
    int clen = sizeof(SOCKADDR_IN), s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN caddr, saddr;

    saddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8888); // Bản chất: ((8888 & 0x00FF) << 2) | ((8888 & 0xFF00) >> 2)

    if (!bind(s, (SOCKADDR*)&saddr, sizeof(saddr))) {
        listen(s, 10);
        int c = accept(s, (SOCKADDR*)&caddr, &clen);
        send(c, welcome, strlen(welcome), 0);
        recv(c, buffer, sizeof(buffer) - 1, 0);

        printf("%s\n", buffer);

        close(c);
    } else {
        printf("Failed to bind\n");
    }

    close(s);
}