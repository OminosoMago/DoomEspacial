#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6000

int main() {
    int server = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server, (struct sockaddr*)&addr, sizeof(addr));
    listen(server, 1);

    int client = accept(server, NULL, NULL);

    char buffer[2048];
    ssize_t n;

    while ((n = recv(client, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, n, stdout);
        fflush(stdout);
    }

    close(client);
    close(server);
}
