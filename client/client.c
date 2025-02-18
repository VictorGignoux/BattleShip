#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>

#include "../typeDef.h"
#include "client.h"

char* id=0;
short sport=0;
int sock=0;	/* communication socket */

struct  sockaddr_in client; /* client SAP  */
struct  sockaddr_in server; /* server SAP  */
int len;
long int ret = 0;

enum cellState board[BOARD_SIZE][BOARD_SIZE];
int nbShots = 30;
bool hasWon = false;

//arg 0 : program call (./client)
//arg 1 : id
//arg 2 : @IP
//arg 3 : sport
int main(int argc, char** argv)
{

    // Text in case not enough arguments are sent
    if (argc!=4)
    {
        fprintf(stderr,"usage: %s id server port\n",argv[0]);
        return -1;
    }

    if(connectToServer(argc, argv) == -1)
    {
        exit(-1);
    }

    setupGame();

    gameLoop();

    return 0;
}

void disconnect(enum disconnectType type) 
{
    // end of the game
    if (close(sock) == -1) 
    {
        fprintf(stderr,"Error while closing the socket: %s\n", strerror(errno));
    }
    else 
    {
        printf("Socket closed.\n");
        printf("Game is over, hope to see you soon\n");
    }
    exit(0);
}
void signalCatch(int sig) 
{
    printf(" Caught\n");
    disconnect(full);
}

int connectToServer(int argc, char** argv) 
{
    // Set the id of the client and the server port
    id= argv[1];
    sport= atoi(argv[3]);


    // Open the socket
    if ((sock = socket (PF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr,"Error in the socket creation: %s\n", strerror(errno));
        return -1;
    }

    // Set the server structure (setup)
    server.sin_family = AF_INET;
    server.sin_port = htons(sport);
    inet_aton(argv[2],&server.sin_addr);

    // Tries to connect to the server and display an error otherwise
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) != 0)
    {
        fprintf(stderr,"Error during the connection:  %s\n",strerror(errno));
        return -1;
    }

    return 0;
}

char * listenToServer() 
{
    len=sizeof(client);
    getsockname(sock,(struct  sockaddr  *)&client,(unsigned int *)&len); // Set the socket to the struct and set its len

    char buf_read[256], buf_write[256];
    char * lastServerMessage;

    // clears the buffers because somehow unwanted characters sneak in
    memset(buf_read, 0, sizeof(buf_read));
    memset(buf_write, 0, sizeof(buf_write));

    // sprintf(buf_write,respondText);
    // "listen" to messages from the server
    while(1)
    {
        if(read(sock, buf_read, SIZE_BUFFERS) > 0) 
        {
            lastServerMessage = buf_read;
            //printf("New message from the server: %s\n", lastServerMessage);
            return lastServerMessage;
        }
        else
        {
            printf("Server connection has been closed. \n");
            break;
        }

        // if(strcmp(helloText,lastServerMessage) == 0) {
        //     ret = write(sock, buf_write, strlen(buf_write));
        //     printf("Message sent to the server (%ld bytes): %s \n",ret,buf_write);
        //     break;
        // }
    }
    //disconnect(full);
}

