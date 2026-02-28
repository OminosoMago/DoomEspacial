#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <termios.h>
#include <string.h>
#include <stdint.h>
#include <sys/select.h>
#include <stdbool.h>

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
    inet_pton(AF_INET, "10.20.26.181", &server_addr.sin_addr);

    enable_raw_mode();

    printf("Enviando inputs por UDP...\n");

    bool keys[256] = {0}; // mapa de estado

    while (1) {
        fd_set rfds;
        struct timeval tv = {0, 50000}; // 50 ms
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);

        int ready = select(STDIN_FILENO+1, &rfds, NULL, NULL, &tv);

        if (ready > 0 && FD_ISSET(STDIN_FILENO, &rfds)) {
            char c;
            if (read(STDIN_FILENO, &c, 1) <= 0) continue;

            uint8_t keycode;

            // detectar flechas
            if (c == 27) {
                char seq[2];
                if (read(STDIN_FILENO, &seq[0], 1) == 0) continue;
                if (read(STDIN_FILENO, &seq[1], 1) == 0) continue;

                if (seq[0] == '[') {
                    switch (seq[1]) {
                        case 'A': keycode = 'U'; break; // UP
                        case 'B': keycode = 'D'; break; // DOWN
                        case 'C': keycode = 'R'; break; // RIGHT
                        case 'D': keycode = 'L'; break; // LEFT
                        default: continue;
                    }
                } else continue;
            } else {
                keycode = c;
            }

            // si no estaba presionada, enviar down
            if (!keys[keycode]) {
                keys[keycode] = true;
                packet.key = keycode;
                packet.pressed = 1;
                sendto(sock, &packet, sizeof(packet), 0,
                       (struct sockaddr*)&server_addr, sizeof(server_addr));
            }
        }

        // cada loop revisa si alguna tecla ya no está presionada
        // (en terminal no podemos detectar release, así que ignoramos por ahora)
        // Esto permite que Doom interprete el movimiento continuo
    }

    close(sock);
    return 0;
}