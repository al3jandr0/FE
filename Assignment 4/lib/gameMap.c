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
#include "gameMap.h"

enum { FALSE, TRUE };

#define DEBUG FALSE

#define debugPrint(...) \
    do { if (DEBUG) fprintf(stderr, __VA_ARGS__); } while (0)

int gamePlayingFlag = FALSE;

int playerCount = 0;

#define MAX 200
struct player *head = NULL;
struct player *curr = NULL;

Position *findFreeHome(int team);

struct player *create_list(int ID)
{
    debugPrint("\n creating player list with headnode as [%d]\n", ID);
    struct player *ptr = (struct player *)malloc(sizeof(struct player));
    if (NULL == ptr)
    {
        printf("\n Node creation failed \n");
        return NULL;
    }
    ptr->ID = ID;
    ptr->next = NULL;

    head = curr = ptr;
    return ptr;
}

struct player *add_to_list(int ID, bool add_to_end)
{
    if (NULL == head)
    {
        return (create_list(ID));
    }

    if (add_to_end)
        debugPrint("\n Adding player to end of list with id [%d]\n", ID);
    else
        debugPrint("\n Adding player to beginning of list with id [%d]\n", ID);

    struct player *ptr = (struct player *)malloc(sizeof(struct player));
    if (NULL == ptr)
    {
        printf("\n Node creation failed \n");
        return NULL;
    }
    ptr->ID = ID;
    ptr->next = NULL;

    if (add_to_end)
    {
        curr->next = ptr;
        curr = ptr;
    }
    else
    {
        ptr->next = head;
        head = ptr;
    }
    return ptr;
}

struct player *search_in_list(int ID, struct player **prev)
{
    struct player *ptr = head;
    struct player *tmp = NULL;
    bool found = false;

    debugPrint("\n Searching the player list for ID [%d] \n", ID);

    while (ptr != NULL)
    {
        if (ptr->ID == ID)
        {
            found = true;
            break;
        }
        else
        {
            tmp = ptr;
            ptr = ptr->next;
        }
    }

    if (true == found)
    {
        if (prev)
            *prev = tmp;
        return ptr;
    }
    else
    {
        return NULL;
    }
}

int delete_from_list(int ID)
{
    struct player *prev = NULL;
    struct player *del = NULL;

    debugPrint("\n Deleting player with id [%d] from list\n", ID);

    del = search_in_list(ID, &prev);
    if (del == NULL)
    {
        return -1;
    }
    else
    {
        if (prev != NULL)
            prev->next = del->next;

        if (del == curr)
        {
            curr = prev;
        }
        else if (del == head)
        {
            head = del->next;
        }
    }

    free(del);
    del = NULL;

    return 0;
}

void print_list(void)
{
    struct player *ptr = head;

    printf("\n -------Printing player list------- \n");
    while (ptr != NULL)
    {
        printf("\n Player Id : [%d] \n Player Team : [%d]\n Player Position : [%d,%d]\n Player State : [%d]\n", ptr->ID, ptr->team, ptr->PlayerPos.x, ptr->PlayerPos.y, ptr->State);
        ptr = ptr->next;
    }
    return;
}

Maze maze;
//Player *PlayerList;
//Player* PlayerList = malloc(MAX * sizeof(Player));

//int homeCount0 = 0;
Cell *HomeList0;
//Cell* HomeList0 = malloc(MAX * sizeof(Cell));
//int homeCount1 = 0;
Cell *HomeList1;
//Cell* HomeList1 = malloc(MAX * sizeof(Cell));

//int jailCount0 = 0;
Cell *JailList0;
//Cell* JailList0 = malloc(MAX * sizeof(Cell));
//int jailCount1 = 0;
Cell *JailList1;
//Cell* JailList1 =  malloc(MAX * sizeof(Cell));

/*
Cell *HomeList0 = malloc(numOfHomes[0] * sizeof *HomeList0);
Cell *HomeList1 = malloc(numOfHomes[1] * sizeof *HomeList1);

Cell *JailList0 = malloc(numOfJails[0] * sizeof *JailList0);
Cell *JailList1 = malloc(numOfJails[1] * sizeof *JailList1);
*/


int getPos(int col, int row)
{
/*    int pos;
    if (row == 0 && col == 0)
    {
        pos = 0;
    }
    else if (row == 0)
    {
        pos = col;
    }
    else if (col == 0)
    {
        pos = row * maze.dimensions.y;
    }
    else
    {
        pos = row * MAX + col;
    }
    return pos;
*/
    return row * maze.dimensions.y + col;
}

//extern int
int findNumHome(int par)
{
    if (par == 1)
    {
        return maze.numOfHomes[0];
    }
    else if (par == 2)
    {
        return maze.numOfHomes[1];
    }
    else
    {
        return -1;
    }
}

//extern int
int findNumJail(int par)
{
    if (par == 1)
    {
        return maze.numOfJails[0];
    }
    else if (par == 2)
    {
        return maze.numOfJails[1];
    }
    else
    {
        return -1;
    }
}

//extern int
int findNumWall()
{
    return maze.numWall;
}

