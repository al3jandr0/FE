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


static void
 findDimensions(Position *pos){
  pos->x=maze.dimensions.x;
  pos->x=maze.dimensions.y;
}



