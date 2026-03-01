#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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


	FILE *metrics = fopen("bandwidth.log", "a");  // ← append mode
	if (metrics == NULL) {
		perror("Failed to open bandwidth.log");  // This will tell you why
		// Fallback to stderr if file fails
		metrics = stderr;
	}
	fprintf(metrics,"\n***********START OF SESSION**********\n\n");

	uint64_t total_received = 0;
	uint64_t total = 0;
	struct timespec start;
	struct timespec timestamp;
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &start);
	clock_gettime(CLOCK_MONOTONIC, &timestamp);

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

    	total += payload_len+8;
    	total_received += payload_len+8;


    	// Stats every second

    	clock_gettime(CLOCK_MONOTONIC, &now);
    	double elapsed = (now.tv_sec - timestamp.tv_sec) +
		 (now.tv_nsec - timestamp.tv_nsec) / 1e9;
    	if (elapsed > 1.0) {
    		fprintf(metrics,"\nBandwidth: %.1f KB/s\n", total/1024.0);
    		fflush(metrics);
    		total = 0;
    		clock_gettime(CLOCK_MONOTONIC, &timestamp);
    	}
    }
	clock_gettime(CLOCK_MONOTONIC, &now);
	fprintf(metrics, "Final total: %.1f KB Final median %.1f KB/s\n***************END OF SESSION***********", total_received/1024.0, (total_received/difftime(now.tv_sec, start.tv_sec))/1024.0);
	fflush(metrics);  //
	fclose(metrics);
    close(client);
    close(server);
}
