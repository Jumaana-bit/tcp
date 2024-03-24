#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAX_PACKET_SIZE 100

void receive_file(int server_socket, FILE *fp) {
    char buffer[MAX_PACKET_SIZE];
    int bytes_received;

    // Receive file data from server and write to file
    while ((bytes_received = recv(server_socket, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, bytes_received, fp);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server_ip>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_socket;
    struct sockaddr_in server_address;
    char filename[100];
    FILE *file;

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation error");
        exit(EXIT_FAILURE);
    }

    // Initialize server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    if (inet_pton(AF_INET, argv[1], &server_address.sin_addr) <= 0) {
        perror("invalid address / address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }

    // Get filename from user
    printf("Enter filename: ");
    scanf("%s", filename);

    // Send filename to server
    send(server_socket, filename, strlen(filename), 0);

    // Open file for writing
    if ((file = fopen(filename, "wb")) == NULL) {
        perror("file open error");
        exit(EXIT_FAILURE);
    }

    // Receive file from server
    receive_file(server_socket, file);

    // Close connection and file
    fclose(file);
    close(server_socket);
    return 0;
}
