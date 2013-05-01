#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <strings.h>
#include <errno.h>
#include <pthread.h>

#include "maze.h"

Maze maze;

extern int
findNumHome(int par)
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

extern int
findNumJail(int par)
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

extern int
findNumWall()
{
    return maze.numWall;
}

extern int
findNumFloor()
{
    return maze.numFloor;
}


extern void
findDimensions(Position *pos)
{
    pos->x = maze.dimensions.x;
    pos->y = maze.dimensions.y;
}

extern void
findCInfo(int column, int row)
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


extern Cell
cellInfo(int column, int row)
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
