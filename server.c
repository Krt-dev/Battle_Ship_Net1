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
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Failed to initialize Winsock");
        return EXIT_FAILURE;
    }

    int server_socket;
    int client_sockets[2];
    struct sockaddr_in server_addr, client_addr;
    int addr_size;
    char buffer[MAX_MESSAGE_SIZE] = {0};
    char game_board[2][BOARD_SIZE][BOARD_SIZE]; // Separate boards for each player

    // Initialize the game boards and place ships for both players
    initialize_board(game_board[0]);
    initialize_board(game_board[1]);
    place_ships(game_board[0]);
    place_ships(game_board[1]);

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

    // Listen for incoming connections (two players)
    if (listen(server_socket, 2) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for players to connect...\n");

    // Accept connections from two players
    for (int i = 0; i < 2; ++i) {
        addr_size = sizeof(client_addr);
        if ((client_sockets[i] = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size)) == -1) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        printf("Player %d connected from %s:%d\n", i + 1, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    }

    // Main game loop
    int current_player = 0; // Start with player 1
    while (1) {
        // Inform players of whose turn it is
        sprintf(buffer, "Player %d's turn. Enter your shot (e.g., A1): ", current_player + 1);
        send(client_sockets[0], buffer, strlen(buffer), 0);
        send(client_sockets[1], buffer, strlen(buffer), 0);

        // Receive shot coordinates from the current player
        recv(client_sockets[current_player], buffer, MAX_MESSAGE_SIZE, 0);
        int row = buffer[0] - '1';
        int col = buffer[1] - 'A';

        // Check if the shot hits a ship
        if (is_hit(game_board[1 - current_player], row, col)) {
            printf("Player %d: Hit at %c%d!\n", current_player + 1, col + 'A', row + 1);
            send(client_sockets[0], "HIT", 3, 0);
            send(client_sockets[1], "HIT", 3, 0);
        } else {
            printf("Player %d: Miss at %c%d.\n", current_player + 1, col + 'A', row + 1);
            send(client_sockets[0], "MISS", 4, 0);
            send(client_sockets[1], "MISS", 4, 0);
        }

        // Check for game over conditions
        int ships_remaining = CARRIER + BATTLESHIP + CRUISER + SUBMARINE + DESTROYER;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (game_board[current_player][i][j] == 'X') {
                    ships_remaining--;
                }
            }
        }

        if (ships_remaining == 0) {
            printf("Player %d wins! All ships sunk! Game over.\n", current_player + 1);
            send(client_sockets[0], "All ships sunk! Game over.", 26, 0);
            send(client_sockets[1], "All ships sunk! Game over.", 26, 0);
            break;
        }

        // Switch to the other player
        current_player = 1 - current_player;
    }

    // Close sockets
    closesocket(server_socket);
    closesocket(client_sockets[0]);
    closesocket(client_sockets[1]);

    WSACleanup();

    return 0;
}
