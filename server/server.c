#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>

#include "../typeDef.h"
#include "server.h"

char* id=0;
short port=0;
int sock=0;	/* communication socket*/
int numPID = 0; // To have multiples connection to the server
bool showBoats = false;

// arg 0 : program call (./server)
// arg 1 : id
// arg 2 : port
// arg 3 : showBoats
int main(int argc, char** argv)
{

    // arguments management
    if (argc < 3)
    {
        fprintf(stderr,"usage: %s id port\n",argv[0]);
        exit(1);
    }

    if(argc > 3)
    {
        showBoats = strcmp(argv[3], "showBoats") == 0;
    }

    if (openServerConnection(argc, argv) == -1) 
    {
        exit(-1);
    }

    signal(SIGINT, signalCatch); // Initialize the CTRL-C signal catching

    setupGame();

    return 0;
}

void signalCatch(int sig) 
{
    printf(" Caught\n");

    // Terminate all child processes and the server
    kill(0, SIGTERM);
    disconnect(full);
}

void disconnect(const enum disconnectType type) 
{
    printf("closing clients connection if any\n");

    if(sock_pipe != -1) 
    {
        if(close(sock_pipe) != -1)
        {
            sock_pipe = -1;
            deletedConnection = true;
            printf("Connection to the client closed\n");
        }
        else 
        {
            printf("Error while trying to close the client socket\n");
        }
    }
    else 
    {
        printf("No client found\n");
    }

    if(type == full) 
    {
        if(close(sock)!=-1) 
        {
            printf("Server terminated\n");
        }
        else 
        {
            printf("Error closing the server socket\n");
        }
        exit(0);
    }
}

