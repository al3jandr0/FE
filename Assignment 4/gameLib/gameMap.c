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
#include <string.h>
#include "maze.h"
#include "maze.c"

enum { FALSE, TRUE };

int gamePlayingFlag = FALSE;

int playerCount = 0;

Player *PlayerList;
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

int start()
{

    if (playerCount > 2 && gamePlayingFlag == FALSE)
    {
        loadMap();
        PlayerList = malloc(MAX * sizeof(Player));
        HomeList0 = malloc(MAX * sizeof(Cell));
        HomeList1 = malloc(MAX * sizeof(Cell));
        JailList0 = malloc(MAX * sizeof(Cell));
        JailList1 = malloc(MAX * sizeof(Cell));
        gamePlayingFlag = TRUE;
        return 1;

    }

    else
        return -1;
}

void stop()
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

    free(PlayerList);

    free(HomeList0);

    free(HomeList1);

    free(JailList0);

    free(JailList1);

    free(maze.cells);

    playerCount = 0;
    // homeCount0 = 0;
    // homeCount1 = 0;
    // jailCount0 = 0;
    // jailCount1 = 0;

    gamePlayingFlag = FALSE;

}

//extern Maze

int loadMap()
{
    FILE *mapFile;
    if ((mapFile = fopen("daGame.map", "r")) == NULL)
    {
        printf("Cannot open file.\n");
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
                newItem.Type = c;
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
                HomeList0[maze.numOfHomes[1]] = newCell;

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

        return 0; // Game not started
        */
}

int addPlayer (Deltas *d)
{

    Player newPlayer

    newPlayer.team = playerCount % 2;

    Position newPlayerPos = findFreeHome(playerCount % 2);

    if (newPlayerPos.x != NULL && newPlayerPos.y != NULL)
    {
        newPlayer.PlayerPos = newPlayerPos;

        newPlayer.ID = playerCount;

        playerCount++;

        Item tempItem;

        newPlayer.i = NULL;

        PlayerList [playerCount] = newPlayer;

        return playerCount;
    }
    else
    {
        return -1;
    }

}

Position findFreeHome(int team)
{

    int p = 0;

    Position newPos;

    if (team = 0)
    {
        for (p = 0; p < maze.numOfHomes[0]; p++)
        {
            if (HomeList0[p].occupied != 1)
            {
                newPos.x = HomeList0[p].Cell_Pos.x;
                newPos.y = HomeList0[p].Cell_Pos.y;
                HomeList0[p].occupied = 1;
                return newPos;
                //break;
            }
        }
    }

    if (team = 1)
    {
        for (p = 0; p < maze.numOfHomes[1]; p++)
        {
            if (HomeList1[p].occupied != 1)
            {
                newPos.x = HomeList1[p].Cell_Pos.x;
                newPos.y = HomeList1[p].Cell_Pos.y;
                HomeList1[p].occupied = 1;
                return newPos;
                //break;
            }
        }
    }

    newPos.x = NULL;
    newPos.y = NULL;

    return newPos;
}

int removePlayer (int playerID)//, Deltas *d)
{
    /*
    if (PlayerList[playerID] == NULL)
    {
        return -1;
    }
    else
    {*/
    free(PlayerList[playerID]);
    return 1;
    //}
}

void jailPlayer(Player tempPlayer)
{

    int p = 0;

    if (tempPlayer.team == Team1)
    {
        for (p = 0; p < maze.numOfJails[0]; p++)
        {
            if (JailList0[p].occupied != 1)
            {
                tempPlayer.PlayerPos.x = JailList0[p].Cell_Pos.x;
                tempPlayer.PlayerPos.y = JailList0[p].Cell_Pos.y;
                JailList0[p].occupied = 1;
                tempPlayer.State = 1;
                break;
            }
        }
    }

    if (tempPlayer.team == Team2)
    {
        for (p = 0; p < maze.numOfJails[1]; p++)
        {
            if (JailList1[p].occupied != 1)
            {
                tempPlayer.PlayerPos.x = JailList1[p].Cell_Pos.x;
                tempPlayer.PlayerPos.y = JailList1[p].Cell_Pos.y;
                JailList1[p].occupied = 1;
                tempPlayer.State = 1;
                break;
            }
        }
    }

}

void tagCheck(Player tempPlayer)
{
    Team tagger = tempPlayer.team;

    int k;
    for (k = 0; k < MAX; k++)
    {
        if (PlayerList[k].team != tagger)
        {
            if ((PlayerList[k].PlayerPos.x ==  tempPlayer.PlayerPos.x) && (PlayerList[k].PlayerPos.y ==  tempPlayer.PlayerPos.y))
            {
                if (PlayerList[k].PlayerPos.x > MAX / 2 && tagger == Team1)
                    jailPlayer(tempPlayer);
                else
                    jailPlayer(PlayerList[k]);
            }
        }
    }

    return;
}

int movePlayer (int playerID/*, Deltas *d*/, char c)  //['U', 'D', 'L', 'R']
{

    // jail check
    if (PlayerList[playerID].State == 0)
    {

        Cell tempCell = cellInfo(PlayerList[playerID].PlayerPos.x, PlayerList[playerID].PlayerPos.y);

        // valid move check
        if ((c == 'U' || c == 'u' || c == '1') && tempCell.p == NULL && (tempCell.C_Type != CT_Wall))

        {
            PlayerList[playerID].PlayerPos.y--;

            // tagging check
            tagCheck(PlayerList[playerID]);

            return 1; // move made

        }

        // valid move check
        if ((c == 'D' || c == 'd' || c == '2')  && tempCell.p == NULL && (tempCell.C_Type != CT_Wall))
        {
            PlayerList[playerID].PlayerPos.y++;

            // tagging check
            tagCheck(PlayerList[playerID]);

            return 1; // move made
        }

        // valid move check
        if ((c == 'L' || c == 'l' || c == '3')  && tempCell.p == NULL && (tempCell.C_Type != CT_Wall))
        {
            PlayerList[playerID].PlayerPos.x--;

            // tagging check
            tagCheck(PlayerList[playerID]);

            return 1; // move made
        }

        // valid move check
        if ((c == 'R' || c == 'r' || c == '4')  && tempCell.p == NULL && (tempCell.C_Type != CT_Wall))
        {
            PlayerList[playerID].PlayerPos.x++;

            // tagging check
            tagCheck(PlayerList[playerID]);
            return 1; // move made
        }
    }
    else
        return -1; // In jail

}

char *formatMaze()
{
    int x = (maze.dimensions.y * maze.dimensions.x);

    char *mazeOutput = (char *)malloc(x);

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

    return mazeOutput;
}

int dimX()
{
    return maze.dimensions.x;
}

int dimY()
{
    return maze.dimensions.y;
}

/*
//extern int
int pickUpItem(int playerID, Deltas *d)
{
    if (Cell.containItem == TRUE)
    {

        Item->itemPos->x = PlayerList[playerID]->PlayerPos->x;
        Item.itemPos->y = PlayerList[playerID]->PlayerPos->y;

        if (Item->itType == Shovel)
        {
            Item->hasAbility = true;
            playerArray[playerID].holdShovel = true;
        }
        else if (Item->itType == Flag_Team1) || (Item->itType == Flag_Team2)
        {
            PlayerList[playerID]->holdFlag = true;
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
