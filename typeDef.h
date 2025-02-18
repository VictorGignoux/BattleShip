//
// Created by victor on 26/09/24.
//

#ifndef TYPEDEF_H
#define TYPEDEF_H

const int SIZE_BUFFERS = 256;
const int AIRCRAFTCARRIER_SIZE = 5;
const int FRIGATE_SIZE = 3;

#define BOARD_SIZE 10

enum orientation {
    VERTICAL = 0,
    HORIZONTAL = 1
};

enum cellState{
    EMPTY,
    PLOUF,
    BOATED
};

enum disconnectType {
    full, // Disconnect all the sockets
    clientOnly // only disconnect the client
};

struct cell {
    int x;
    int y;
};

const int MAXCONNECTIONS = 5;

bool deletedConnection=true;

int sock_pipe=-1; /* dialog socket */
struct  sockaddr_in server; /* server SAP */

char * MSG_BOAT_TOUCHED = "Boat touched !\n";
char * MSG_BOAT_DROWN = "Boat drown !\n";
char * MSG_GAME_LOST = "You lost the game...\n";
char * MSG_GAME_WON = "You won the game !\n";
char * MSG_NOTHING_TOUCHED = "Nothing touched...\n";

#endif //TYPEDEF_H