//extern int
int findNumFloor()
{
    return maze.numFloor;
}


//extern void
void findDimensions(Position *pos)
{
    pos->x = maze.dimensions.x;
    pos->y = maze.dimensions.y;
}

//extern void
void findCInfo(int column, int row)
{
    int pos;
    if (row == 0 && column == 0)
    {
        pos = 0;
    }
    else if (row == 0)
    {
        pos = column;
    }
    else if (column == 0)
    {
        pos = row * maze.dimensions.y;
    }
    else
    {
        pos = row * MAX + column;
    }
    maze.cells[pos];
    printf("\nCell Info for (%d,%d)-(column,row) is:\nCell Type is '%c',  Cell Team is %d, ", column, row, maze.cells[pos].C_Type, maze.cells[pos].Cell_Team);
    if (maze.cells[pos].p == NULL)
    {
        printf("Cell Is Not Occupied\n");
    }
    else
    {
        printf("Cell Is Occupied\n");
    }
}


//extern Cell
Cell cellInfo(int column, int row)
{
    int pos;
    if (row == 0 && column == 0)
    {
        pos = 0;
    }
    else if (row == 0)
    {
        pos = column;
    }
    else if (column == 0)
    {
        pos = row * maze.dimensions.y;
    }
    else
    {
        pos = row * MAX + column;
    }

    return maze.cells[pos];

    /*
    printf("\nCell Info for (%d,%d)-(column,row) is:\nCell Type is '%c',  Cell Team is %d, ", column, row, maze.cells[pos].C_Type, maze.cells[pos].Cell_Team);

    if (maze.cells[pos].p == NULL)
    {
        printf("Cell Is Not Occupied\n");
    }
    else
    {
        printf("Cell Is Occupied\n");
    }
    */
}

int startGame(Deltas *d)
{
    if (playerCount > -1 && gamePlayingFlag == FALSE)
    {
        printf("Starting the Game\n");

        //PlayerList = malloc(MAX * sizeof(Player));
        HomeList0 = malloc(MAX * sizeof(Cell));
        HomeList1 = malloc(MAX * sizeof(Cell));
        JailList0 = malloc(MAX * sizeof(Cell));
        JailList1 = malloc(MAX * sizeof(Cell));

        printf("Loading the Map\n");

        if (loadMap() < 0)
           return -1;

        Item FlagT0;
        resetItem(d, FlagT0, 0);

        Item FlagT1;
        resetItem(d, FlagT1, 1);

        Item ShovelT0;
        resetItem(d, ShovelT0, 0);

        Item ShovelT1;
        resetItem(d, ShovelT1, 1);

        gamePlayingFlag = TRUE;
        return 1;
    }
    else
        printf("Cannot start the Game yet.\n");

    return -1;
}

int stopGame()
{
    int x;

    /*

        for (x = 0; x < MAX; x++)
        {
            free(PlayerList[x]);
        }

        for (x = 0; x < maze.numOfHomes[0]; x++)
        {
            free(HomeList0[x]);
        }

        for (x = 0; x < maze.numOfHomes[1]; x++)
        {
            free(HomeList1[x]);
        }

        for (x = 0; x < maze.numOfJails[0]; x++)
        {
            free(JailList0[x]);
        }

        for (x = 0; x < maze.numOfJails[1]; x++)
        {
            free(JailList1[x]);
        }

    */

    if (gamePlayingFlag == TRUE)
    {
        //free(PlayerList);

        int i = 0, ret = 0;

        struct player *ptr = NULL;

        for (i = 0; i < playerCount; i++)
        {
            ret = delete_from_list(i);

            if (ret != 0)
            {
                printf("\n delete [val = %d] failed, no such element found\n", i);
            }
            else
            {
                printf("\n delete [val = %d]  passed \n", i);
            }
        }


        free(HomeList0);

        free(HomeList1);

        free(JailList0);

        free(JailList1);

        maze.numFloor = 0;
        maze.numWall = 0;
        maze.numCells = 0;
        maze.numOfJails[0] = 0;
        maze.numOfJails[1] = 0;
        maze.numOfHomes[0] = 0;
        maze.numOfHomes[1] = 0;
        maze.dimensions.x = -1;
        maze.dimensions.y = -1;

        free(maze.cells);

        playerCount = 0;
        // homeCount0 = 0;
        // homeCount1 = 0;
        // jailCount0 = 0;
        // jailCount1 = 0;

        gamePlayingFlag = FALSE;
        return 1;
    }
    else
        return -1;

}



