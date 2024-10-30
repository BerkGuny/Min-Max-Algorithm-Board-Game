#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h> // Needed for INT_MAX

#define BOARD_SIZE 7
#define POSITION_SIZE 3 // Maximum size for each position string
#define TOTAL_POSITIONS (BOARD_SIZE * BOARD_SIZE) // 49 positions in a 7x7 board
#define POSITION_NAME_SIZE 3

int numPieces; // number of X's and O's

// Dynamic array declarations
char(*oarray)[POSITION_SIZE];
char(*ovirtualarray1)[POSITION_SIZE];
char(*ovirtualarray2)[POSITION_SIZE];
char(*ovirtualarray3)[POSITION_SIZE];
char(*ovirtualarray4)[POSITION_SIZE];

char(*simulation_o_array)[POSITION_SIZE];
char(*simulation_x_array)[POSITION_SIZE];

char(*ovirtualarray5)[POSITION_SIZE];
char(*xvirtualarray5)[POSITION_SIZE];

char(*xarray)[POSITION_SIZE];
char(*xvirtualarray1)[POSITION_SIZE];
char(*xvirtualarray2)[POSITION_SIZE];
char(*xvirtualarray3)[POSITION_SIZE];
char(*xvirtualarray4)[POSITION_SIZE];

char available_places[TOTAL_POSITIONS][POSITION_SIZE];

char from_place[POSITION_NAME_SIZE];
char togo_place[POSITION_NAME_SIZE];

void initializeAvailablePlaces(char board[BOARD_SIZE][BOARD_SIZE]) {
    int count = 0;
    for (char row = 'a'; row < 'a' + BOARD_SIZE; row++) {
        for (int col = 1; col <= BOARD_SIZE; col++) {
            if (board[row - 'a'][col - 1] == ' ') {
                snprintf(available_places[count], POSITION_SIZE, "%c%d", row, col);
                count++;
            }
        }
    }
    if (count < TOTAL_POSITIONS) {
        available_places[count][0] = '\0';
    }
}

void initializeDynamicArrays() {
    // Allocate memory based on the number of pieces
    oarray = malloc(numPieces * sizeof(*oarray));
    ovirtualarray1 = malloc(numPieces * 4 * sizeof(*ovirtualarray1));
    ovirtualarray2 = malloc(numPieces * 16 * sizeof(*ovirtualarray2));
    ovirtualarray3 = malloc(numPieces * 64 * sizeof(*ovirtualarray3));
    ovirtualarray4 = malloc(numPieces * 256 * sizeof(*ovirtualarray4));

    xarray = malloc(numPieces * sizeof(*xarray));
    xvirtualarray1 = malloc(numPieces * 4 * sizeof(*xvirtualarray1));
    xvirtualarray2 = malloc(numPieces * 16 * sizeof(*xvirtualarray2));
    xvirtualarray3 = malloc(numPieces * 64 * sizeof(*xvirtualarray3));
    xvirtualarray4 = malloc(numPieces * 256 * sizeof(*xvirtualarray4));

    ovirtualarray5 = malloc(numPieces * 1024 * sizeof(*ovirtualarray5)); // or another appropriate size
    xvirtualarray5 = malloc(numPieces * 1024 * sizeof(*xvirtualarray5)); // or another appropriate size

    simulation_o_array = malloc(numPieces * sizeof(*simulation_o_array));
    simulation_x_array = malloc(numPieces * sizeof(*simulation_x_array));

}

void updateO(char board[BOARD_SIZE][BOARD_SIZE]) {
    int index = 0;
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board[y][x] == 'O') {
                snprintf(oarray[index], POSITION_SIZE, "%c%d", 'a' + y, x + 1);
                index++;
            }
        }
    }
    // Mark the end of valid entries if the array is not fully filled
    if (index < BOARD_SIZE) {
        oarray[index][0] = '\0';
    }
}

void updateX(char board[BOARD_SIZE][BOARD_SIZE]) {
    int index = 0;
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board[y][x] == 'X') {
                snprintf(xarray[index], POSITION_SIZE, "%c%d", 'a' + y, x + 1);
                index++;
            }
        }
    }
    if (index < BOARD_SIZE) {
        xarray[index][0] = '\0'; // Mark the end of valid entries
    }
}


// Convert the row letter to an index
int rowToIndex(char row) {
    return row - 'a';
}

// Convert the column number to an index
int colToIndex(char col) {
    return col - '1';
}

// Check if the move is valid
int isValidMove(char board[BOARD_SIZE][BOARD_SIZE], int x, int y, int newX, int newY, char player) {
    // Check within the board boundaries
    if (newX < 0 || newX >= BOARD_SIZE || newY < 0 || newY >= BOARD_SIZE) return 0;

    // Check destination is empty
    if (board[newY][newX] != ' ') return 0;

    // Check move is adjacent (up, down, left, or right)
    if (abs(newX - x) + abs(newY - y) != 1) return 0;

    // Check if the original position contains the player's piece
    if (board[y][x] != player) return 0;

    return 1; // Valid move
}


// Print the board
void printBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    printf("    ");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf(" %d  ", i + 1);
    }
    printf("\n");

    for (int y = 0; y < BOARD_SIZE; y++) {
        printf(" %c ", 'a' + y);
        for (int x = 0; x < BOARD_SIZE; x++) {
            printf("| %c ", board[y][x]);
        }

        printf("|\n");
        printf("----");
        for (int i = 0; i < BOARD_SIZE; i++) {
            printf("----");
        }
        printf("\n");
    }
}

