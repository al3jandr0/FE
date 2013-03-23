#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <strings.h>
#include <errno.h>
#include <pthread.h>

#include "maze.h"
Maze maze;

extern int
findNumHome(int par){
  if(par==0){
    return maze.numOfHomes[0];
  }else{
    return maze.numOfHomes[1];
  }
}

extern int
findNumJail(int par){
 if(par==0){
    return maze.numOfJails[0];
  }else{
    return maze.numOfJails[1];
  }
}


extern int
findNumWall(){
  return maze.numWall;
}

extern int
findNumFloor(){
return maze.numFloor;
}


extern void
findDimensions(Position *pos)
{
  pos->x=maze.dimensions.x;
  pos->y=maze.dimensions.y;
}

extern void
findCInfo(int column, int row)
{
  int pos;
  if (row==0&&column==0){
    pos=0;
  } else if (row==0) {
    pos=column;
  } else if (column == 0) {
    pos = row*200;
  }else {
    pos = row*column*200;
  }
  printf("    this is pos of cell - %d   ", pos);
  
  maze.cells[pos];
 printf("\nCell Type=%d \n Cell Team=%d\n  ",maze.cells[pos].C_Type,maze.cells[pos].Cell_Team);
 if (maze.cells[pos].p==NULL){
   printf("Cell Is Not Occupied");
 } else {
   printf("Cell Is Occupied");
 }
 printf("    this is x - %d---this is y - %d   ",maze.cells[pos].Cell_Pos.x,maze.cells[pos].Cell_Pos.y);
 
}

