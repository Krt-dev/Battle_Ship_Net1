#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define PORT 8080
#define BOARD_SIZE 8
#define MAX_MESSAGE_SIZE 1024


#define CARRIER 1
#define BATTLESHIP 1
#define CRUISER 1
#define SUBMARINE 1
#define DESTROYER 1


void initialize_board(char board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            board[i][j] = ' ';
        }
    }
}


void print_board(char board[BOARD_SIZE][BOARD_SIZE]) {
    printf("    A B C D E F G H\n");
    for (int i = 0; i < BOARD_SIZE; ++i) {
        printf("%d |", i + 1);
        for (int j = 0; j < BOARD_SIZE; ++j) {
            printf(" %c", board[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}


void place_ships(char board[BOARD_SIZE][BOARD_SIZE]) {

   
    for (int i = 0; i < CARRIER; ++i) {
        char input[3];
        int placex = 0, placey = 0;
        printf("\nWhere do you want to place the Carrier?\n");
        fflush(stdin);
        scanf("%s", input);
        placex = input[0] - 'A';
        placey = input[1] - '1';
        board[placey][placex] = 'S';
    }

    
    for (int i = 0; i < BATTLESHIP; ++i) {
        char input[3];
        int placex = 0, placey = 0;
        printf("\nWhere do you want to place the Battle Ship?\n");
        fflush(stdin);
        scanf("%s", input);
        placex = input[0] - 'A';
        placey = input[1] - '1';
        board[placey][placex] = 'S';
    }

    for (int i = 0; i < CRUISER; ++i) {
        char input[3];
        int placex = 0, placey = 0;
        printf("\nWhere do you want to place the Cruiser?\n");
        fflush(stdin);
        scanf("%s", input);
        placex = input[0] - 'A';
        placey = input[1] - '1';
        board[placey][placex] = 'S';
    }

    
    for (int i = 0; i < SUBMARINE; ++i) {
        char input[3];
        int placex = 0, placey = 0;
        printf("\nWhere do you want to place the Submarine?\n");
        fflush(stdin);
        scanf("%s", input);
        placex = input[0] - 'A';
        placey = input[1] - '1';
        board[placey][placex] = 'S';
    }

   
    for (int i = 0; i < DESTROYER; ++i) {
        char input[3];
        int placex = 0, placey = 0;
        printf("\nWhere do you want to place the Destroyer?\n");
        fflush(stdin);
        scanf("%s", input);
        placex = input[0] - 'A';
        placey = input[1] - '1';
        board[placey][placex] = 'S';
    }
}


// int is_hit(char board[BOARD_SIZE][BOARD_SIZE], int row, int col) {
//     return (board[row][col] != ' ');  
// }

//ang epass index sa player, 3d array
int is_hit(char board[BOARD_SIZE][BOARD_SIZE], int row, int col) {
    if (board[row][col] == 'S'){
        board[row][col] = ' ';
        //printf("\n\n\n\n");
        return 1;
    }
    return 0;
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
    char game_board[2][BOARD_SIZE][BOARD_SIZE]; 


    
    initialize_board(game_board[0]);
    initialize_board(game_board[1]);
    
    
        printf("\nthis is player 1's board\n");
        print_board(game_board[0]);
        printf("\nthis is player 2's board\n");
        print_board(game_board[1]);
        printf("\nthis is player 1's ship placement\n");
        place_ships(game_board[0]);
        printf("\nthis is player 2's ship placement\n");
        place_ships(game_board[1]);
        printf("\nthis is player 1's board with ships\n");
        print_board(game_board[0]);
        printf("\nthis is player 2's board with ships\n");
        print_board(game_board[1]);

    
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    
    if (listen(server_socket, 2) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for players to connect...\n");

    
    for (int i = 0; i < 2; ++i) {
        addr_size = sizeof(client_addr);
        if ((client_sockets[i] = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size)) == -1) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        printf("Player %d connected from %s:%d\n", i + 1, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    }

   

    
int current_player = 0; 
while (1) {
    
    sprintf(buffer, "Player %d", current_player + 1);
    // send(client_sockets[0], buffer, strlen(buffer), 0); //1st send to P1
    // send(client_sockets[1], buffer, strlen(buffer), 0); //1st send to P2
    send(client_sockets[current_player], buffer, strlen(buffer), 0);

    ZeroMemory(&buffer, sizeof(buffer));

    
    recv(client_sockets[current_player], buffer, MAX_MESSAGE_SIZE, 0);
    int col = buffer[0] - 'A';
    int row = buffer[1] - '1';

    
    if (is_hit(game_board[1 - current_player], row, col)) { 
        //printf("\nPlayer %d: Hit at %c%d!\n", current_player + 1, col + 'A', row + 1);
        // send(client_sockets[0], "HIT", 3, 0);
        // send(client_sockets[1], "HIT", 3, 0);
        send(client_sockets[current_player], "HIT", MAX_MESSAGE_SIZE, 0);
    } else {
        //printf("\nPlayer %d: Miss at %c%d.\n", current_player + 1, col + 'A', row + 1);
        // send(client_sockets[0], "MISS", 4, 0);
        // send(client_sockets[1], "MISS", 4, 0);
        send(client_sockets[current_player], "MISS", MAX_MESSAGE_SIZE, 0);
    }

    
    int ships_remaining = 0;
    for (int i = 0; i < BOARD_SIZE; ++i) {
    for (int j = 0; j < BOARD_SIZE; ++j) {
        if (game_board[1 - current_player][i][j] != ' ') {
            ships_remaining++;
        }
    }
}

if (ships_remaining == 0) {
    printf("Player %d wins! All ships sunk! Game over.\n", current_player + 1);
    send(client_sockets[0], "\nAll ships Sunk! Game over.\n", 32, 0);
    send(client_sockets[1], "\nAll ships Sunk! Game over.\n", 32, 0);
    break;
}




    
    //current_player = 1 - current_player;

    (current_player==0)?(current_player=1):(current_player=0);
}



    closesocket(server_socket);
    closesocket(client_sockets[0]);
    closesocket(client_sockets[1]);

    WSACleanup();

    return 0;
}