void updateAvailablePlaces(char board[BOARD_SIZE][BOARD_SIZE], char available_places[TOTAL_POSITIONS][POSITION_SIZE]) {
    int count = 0;
    for (char row = 'a'; row < 'a' + BOARD_SIZE; row++) {
        for (int col = 1; col <= BOARD_SIZE; col++) {
            if (board[row - 'a'][col - 1] == ' ') {
                snprintf(available_places[count], POSITION_SIZE, "%c%d", row, col);
                count++;
            }
        }
    }
    if (count < TOTAL_POSITIONS) {
        available_places[count][0] = '\0'; // Mark the end of the list
    }
}
int evaluateMove(int x, int y, char(*staticOArray)[POSITION_SIZE], int numStaticO) {
    int score = 0;
    int dx[] = { -1, 1, 0, 0 };
    int dy[] = { 0, 0, -1, 1 };

    for (int dir = 0; dir < 4; dir++) {
        int newX = x + dx[dir];
        int newY = y + dy[dir];
        if (newX >= 0 && newX < BOARD_SIZE && newY >= 0 && newY < BOARD_SIZE) {
            score += 1;
        }
    }
    return score;
}
int evaluateBestXMoveConsideringStaticO(char board[BOARD_SIZE][BOARD_SIZE], int xIndex, char(*staticOArray)[POSITION_SIZE], int numStaticO) {
    int bestMoveValue = INT_MIN; // Holds the best move's value
    int bestMoveIndex = -1; // Holds the index of the best move
    int xRow = xarray[xIndex][0] - 'a'; // Convert character row to index
    int xCol = xarray[xIndex][1] - '1'; // Convert character column to index

    // Directions: left, right, up, down
    int dx[] = { -1, 1, 0, 0 };
    int dy[] = { 0, 0, -1, 1 };

    // Evaluate each possible move
    for (int dir = 0; dir < 4; dir++) {
        int newX = xCol + dx[dir];
        int newY = xRow + dy[dir];

        // Ensure the new position is within the board and empty
        if (newX >= 0 && newX < BOARD_SIZE && newY >= 0 && newY < BOARD_SIZE && board[newY][newX] == ' ') {
            // Evaluate the move
            int moveValue = evaluateMove(newX, newY, staticOArray, numStaticO); // Placeholder function

            // Update best move if this move is better
            if (moveValue > bestMoveValue) {
                bestMoveValue = moveValue;
                bestMoveIndex = dir; // Assign the direction as the best move
            }
        }
    }

    return bestMoveIndex; // Return the index of the best move
}

int evaluatePosition(char position[POSITION_SIZE]) {
    int score = 0;
    int row = position[0] - 'a';
    int col = position[1] - '1';

    score = (BOARD_SIZE / 2) - abs((BOARD_SIZE / 2) - row) + (BOARD_SIZE / 2) - abs((BOARD_SIZE / 2) - col);

    return score;
}

int countAdjacentEmptySpaces(char board[BOARD_SIZE][BOARD_SIZE], char player) {
    int dx[] = { -1, 1, 0, 0 }; // Directions: left, right, up, down
    int dy[] = { 0, 0, -1, 1 };
    int count = 0;
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board[y][x] == player) {
                for (int dir = 0; dir < 4; dir++) {
                    int newY = y + dy[dir];
                    int newX = x + dx[dir];
                    if (newY >= 0 && newY < BOARD_SIZE && newX >= 0 && newX < BOARD_SIZE && board[newY][newX] == ' ') {
                        count++;
                    }
                }
            }
        }
    }
    return count;
}

void make_tree_o(char board[BOARD_SIZE][BOARD_SIZE]) {
    int dx[] = { -1, 1, 0, 0 }; // Directions: left, right, up, down
    int dy[] = { 0, 0, -1, 1 };

    int virtualIndex1 = 0, virtualIndex2 = 0, virtualIndex3 = 0, virtualIndex4 = 0;

    // Simulate one-step moves for all 'O' pieces and populate ovirtualarray1
    for (int i = 0; oarray[i][0] != '\0' && i < numPieces; i++) {
        int oRow = oarray[i][0] - 'a';
        int oCol = oarray[i][1] - '1';

        for (int dir = 0; dir < 4; dir++) {
            int newORow = oRow + dy[dir];
            int newOCol = oCol + dx[dir];

            if (newORow >= 0 && newORow < BOARD_SIZE && newOCol >= 0 && newOCol < BOARD_SIZE && board[newORow][newOCol] == ' ') {
                snprintf(ovirtualarray1[virtualIndex1], POSITION_SIZE, "%c%d", 'a' + newORow, newOCol + 1);
            }
            else {
                strcpy(ovirtualarray1[virtualIndex1], "ff"); // Mark invalid moves as "ff"
            }
            virtualIndex1++;
        }
    }

    // Simulate two-step moves based on ovirtualarray1 and populate ovirtualarray2
    for (int i = 0; i < virtualIndex1; i++) {
        if (strcmp(ovirtualarray1[i], "ff") != 0) {
            int oRow = ovirtualarray1[i][0] - 'a';
            int oCol = ovirtualarray1[i][1] - '1';

            for (int dir = 0; dir < 4; dir++) {
                int newORow = oRow + dy[dir];
                int newOCol = oCol + dx[dir];

                if (newORow >= 0 && newORow < BOARD_SIZE && newOCol >= 0 && newOCol < BOARD_SIZE && board[newORow][newOCol] == ' ') {
                    snprintf(ovirtualarray2[virtualIndex2], POSITION_SIZE, "%c%d", 'a' + newORow, newOCol + 1);
                }
                else {
                    strcpy(ovirtualarray2[virtualIndex2], "ff");
                }
                virtualIndex2++;
            }
        }
        else {
            for (int j = 0; j < 4; j++) {
                strcpy(ovirtualarray2[virtualIndex2++], "ff");
            }
        }
    }

    // Simulate three-step moves based on ovirtualarray2 and populate ovirtualarray3
    for (int i = 0; i < virtualIndex2; i++) {
        if (strcmp(ovirtualarray2[i], "ff") != 0) {
            int oRow = ovirtualarray2[i][0] - 'a';
            int oCol = ovirtualarray2[i][1] - '1';

            for (int dir = 0; dir < 4; dir++) {
                int newORow = oRow + dy[dir];
                int newOCol = oCol + dx[dir];

                if (newORow >= 0 && newORow < BOARD_SIZE && newOCol >= 0 && newOCol < BOARD_SIZE && board[newORow][newOCol] == ' ') {
                    snprintf(ovirtualarray3[virtualIndex3], POSITION_SIZE, "%c%d", 'a' + newORow, newOCol + 1);
                }
                else {
                    strcpy(ovirtualarray3[virtualIndex3], "ff");
                }
                virtualIndex3++;
            }
        }
        else {
            for (int j = 0; j < 4; j++) {
                strcpy(ovirtualarray3[virtualIndex3++], "ff");
            }
        }
    }

    // Simulate four-step moves based on ovirtualarray3 and populate ovirtualarray4
    for (int i = 0; i < virtualIndex3; i++) {
        if (strcmp(ovirtualarray3[i], "ff") != 0) {
            int oRow = ovirtualarray3[i][0] - 'a';
            int oCol = ovirtualarray3[i][1] - '1';

            for (int dir = 0; dir < 4; dir++) {
                int newORow = oRow + dy[dir];
                int newOCol = oCol + dx[dir];

                if (newORow >= 0 && newORow < BOARD_SIZE && newOCol >= 0 && newOCol < BOARD_SIZE && board[newORow][newOCol] == ' ') {
                    snprintf(ovirtualarray4[virtualIndex4], POSITION_SIZE, "%c%d", 'a' + newORow, newOCol + 1);
                }
                else {
                    strcpy(ovirtualarray4[virtualIndex4], "ff");
                }
                virtualIndex4++;
            }
        }
        else {
            for (int j = 0; j < 4; j++) {
                strcpy(ovirtualarray4[virtualIndex4++], "ff");
            }
        }
    }
    //ovirtualarray5
    int virtualIndex5 = 0;
    for (int i = 0; i < virtualIndex4; i++) {
        if (strcmp(ovirtualarray4[i], "ff") != 0) {
            int oRow = ovirtualarray4[i][0] - 'a';
            int oCol = ovirtualarray4[i][1] - '1';

            for (int dir = 0; dir < 4; dir++) {
                int newORow = oRow + dy[dir];
                int newOCol = oCol + dx[dir];

                if (newORow >= 0 && newORow < BOARD_SIZE && newOCol >= 0 && newOCol < BOARD_SIZE && board[newORow][newOCol] == ' ') {
                    snprintf(ovirtualarray5[virtualIndex5], POSITION_SIZE, "%c%d", 'a' + newORow, newOCol + 1);
                }
                else {
                    strcpy(ovirtualarray5[virtualIndex5], "ff");
                }
                virtualIndex5++;
            }
        }
        else {
            for (int j = 0; j < 4; j++) {
                strcpy(ovirtualarray5[virtualIndex5++], "ff");
            }
        }
    }
}


