#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server_ip>\n", argv[0]);
        return -1;
    }
    
    int sock, valread;
    struct sockaddr_in server;
    char buffer[BUFFER_SIZE] = {0};
    
    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation error");
        return -1;
    }
    
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    
    // Convert server IP address from text to binary form
    if (inet_pton(AF_INET, argv[1], &server.sin_addr) <= 0) {
        perror("invalid address / address not supported");
        return -1;
    }
    
    // Connect to server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connection failed");
        return -1;
    }
    
    // Receive "Hello" message from server
    valread = read(sock, buffer, BUFFER_SIZE);
    printf("Server message: %s\n", buffer);
    
    // Close connection and exit
    close(sock);
    return 0;
}
