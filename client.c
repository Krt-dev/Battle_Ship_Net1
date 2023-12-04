#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 8080
#define MAX_MESSAGE_SIZE 1024



int main() {
    char iboard[8][8];
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Failed to initialize Winsock");
        return EXIT_FAILURE;
    }

    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[MAX_MESSAGE_SIZE] = {0};

    
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        perror("Socket creation failed");
        WSACleanup();
        return EXIT_FAILURE;
    }

    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    char server_ip[15];
    printf("---------WELCOME TO BATTLESHIP---------\n");
    printf("Enter the server's IP address: ");
    fgets(server_ip, sizeof(server_ip), stdin);
    server_ip[strcspn(server_ip, "\n")] = '\0';  

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid Address not supported");
        exit(EXIT_FAILURE);
    }

    
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("\nConnected to the server. Waiting for other Player\n");

    printf("BATTLEFIELD\n");
    printf("    A B C D E F G H\n");
    for (int i = 0; i < 8; ++i) {
        printf("%d |", i + 1);
        for (int j = 0; j < 8; ++j) {
            printf("  ", iboard[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    
    while (1) {

        
        recv(client_socket, buffer, MAX_MESSAGE_SIZE, 0); 
        printf("\n----Your Turn %s ----\n", buffer);

        ZeroMemory(&buffer, sizeof(buffer));

        
        printf("\n-Enter your shot:\n");
        fgets(buffer, MAX_MESSAGE_SIZE, stdin);

        
        send(client_socket, buffer, strlen(buffer), 0);

      
        recv(client_socket, buffer, MAX_MESSAGE_SIZE, 0);
        printf("\n===Your shot is a: %s ===\n", buffer);

       
        if (strcmp(buffer, "\nAll ships Sunk! Game over.\n") == 0) {
            break;
        }
    }

    
    closesocket(client_socket);

    WSACleanup();

    return 0;
}