void make_tree_x(char board[BOARD_SIZE][BOARD_SIZE]) {
    int dx[] = { -1, 1, 0, 0 }; // Directions: left, right, up, down
    int dy[] = { 0, 0, -1, 1 }; // Directions: up, down, left, right

    int virtualIndex1 = 0, virtualIndex2 = 0, virtualIndex3 = 0, virtualIndex4 = 0, virtualIndex5 = 0;


    // Simulate one-step moves for all 'X' pieces and populate xvirtualarray1
    for (int i = 0; xarray[i][0] != '\0' && i < numPieces; i++) {
        int xRow = xarray[i][0] - 'a';
        int xCol = xarray[i][1] - '1';

        for (int dir = 0; dir < 4; dir++) {
            int newXRow = xRow + dy[dir];
            int newXCol = xCol + dx[dir];

            if (newXRow >= 0 && newXRow < BOARD_SIZE && newXCol >= 0 && newXCol < BOARD_SIZE && board[newXRow][newXCol] == ' ') {
                snprintf(xvirtualarray1[virtualIndex1], POSITION_SIZE, "%c%d", 'a' + newXRow, newXCol + 1);
            }
            else {
                strcpy(xvirtualarray1[virtualIndex1], "ff");
            }
            virtualIndex1++;
        }
    }

    // Simulate two-step moves based on xvirtualarray1 and populate xvirtualarray2
    for (int i = 0; i < virtualIndex1; i++) {
        if (strcmp(xvirtualarray1[i], "ff") != 0) {
            int xRow = xvirtualarray1[i][0] - 'a';
            int xCol = xvirtualarray1[i][1] - '1';

            for (int dir = 0; dir < 4; dir++) {
                int newXRow = xRow + dy[dir];
                int newXCol = xCol + dx[dir];

                if (newXRow >= 0 && newXRow < BOARD_SIZE && newXCol >= 0 && newXCol < BOARD_SIZE && board[newXRow][newXCol] == ' ') {
                    snprintf(xvirtualarray2[virtualIndex2], POSITION_SIZE, "%c%d", 'a' + newXRow, newXCol + 1);
                }
                else {
                    strcpy(xvirtualarray2[virtualIndex2], "ff");
                }
                virtualIndex2++;
            }
        }
        else {
            for (int j = 0; j < 4; j++) {
                strcpy(xvirtualarray2[virtualIndex2++], "ff");
            }
        }
    }

    // Simulate three-step moves based on xvirtualarray2 and populate xvirtualarray3
    for (int i = 0; i < virtualIndex2; i++) {
        if (strcmp(xvirtualarray2[i], "ff") != 0) {
            int xRow = xvirtualarray2[i][0] - 'a';
            int xCol = xvirtualarray2[i][1] - '1';

            for (int dir = 0; dir < 4; dir++) {
                int newXRow = xRow + dy[dir];
                int newXCol = xCol + dx[dir];

                if (newXRow >= 0 && newXRow < BOARD_SIZE && newXCol >= 0 && newXCol < BOARD_SIZE && board[newXRow][newXCol] == ' ') {
                    snprintf(xvirtualarray3[virtualIndex3], POSITION_SIZE, "%c%d", 'a' + newXRow, newXCol + 1);
                }
                else {
                    strcpy(xvirtualarray3[virtualIndex3], "ff");
                }
                virtualIndex3++;
            }
        }
        else {
            for (int j = 0; j < 4; j++) {
                strcpy(xvirtualarray3[virtualIndex3++], "ff");
            }
        }
    }

    // Simulate four-step moves based on xvirtualarray3 and populate xvirtualarray4
    for (int i = 0; i < virtualIndex3; i++) {
        if (strcmp(xvirtualarray3[i], "ff") != 0) {
            int xRow = xvirtualarray3[i][0] - 'a';
            int xCol = xvirtualarray3[i][1] - '1';

            for (int dir = 0; dir < 4; dir++) {
                int newXRow = xRow + dy[dir];
                int newXCol = xCol + dx[dir];

                if (newXRow >= 0 && newXRow < BOARD_SIZE && newXCol >= 0 && newXCol < BOARD_SIZE && board[newXRow][newXCol] == ' ') {
                    snprintf(xvirtualarray4[virtualIndex4], POSITION_SIZE, "%c%d", 'a' + newXRow, newXCol + 1);
                }
                else {
                    strcpy(xvirtualarray4[virtualIndex4], "ff");
                }
                virtualIndex4++;
            }
        }
        else {
            for (int j = 0; j < 4; j++) {
                strcpy(xvirtualarray4[virtualIndex4++], "ff");
            }
        }
    }
    for (int i = 0; i < virtualIndex4; i++) {
        if (strcmp(xvirtualarray4[i], "ff") != 0) {
            int xRow = xvirtualarray4[i][0] - 'a';
            int xCol = xvirtualarray4[i][1] - '1';

            for (int dir = 0; dir < 4; dir++) {
                int newXRow = xRow + dy[dir];
                int newXCol = xCol + dx[dir];

                // Check if new position is valid and within the board
                if (newXRow >= 0 && newXRow < BOARD_SIZE && newXCol >= 0 && newXCol < BOARD_SIZE && board[newXRow][newXCol] == ' ') {
                    snprintf(xvirtualarray5[virtualIndex5], POSITION_SIZE, "%c%d", 'a' + newXRow, newXCol + 1);
                }
                else {
                    strcpy(xvirtualarray5[virtualIndex5], "ff"); // Mark invalid moves as "ff"
                }
                virtualIndex5++;
            }
        }
        else {
            for (int j = 0; j < 4; j++) {
                strcpy(xvirtualarray5[virtualIndex5++], "ff"); // Fill with "ff" if no move is possible
            }
        }
    }
}
int countEmptySpacesAroundO(char board[BOARD_SIZE][BOARD_SIZE], char* position) {
    int oRow = position[0] - 'a';
    int oCol = position[1] - '1';
    int count = 0;
    int dx[] = { -1, 1, 0, 0 }; // Directions: left, right, up, down
    int dy[] = { 0, 0, -1, 1 }; // Directions: up, down, left, right

    for (int dir = 0; dir < 4; dir++) {
        int newRow = oRow + dy[dir];
        int newCol = oCol + dx[dir];
        if (newRow >= 0 && newRow < BOARD_SIZE && newCol >= 0 && newCol < BOARD_SIZE && board[newRow][newCol] == ' ') {
            count++;
        }
    }
    return count;
}
int countEmptySpacesAroundX(char board[BOARD_SIZE][BOARD_SIZE], char* position) {
    int xRow = position[0] - 'a';
    int xCol = position[1] - '1';
    int count = 0;
    int dx[] = { -1, 1, 0, 0 }; // Directions: left, right, up, down
    int dy[] = { 0, 0, -1, 1 }; // Directions: up, down, left, right

    for (int dir = 0; dir < 4; dir++) {
        int newRow = xRow + dy[dir];
        int newCol = xCol + dx[dir];
        if (newRow >= 0 && newRow < BOARD_SIZE && newCol >= 0 && newCol < BOARD_SIZE && board[newRow][newCol] == ' ') {
            count++;
        }
    }
    return count;
}

