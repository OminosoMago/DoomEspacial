// servidor.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    char buffer[1];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(6000);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 1);

    printf("Esperando cliente...\n");
    client_fd = accept(server_fd, NULL, NULL);
    printf("Cliente conectado\n");

    while (recv(client_fd, buffer, 1, 0) > 0) {
        printf("Tecla recibida: %c\n", buffer[0]);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}