int resetGame(Deltas *d)
{
    if (playerCount > -1)
    {
        printf("Starting the Game\n");

        //PlayerList = malloc(MAX * sizeof(Player));
        HomeList0 = malloc(MAX * sizeof(Cell));
        HomeList1 = malloc(MAX * sizeof(Cell));
        JailList0 = malloc(MAX * sizeof(Cell));
        JailList1 = malloc(MAX * sizeof(Cell));

        printf("Loading the Map\n");

        loadMap();

        Item FlagT0;
        resetItem(d, FlagT0, 0);

        Item FlagT1;
        resetItem(d, FlagT1, 1);

        Item ShovelT0;
        resetItem(d, ShovelT0, 0);

        Item ShovelT1;
        resetItem(d, ShovelT1, 1);


        gamePlayingFlag = TRUE;
        int i;
        struct player *ptr = NULL;
        Position *newPlayerPos = NULL;


        for ( i = 0; i < playerCount; ++i)
        {

            ptr = search_in_list(i, NULL);

            if (NULL == ptr)
            {
            }
            else
            {
                newPlayerPos = findFreeHome(ptr->team);

                if (newPlayerPos->x != NULL && newPlayerPos->y != NULL)
                {
                    ptr->PlayerPos = *newPlayerPos;
                }

                add_delta_player(d, ptr, sizeof(struct player));
            }
        }

        return 1;

    }

    else
        printf("Error: Cannot restart the Game!\n");

    return -1;

}

//extern Maze

int loadMap()
{
    FILE *mapFile;
    if ((mapFile = fopen("./../lib/daGame.map", "r")) == NULL)
    {
        perror ("Cannot Open daGame.map. ");
        return -1;
    }

    int c;
    int columnCounter = 0;
    while ((c = fgetc(mapFile)) != 10)
    {
        columnCounter++;
    }
    maze.dimensions.y = columnCounter;

    int rowCounter = 1;
    char s[columnCounter + 2];

    while ((fgets(s, columnCounter + 2, mapFile)) != NULL)
    {
        rowCounter++;
    }
    maze.dimensions.x = rowCounter;

    rewind(mapFile);

    maze.cells = malloc((columnCounter * rowCounter) * sizeof(Cell));

    int currentIndex = 0;
    while ((c = fgetc(mapFile)) != EOF)
    {
        if (c != 10)
        {

            Cell newCell;
            newCell.C_Type = c;
            newCell.Cell_Pos.x = currentIndex / maze.dimensions.x;
            newCell.Cell_Pos.y = currentIndex % maze.dimensions.y;
            if (newCell.Cell_Pos.y < (MAX / 2))
            {
                newCell.Cell_Team = Team1;
            }
            else
            {
                newCell.Cell_Team = Team2;
            }
            newCell.p = NULL;
            maze.cells[currentIndex] = newCell;

            Item newItem;

            if (c == 's' || c == 'S' || c == 'F' || c == 'f')
            {
                newItem.itType = c;
            }

            if (c == '#')
            {
                maze.numWall++;
            }
            else
            {
                maze.numFloor++;
            }

            if (c == 'j')
            {

                JailList0[maze.numOfJails[0]] = newCell;
                maze.numOfJails[0]++;
            }
            else if (c == 'J')
            {
                JailList1[maze.numOfJails[1]] = newCell;
                maze.numOfJails[1]++;
            }
            else if (c == 'h')
            {
                HomeList0[maze.numOfHomes[0]] = newCell;
                maze.numOfHomes[0]++;
            }
            else if (c == 'H')
            {
                HomeList1[maze.numOfHomes[1]] = newCell;

                maze.numOfHomes[1]++;
            }

            currentIndex++;

        }
        else
        {
        }
    }
    fclose(mapFile);
    //return maze;

    return 1;
}

//extern void
void dumpMap()
{
    int x = (maze.dimensions.y * maze.dimensions.x);
    int i = 0;

    for (i = 0; i < (x); i++)
    {

        int c =  maze.cells[i].C_Type;
        char print = (char) c;

        if ((i % maze.dimensions.y) == 0)
        {
            printf("\n%c", print);
        }
        else
        {
            printf("%c", print);
        }
    }

    return;
}

int gameStat()
{
    int p;
    int checker = 0;

    if (gamePlayingFlag == FALSE)
        return -1;


    if (playerCount == 0 && gamePlayingFlag == TRUE)
        return 2;

    for (p = 0; p < maze.numOfJails[1]; p++)
    {
        if (JailList1[p].occupied == 1)
        {
            checker++;
        }
    }

    if (checker == 0 ) //&&  flag1 == TRUE)
        return 3;


    checker = 0;


    //return 4; // Team 2 wins
    for (p = 0; p < maze.numOfJails[0]; p++)
    {
        if (JailList0[p].occupied == 1)
        {
            checker++;
        }
    }

    if (checker == 0 ) //&&  flag0 == TRUE)
        return 4;


    return 1;

    /*
    // flag location
    int temp = 0;

    for ()
    {

    }

    2


    if (teamWins() == 2)


        return 4; // Team 2 wins

    if (teamWins() == 1)
        return 3; // Team 1 wins

    if (teamWins() == 2)
        return 2; // Game is a Draw

        return 1; // Game started

        return -1; // Game not started
        */
}


