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

int playerCount = 0;
Player *PlayerList = malloc(MAX *sizeof *PlayerList);

int homeCount0 = 0;
Cell *HomeList0 = malloc(MAX *sizeof *HomeList0);
int homeCount1 = 0;
Cell *HomeList1 = malloc(MAX *sizeof *HomeList1);

int jailCount0 = 0;
Cell *JailList0 = malloc(MAX *sizeof *JailList0);
int jailCount1 = 0;
Cell *JailList1 = malloc(MAX *sizeof *JailList1);

/*
Cell *HomeList0 = malloc(numOfHomes[0] * sizeof *HomeList0);
Cell *HomeList1 = malloc(numOfHomes[1] * sizeof *HomeList1);

Cell *JailList0 = malloc(numOfJails[0] * sizeof *JailList0);
Cell *JailList1 = malloc(numOfJails[1] * sizeof *JailList1);
*/

void start()
{
    loadMap();
}

void stop()
{
    int x;

    for (x = 0; x < MAX; x++)
    {
        free(PlayerList[x]);
    }

    for (x = 0; x < numOfHomes[0]; x++)
    {
        free(HomeList0[x]);
    }

    for (x = 0; x < numOfHomes[1]; x++)
    {
        free(HomeList1[x]);
    }

    for (x = 0; x < numOfJails[0]; x++)
    {
        free(JailList0[x]);
    }

    for (x = 0; x < numOfJails[1]; x++)
    {
        free(JailList1[x]);
    }

    free(maze);

    playerCount = 0;
    homeCount0 = 0;
    homeCount1 = 0;
    jailCount0 = 0;
    jailCount1 = 0;
}

//extern Maze

void loadMap()
{
    FILE *mapFile;
    if ((mapFile = fopen("daGame.map", "r")) == NULL)
    {
        printf("Cannot open file.\n");
        return maze;
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
            if (newCell.Cell_Pos.y < (MAX / 2)
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
    return;
}

extern void
dumpMap()
{
    int x = (maze.dimensions.y * maze.dimensions.x);
    for (int i = 0; i < (x); i++)
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
}

int gameStat()
{

    return 4; // Team 2 wins

    return 3; // Team 1 wins

    return 2; // Game is a Draw

    return 1; // Game started

    return 0; // Game not started
}

int addPlayer (Deltas *d)
{

    Player newPlayer

    newPlayer.team = count % 2;

    Position newPlayerPos = findFreeHome(count % 2);

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
            if (homeCount0[p].occupied != 1)
            {
                newPos.x = homeCount0[p].Cell_Pos.x;
                newPos.y = homeCount0[p].Cell_Pos.y;
                homeCount0[p].occupied = 1;
                return newPos;
                //break;
            }
        }
    }

    if (team = 1)
    {
        for (p = 0; p < maze.numOfHomes[1]; p++)
        {
            if (homeCount1[p].occupied != 1)
            {
                newPos.x = homeCount1[p].Cell_Pos.x;
                newPos.y = homeCount1[p].Cell_Pos.y;
                homeCount1[p].occupied = 1;
                return newPos;
                //break;
            }
        }
    }

    newPos.x = NULL;
    newPos.y = NULL;

    return newPos;
}

int removePlayer (int playerID, Deltas *d)
{
    free([playerID]);
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
            if (PlayerList[k].PlayerPos ==  tempPlayer.PlayerPos)
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

int movePlayer (int playerID, Deltas *d, char c)  //['U', 'D', 'L', 'R']
{

    // jail check
    if (PlayerList[playerID].State = 0)
    {

        Cell tempCell = cellInfo(PlayerList[playerID].x, PlayerList[playerID].y)

                        // valid move check
                        if ((c == 'U' || c == 'u' || c == '1') && tempCell.p == NULL && (tempCell.C_Type != CT_Wall))

        {
            PlayerList[playerID].y--;

            // tagging check
            tagCheck(PlayerList[playerID]);
        }

        // valid move check
        if ((c == 'D' || c == 'd' || c == '2')  && tempCell.p == NULL && (tempCell.C_Type != CT_Wall))
        {
            PlayerList[playerID].y++;

            // tagging check
            tagCheck(PlayerList[playerID]);

        }

        // valid move check
        if ((c == 'L' || c == 'l' || c == '3')  && tempCell.p == NULL && (tempCell.C_Type != CT_Wall))
        {
            PlayerList[playerID].x--;

            // tagging check
            tagCheck(PlayerList[playerID]);

        }

        // valid move check
        if ((c == 'R' || c == 'r' || c == '4')  && tempCell.p == NULL && (tempCell.C_Type != CT_Wall))
        {
            PlayerList[playerID].x++;

            // tagging check
            tagCheck(PlayerList[playerID]);
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

}