void calculateNewPosition(char currentPosition[POSITION_SIZE], int moveIndex, char newPosition[POSITION_SIZE]) {
    int row = currentPosition[0] - 'a';
    int col = currentPosition[1] - '1';
    switch (moveIndex) {
    case 0: // move up
        row--;
        break;
    case 1: // move down
        row++;
        break;
    case 2: // move left
        col--;
        break;
    case 3: // move right
        col++;
        break;
    }
    // Check if new position is valid and within the board boundaries
    if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE) {
        snprintf(newPosition, POSITION_SIZE, "%c%d", 'a' + row, col + 1);
    }
    else {
        strcpy(newPosition, "ff");  // "ff" indicates an invalid or out-of-bounds position
    }
}

int simulation_x(char board[BOARD_SIZE][BOARD_SIZE], char(*xarray)[POSITION_SIZE], char(*simulation_x_array)[POSITION_SIZE]) {
    int totalEmptySpaces = 0;

    for (int xIndex = 0; xIndex < numPieces; xIndex++) {
        int bestEmptySpaces = -1;
        char bestMove[POSITION_SIZE] = "ff";
        int xRow = xarray[xIndex][0] - 'a';
        int xCol = xarray[xIndex][1] - '1';

        for (int moveIndex = 0; moveIndex < 4; moveIndex++) {
            char potentialMove[POSITION_SIZE];
            calculateNewPosition(xarray[xIndex], moveIndex, potentialMove);

            if (strcmp(potentialMove, "ff") != 0) {  // Check for a valid potential move
                int newCol = potentialMove[1] - '1';
                int newRow = potentialMove[0] - 'a';

                if (isValidMove(board, xCol, xRow, newCol, newRow, 'X')) {
                    int emptySpaces = countEmptySpacesAroundX(board, potentialMove); // Implement this function
                    if (emptySpaces > bestEmptySpaces) {
                        bestEmptySpaces = emptySpaces;
                        strcpy(bestMove, potentialMove);
                    }
                }
            }
        }

        if (strcmp(bestMove, "ff") != 0) {
            strcpy(simulation_x_array[xIndex], bestMove);
        }
        else {
            strcpy(simulation_x_array[xIndex], xarray[xIndex]);
        }

        totalEmptySpaces += bestEmptySpaces;
    }

    return totalEmptySpaces;
}

int simulated_step1_x(char board[BOARD_SIZE][BOARD_SIZE], char(*xarray)[POSITION_SIZE], char(*simulation_x_array)[POSITION_SIZE]) {
    int totalEmptySpaces = 0;
    char tempBoard[BOARD_SIZE][BOARD_SIZE];
    memcpy(tempBoard, board, sizeof(char) * BOARD_SIZE * BOARD_SIZE);

    for (int xIndex = 0; xIndex < numPieces; xIndex++) {
        int bestEmptySpaces = -1;
        char bestMove[POSITION_SIZE] = "ff";
        int xRow = xarray[xIndex][0] - 'a';
        int xCol = xarray[xIndex][1] - '1';

        for (int moveIndex = 0; moveIndex < 4; moveIndex++) {
            char potentialMove[POSITION_SIZE];
            calculateNewPosition(xarray[xIndex], moveIndex, potentialMove);

            if (strcmp(potentialMove, "ff") != 0) {
                int newCol = potentialMove[1] - '1';
                int newRow = potentialMove[0] - 'a';

                if (isValidMove(tempBoard, xCol, xRow, newCol, newRow, 'X')) {
                    tempBoard[xRow][xCol] = ' ';
                    tempBoard[newRow][newCol] = 'X';
                    int emptySpaces = countAdjacentEmptySpaces(tempBoard, 'O');
                    if (emptySpaces > bestEmptySpaces) {
                        bestEmptySpaces = emptySpaces;
                        strcpy(bestMove, potentialMove);
                    }
                    tempBoard[xRow][xCol] = 'X';
                    tempBoard[newRow][newCol] = ' ';
                }
            }
        }
        if (strcmp(bestMove, "ff") != 0) {
            strcpy(simulation_x_array[xIndex], bestMove);
        }
        else {
            strcpy(simulation_x_array[xIndex], xarray[xIndex]);
        }
    }

    for (int xIndex = 0; xIndex < numPieces; xIndex++) {
        totalEmptySpaces += countEmptySpacesAroundX(tempBoard, simulation_x_array[xIndex]);
    }

    return totalEmptySpaces;
}