Position *findFreeHome(int team)
{

    printf("Trying to find location for team %d\n", team);

    int p = 0;

    Position *newPos = NULL;

    newPos = malloc(sizeof(Position));

    if (team == 0)
    {
        printf("%s\n", "Team 0");

        for (p = 0; p < maze.numOfHomes[0]; p++)
        {
            if (HomeList0[p].occupied != team)
            {
                newPos->x = HomeList0[p].Cell_Pos.x;
                newPos->y = HomeList0[p].Cell_Pos.y;
                HomeList0[p].occupied = team;
                printf("Free Location for team %d found at %d, %d\n", team, newPos->x, newPos->y);
                return newPos;
                //break;
            }
        }
    }

    if (team == 1)
    {
        printf("%s\n", "Team 1");
        for (p = 0; p < maze.numOfHomes[1]; p++)
        {
            if (HomeList1[p].occupied != team)
            {
                newPos->x = HomeList1[p].Cell_Pos.x;
                newPos->y = HomeList1[p].Cell_Pos.y;
                HomeList1[p].occupied = team;
                printf("Free Location for team %d found at %d, %d\n", team, newPos->x, newPos->y);
                return newPos;
                //break;
            }
        }
    }

    newPos->x = 1;
    newPos->y = 1;

    printf("Free Location for team %d found at %d, %d\n", team, newPos->x, newPos->y);


    return newPos;
}

int pickUpItem(int playerID, Deltas *d)
{
    struct player *ptr = NULL;
    // Cell possibleItemCell;
    Item *itPtr = NULL;
    int pos;

    ptr = search_in_list(playerID, NULL);


    pos = getPos(ptr->PlayerPos.y, ptr->PlayerPos.x);

    //maze.cells[pos].Cell_Pos.x=ptr->PlayerPos.x;
    //maze.cells[pos].Cell_Pos.y=ptr->PlayerPos.y;
    if (maze.cells[pos].containItem == TRUE)
    {

        itPtr->ItemPos.x = ptr->PlayerPos.x;
        itPtr->ItemPos.y = ptr->PlayerPos.y;


        ptr->i = itPtr;


        maze.cells[pos].containItem = FALSE;


        add_delta_player(d, ptr, sizeof(struct player));
        add_delta_cell(d, &(maze.cells[pos]), sizeof(Cell));
        add_delta_item(d, itPtr, sizeof(Item));

        return 1;
    }
    else
    {
        return -1;
    }
}

int resetItem(Deltas *d, Item it, int team)
{
    //Item *itPtr = NULL;
    int row, col, pos;
    int dx = dimX();
    int dy = dimY();


    //printf("it.ItemPos.x before: %d\n", it.ItemPos.x);
    // printf("it.ItemPos.y before: %d\n", it.ItemPos.y);

    if (it.itType == 3)
    {

        Position *NewPos = NULL;
        NewPos = findFreeHome(team);
        pos = getPos(NewPos->y, NewPos->x);

        // printf("it.ItemPos.x before: %d\n", it.ItemPos.x);
        // printf("it.ItemPos.y before: %d\n", it.ItemPos.y);

        it.ItemPos.x = NewPos->x;
        it.ItemPos.y = NewPos->y;
        //  it.Cell.containItem=TRUE;
        //  it.ItemPos=maze.cells[NewPos].Cell_Pos;
        // printf("it.ItemPos.x after: %d\n", it.ItemPos.x);
        // printf("it.ItemPos.y after: %d\n", it.ItemPos.y);
        // printf("maze.cells[pos].Cell_Pos.x: %d\n", maze.cells[pos].Cell_Pos.x);
        // printf("maze.cells[pos].Cell_Pos.y: %d\n", maze.cells[pos].Cell_Pos.y);

        maze.cells[pos].Cell_Pos.x = it.ItemPos.x;
        maze.cells[pos].Cell_Pos.y = it.ItemPos.y;
        maze.cells[pos].containItem = TRUE;
        maze.cells[pos].item = it;

        add_delta_cell(d, &(maze.cells[pos]), sizeof(Cell));
        add_delta_item(d, &it, sizeof(Item));

        // it.ItemPos=NewPos;
    }

    // printf("it.ItemPos.x after: %d\n", it.ItemPos.x);
    // printf("it.ItemPos.y after: %d\n", it.ItemPos.y);
    // printf("maze.cells[pos].Cell_Pos.x: %d\n", maze.cells[pos].Cell_Pos.x);
    // printf("maze.cells[pos].Cell_Pos.y: %d\n", maze.cells[pos].Cell_Pos.y);



    else if (it.itType = 1)//position at the part of team1
    {


        do
        {

            row = (rand() % (dx / 2) + 0);
            col = (rand() % (dy / 2) + 0);
            //     printf("row: %d\n", row);
            //     printf("col %d\n", col);

            //       printf("dx: %d\n", dx);
            //       printf("dy %d\n", dy);

            pos = getPos(col, row);

            //      printf("it.ItemPos.x stays: %d\n", it.ItemPos.x);
            //      printf("it.ItemPos.y stays: %d\n", it.ItemPos.y);

        }
        while (maze.cells[pos].C_Type != CT_Floor);



        it.ItemPos.x = row;
        it.ItemPos.y = col;

        maze.cells[pos].Cell_Pos.x = it.ItemPos.x;
        maze.cells[pos].Cell_Pos.y = it.ItemPos.y;
        maze.cells[pos].item = it;
        maze.cells[pos].containItem = TRUE;

        add_delta_cell(d, &(maze.cells[pos]), sizeof(Cell));
        add_delta_item(d, &it, sizeof(Item));

        return 1;

    }
    else if (it.itType = 2)//position at the part of team2
    {
        do
        {
            row = (rand()  % dx + (dx / 2));
            col = (rand()  % dy + (dy / 2));


            pos = getPos(col, row);

        }
        while (maze.cells[pos].C_Type != CT_Floor);
        it.ItemPos.x = row;
        it.ItemPos.y = col;

        maze.cells[pos].Cell_Pos.x = it.ItemPos.x;
        maze.cells[pos].Cell_Pos.y = it.ItemPos.y;
        maze.cells[pos].item = it;
        maze.cells[pos].containItem = TRUE;

        add_delta_cell(d, &(maze.cells[pos]), sizeof(Cell));
        add_delta_item(d, &it, sizeof(Item));

        return 1;
    }


    else
    {
        return -1;
    }
}


