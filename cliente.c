#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <termios.h>
#include <string.h>

typedef struct {
    uint8_t key;
    uint8_t pressed;
} InputPacket;

void enable_raw_mode() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

int main() {
    int sock;
    struct sockaddr_in server_addr;
    InputPacket packet;

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(6000);
    inet_pton(AF_INET, "10.20.33.103", &server_addr.sin_addr);

    enable_raw_mode();

    printf("Enviando inputs por UDP...\n");

    char c;
    while (read(STDIN_FILENO, &c, 1) > 0) {
        packet.key = c;
        packet.pressed = 1;
        sendto(sock, &packet, sizeof(packet), 0,
               (struct sockaddr*)&server_addr, sizeof(server_addr));

        // enviar key release inmediatamente
        packet.pressed = 0;
        sendto(sock, &packet, sizeof(packet), 0,
               (struct sockaddr*)&server_addr, sizeof(server_addr));
    }

    return 0;
}