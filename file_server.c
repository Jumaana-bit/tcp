#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define SERVER_TCP_PORT 3000 /* well-known port */
#define BUFLEN 100 /* buffer length */

int main() {
    int sd, new_sd, client_len;
    struct sockaddr_in server, client;
    FILE *file_ptr;
    char buffer[BUFLEN];

    /* Create a stream socket */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Can't create a socket");
        exit(1);
    }

    /* Bind an address to the socket */
    bzero((char *)&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_TCP_PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("Can't bind name to socket");
        exit(1);
    }

    /* Listen for incoming connections */
    listen(sd, 5);

    while (1) {
        /* Accept incoming connection */
        client_len = sizeof(client);
        new_sd = accept(sd, (struct sockaddr *)&client, &client_len);
        if (new_sd < 0) {
            perror("Can't accept client");
            exit(1);
        }

        /* Receive filename from client */
        if (recv(new_sd, buffer, BUFLEN, 0) < 0) {
            perror("Can't receive filename");
            close(new_sd);
            continue;
        }

        /* Open requested file */
        if ((file_ptr = fopen(buffer, "rb")) == NULL) {
            perror("Can't open file");
            /* Send error message to client */
            strcpy(buffer, "Error: File not found or cannot be opened");
            send(new_sd, buffer, strlen(buffer), 0);
            close(new_sd);
            continue;
        }

        /* Send file data to client */
        while (!feof(file_ptr)) {
            int bytes_read = fread(buffer, 1, BUFLEN, file_ptr);
            send(new_sd, buffer, bytes_read, 0);
        }

        fclose(file_ptr);
        close(new_sd);
    }

    close(sd);

    return 0;
}
