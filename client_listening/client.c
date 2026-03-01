#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <termios.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <lz4.h>

#define TCP_PORT 6000
#define UDP_PORT 6000
#define MAX_OUT_BUFFER 1600000
#define IP "10.20.26.181"

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

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(TCP_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server, (struct sockaddr*)&addr, sizeof(addr));
    listen(server, 1);

    int client = accept(server, NULL, NULL);

    char *buffer = malloc(MAX_OUT_BUFFER);
    char four_buffer[4];
    char *decompressed = malloc(MAX_OUT_BUFFER);

    printf("Escuchando en puerto TCPi:\n");

    while (1) {
        // Receive length uncompressed prefix
        ssize_t lenu_prefix = recv(client, four_buffer, 4, MSG_WAITALL);
        if (lenu_prefix != 4) break;

        size_t uncompressed_len = ntohl(*(uint32_t*)four_buffer);

        // Receive length compressed prefix
        ssize_t lenc_prefix = recv(client, four_buffer, 4, MSG_WAITALL);
        if (lenc_prefix != 4) break;

        size_t compressed_len = ntohl(*(uint32_t*)four_buffer);

        // Receive compressed payload
        ssize_t payload_len = recv(client, buffer, compressed_len, MSG_WAITALL);
        if (payload_len <= 0) break;

        // Decompress to stdout
        int decomp_len = LZ4_decompress_safe(buffer, decompressed, payload_len, uncompressed_len);
        if (decomp_len > 0) {
            fwrite(decompressed, 1, decomp_len, stdout);
            fflush(stdout);
        } else {
            // Fallback error: treat as raw (if sender fell back)
            fwrite(buffer, 1, payload_len, stdout);
            fflush(stdout);
        }
    }
    close(client);
    close(server);
}

/* ---------------- THREAD: CLIENTE UDP (TECLADO) ---------------- */

void* cliente_udp(void* arg) {
    int sock;
    struct sockaddr_in server_addr;
    InputPacket packet;

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(UDP_PORT);
    inet_pton(AF_INET, IP, &server_addr.sin_addr);//ip de destino

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

    pthread_create(&th_client, NULL, cliente_udp, NULL);
    pthread_create(&th_server, NULL, servidor_tcp, NULL);

    pthread_detach(th_server);
    pthread_detach(th_client);

    while (1) pause();


    return 0;
}

