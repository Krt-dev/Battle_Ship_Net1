#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define PORT 8080
#define BOARD_SIZE 8
#define MAX_MESSAGE_SIZE 1024

// Define ship types and their sizes
#define CARRIER 5
#define BATTLESHIP 4
#define CRUISER 3
#define SUBMARINE 3
#define DESTROYER 2

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
    int server_socket;
    int client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[MAX_MESSAGE_SIZE] = {0};
    char game_board[BOARD_SIZE][BOARD_SIZE];

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 1) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for a connection...\n");

    // Accept connection
    addr_size = sizeof(client_addr);
    if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size)) == -1) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // Initialize the game board and place ships
    initialize_board(game_board);
    place_ships(game_board);

    // Main game loop
    	while (1) {
        // Player's turn
        print_board(game_board);
        printf("Waiting for player's shot...\n");
        
        // Receive shot coordinates from the client
        recv(client_socket, buffer, MAX_MESSAGE_SIZE, 0);
        int row = buffer[0] - '1';
        int col = buffer[1] - 'A';

        // Check if the shot hits a ship
        if (is_hit(game_board, row, col)) {
            printf("Hit at %c%d!\n", col + 'A', row + 1);
            game_board[row][col] = 'X';
            send(client_socket, "HIT", 3, 0);
        } else {
            printf("Miss at %c%d.\n", col + 'A', row + 1);
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

        // Opponent's turn
        print_board(game_board);
        printf("Waiting for opponent's shot...\n");
        
        // Get user input for shot coordinates
        printf("Enter your shot (e.g., A1): ");
        fgets(buffer, MAX_MESSAGE_SIZE, stdin);

        // Send shot coordinates to the client
        send(client_socket, buffer, strlen(buffer), 0);

        // Receive and display the server's response (HIT or MISS)
        recv(client_socket, buffer, MAX_MESSAGE_SIZE, 0);
        printf("Server: %s\n", buffer);

        // Check for game over conditions (server announces all ships sunk)
        if (strcmp(buffer, "All ships sunk! Game over.") == 0) {
            break;
        }
    }

    // Close sockets
    closesocket(server_socket);
    closesocket(client_socket);

    return 0;
}
