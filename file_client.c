#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_TCP_PORT 3000 /* well-known port */
#define BUFLEN 100 /* buffer length */

int main(int argc, char *argv[]) {
    int sd, port;
    struct sockaddr_in server;
    FILE *file_ptr;
    char buffer[BUFLEN];

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <filename>\n", argv[0]);
        exit(1);
    }

    /* Create a stream socket */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Can't create a socket");
        exit(1);
    }

    /* Specify server address */
    bzero((char *)&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_TCP_PORT);
    if (inet_pton(AF_INET, argv[1], &server.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        exit(1);
    }

    /* Connect to the server */
    if (connect(sd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("Can't connect");
        exit(1);
    }

    /* Send filename to server */
    if (send(sd, argv[2], strlen(argv[2]), 0) < 0) {
        perror("Can't send filename");
        close(sd);
        exit(1);
    }

    /* Open local file for writing */
    if ((file_ptr = fopen("downloaded_file", "wb")) == NULL) {
        perror("Can't open local file for writing");
        close(sd);
        exit(1);
    }

    /* Receive file data from server */
    while (1) {
        int bytes_received = recv(sd, buffer, BUFLEN, 0);
        if (bytes_received <= 0) {
            break; // End of transmission
        }
        fwrite(buffer, 1, bytes_received, file_ptr);
    }

    fclose(file_ptr);
    close(sd);

    return 0;
}