int dropItem(int playerID, Deltas *d, Item it)
{
    struct player *ptr = NULL;

    Item *itPtr = NULL;
    int pos;

    ptr = search_in_list(playerID, NULL);
    ptr = search_in_list(playerID, NULL);

    if (ptr->i->itType != 0)
    {
        pos = getPos(ptr->PlayerPos.y, ptr->PlayerPos.x);
        maze.cells[pos].item = *(ptr->i); //not sure

        maze.cells[pos].Cell_Pos.x = ptr->i->ItemPos.x;
        maze.cells[pos].Cell_Pos.y = ptr->i->ItemPos.y;
        it.ItemPos.x = maze.cells[pos].Cell_Pos.x;
        it.ItemPos.y = maze.cells[pos].Cell_Pos.y;

        ptr->i->itType = 0;
        //whereDropped.containItem= TRUE;





        /*        add_delta_player(d, playerArray[playerID], sizeof(Player));
             add_delta_cell(d, Cell, sizeof(Cell));
             add_delta_item(d, item, sizeof(item));
          */

        add_delta_player(d, ptr, sizeof(struct player));
        add_delta_cell(d, &(maze.cells[pos]), sizeof(Cell));
        add_delta_item(d, &it, sizeof(Item));

        return 1;
    }
    else
    {
        return -1;
    }
}

int breakWall(int playerID, int x, int y, Deltas *d)
{
    struct player *ptr = NULL;

    int pos;

    ptr = search_in_list(playerID, NULL);

    pos = getPos(ptr->PlayerPos.y, ptr->PlayerPos.x);

    if ( ptr->i == NULL )
       return -1;

    if ((ptr->i->itType == Shovel) && (maze.cells[pos].C_Type == CT_Wall))
    {

        ptr->PlayerPos.x = x;
        ptr->PlayerPos.y = y;

        //  Player->PlayerPos->y=CheckCell->Cell_Pos->y;

        maze.cells[pos].C_Type = CT_Floor;

        resetItem(d, *(ptr->i), ptr->team);
        ptr->i = NULL;


        /*
        add_delta_player(d, Player, sizeof(Player));
        add_delta_cell(d, CheckCell, sizeof(Cell));
        add_delta_item(d, item, sizeof(item));
        */

        add_delta_player(d, ptr, sizeof(struct player));
        add_delta_cell(d, &(maze.cells[pos]), sizeof(Cell));

        return 1;

    }
    else
    {
        return -1;
    }
}

int addPlayer(Deltas *d)
{
    struct player *newPlayer = NULL;

    int i;

    int temp = 0;

    struct player *ptr = NULL;

    if (playerCount < MAX)
    {

    }
    else
    {
        for (i = 0; i < playerCount; i++)
        {
            ptr = search_in_list(i, NULL);

            if (NULL == ptr)
            {
            }
            else
            {
                temp++;
            }
        }
    }

    if (temp < MAX)
    {
        printf("%s\n", "\nAdding Player!");

        add_to_list(playerCount, true);

        newPlayer = search_in_list(playerCount, NULL);

        newPlayer->team = playerCount % 2;

        Position *newPlayerPos = NULL;

        newPlayerPos = findFreeHome(playerCount % 2);

        if (newPlayerPos->x != NULL && newPlayerPos->y != NULL)
        {
            newPlayer->PlayerPos = *newPlayerPos;

            newPlayer->ID = playerCount;

            Item tempItem;

            newPlayer->i = NULL;

            add_delta_player(d, newPlayer, sizeof(struct player));

            return playerCount++;
        }
    }
    return -1;

}

int removePlayer (int playerID, Deltas *d)
{
    int ret = delete_from_list(playerID);

    if (ret != 0)
    {
        printf("\n Delete [player id = %d] failed, no such element found\n", playerID);
        return -1;
    }
    else
    {
        printf("\n Delete [player id = %d] - Done. \n", playerID);
        //add_delta_player(d, newPlayer, sizeof(struct player));
        return 1;
    }

    /*
    if (ptr == NULL)
    {
        return -1;
    }
    else
    {*/
    //free(ptr);
    //return 1;
    //}
}

