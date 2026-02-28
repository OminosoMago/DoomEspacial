#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    inet_pton(AF_INET, "192.168.1.2", &server.sin_addr); // IP del destino

    connect(sock, (struct sockaddr*)&server, sizeof(server));

    char tecla = 1; // digamos que 1 = ESC
    send(sock, &tecla, 1, 0);

    close(sock);
}
