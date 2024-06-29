#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8888

void listDirectoryContents(const char* path, int clientSocket) {
    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir(path)) != NULL) {
        // Send HTML header
        const char* header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        send(clientSocket, header, strlen(header), 0);

        // Start HTML document
        const char* htmlStart = "<html><body><ul>";
        send(clientSocket, htmlStart, strlen(htmlStart), 0);

        // List each entry in the directory
        while ((ent = readdir(dir)) != NULL) {
            char link[1024];
            sprintf(link, "<li><a href=\"%s/%s\">%s</a></li>", path, ent->d_name, ent->d_name);
            send(clientSocket, link, strlen(link), 0);
        }

        // End HTML document
        const char* htmlEnd = "</ul></body></html>";
        send(clientSocket, htmlEnd, strlen(htmlEnd), 0);

        closedir(dir);
    }
    else {
        // Could not open directory
        perror("");
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8888
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    while (1) {
        printf("Listening on port %d...\n", PORT);
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Read the incoming request
        char buffer[30000] = { 0 };
        read(new_socket, buffer, 30000);
        printf("%s\n", buffer);

        // List the contents of the root directory
        listDirectoryContents("/", new_socket);

        close(new_socket);
    }

    return 0;
}