int jailPlayer(struct player *tempPlayer)// Player tempPlayer)
{
    int p = 0;

    if (tempPlayer->team == Team1)
    {
        for (p = 0; p < maze.numOfJails[0]; p++)
        {
            if (JailList0[p].occupied != 1)
            {
                tempPlayer->PlayerPos.x = JailList0[p].Cell_Pos.x;
                tempPlayer->PlayerPos.y = JailList0[p].Cell_Pos.y;
                JailList0[p].occupied = tempPlayer->team;
                tempPlayer->State = 1;
                return 1;
            }
        }
    }

    if (tempPlayer->team == Team2)
    {
        for (p = 0; p < maze.numOfJails[1]; p++)
        {
            if (JailList1[p].occupied != 1)
            {
                tempPlayer->PlayerPos.x = JailList1[p].Cell_Pos.x;
                tempPlayer->PlayerPos.y = JailList1[p].Cell_Pos.y;
                JailList1[p].occupied = 1;
                tempPlayer->State = tempPlayer->team;
                return 1;
            }
        }
    }

    return -1;

}

void tagCheck(struct player *tempPlayer)
{
    Team tagger = tempPlayer->team;

    int k;

    struct player *ptr = NULL;

    for (k = 0; k < MAX; k++)
    {
        ptr = search_in_list(k, NULL);

        if (NULL == ptr)
        {
            debugPrint("\n Search [playerID = %d] failed, no such element found\n", k);
        }
        else
        {
            debugPrint("\n Search passed [playerID = %d]\n", ptr->ID);

            if (ptr->team != tagger)
            {
                if ((ptr->PlayerPos.x ==  tempPlayer->PlayerPos.x) && (ptr->PlayerPos.y ==  tempPlayer->PlayerPos.y))
                {
                    if (ptr->PlayerPos.x > MAX / 2 && tagger == Team1)
                        jailPlayer(tempPlayer);
                    else
                        jailPlayer(ptr);
                }
            }
        }

    }

    return;
}