int simulated_step3_x(char board[BOARD_SIZE][BOARD_SIZE], char(*xarray)[POSITION_SIZE], char(*simulation_x_array)[POSITION_SIZE]) {
    int totalEmptySpaces = 0;
    char tempBoard[BOARD_SIZE][BOARD_SIZE];
    char tempSimulation[BOARD_SIZE][POSITION_SIZE]; // Temporary array to store the simulation results

    memcpy(tempBoard, board, sizeof(char) * BOARD_SIZE * BOARD_SIZE);  // Copy the original board

    // Copy the original X array to the simulation array
    for (int i = 0; i < numPieces; i++) {
        strcpy(simulation_x_array[i], xarray[i]);
    }

    // Perform the simulation 3 times
    for (int simRound = 0; simRound < 3; simRound++) {
        // For each 'X' piece
        for (int xIndex = 0; xIndex < numPieces; xIndex++) {
            int bestEmptySpaces = -1;
            char bestMove[POSITION_SIZE] = "ff";
            int xRow = simulation_x_array[xIndex][0] - 'a';
            int xCol = simulation_x_array[xIndex][1] - '1';

            // Try each possible move
            for (int moveIndex = 0; moveIndex < 4; moveIndex++) {
                char potentialMove[POSITION_SIZE];
                calculateNewPosition(simulation_x_array[xIndex], moveIndex, potentialMove);

                if (strcmp(potentialMove, "ff") != 0) {
                    int newCol = potentialMove[1] - '1';
                    int newRow = potentialMove[0] - 'a';

                    if (isValidMove(tempBoard, xCol, xRow, newCol, newRow, 'X')) {
                        tempBoard[xRow][xCol] = ' ';
                        tempBoard[newRow][newCol] = 'X';

                        int emptySpaces = countAdjacentEmptySpaces(tempBoard, 'O');
                        if (emptySpaces > bestEmptySpaces) {
                            bestEmptySpaces = emptySpaces;
                            strcpy(bestMove, potentialMove);
                        }
                        tempBoard[xRow][xCol] = 'X';
                        tempBoard[newRow][newCol] = ' ';
                    }
                }
            }

            if (strcmp(bestMove, "ff") != 0) {
                strcpy(tempSimulation[xIndex], bestMove);
            }
            else {
                strcpy(tempSimulation[xIndex], simulation_x_array[xIndex]);
            }
        }

        // Copy the results of this simulation round to the simulation_x_array for the next round
        for (int i = 0; i < numPieces; i++) {
            strcpy(simulation_x_array[i], tempSimulation[i]);
        }
    }

    // Calculate total empty spaces around 'O' after 3 rounds of simulation
    for (int xIndex = 0; xIndex < numPieces; xIndex++) {
        totalEmptySpaces += countEmptySpacesAroundO(tempBoard, simulation_x_array[xIndex]);
    }

    return totalEmptySpaces;
}

int simulated_step1_o(char board[BOARD_SIZE][BOARD_SIZE], char(*oarray)[POSITION_SIZE], char(*simulation_o_array)[POSITION_SIZE]) {
    int totalEmptySpaces = 0;
    char tempBoard[BOARD_SIZE][BOARD_SIZE];
    memcpy(tempBoard, board, sizeof(char) * BOARD_SIZE * BOARD_SIZE);
    for (int oIndex = 0; oIndex < numPieces; oIndex++) {
        int bestEmptySpaces = -1;
        char bestMove[POSITION_SIZE] = "ff";
        int oRow = oarray[oIndex][0] - 'a';
        int oCol = oarray[oIndex][1] - '1';
        for (int moveIndex = 0; moveIndex < 4; moveIndex++) {
            char potentialMove[POSITION_SIZE];
            calculateNewPosition(oarray[oIndex], moveIndex, potentialMove);

            if (strcmp(potentialMove, "ff") != 0) {
                int newCol = potentialMove[1] - '1';
                int newRow = potentialMove[0] - 'a';

                if (isValidMove(tempBoard, oCol, oRow, newCol, newRow, 'O')) {
                    tempBoard[oRow][oCol] = ' ';
                    tempBoard[newRow][newCol] = 'O';
                    int emptySpaces = countAdjacentEmptySpaces(tempBoard, 'X');
                    if (emptySpaces > bestEmptySpaces) {
                        bestEmptySpaces = emptySpaces;
                        strcpy(bestMove, potentialMove);
                    }
                    tempBoard[oRow][oCol] = 'O';
                    tempBoard[newRow][newCol] = ' ';
                }
            }
        }
        if (strcmp(bestMove, "ff") != 0) {
            strcpy(simulation_o_array[oIndex], bestMove);
        }
        else {
            strcpy(simulation_o_array[oIndex], oarray[oIndex]);
        }
    }
    for (int oIndex = 0; oIndex < numPieces; oIndex++) {
        totalEmptySpaces += countEmptySpacesAroundX(tempBoard, simulation_o_array[oIndex]);
    }

    return totalEmptySpaces;
}

int simulated_step3_o(char board[BOARD_SIZE][BOARD_SIZE], char(*oarray)[POSITION_SIZE], char(*simulation_o_array)[POSITION_SIZE]) {
    int totalEmptySpaces = 0;
    char tempBoard[BOARD_SIZE][BOARD_SIZE];
    char tempSimulation[BOARD_SIZE][POSITION_SIZE]; // Temporary array to store the simulation results

    memcpy(tempBoard, board, sizeof(char) * BOARD_SIZE * BOARD_SIZE);  // Copy the original board

    // Copy the original O array to the simulation array
    for (int i = 0; i < numPieces; i++) {
        strcpy(simulation_o_array[i], oarray[i]);
    }

    // Perform the simulation 3 times
    for (int simRound = 0; simRound < 3; simRound++) {
        // For each 'O' piece
        for (int oIndex = 0; oIndex < numPieces; oIndex++) {
            int bestEmptySpaces = -1;
            char bestMove[POSITION_SIZE] = "ff";
            int oRow = simulation_o_array[oIndex][0] - 'a';
            int oCol = simulation_o_array[oIndex][1] - '1';

            // Try each possible move
            for (int moveIndex = 0; moveIndex < 4; moveIndex++) {
                char potentialMove[POSITION_SIZE];
                calculateNewPosition(simulation_o_array[oIndex], moveIndex, potentialMove);

                if (strcmp(potentialMove, "ff") != 0) {
                    int newCol = potentialMove[1] - '1';
                    int newRow = potentialMove[0] - 'a';

                    if (isValidMove(tempBoard, oCol, oRow, newCol, newRow, 'O')) {
                        tempBoard[oRow][oCol] = ' ';
                        tempBoard[newRow][newCol] = 'O';

                        int emptySpaces = countAdjacentEmptySpaces(tempBoard, 'X');
                        if (emptySpaces > bestEmptySpaces) {
                            bestEmptySpaces = emptySpaces;
                            strcpy(bestMove, potentialMove);
                        }
                        tempBoard[oRow][oCol] = 'O';
                        tempBoard[newRow][newCol] = ' ';
                    }
                }
            }

            if (strcmp(bestMove, "ff") != 0) {
                strcpy(tempSimulation[oIndex], bestMove);
            }
            else {
                strcpy(tempSimulation[oIndex], simulation_o_array[oIndex]);
            }
        }

        // Copy the results of this simulation round to the simulation_o_array for the next round
        for (int i = 0; i < numPieces; i++) {
            strcpy(simulation_o_array[i], tempSimulation[i]);
        }
    }

    // Calculate total empty spaces around 'X' after 3 rounds of simulation
    for (int oIndex = 0; oIndex < numPieces; oIndex++) {
        totalEmptySpaces += countEmptySpacesAroundX(tempBoard, simulation_o_array[oIndex]);
    }

    return totalEmptySpaces;
}

