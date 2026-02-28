#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6000

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sock, (struct sockaddr*)&addr, sizeof(addr));

    char buffer[2048];

    while (1) {
        ssize_t n = recv(sock, buffer, sizeof(buffer)-1, 0);
        if (n > 0) {
            buffer[n] = '\0';
            printf("%s", buffer);
            fflush(stdout);
        }
    }

    close(sock);
    return 0;
}