void write_board(enum cellState aBoard[BOARD_SIZE][BOARD_SIZE], int x, int y)
{

    system("clear");

    int i, j;
    char buf_write[256];

    for(i = 0; i < BOARD_SIZE; i++)
    {
        // draw the first line
        for(j = 0; j < BOARD_SIZE; j++)
        {
            write(1, "\033[40m      \033[0m", 15);
        }
        write(1, "\033[40m   \033[0m", 12);
        write(1, "\n", 2);

        // draw the lines and the cells depending to their state
        for(j = 0; j < BOARD_SIZE; j++)
        {
            write(1, "\033[40m   \033[0m", 12);
            if(aBoard[i][j] == EMPTY)
            {
                write(1, "\033[43m   \033[0m", 12);
            }
            else if(aBoard[i][j] == PLOUF)
            {
                write(1, "\033[47m   \033[0m", 12);
            }
            else if(aBoard[i][j] == BOATED)
            {
                write(1, "\033[41m   \033[0m", 12);
            }
        }

        // draw the number of the line
        write(1, "\033[40m   \033[0m", 12);
        sprintf(buf_write, " %d", i+1);
        write(1, buf_write, strlen(buf_write));
        write(1, "\n", 2);
    }

    // draw the last line
    for(j = 0; j < BOARD_SIZE; j++)
    {
        write(1, "\033[40m      \033[0m", 15);
    }
    write(1, "\033[40m   \033[0m", 12);
    write(1, "\n", 2);

    // draw the numbers of the columns
    for(i = 0; i < BOARD_SIZE; i++)
    {
        sprintf(buf_write, "    %d ", i+1);
        write(1, buf_write, 6);
    }
    write(1, "\n", 2);

    printf("Nombre de tentatives restantes : %d\n", nbShots);

    if(x != -1 && y != -1)
    {
        printf("Dernière case jouée : %d-%d\n", x, y);
    }
}

void write_board2(enum cellState aBoard[BOARD_SIZE][BOARD_SIZE], int x, int y)
{

    system("clear");

    int i, j;
    char buf_write[256];

    for(i = 0; i < BOARD_SIZE; i++)
    {
        // draw the lines
        for(j = 0; j < BOARD_SIZE; j++)
        {
            if(aBoard[i][j] == EMPTY)
            {
                write(1, "\033[43m . \033[0m", 12);
            }
            else if(aBoard[i][j] == PLOUF)
            {
                write(1, "\033[47m O \033[0m", 12);
            }
            else if(aBoard[i][j] == BOATED)
            {
                write(1, "\033[41m X \033[0m", 12);
            }
        }
        // draw the numbers of the lines
        sprintf(buf_write, " %d", i+1);
        write(1, buf_write, strlen(buf_write));
        write(1, "\n", 2);
    }

    //draw the numbers of the columns
    for(i = 0; i < BOARD_SIZE; i++)
    {
        sprintf(buf_write, " %d ", i+1);
        write(1, buf_write, 3);
    }
    write(1, "\n", 2);

    printf("Nombre de tentatives restantes : %d\n", nbShots);

    if(x != -1 && y != -1)
    {
        printf("Dernière case jouée : %d-%d\n", x, y);
    }
}

void setupGame() 
{
    // initialisation of the board
    int i, j;
    for(i = 0; i < BOARD_SIZE; i++)
    {
        for(j = 0; j < BOARD_SIZE; j++)
        {
            board[i][j] = EMPTY;
        }
    }

    write_board(board, -1, -1);
}

void gameLoop() 
{
    while(!hasWon && nbShots > 0)
    {
        char buf_write[256];
        int x=0, y=0;
        printf("Entrez une case (x-y) : ");
        scanf("%d-%d", &y, &x);

        if(x > 0 && x <= BOARD_SIZE && y > 0 && y <= BOARD_SIZE)
        {
            if(board[x-1][y-1] != EMPTY)
            {
                printf("ERREUR : case deja jouee !\n");
            }
            else
            {
                //envoi de la case au serveur
                nbShots--;
                sprintf(buf_write, "%d-%d", y, x);
                write(sock, buf_write, strlen(buf_write));

                //check message de retour
                char response[256];
                sprintf(response, listenToServer());
                if(strcmp(response, MSG_NOTHING_TOUCHED))
                {
                    board[x-1][y-1] = PLOUF;
                }
                else
                {
                    board[x-1][y-1] = BOATED;
                }

                hasWon = strcmp(response, MSG_GAME_WON) == 0;

                write_board(board, x, y);
                printf(response);
            }
        }
        else
        {
            printf("ERREUR : case inexistante !\n");
        }

        int buffer = 0;
        while(buffer != '\n' && buffer != EOF)
        {
            buffer = getchar();
        }
    }

    if(nbShots == 0)
    {
        printf(MSG_GAME_LOST);
    }
}