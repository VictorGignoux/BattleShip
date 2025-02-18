//
// Created by axel on 29/09/24.
//

#ifndef CLIENT_H

void signalCatch(int); // function to catch signals such as CTRL-C
void disconnect(enum disconnectType); // fully disconnect the socket put in the parameters

int connectToServer(int argc, char** argv);
char * listenToServer();
int listenToKeys();
void setupGame();
void gameLoop();

/**
 * @brief displays the board in the terminal (big version)
 *
 * @param aBoard an array that contains the state of the cells board
 */
void write_board(enum cellState aBoard[BOARD_SIZE][BOARD_SIZE], int x, int y);

/**
 * @brief displays the board in the terminal (small version)
 *
 * @param aBoard an array that contains the state of the cells board
 */
void write_board2(enum cellState aBoard[BOARD_SIZE][BOARD_SIZE], int x, int y);
#define CLIENT_H

#endif //CLIENT_H
