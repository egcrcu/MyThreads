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
    int server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (server_socket < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server running on port %d...\n", PORT);

    struct sockaddr_in client_address;
    socklen_t client_address_length = sizeof(client_address);
    char buffer[BUFFER_SIZE];

    while (1) {
        int received_bytes = recvfrom(server_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_address, &client_address_length);
        sendto(server_socket, buffer, received_bytes, 0, (struct sockaddr*)&client_address, client_address_length);
    }

    close(server_socket);
    return 0;
}

