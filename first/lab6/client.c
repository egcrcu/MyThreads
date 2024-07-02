#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (client_socket < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(PORT);

    char buffer[BUFFER_SIZE] = "Hello from client";

    sendto(client_socket, buffer, strlen(buffer), 0, (struct sockaddr*)&server_address, sizeof(server_address));

    memset(buffer, 0, BUFFER_SIZE);

    int received_bytes = recvfrom(client_socket, buffer, BUFFER_SIZE, 0, NULL, NULL);

    printf("Received message from server: %s\n", buffer);

    close(client_socket);
    return 0;
}