int simulation_o(char board[BOARD_SIZE][BOARD_SIZE], char(*oarray)[POSITION_SIZE], char(*simulation_o_array)[POSITION_SIZE]) {
    int totalEmptySpaces = 0;

    // Iterate over each 'O' piece
    for (int oIndex = 0; oIndex < numPieces; oIndex++) {
        int bestEmptySpaces = -1; // Track the best move's empty adjacent spaces
        char bestMove[POSITION_SIZE] = "ff"; // Track the best move

        // Extract the row and column for the current 'O' piece
        int oRow = oarray[oIndex][0] - 'a';
        int oCol = oarray[oIndex][1] - '1';

        // Try all possible moves for the current 'O' piece
        for (int moveIndex = 0; moveIndex < 4; moveIndex++) {
            char potentialMove[POSITION_SIZE];
            calculateNewPosition(oarray[oIndex], moveIndex, potentialMove);

            // Check if the potential move is valid
            if (strcmp(potentialMove, "ff") != 0) {
                int newCol = potentialMove[1] - '1';
                int newRow = potentialMove[0] - 'a';

                if (isValidMove(board, oCol, oRow, newCol, newRow, 'O')) {
                    int emptySpaces = countEmptySpacesAroundO(board, potentialMove); // Implement this function
                    if (emptySpaces > bestEmptySpaces) {
                        bestEmptySpaces = emptySpaces;
                        strcpy(bestMove, potentialMove);
                    }
                }
            }
        }

        if (strcmp(bestMove, "ff") != 0) {
            strcpy(simulation_o_array[oIndex], bestMove);
        }
        else {
            strcpy(simulation_o_array[oIndex], oarray[oIndex]);
        }
        totalEmptySpaces += bestEmptySpaces;
    }

    return totalEmptySpaces;
}

void selection_for_o(char board[BOARD_SIZE][BOARD_SIZE]) {
    strcpy(from_place, "ff");
    strcpy(togo_place, "ff");
    for (int i = 0; i < numPieces; i++) {
        int oRow = oarray[i][0] - 'a';
        int oCol = oarray[i][1] - '1';
        int newRow = oRow;
        int newCol = oCol;
        int moved = 0;

        if (!moved && oRow == 0 && board[1][oCol] == ' ') {
            newRow = 1;
            moved = 1;
        }
        else if (!moved && oRow == BOARD_SIZE - 1 && board[BOARD_SIZE - 2][oCol] == ' ') {
            newRow = BOARD_SIZE - 2;
            moved = 1;
        }

        if (!moved && oCol == 0 && board[oRow][1] == ' ') {
            newCol = 1;
            moved = 1;
        }
        else if (!moved && oCol == BOARD_SIZE - 1 && board[oRow][BOARD_SIZE - 2] == ' ') {
            newCol = BOARD_SIZE - 2;
            moved = 1;
        }

        if (moved) {
            char fromPosition[POSITION_SIZE], toPosition[POSITION_SIZE];
            snprintf(fromPosition, POSITION_SIZE, "%c%d", 'a' + oRow, oCol + 1);
            snprintf(toPosition, POSITION_SIZE, "%c%d", 'a' + newRow, newCol + 1);

            board[oRow][oCol] = ' ';
            board[newRow][newCol] = 'O';
            snprintf(oarray[i], POSITION_SIZE, "%c%d", 'a' + newRow, newCol + 1);
            printf("Computer moved O from %s to %s\n", fromPosition, toPosition);
            return;
        }
    }

    int oValues5[numPieces * 1024];
    int oValues4[numPieces * 256];
    int oValues3[numPieces * 64];
    int oValues2[numPieces * 16];
    int oValues1[numPieces * 4];

    int xImpact = simulation_x(board, xarray, simulation_x_array);
    for (int i = 0; i < numPieces * 1024; i++) {
        if (strcmp(ovirtualarray5[i], "ff") != 0) {
            oValues5[i] = evaluatePosition(ovirtualarray5[i]) - xImpact;
        }
        else {
            oValues5[i] = INT_MAX;
        }
    }
    for (int i = 0; i < numPieces * 256; i++) {
        oValues4[i] = -5000;
        for (int j = 0; j < 4; j++) {
            int idx = i * 4 + j;
            if (oValues5[idx] > oValues4[i]) {
                oValues4[i] = oValues5[idx];
            }
        }
    }
    for (int i = 0; i < numPieces * 64; i++) {
        oValues3[i] = -5000;
        for (int j = 0; j < 4; j++) {
            int idx = i * 4 + j;
            if (oValues4[idx] > oValues3[i]) {
                oValues3[i] = oValues4[idx];
            }
        }
    }

    int xSimulationImpact1 = simulated_step3_x(board, xarray, simulation_x_array);
    for (int i = 0; i < numPieces * 4; i++) {
        oValues1[i] -= xSimulationImpact1;
    }

    for (int i = 0; i < numPieces * 16; i++) {
        oValues2[i] = -5000;
        for (int j = 0; j < 4; j++) {
            int idx = i * 4 + j;
            if (oValues3[idx] > oValues2[i]) {
                oValues2[i] = oValues3[idx];
            }
        }
    }
    for (int i = 0; i < numPieces * 4; i++) {
        oValues1[i] = -5000;
        for (int j = 0; j < 4; j++) {
            int idx = i * 4 + j;
            if (oValues2[idx] > oValues1[i]) {
                oValues1[i] = oValues2[idx];
            }
        }
    }

    int xSimulationImpact = simulated_step1_x(board, xarray, simulation_x_array);
    for (int i = 0; i < numPieces * 4; i++) {
        oValues1[i] -= xSimulationImpact;
    }

    int bestValue = INT_MIN;
    int bestMoveIndex = -1;
    int bestOriginIndex = -1;
    for (int i = 0; i < numPieces; i++) {
        int oValueIndex = i * 4;
        for (int j = 0; j < 4; j++) {
            if (oValues1[oValueIndex + j] > bestValue) {
                bestValue = oValues1[oValueIndex + j];
                bestMoveIndex = oValueIndex + j;
                bestOriginIndex = i;
            }
        }
    }

    if (bestOriginIndex != -1 && bestMoveIndex != -1) {
        strcpy(from_place, oarray[bestOriginIndex]);
        strcpy(togo_place, ovirtualarray1[bestMoveIndex]);

        int fromRow = from_place[0] - 'a';
        int fromCol = from_place[1] - '1';
        int toRow = togo_place[0] - 'a';
        int toCol = togo_place[1] - '1';

        if (isValidMove(board, fromCol, fromRow, toCol, toRow, 'O')) {
            board[fromRow][fromCol] = ' ';
            board[toRow][toCol] = 'O';
            printf("Computer moves O from %s to %s\n", from_place, togo_place);
        }
        else {
            printf("Error: Computed best move for O is not valid.\n");
            // If no valid optimal move, attempt any valid move
            int moved = 0;
            for (int i = 0; i < numPieces && !moved; i++) {
                for (int j = 0; j < 4; j++) {
                    int moveIdx = i * 4 + j;
                    if (strcmp(ovirtualarray1[moveIdx], "ff") != 0) {
                        strcpy(from_place, oarray[i]);
                        strcpy(togo_place, ovirtualarray1[moveIdx]);

                        int fromRow = from_place[0] - 'a';
                        int fromCol = from_place[1] - '1';
                        int toRow = togo_place[0] - 'a';
                        int toCol = togo_place[1] - '1';

                        if (isValidMove(board, fromCol, fromRow, toCol, toRow, 'O')) {
                            board[fromRow][fromCol] = ' ';
                            board[toRow][toCol] = 'O';
                            printf("O moved from %s to %s inn.\n", from_place, togo_place);
                            moved = 1;
                            break;
                        }
                    }
                }
            }
            if (!moved) {
                printf("No valid move available for O. Passing turn.\n");
            }
        }
    }
    else {
        printf("No valid move available for O. Passing turn.\n");
    }
}


