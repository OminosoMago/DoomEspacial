#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6000

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sock, (struct sockaddr*)&addr, sizeof(addr));

    listen(sock, 5);

    char buffer[2048];

    while (1) {
        int client_sock = accept(sock, NULL, NULL);
        if (client_sock < 0) continue;

        ssize_t n;
        while((n = recv(client_sock, buffer, sizeof(buffer)-1, 0)) > 0) {
            buffer[n] = '\0';
            printf("%s", buffer);
            fflush(stdout);
        }
        close(client_sock);

    }

    close(sock);
    return 0;
}

