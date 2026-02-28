#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <termios.h>
#include <string.h>
#include <stdint.h>

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
    inet_pton(AF_INET, "10.20.26.181", &server_addr.sin_addr);

    enable_raw_mode();

    printf("Enviando inputs por UDP...\n");

    char c;
    while (read(STDIN_FILENO, &c, 1) > 0) {
        packet.key = c;

        packet.pressed = 1;

        // Detectar flechas
        if (c == 27) {  // ESC
            char seq[2];

            if (read(STDIN_FILENO, &seq[0], 1) == 0) continue;
            if (read(STDIN_FILENO, &seq[1], 1) == 0) continue;

            if (seq[0] == '[') {
                switch (seq[1]) {
                    case 'A': packet.key = 'U'; break; // UP
                    case 'B': packet.key = 'D'; break; // DOWN
                    case 'C': packet.key = 'R'; break; // RIGHT
                    case 'D': packet.key = 'L'; break; // LEFT
                    default: continue;
                }
            } else {
                continue;
            }

        } else {
            packet.key = c;
        }

        // Enviar key down
        sendto(sock, &packet, sizeof(packet), 0,
               (struct sockaddr*)&server_addr, sizeof(server_addr));

        // enviar key release inmediatamente
        // Enviar key up
        packet.pressed = 0;
        sendto(sock, &packet, sizeof(packet), 0,
               (struct sockaddr*)&server_addr, sizeof(server_addr));
    }

    close(sock);
    return 0;
}