//
// Created by axel on 29/09/24.
//

#ifndef SERVER_H

void signalCatch(int); // function to catch signals such as CTRL-C
void disconnect(enum disconnectType); // fully disconnect the socket put in the parameters

int openServerConnection(int argc, char** argv);
int connectClients(struct cell * aircraftCarrier, struct cell * frigate, int * aircraftCarrierDeadCells, int * frigateDeadCells);
char * checkPlayerPlay(struct cell * aircraftCarrier, struct cell * frigate, int * aircraftCarrierDeadCells, int * frigateDeadCells, char * buf_read);
void setupGame();
/**
 * @brief checks if the random position given is valid in the board
 *
 * @param aStartPosition the cell where the center of the boat is
 * @param anOrientation vertical or horizontal
 * @param aBoatSize aircraft carrier (5) or frigate (3)
 * @return true if the boat doesn't goes out of the board
 */
bool is_valid_position(struct cell aStartPosition, enum orientation anOrientation, int aBoatSize);

/**
 * @brief place the boats in random position and orientation
 *
 * @param anAircraftCarrier a 5 cells boat
 * @param aFrigate a 3 cells boat
 */
void place_boats(struct cell anAircraftCarrier[AIRCRAFTCARRIER_SIZE], struct cell aFrigate[FRIGATE_SIZE]);
#define SERVER_H

#endif //SERVER_H