void selection_for_x(char board[BOARD_SIZE][BOARD_SIZE]) {
    strcpy(from_place, "ff");
    strcpy(togo_place, "ff");
    for (int i = 0; i < numPieces; i++) {
        int xRow = xarray[i][0] - 'a';
        int xCol = xarray[i][1] - '1';
        int newRow = xRow;
        int newCol = xCol;
        int moved = 0;

        if (!moved && xRow == 0 && board[1][xCol] == ' ') {
            newRow = 1;
            moved = 1;
        }
        else if (!moved && xRow == BOARD_SIZE - 1 && board[BOARD_SIZE - 2][xCol] == ' ') {
            newRow = BOARD_SIZE - 2;
            moved = 1;
        }

        if (!moved && xCol == 0 && board[xRow][1] == ' ') {
            newCol = 1;
            moved = 1;
        }
        else if (!moved && xCol == BOARD_SIZE - 1 && board[xRow][BOARD_SIZE - 2] == ' ') {
            newCol = BOARD_SIZE - 2;
            moved = 1;
        }

        if (moved) {
            char fromPosition[POSITION_SIZE], toPosition[POSITION_SIZE];
            snprintf(fromPosition, POSITION_SIZE, "%c%d", 'a' + xRow, xCol + 1);
            snprintf(toPosition, POSITION_SIZE, "%c%d", 'a' + newRow, newCol + 1);

            board[xRow][xCol] = ' ';
            board[newRow][newCol] = 'X';
            snprintf(xarray[i], POSITION_SIZE, "%c%d", 'a' + newRow, newCol + 1);
            printf("Computer moved X from %s to %s\n", fromPosition, toPosition);
            return;
        }
    }

    int xValues5[numPieces * 1024];
    int xValues4[numPieces * 256];
    int xValues3[numPieces * 64];
    int xValues2[numPieces * 16];
    int xValues1[numPieces * 4];

    int oImpact = simulation_o(board, oarray, simulation_o_array);
    // Populate xValues5
    for (int i = 0; i < numPieces * 1024; i++) {
        if (strcmp(xvirtualarray5[i], "ff") != 0) {
            xValues5[i] = evaluatePosition(xvirtualarray5[i]) - oImpact;
        }
        else {
            xValues5[i] = INT_MAX; // Assign a large number for invalid positions
        }
    }

    // 5->4
    for (int i = 0; i < numPieces * 256; i++) {
        xValues4[i] = -5000;
        for (int j = 0; j < 4; j++) {
            int idx = i * 4 + j;
            if (xValues5[idx] > xValues4[i]) {
                xValues4[i] = xValues5[idx];
            }
        }
    }

    // 4->3
    for (int i = 0; i < numPieces * 64; i++) {
        xValues3[i] = -5000;
        for (int j = 0; j < 4; j++) {
            int idx = i * 4 + j;
            if (xValues4[idx] > xValues3[i]) {
                xValues3[i] = xValues4[idx];
            }
        }
    }

    int oSimulationImpact1 = simulated_step3_o(board, oarray, simulation_o_array);
    for (int i = 0; i < numPieces * 4; i++) {
        xValues1[i] -= oSimulationImpact1;
    }
    // 3->2
    for (int i = 0; i < numPieces * 16; i++) {
        xValues2[i] = -5000;
        for (int j = 0; j < 4; j++) {
            int idx = i * 4 + j;
            if (xValues3[idx] > xValues2[i]) {
                xValues2[i] = xValues3[idx];
            }
        }
    }

    // 2->1
    for (int i = 0; i < numPieces * 4; i++) {
        xValues1[i] = -5000;
        for (int j = 0; j < 4; j++) {
            int idx = i * 4 + j;
            if (xValues2[idx] > xValues1[i]) {
                xValues1[i] = xValues2[idx];
            }
        }
    }

    int oSimulationImpact = simulated_step1_o(board, oarray, simulation_o_array);
    for (int i = 0; i < numPieces * 4; i++) {
        xValues1[i] -= oSimulationImpact;
    }

    // Determine the best move
    int bestValue = INT_MIN;
    int bestMoveIndex = -1;
    int bestOriginIndex = -1;
    for (int i = 0; i < numPieces; i++) {
        int xValueIndex = i * 4;
        for (int j = 0; j < 4; j++) {
            if (xValues1[xValueIndex + j] > bestValue) {
                bestValue = xValues1[xValueIndex + j];
                bestMoveIndex = xValueIndex + j;
                bestOriginIndex = i;
            }
        }
    }

    // Implement the best move or handle no move found
    if (bestOriginIndex != -1 && bestMoveIndex != -1) {
        strcpy(from_place, xarray[bestOriginIndex]);
        strcpy(togo_place, xvirtualarray1[bestMoveIndex]);

        int fromRow = from_place[0] - 'a';
        int fromCol = from_place[1] - '1';
        int toRow = togo_place[0] - 'a';
        int toCol = togo_place[1] - '1';

        if (isValidMove(board, fromCol, fromRow, toCol, toRow, 'X')) {
            board[fromRow][fromCol] = ' ';
            board[toRow][toCol] = 'X';
            printf("Computer moves X from %s to %s\n", from_place, togo_place);
        }
        else {
            printf("Error: Computed best move for X is not valid.\n");
            // Attempt any valid move if the computed move is not valid
            int moved = 0;
            for (int i = 0; i < numPieces && !moved; i++) {
                for (int j = 0; j < 4; j++) {
                    int moveIdx = i * 4 + j;
                    if (strcmp(xvirtualarray1[moveIdx], "ff") != 0) {
                        strcpy(from_place, xarray[i]);
                        strcpy(togo_place, xvirtualarray1[moveIdx]);

                        fromRow = from_place[0] - 'a';
                        fromCol = from_place[1] - '1';
                        toRow = togo_place[0] - 'a';
                        toCol = togo_place[1] - '1';

                        if (isValidMove(board, fromCol, fromRow, toCol, toRow, 'X')) {
                            board[fromRow][fromCol] = ' ';
                            board[toRow][toCol] = 'X';
                            printf("X moved from %s to %s inn a.\n", from_place, togo_place);
                            moved = 1;
                            break;
                        }
                    }
                }
            }
            if (!moved) printf("No valid move available for X. Passing turn.\n");
        }
    }
    else {
        printf("No valid move available for X. Passing turn.\n");
    }
}


