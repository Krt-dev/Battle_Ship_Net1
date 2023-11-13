#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 8080
#define MAX_MESSAGE_SIZE 1024

#define CARRIER 5
#define BATTLESHIP 4
#define CRUISER 3
#define SUBMARINE 3
#define DESTROYER 2

#define BOARD_SIZE 8

char game_board[BOARD_SIZE][BOARD_SIZE];


// Function to initialize the game board
void initialize_board(char board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            board[i][j] = ' ';
        }
    }
}

// Function to print the game board
void print_board(char board[BOARD_SIZE][BOARD_SIZE]) {
    printf("   A B C D E F G H\n");
    for (int i = 0; i < BOARD_SIZE; ++i) {
        printf("%d |", i + 1);
        for (int j = 0; j < BOARD_SIZE; ++j) {
            printf(" %c", board[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Function to place ships on the board
void place_ships(char board[BOARD_SIZE][BOARD_SIZE]) {
    // For simplicity, ships are placed randomly in this example
    // In a real game, you might implement a more strategic placement
    // or let players place their own ships.

    // Carrier (size: 5)
    for (int i = 0; i < CARRIER; ++i) {
        board[0][i] = 'C';
    }

    // Battleship (size: 4)
    for (int i = 0; i < BATTLESHIP; ++i) {
        board[1][i] = 'B';
    }

    // Cruiser (size: 3)
    for (int i = 0; i < CRUISER; ++i) {
        board[2][i] = 'R';
    }

    // Submarine (size: 3)
    for (int i = 0; i < SUBMARINE; ++i) {
        board[3][i] = 'S';
    }

    // Destroyer (size: 2)
    for (int i = 0; i < DESTROYER; ++i) {
        board[4][i] = 'D';
    }
}

// Function to check if a shot hits a ship
int is_hit(char board[BOARD_SIZE][BOARD_SIZE], int row, int col) {
    return (board[row][col] != ' ');
}


int main() {
    //int client_socket;
    struct sockaddr_in server_addr;
    char buffer[MAX_MESSAGE_SIZE] = {0};

   WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        perror("Failed to initialize Winsock");
        return EXIT_FAILURE;
    }

    // Create socket
    SOCKET client_socket;
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
    server_ip[strcspn(server_ip, "\n")] = '\0';  // Remove the newline character

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server\n");

    // Main game loop
    while (1) {
        // Opponent's turn
        printf("Waiting for opponent's shot...\n");

        // Receive shot coordinates from the server
        recv(client_socket, buffer, MAX_MESSAGE_SIZE, 0);
        int row = buffer[0] - '1';
        int col = buffer[1] - 'A';

        // Check if the shot hits a ship
        if (is_hit(game_board, row, col)) {
            printf("Opponent: Hit at %c%d!\n", col + 'A', row + 1);
            game_board[row][col] = 'X';
            send(client_socket, "HIT", 3, 0);
        } else {
            printf("Opponent: Miss at %c%d.\n", col + 'A', row + 1);
            game_board[row][col] = 'O';
            send(client_socket, "MISS", 4, 0);
        }

        // Check for game over conditions
        int ships_remaining = CARRIER + BATTLESHIP + CRUISER + SUBMARINE + DESTROYER;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (game_board[i][j] == 'X') {
                    ships_remaining--;
                }
            }
        }

        if (ships_remaining == 0) {
            printf("All ships sunk! Game over.\n");
            send(client_socket, "All ships sunk! Game over.", 26, 0);
            break;
        }

        // Player's turn
        print_board(game_board);
        printf("Enter your shot (e.g., A1): ");
        fgets(buffer, MAX_MESSAGE_SIZE, stdin);

        // Send shot coordinates to the server
        send(client_socket, buffer, strlen(buffer), 0);

        // Receive and display the server's response (HIT or MISS)
        recv(client_socket, buffer, MAX_MESSAGE_SIZE, 0);
        printf("Server: %s\n", buffer);

        // Check for game over conditions (server announces all ships sunk)
        if (strcmp(buffer, "All ships sunk! Game over.") == 0) {
            break;
        }
    }

    // Close the client socket
    closesocket(client_socket);

    return 0;
}
