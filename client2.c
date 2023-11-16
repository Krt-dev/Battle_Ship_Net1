#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 8080
#define MAX_MESSAGE_SIZE 1024

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Failed to initialize Winsock");
        return EXIT_FAILURE;
    }

    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[MAX_MESSAGE_SIZE] = {0};

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        perror("Socket creation failed");
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Setup server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    // Input the server's IP address
    char server_ip[15];
    printf("Enter the server's IP address: ");
    fgets(server_ip, sizeof(server_ip), stdin);
    server_ip[strcspn(server_ip, "\n")] = '\0';

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("\nConnected to the server. Game starting!\n");

    // Main game loop for Player 2
    while (1) {
        // Receive information about whose turn it is
        recv(client_socket, buffer, MAX_MESSAGE_SIZE, 0);
        printf("\n%s\n", buffer);

        recv(client_socket, buffer, MAX_MESSAGE_SIZE, 0);
        printf("\nServer: %s\n", buffer);

        // Get user input for shot coordinates
        printf("\nEnter your shot player 2: \n");
        fgets(buffer, MAX_MESSAGE_SIZE, stdin);

        // Send shot coordinates to the server
        send(client_socket, buffer, strlen(buffer), 0);

        // Receive and display the server's response (HIT or MISS)
        recv(client_socket, buffer, MAX_MESSAGE_SIZE, 0);
        printf("\nServer: %s\n", buffer);

        // Check for game over conditions (server announces all ships sunk)
        if (strcmp(buffer, "\nAll ships sunk! Game over.\n") == 0) {
            break;
        }
    }

    // Close the client socket
    closesocket(client_socket);

    WSACleanup();

    return 0;
}