void freeDynamicArrays() {
    // Free all allocated memory
    free(oarray);
    free(ovirtualarray1);
    free(ovirtualarray2);
    free(ovirtualarray3);
    free(ovirtualarray4);

    free(xarray);
    free(xvirtualarray1);
    free(xvirtualarray2);
    free(xvirtualarray3);
    free(xvirtualarray4);
}

int main() {
    srand(time(NULL));
    char board[BOARD_SIZE][BOARD_SIZE];
    memset(board, ' ', sizeof(board));

    printf("Enter the number of X's and O's: ");
    scanf("%d", &numPieces);

    initializeDynamicArrays();

    for (int i = 0; i < numPieces; i++) {
        int row, col;
        do {
            row = rand() % BOARD_SIZE;
            col = rand() % BOARD_SIZE;
        } while (board[row][col] != ' ');
        board[row][col] = 'X';
    }

    for (int i = 0; i < numPieces; i++) {
        int row, col;
        do {
            row = rand() % BOARD_SIZE;
            col = rand() % BOARD_SIZE;
        } while (board[row][col] != ' ');
        board[row][col] = 'O';
    }

    int playerChoice, maxTurns;
    printf("Do you want to be Player 1 (X) or 2 (O)? (1/2): ");
    scanf("%d", &playerChoice);
    printf("Enter the maximum number of turns: ");
    scanf("%d", &maxTurns);

    for (int turn = 0; turn < maxTurns; turn++) {
        printBoard(board);
        if ((playerChoice == 1 && turn % 2 == 0) || (playerChoice == 2 && turn % 2 != 0)) {
            char selectedPiece[POSITION_NAME_SIZE];
            char moveTo[POSITION_NAME_SIZE];
            int x, y, newX, newY;
            do {
                printf("Your turn. Choose piece to move (e.g., a1): ");
                scanf("%s", selectedPiece);
                x = colToIndex(selectedPiece[1]);
                y = rowToIndex(selectedPiece[0]);

                printf("Choose new position for %c%c (e.g., a2): ", selectedPiece[0], selectedPiece[1]);
                scanf("%s", moveTo);
                newX = colToIndex(moveTo[1]);
                newY = rowToIndex(moveTo[0]);
            } while (!isValidMove(board, x, y, newX, newY, playerChoice == 1 ? 'X' : 'O'));

            board[newY][newX] = playerChoice == 1 ? 'X' : 'O';
            board[y][x] = ' ';
            printf("Moved %c%c to %c%c\n", selectedPiece[0], selectedPiece[1], moveTo[0], moveTo[1]);
        }
        else {
            if (playerChoice == 1) {
                updateO(board);
                make_tree_o(board);
                selection_for_o(board);
            }
            else {
                updateX(board);
                make_tree_x(board);
                selection_for_x(board);
            }
        }
        updateX(board);
        updateO(board);
        printBoard(board);

        int emptySpacesAroundX1 = countAdjacentEmptySpaces(board, 'X');
        int emptySpacesAroundO1 = countAdjacentEmptySpaces(board, 'O');

        if (emptySpacesAroundX1 == 0 || emptySpacesAroundO1 == 0) {
            if (emptySpacesAroundX1 == 0 && emptySpacesAroundO1 > 0) {
                printf("O has won, X has no more moves!\n");
                return 0;
            }
            else if (emptySpacesAroundO1 == 0 && emptySpacesAroundX1 > 0) {
                printf("X has won, O has no more moves!\n");
                return 0;
            }
            else if (emptySpacesAroundX1 == 0 && emptySpacesAroundO1 == 0) {
                printf("It's a tie, neither X nor O has any moves left!\n");
                return 0;
            }
        }

    }

    int emptySpacesAroundX = countAdjacentEmptySpaces(board, 'X');
    int emptySpacesAroundO = countAdjacentEmptySpaces(board, 'O');
    printf("Game over. Final board:\n");
    printBoard(board);
    if (emptySpacesAroundX > emptySpacesAroundO) {
        printf("X Won with more empty spaces adjacent to its pieces!\n");
    }
    else if (emptySpacesAroundO > emptySpacesAroundX) {
        printf("O Won with more empty spaces adjacent to its pieces!\n");
    }
    else {
        printf("It's a tie, both X and O have equal adjacent empty spaces!\n");
    }

    freeDynamicArrays();

    return 0;
}