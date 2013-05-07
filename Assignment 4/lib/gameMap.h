/******************************************************************************
* Game Logic
****************************************************************************/
#ifndef GAMELIB_H
#define GAMELIB_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <strings.h>
#include <errno.h>
#include <pthread.h>
#include "general_utils.h"

typedef struct
{
    int x;
    int y;
} Position;

typedef enum
{
    CT_Floor = ' ',
    CT_Wall = '#',
    CT_Jailj = 'j',
    CT_JailJ = 'J',
    CT_Homeh = 'h',
    CT_HomeH = 'H',
} Cell_Type;

typedef enum
{
    Team1 = 1,
    Team2 = 2
} Team;

typedef enum
{
    None = 0,
    Flag_Team1 = 1,
    Flag_Team2 = 2,
    Shovel = 3
} Item_Type;

typedef struct
{
    Item_Type itType;
    Position ItemPos;
} Item;

struct player
{
    int team;
    Position PlayerPos;
    int ID;
	int FD;
    int State;
    Item *i;
    struct player *next;
};

typedef struct
{
    Cell_Type C_Type;
    Position Cell_Pos;
    struct player *p;
    Team Cell_Team;
    Item item;
    int occupied;
    int containItem;
} Cell;

typedef struct
{
    Cell *cells;
    int numFloor;
    int numWall;
    int numCells;
    int numOfJails[2];
    int numOfHomes[2];
    Position dimensions;
} Maze;

/*
int startGame(Deltas *d); // why does it takes deltas. It hsould not. flags should not show up at start_
int stopGame();
int resetGame(Deltas *d);
int gameStat();
int pickUpItem(int playerID, Deltas *d);
int dropItem(int playerID, Deltas *d, Item it);
int breakWall(int playerID, int x, int y, Deltas *d);
int addPlayer(Deltas *d);
int removePlayer (int playerID, Deltas *d);
int movePlayer (int playerID, Deltas *d, char c);
void formatMaze(char * mazeOutput);
int dimX();
int dimY();
Position *findFreeHome(int team);
*/

/******************************************************************************
* Make Array
******************************************************************************/
struct player *create_list(int ID);

/******************************************************************************
* Add a new Player
******************************************************************************/
struct player *add_to_list(int ID, bool add_to_end);

/******************************************************************************
* Find a Player
******************************************************************************/
struct player *search_in_list(int ID, struct player **prev);

/******************************************************************************
* Find a Player using FD
******************************************************************************/
struct player *search_in_list_fd(int fd, struct player **prev);

/******************************************************************************
* Remove a Player
******************************************************************************/
int delete_from_list(int ID);

/******************************************************************************
* Remove a Player using fd
******************************************************************************/
int delete_from_list_fd(int fd);

/******************************************************************************
* Print Player Array
******************************************************************************/
void print_list(void);

/******************************************************************************
* Location in Maze
******************************************************************************/
int getPos(int col, int row);

/******************************************************************************
* Returns Cell Information
******************************************************************************/
Cell cellInfo(int column, int row);

/******************************************************************************
* Starts the Game 
******************************************************************************/
int startGame();

/******************************************************************************
* Fully Stops the Game 
******************************************************************************/
int stopGame();

/******************************************************************************
* Reset the Game for Next Round 
******************************************************************************/
int resetGame(Deltas *d);

/******************************************************************************
* Load the Map
******************************************************************************/
int loadMap();

/******************************************************************************
* Print map to console
******************************************************************************/
void dumpMap();

/******************************************************************************
* Check the status of the game
******************************************************************************/
int gameStat();

/******************************************************************************
* Find a Free Home cell based on team number
******************************************************************************/
Position *findFreeHome(int team);

/******************************************************************************
* Pick up an Item
******************************************************************************/
int pickUpItem(int playerID, Deltas *d);

/******************************************************************************
* Reset the Items
******************************************************************************/
int resetItem(Deltas *d, Item it, int team);

/******************************************************************************
* Drop the Item
******************************************************************************/
int dropItem(int playerID, Deltas *d, Item it);

/******************************************************************************
* Break a Wall 
******************************************************************************/
int breakWall(int playerID, int x, int y, Deltas *d);

/******************************************************************************
* Add a new Player
******************************************************************************/
int addPlayer(int fd, Deltas *d);

/******************************************************************************
* Remove a Player
******************************************************************************/
int removePlayer (int playerID, Deltas *d);

/******************************************************************************
* Remove a Player using the fd 
******************************************************************************/
int removePlayerFD(int fd, Deltas *d);

/******************************************************************************
* Jail a Player
******************************************************************************/
int jailPlayer(struct player *tempPlayer);

/******************************************************************************
* Tag Checker
******************************************************************************/
void tagCheck(struct player *tempPlayer);

/******************************************************************************
* Move Player
******************************************************************************/
int movePlayer (int playerID, Deltas *d, char c);

/******************************************************************************
* Format Maze for output
******************************************************************************/
void formatMaze(char * mazeOutput);

/******************************************************************************
* Return x dimensions
******************************************************************************/
int dimX();

/******************************************************************************
* Return y dimensions
******************************************************************************/
int dimY();

#endif