int openServerConnection(int argc, char** argv) 
{
    id= argv[1];
    port= atoi(argv[2]);

    // Socket creation
    if ((sock = socket (PF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr,"%s: socket %s\n",argv[0],strerror(errno));
        return -1;
    }

    // Set the server structure (setup)
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    // Set the server address and size to the socket
    if (bind (sock, (struct sockaddr *)&server, sizeof(server)) != 0)
    {
        if(errno == EADDRINUSE) 
        {
            fprintf(stderr,"The port you're trying to bind the server into is already in use. \n");
            printf("If you just closed a server the delay before re-opening it is of 60sec.\n");
            return -1;
        }
        else
        {
            fprintf(stderr,"Error while trying to bind the socket:  %s\n",strerror(errno));
            return -1;
        }
    }
    return 1;
}

int connectClients(struct cell * aircraftCarrier, struct cell * frigate, int * aircraftCarrierDeadCells, int * frigateDeadCells) 
{
    // Listen on the socket for the number of connection, if it can't: error.
    if (listen(sock, MAXCONNECTIONS) != 0) 
    {
        fprintf(stderr, "Error while trying to listen to the socket: %s\n", strerror(errno));
        return -1;
    }

    while (1)
    {
        fflush(stdout);  // Flush the output buffer

        struct sockaddr_in client; /* client SAP */
        int len = sizeof(client);
        char buf_read[256], buf_write[256];

        // clears the buffers because somehow unwanted characters sneak in
        memset(buf_read, 0, sizeof(buf_read));
        memset(buf_write, 0, sizeof(buf_write));

        // Accept the connection request and initiate the dialog socket
        int new_sock_pipe = accept(sock, (struct sockaddr *)&client, (unsigned int *)&len);
        if (new_sock_pipe < 0) 
        {
            fprintf(stderr, "Error on accepting connection: %s\n", strerror(errno));
            continue;
        }

        printf("Game for %s:%hu\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        place_boats(aircraftCarrier, frigate);

        int pid = fork();  // Fork a child process to handle the client

        if (pid < 0) 
        {
            fprintf(stderr, "Error on fork: %s\n", strerror(errno));
            close(new_sock_pipe);  // Close the socket if fork fails
            continue;
        }

        if (pid == 0) 
        {  
            // Child process
            close(sock);  // Child doesn't need the listening socket

            // Child handles communication with the client
            while (1) 
            {
                if (read(new_sock_pipe, buf_read, sizeof(buf_read)) > 0) 
                {
                    printf("========================== Client %s:%hu ==========================\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
                    printf("Message from client : ", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
                    printf("%s\n", buf_read);

                    char* playerPlay = checkPlayerPlay(aircraftCarrier, frigate, aircraftCarrierDeadCells, frigateDeadCells, buf_read);
                    sprintf(buf_write, "%s", playerPlay);
                    printf("Response : %s", buf_write);

                    write(new_sock_pipe, buf_write, strlen(buf_write));

                    // Prepare response
                    if (strcmp(playerPlay, MSG_GAME_WON) == 0 || strcmp(playerPlay, MSG_GAME_LOST) == 0) 
                    {
                        printf("Player Won the game! \n");
                        printf("=========================================================================== \n\n");
                        break;  // Game over, exit the loop
                    }

                    printf("=========================================================================== \n\n");
                } 
                else 
                {
                    // Client disconnected
                    printf("Client '%s:%hu' disconnected.\n",inet_ntoa(client.sin_addr), ntohs(client.sin_port));
                    break;
                }
            }

            close(new_sock_pipe);  // Close the client's socket in the child
            exit(0);  // Terminate the child process
        } 
        else 
        {  
            // Parent process
            close(new_sock_pipe);
        }
    }
}

char * checkPlayerPlay(struct cell * aircraftCarrier, struct cell * frigate, int * aircraftCarrierDeadCells, int * frigateDeadCells, char * buf_read)
{
    int x=0, y=0;
    const char * separator = "-";
    char * strToken = strtok(buf_read, separator);
    while(strToken != NULL)
    {
        if(x > 0)
        {
            y = atoi(strToken);
        }
        else
        {
            x = atoi(strToken);
        }
        strToken = strtok(NULL, separator);
    }

    int i = 0;
    bool aircratTouched, frigateTouched;
    while(i < AIRCRAFTCARRIER_SIZE && !aircratTouched)
    {
        if(aircraftCarrier[i].x == x && aircraftCarrier[i].y == y)
        {
            aircratTouched = true;
            *aircraftCarrierDeadCells += 1;
            
        }
        i++;
    }
    i = 0;
    while(i < FRIGATE_SIZE && !frigateTouched)
    {
        if(frigate[i].x == x && frigate[i].y == y)
        {
            frigateTouched = true;
            *frigateDeadCells += 1;
        }
        i++;
    }

    if(*frigateDeadCells + *aircraftCarrierDeadCells == AIRCRAFTCARRIER_SIZE + FRIGATE_SIZE)
    {
        return MSG_GAME_WON;
    }
    else if(aircratTouched)
    {
        if(*aircraftCarrierDeadCells == AIRCRAFTCARRIER_SIZE)
        {
            return MSG_BOAT_DROWN;
        }
        else
        {
            return MSG_BOAT_TOUCHED;
        }
    }
    else if(frigateTouched)
    {
        if(*frigateDeadCells == FRIGATE_SIZE)
        {
            return MSG_BOAT_DROWN;
        }
        else
        {
            return MSG_BOAT_TOUCHED;
        }
    }
    else
    {
        return MSG_NOTHING_TOUCHED;
    }
}

bool is_valid_position(struct cell aStartPosition, enum orientation anOrientation, int aBoatSize) 
{
    bool valid;
    if(anOrientation == VERTICAL) 
    {
        valid = aStartPosition.y + aBoatSize/2 <= BOARD_SIZE && aStartPosition.y - aBoatSize/2 > 0;
    }
    else if(anOrientation == HORIZONTAL) 
    {
        valid = aStartPosition.x + aBoatSize/2 <= BOARD_SIZE && aStartPosition.x - aBoatSize/2 > 0;
    }

    return valid;
}

void place_boats(struct cell anAircraftCarrier[AIRCRAFTCARRIER_SIZE], struct cell aFrigate[FRIGATE_SIZE])
{
    int i;
    struct cell newCell;
    enum orientation newOrientation;

    // ------------------- placing aircratf carrier --------------------
    newOrientation = rand() % 2;
    do
    {
        newCell.x = rand() % BOARD_SIZE +1;
        newCell.y = rand() % BOARD_SIZE +1;
    }while(!is_valid_position(newCell, newOrientation, AIRCRAFTCARRIER_SIZE));

    // display boat position (center cell x, center cell y, orientation)
    // printf("%d, %d, %d\n", newCell.x, newCell.y, newOrientation);

    if(newOrientation == VERTICAL)
    {
        for(i = 0; i < AIRCRAFTCARRIER_SIZE; i++)
        {
            anAircraftCarrier[i].x = newCell.x;
            anAircraftCarrier[i].y = newCell.y + i-2;
        }
    }
    else
    {
        for(i = 0; i < AIRCRAFTCARRIER_SIZE; i++)
        {
            anAircraftCarrier[i].y = newCell.y;
            anAircraftCarrier[i].x = newCell.x + i-2;
        }
    }

    // display boat position
    if(showBoats)
    {
        printf("\n[ AIRCRAFT CARRIER ]\n");
        for(i = 0; i < AIRCRAFTCARRIER_SIZE; i++) 
        {
            printf("(%d-%d)\n", anAircraftCarrier[i].x, anAircraftCarrier[i].y);
        }
    }

    // ------------------------ placing frigate --------------------
    newOrientation = rand() % 2;
    do
    {
        newCell.x = rand() % BOARD_SIZE +1;
        newCell.y = rand() % BOARD_SIZE +1;
    }while(!is_valid_position(newCell, newOrientation, AIRCRAFTCARRIER_SIZE));

    // dipsplay boat positiion (center cell x, center cell y, orientation)
    // printf("%d, %d, %d\n", newCell.x, newCell.y, newOrientation);

    if(newOrientation == VERTICAL)
    {
        for(i = 0; i < FRIGATE_SIZE; i++)
        {
            aFrigate[i].x = newCell.x;
            aFrigate[i].y = newCell.y + i-1;
        }
    }
    else
    {
        for(i = 0; i < FRIGATE_SIZE; i++)
        {
            aFrigate[i].y = newCell.y;
            aFrigate[i].x = newCell.x + i-1;
        }
    }

    // display boat position
    if(showBoats)
    {
        printf("\n[ FRIGATE ]\n");
        for(i = 0; i < FRIGATE_SIZE; i++) {
            printf("(%d-%d)\n", aFrigate[i].x, aFrigate[i].y);
        }
    }
}

void setupGame() 
{
    srand(time(NULL));

    struct cell aircraftCarrier[AIRCRAFTCARRIER_SIZE];
    int aircraftCarrierDeadCells = 0;

    struct cell frigate[FRIGATE_SIZE];
    int frigateDeadCells = 0;

    system("clear");

    printf("Serveur en attente des joueurs...\n");

    if (connectClients(aircraftCarrier, frigate, &aircraftCarrierDeadCells, &frigateDeadCells) == -1) 
    {
        exit(-1);
    }
}