#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "lz4.h"

#define PORT 6000
#define MAX_OUT_BUFFER 1600000

int main() {
    int server = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server, (struct sockaddr*)&addr, sizeof(addr));
    listen(server, 1);

	int client = accept(server, NULL, NULL);

	char *buffer = malloc(MAX_OUT_BUFFER);
    char four_buffer[4];
	char *decompressed = malloc(MAX_OUT_BUFFER);

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
