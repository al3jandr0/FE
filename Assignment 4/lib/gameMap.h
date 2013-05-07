/******************************************************************************
* Copyright (C) 2011 by Jonathan Appavoo, Boston University
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
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

/*
typedef struct
{
    Team team;
    Position PlayerPos;
    int ID;
    int State;
    Item *i;
} Player;
*/

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
    //int hasAbility;
} Item;

struct player
{
    int team;
    Position PlayerPos;
    int ID;
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

int startGame(Deltas *d); // why does it takes deltas. It hsould not. flags should not show up at start_
int stopGame();
int resetGame(Deltas *d);
int gameStat();
int pickUpItem(int playerID, Deltas *d);
int dropItem(int playerID, Deltas *d, Item it);
// int breakWall(int playerID, int x, int y, Deltas *d); // do I need this ? 
int addPlayer(Deltas *d);
int removePlayer (int playerID, Deltas *d);
int movePlayer (int playerID, Deltas *d, char c);  //['U', 'D', 'L', 'R']
void formatMaze(char * mazeOutput);
int dimX();
int dimY();




#endif
