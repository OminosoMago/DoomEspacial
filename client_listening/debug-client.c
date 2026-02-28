#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

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


	FILE *metrics = fopen("bandwidth.log", "a");  // ← append mode
	if (metrics == NULL) {
		perror("Failed to open bandwidth.log");  // This will tell you why
		// Fallback to stderr if file fails
		metrics = stderr;
	}
	fprintf(metrics,"\n***********START OF SESSION**********\n\n");

    char buffer[2048];
    ssize_t n;
	uint64_t total_received = 0;
	uint64_t total = 0;
	struct timespec start;
	struct timespec timestamp;
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &start);
	clock_gettime(CLOCK_MONOTONIC, &timestamp);


    while ((n = recv(client, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, n, stdout);
        fflush(stdout);

    	if (n > 0) {
    		total += n;
    		total_received += n;
    		buffer[n] = 0;
    	}

    	// Stats every second

    	clock_gettime(CLOCK_MONOTONIC, &now);
    	double elapsed = difftime(now.tv_sec, timestamp.tv_sec) +
                 difftime(now.tv_nsec, timestamp.tv_nsec) / 1e9;
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