int movePlayer (int playerID, Deltas *d, char c)  //['U', 'D', 'L', 'R']
{

    printf("\ngamePlayingFlag - %d\n", gamePlayingFlag);

    if (gamePlayingFlag == TRUE)
    {
        struct player *ptr = NULL;

        printf("Looking for player - %d \n", playerID);
        ptr = search_in_list(playerID, NULL);

        if (NULL == ptr)
        {
            debugPrint("\n Search [playerID = %d] failed, no such element found\n", playerID);
            return -1;
        }
        else
        {
            debugPrint("\n Search passed [playerID = %d]\n", ptr->ID);
        }

        // jail check
        if (ptr->State == 0)
        {

            Cell tempCellXU = cellInfo((ptr->PlayerPos.y) + 0, (ptr->PlayerPos.x) - 1);

            Cell tempCellXD = cellInfo((ptr->PlayerPos.y) + 0, (ptr->PlayerPos.x) + 1);

            Cell tempCellYL = cellInfo((ptr->PlayerPos.y) - 1, (ptr->PlayerPos.x) + 0);

            Cell tempCellYR = cellInfo((ptr->PlayerPos.y) + 1, (ptr->PlayerPos.x) + 0);

            debugPrint("\nTrying to do a %c\n", c);
            debugPrint("tempCellXU: type - %c\tlocation - %d, %d\n", tempCellXU.C_Type, tempCellXU.Cell_Pos.x, tempCellXU.Cell_Pos.y);
            debugPrint("tempCellXD: type - %c\tlocation - %d, %d\n", tempCellXD.C_Type, tempCellXD.Cell_Pos.x, tempCellXD.Cell_Pos.y);
            debugPrint("tempCellYL: type - %c\tlocation - %d, %d\n", tempCellYL.C_Type, tempCellYL.Cell_Pos.x, tempCellYL.Cell_Pos.y);
            debugPrint("tempCellYR: type - %c\tlocation - %d, %d\n", tempCellYR.C_Type, tempCellYR.Cell_Pos.x, tempCellYR.Cell_Pos.y);

            if ((c == 'U' || c == 'u' || c == '1'))
            {
                breakWall(playerID, (ptr->PlayerPos.y) + 0, (ptr->PlayerPos.x) - 1, d);
            }

            if ((c == 'D' || c == 'd' || c == '2'))
            {
                breakWall(playerID, (ptr->PlayerPos.y) + 0, (ptr->PlayerPos.x) + 1, d);
            }

            if ((c == 'L' || c == 'l' || c == '3'))
            {
                breakWall(playerID, (ptr->PlayerPos.y) - 1, (ptr->PlayerPos.x) + 0, d);
            }

            if ((c == 'R' || c == 'r' || c == '4'))
            {
                breakWall(playerID, (ptr->PlayerPos.y) + 1, (ptr->PlayerPos.x) + 0, d);
            }

            // IDid move check
            if ((c == 'U' || c == 'u' || c == '1') && tempCellXU.occupied != ptr->team && (tempCellXU.C_Type != CT_Wall && tempCellXU.C_Type != CT_Jailj && tempCellXU.C_Type != CT_JailJ))
            {
                ptr->PlayerPos.x--;

                pickUpItem(playerID, d);

		if (ptr->i != NULL)
                if (ptr->i->itType == 3)
                {
                    dropItem(playerID, d, *(ptr->i));
                }

                tempCellXU.occupied = ptr->team;

                // tagging check
                tagCheck(ptr);

                add_delta_player(d, ptr, sizeof(struct player));

                return 1; // move made
            }

            if ((c == 'D' || c == 'd' || c == '2') && tempCellXD.occupied != ptr->team && (tempCellXD.C_Type != CT_Wall && tempCellXD.C_Type != CT_Jailj && tempCellXD.C_Type != CT_JailJ))
            {
                ptr->PlayerPos.x++;
                pickUpItem(playerID, d);

		if (ptr->i != NULL)
                if (ptr->i->itType == 3)
                {
                    dropItem(playerID, d, *(ptr->i));
                }


                tempCellXD.occupied = ptr->team;

                // tagging check
                tagCheck(ptr);

                add_delta_player(d, ptr, sizeof(struct player));


                return 1; // move made
            }

            if ((c == 'L' || c == 'l' || c == '3') && tempCellYL.occupied != ptr->team && (tempCellYL.C_Type != CT_Wall && tempCellYL.C_Type != CT_Jailj && tempCellYL.C_Type != CT_JailJ))
            {
                ptr->PlayerPos.y--;
                pickUpItem(playerID, d);

		if (ptr->i != NULL)
                if (ptr->i->itType == 3)
                {
                    dropItem(playerID, d, *(ptr->i));
                }

                tempCellYL.occupied = ptr->team;

                // tagging check
                tagCheck(ptr);

                add_delta_player(d, ptr, sizeof(struct player));

                return 1; // move made
            }

            if ((c == 'R' || c == 'r' || c == '4') && tempCellYR.occupied != ptr->team && (tempCellYR.C_Type != CT_Wall && tempCellYR.C_Type != CT_Jailj && tempCellYR.C_Type != CT_JailJ))
            {
                ptr->PlayerPos.y++;
                pickUpItem(playerID, d);

		if (ptr->i != NULL)
                if (ptr->i->itType == 3)
                {
                    dropItem(playerID, d, *(ptr->i));
                }

                tempCellYR.occupied = ptr->team;

                // tagging check
                tagCheck(ptr);

                add_delta_player(d, ptr, sizeof(struct player));

                return 1; // move made
            }

            /*
                        // IDid move check
                        if ((c == 'D' || c == 'd' || c == '2')  && tempCell.p == NULL && (tempCell.C_Type != CT_Wall))
                        {
                            ptr->PlayerPos.y++;

                            // tagging check
                            tagCheck(ptr);

                            return 1; // move made
                        }

                        // IDid move check
                        if ((c == 'L' || c == 'l' || c == '3')  && tempCell.p == NULL && (tempCell.C_Type != CT_Wall))
                        {
                            ptr->PlayerPos.x--;

                            // tagging check
                            tagCheck(ptr);

                            return 1; // move made
                        }

                        // IDid move check
                        if ((c == 'R' || c == 'r' || c == '4')  && tempCell.p == NULL && (tempCell.C_Type != CT_Wall))
                        {
                            ptr->PlayerPos.x++;

                            // tagging check
                            tagCheck(ptr);
                            return 1; // move made
                        }

                        */
        }
        else
            return -1; // In jail
    }
    //else
    return -1;
}

void formatMaze(char * mazeOutput)
{
    int x = (maze.dimensions.y * maze.dimensions.x);

    int i;

    for (i = 0; i < x; i++)
    {

        int c =  maze.cells[i].C_Type;
        char print = (char) c;

        if ((i % maze.dimensions.y) == 0)
        {
            mazeOutput[i] = print;
            //printf("\n%c", print);
        }
        else
        {
            mazeOutput[i] = print;
            //printf("%c", print);
        }

    }

    return;
}

int dimX()
{
    return maze.dimensions.x;
}

int dimY()
{
    return maze.dimensions.y;
}


addToMap()
{
    // Player Array
    // Items Array
    // to maze
    // maze[player.x][player.y] = 'p'
    // maze[item.x][item.y] = 'item.type'
}

