#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <termios.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>

#define TCP_PORT 6000
#define UDP_PORT 6000

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

/* ---------------- THREAD: SERVIDOR TCP ---------------- */

void* servidor_tcp(void* arg) {
    int server = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(TCP_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server, (struct sockaddr*)&addr, sizeof(addr));
    listen(server, 1);

    printf("[SERVIDOR] Esperando conexión TCP...\n");
    int client = accept(server, NULL, NULL);
    printf("[SERVIDOR] Cliente conectado.\n");

    char buffer[2048];
    ssize_t n;

    while ((n = recv(client, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, n, stdout);
        fflush(stdout);
    }

    close(client);
    close(server);
    return NULL;
}

/* ---------------- THREAD: CLIENTE UDP (TECLADO) ---------------- */

void* cliente_udp(void* arg) {
    int sock;
    struct sockaddr_in server_addr;
    InputPacket packet;

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(UDP_PORT);
    inet_pton(AF_INET, "10.20.30.192", &server_addr.sin_addr);

    enable_raw_mode();

    printf("[CLIENTE] Enviando inputs por UDP...\n");

    char c;
    while (read(STDIN_FILENO, &c, 1) > 0) {
        packet.key = c;
        packet.pressed = 1;

        if (c == 27) {  // ESC
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) == 0) continue;
            if (read(STDIN_FILENO, &seq[1], 1) == 0) continue;

            if (seq[0] == '[') {
                switch (seq[1]) {
                    case 'A': packet.key = 'U'; break;
                    case 'B': packet.key = 'D'; break;
                    case 'C': packet.key = 'R'; break;
                    case 'D': packet.key = 'L'; break;
                    default: continue;
                }
            } else continue;
        }

        sendto(sock, &packet, sizeof(packet), 0,
               (struct sockaddr*)&server_addr, sizeof(server_addr));

        packet.pressed = 0;
        sendto(sock, &packet, sizeof(packet), 0,
               (struct sockaddr*)&server_addr, sizeof(server_addr));
    }

    close(sock);
    return NULL;
}

/* ---------------- MAIN ---------------- */

int main() {
    pthread_t th_server, th_client;

    pthread_create(&th_server, NULL, servidor_tcp, NULL);
    pthread_create(&th_client, NULL, cliente_udp, NULL);

    pthread_join(th_server, NULL);
    pthread_join(th_client, NULL);

    return 0;
}

