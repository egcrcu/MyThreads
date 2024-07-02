/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 4096
#define PROXY_PORT 8080
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8080

void *handle_client(void *arg) {
    printf("Handling client connection...\n");
    int client_socket = *((int *)arg);
    char buffer[MAX_BUFFER_SIZE];
    ssize_t bytes_received;

    // Read the request from the client
    bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received < 0) {
        perror("Error reading from client");
        close(client_socket);
        pthread_exit(NULL);
    }

    // Create a socket to connect to the server
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error creating server socket");
        close(client_socket);
        pthread_exit(NULL);
    }

    // Set up the server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr) <= 0) {
        perror("Error setting up server address");
        close(client_socket);
        close(server_socket);
        pthread_exit(NULL);
    }

    // Connect to the server
    if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server");
        close(client_socket);
        close(server_socket);
        pthread_exit(NULL);
    }

    // Forward the request to the server
    if (send(server_socket, buffer, bytes_received, 0) < 0) {
        perror("Error sending to server");
        close(client_socket);
        close(server_socket);
        pthread_exit(NULL);
    }

    // Forward the response to the client
    while ((bytes_received = recv(server_socket, buffer, sizeof(buffer), 0)) > 0) {
        if (send(client_socket, buffer, bytes_received, 0) < 0) {
            perror("Error sending to client");
            break;
        }
    }

    printf("Closing client connection...\n");

    // Close the sockets
    close(client_socket);
    close(server_socket);

    pthread_exit(NULL);
}

int main() {
    int proxy_socket, client_socket;
    struct sockaddr_in proxy_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Create a socket for the proxy
    proxy_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (proxy_socket < 0) {
        perror("Error creating proxy socket");
        exit(EXIT_FAILURE);
    }

    // Set up the proxy address
    memset(&proxy_addr, 0, sizeof(proxy_addr));
    proxy_addr.sin_family = AF_INET;
    proxy_addr.sin_addr.s_addr = INADDR_ANY;
    proxy_addr.sin_port = htons(PROXY_PORT);

    // Bind the socket to the address
    if (bind(proxy_socket, (struct sockaddr *)&proxy_addr, sizeof(proxy_addr)) < 0) {
        perror("Error binding proxy socket");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(proxy_socket, 10) < 0) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("Proxy listening on port %d...\n", PROXY_PORT);

    while (1) {
        // Accept a connection from the client
        client_socket = accept(proxy_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            perror("Error accepting connection");
            continue;
        }

        // Create a thread to handle the client
        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, &client_socket) != 0) {
            perror("Error creating thread");
            close(client_socket);
            continue;
        }

        // Detach the thread to allow it to run independently
        pthread_detach(tid);
    }

    // Close the proxy socket (this part will not be reached in this simple example)
    close(proxy_socket);

    return 0;
}*/


#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 8192

struct client_state
{
    int client_fd;
};

void *request_handler(void *param);

int main(int argc, const char *argv[])
{
    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket < 0)
    {
        perror("Error creating socket");
        return 1;
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8889);

    if (bind(listen_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Error binding socket");
        close(listen_socket);
        return 1;
    }

    if (listen(listen_socket, SOMAXCONN) < 0)
    {
        perror("Error listening to socket");
        close(listen_socket);
        return 1;
    }

    printf("Accepting client connections...\n");

    while (1)
    {
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);

        int client_socket = accept(listen_socket, (struct sockaddr *)&client_address, &client_address_len);
        if (client_socket < 0)
        {
            perror("Error accepting client");
            continue;
        }

        struct client_state *state = malloc(sizeof(struct client_state));
        state->client_fd = client_socket;

        pthread_t thread;
        if (pthread_create(&thread, NULL, request_handler, state) != 0)
        {
            perror("Error creating thread");
            close(client_socket);
            free(state);
        }
        else
        {
            pthread_detach(thread);
        }
    }

    close(listen_socket);

    return 0;
}

void *request_handler(void *param)
{
    struct client_state *state = (struct client_state *)param;
    int client_socket = state->client_fd;

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received <= 0)
    {
        perror("Error receiving data");
        close(client_socket);
        free(state);
        pthread_exit(NULL);
    }

    buffer[bytes_received] = '\0';

    // Определение хоста и порта из запроса
    char *host_start = strstr(buffer, "Host: ");
    if (host_start == NULL)
    {
        perror("Invalid request format");
        close(client_socket);
        free(state);
        pthread_exit(NULL);
    }

    host_start += strlen("Host: ");
    char *host_end = strchr(host_start, '\r');
    if (host_end == NULL)
    {
        perror("Invalid request format");
        close(client_socket);
        free(state);
        pthread_exit(NULL);
    }

    *host_end = '\0'; // Заменяем символ новой строки

    // Создание структуры addrinfo для получения информации об адресе хоста
    struct addrinfo hints, *host_info;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host_start, "80", &hints, &host_info) != 0)
    {
        perror("Error resolving host");
        close(client_socket);
        free(state);
        pthread_exit(NULL);
    }

    // Создание сокета для подключения к удаленному серверу
    int remote_socket = socket(host_info->ai_family, host_info->ai_socktype, host_info->ai_protocol);
    if (remote_socket < 0)
    {
        perror("Error creating remote socket");
        close(client_socket);
        freeaddrinfo(host_info);
        free(state);
        pthread_exit(NULL);
    }

    // Подключение к удаленному серверу
    if (connect(remote_socket, host_info->ai_addr, host_info->ai_addrlen) < 0)
    {
        perror("Error connecting to remote server");
        close(client_socket);
        close(remote_socket);
        freeaddrinfo(host_info);
        free(state);
        pthread_exit(NULL);
    }

    // Пересылка данных от клиента к удаленному серверу
    if (send(remote_socket, buffer, bytes_received, 0) < 0)
    {
        perror("Error sending data to remote server");
        close(client_socket);
        close(remote_socket);
        freeaddrinfo(host_info);
        free(state);
        pthread_exit(NULL);
    }

    // Пересылка данных от удаленного сервера клиенту
    while (1)
    {
        ssize_t bytes_read = recv(remote_socket, buffer, sizeof(buffer), 0);
        if (bytes_read <= 0)
            break;

        if (send(client_socket, buffer, bytes_read, 0) < 0)
        {
            perror("Error sending data to client");
            break;
        }
    }

    close(client_socket);
    close(remote_socket);
    freeaddrinfo(host_info);
    free(state);
    pthread_exit(NULL);
}