/*
void addPlayerTest(int num)
{
    int i;

    for ( i = 0; i < num; i++)
    {
        addPlayer();
    }


    print_list();

    int temp = 0;

    struct player *ptr = NULL;

    for (i = 0; i < playerCount; i++)
    {
        ptr = search_in_list(i, NULL);

        if (NULL == ptr)
        {
        }
        else
        {
            temp++;
        }
    }

    printf("\nTotal player count is - %d\n", temp);

    for ( i = 0; i <= num; i++)
    {
        removePlayer(i);
    }


    print_list();

    temp = 0;

    ptr = NULL;

    for (i = 0; i < playerCount; i++)
    {
        ptr = search_in_list(i, NULL);

        if (NULL == ptr)
        {
        }
        else
        {
            temp++;
        }
    }

    printf("\nTotal player count is - %d\n", temp);
}

void homeAndJailTest()
{
    int x = 0;

    for (x = 0; x < maze.numOfHomes[0]; x++)
    {
        printf("\nType - %c \nTeam - %d \n[%d, %d]\n", (char)HomeList0[x].C_Type, HomeList0[x].Cell_Team , HomeList0[x].Cell_Pos.x, HomeList0[x].Cell_Pos.y);
    }
    for (x = 0; x < maze.numOfHomes[1]; x++)
    {
        printf("\nType - %c \nTeam - %d \n[%d, %d]\n", (char)HomeList1[x].C_Type, HomeList1[x].Cell_Team , HomeList1[x].Cell_Pos.x, HomeList1[x].Cell_Pos.y);
    }
    for (x = 0; x < maze.numOfJails[0]; x++)
    {
        printf("\nType - %c \nTeam - %d \n[%d, %d]\n", (char)JailList0[x].C_Type, JailList0[x].Cell_Team , JailList0[x].Cell_Pos.x, JailList0[x].Cell_Pos.y);
    }
    for (x = 0; x < maze.numOfJails[1]; x++)
    {
        printf("\nType - %c \nTeam - %d \n[%d, %d]\n", (char)JailList1[x].C_Type, JailList1[x].Cell_Team , JailList1[x].Cell_Pos.x, JailList1[x].Cell_Pos.y);
    }
}

void moveTest(int num)
{
    int temp = 0;
    int move = 0;
    int i;

    for (i = 0; i < num; ++i)
    {
        temp = addPlayer();
        printf("Player id returned : %d\n", temp);
        print_list();
        move = movePlayer (temp, 'U');
        printf("The move was - %d\n", move);
        print_list();
        move = movePlayer (temp, 'D');
        printf("The move was - %d\n", move);
        print_list();
        move = movePlayer (temp, 'L');
        printf("The move was - %d\n", move);
        print_list();
        move = movePlayer (temp, 'R');
        printf("The move was - %d\n", move);
        print_list();
    }
}

int itemTest(void)
{
    Item testIt;
    testIt.itType = Flag_Team1;
    testIt.ItemPos.x = 0;
    testIt.ItemPos.y = 0;
    resetItem(testIt, 1);
}

*/
/*
int main(int argc, char const *argv[])
{
    //startGame();
    //dumpMap();
    return 0;
}
*/

/*
//extern int
int pickUpItem(int playerID, Deltas *d)
{
    if (Cell.containItem == TRUE)
    {

        Item->itemPos->x = ptr->PlayerPos->x;
        Item.itemPos->y = ptr->PlayerPos->y;

        if (Item->itType == Shovel)
        {
            Item->hasAbility = true;
            playerArray[playerID].holdShovel = true;
        }
        else if (Item->itType == Flag_Team1) || (Item->itType == Flag_Team2)
        {
            ptr->holdFlag = true;
            Item->hasAbility = false;
        }

        Cell.containItem = false;
        add_delta_player(d, playerArray[playerID], sizeof(Player));
        add_delta_cell(d, Cell, sizeof(Cell));
        add_delta_item(d, item, sizeof(item));
        return 1;
    }
    else
    {
        return -1;
    }
}

//extern int
int breakWall(int x, int y, Deltas *d)
{

    Cell CheckCell = maze[x][y];

    if (CheckCell->C_Type == CT_Wall)
    {

        // Player->PlayerPos->x=CheckCell->Cell_Pos->x;

        //  Player->PlayerPos->y=CheckCell->Cell_Pos->y;

        CheckCell->C_type = CT_Floor;

        Player->holdShovel = false;

        add_delta_player(d, Player, sizeof(Player));
        add_delta_cell(d, CheckCell, sizeof(Cell));
        add_delta_item(d, item, sizeof(item));

        return 1;

    }
    else
    {
        return -1;
    }
}

//extern int
int resetItem(Deltas *d, Item it)
{

    if (it->itType = Shovel)
    {
        //position to the base

    }
    else if (itemType = Flag_team1)
    {

        //position at the part of team1
    }
    else if (itemType = Flag_team2)
    {

        //position at the part of team2
    }
    return 1;
    else
    {
        return -1;
    }
}

*/
/*
//extern int
int dropItem(int playerID, Deltas *d, Item it)
{
    if (Cell->C_Type == CT_Floor)
    {



        // if(playerarray[id]-> holdFlag == TRUE)
        // {
        // playerarray[id]-> item  == False

        // }



        // playerArray[playerID
        //Cell->Cell_Pos->x=it->itemPos->x;
        //Cell->Cell_Pos->y=it->itemPos->y;
        if (playerArray[playerID]->holdFlag == TRUE)
        {
            playerArray[playerID]->holdFlag = FALSE;

        }
        else if (playerArray[playerID]->holdShover = TRUE)
        {
            playerArray[playerID]->holdShovel = FALSE;
        }
        add_delta_player(d, playerArray[playerID], sizeof(Player));
        add_delta_cell(d, Cell, sizeof(Cell));
        add_delta_item(d, item, sizeof(item));

        return 1;
    }
    else
    {
        return -1;
    }
}

*/
