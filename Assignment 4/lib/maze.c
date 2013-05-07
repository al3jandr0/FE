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
  if(par==1){
    return maze.numOfHomes[0];
  }else if (par == 2){
    return maze.numOfHomes[1];
  } else {
    return-1;
  }
}

extern int
findNumJail(int par){
  if(par==1){
    return maze.numOfJails[0];
  }else if (par == 2){
    return maze.numOfJails[1];
  } else {
    return-1;
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

extern int get_maze_dimx()
{
   return maze.dimensions.x;
}

extern int get_maze_dimy()
{
   return maze.dimensions.y;
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
    pos = row*maze.dimensions.y;
  }else {
    pos = row*200 + column;
  }
  maze.cells[pos];
  printf("\nCell Info for (%d,%d)-(column,row) is:\nCell Type is '%c',  Cell Team is %d, ",column, row, maze.cells[pos].C_Type, maze.cells[pos].Cell_Team);
  if (maze.cells[pos].p==NULL){
    printf("Cell Is Not Occupied\n");
  } else {
    printf("Cell Is Occupied\n");
  }
}

extern char
get_cell_type(int column, int row)
{
   int I, x, y;

   y = maze.dimensions.y;
   x = maze.dimensions.x;
   if (column > x ) return '?';
   if (row    > y ) return '?';

   I = row*y + column;
   return maze.cells[I].C_Type;
}

extern char
get_cell_team(int column, int row)
{
   int I, team, x, y;

   y = maze.dimensions.y;
   x = maze.dimensions.x;
   if (column > x ) return '?';
   if (row    > y ) return '?';

   I = row*y + column;
   team = maze.cells[I].Cell_Team;

   if (team == 1) return '1';
   if (team == 2) return '2';
   
   return '?';
}

extern char
is_cell_occupied(int column, int row)
{
   int I, x, y;
   y = maze.dimensions.y;
   x = maze.dimensions.x;
   if (column > x ) return '?';
   if (row    > y ) return '?';

   I = row*y + column;

   if (maze.cells[I].p == NULL)
      return 'N';

   return 'Y';
}

extern Maze loadMap()
{
  FILE *mapFile;
  if((mapFile = fopen("./../daGame.map", "r"))==NULL) {
    printf("Cannot open file.\n");
    return maze;
  }
  
  int c;
  int columnCounter = 0;  
  while ((c = fgetc(mapFile)) != 10) {
    columnCounter++;
  }
  maze.dimensions.y = columnCounter;

  int rowCounter = 1;
  char s[columnCounter+2];

  while ((fgets(s, columnCounter+2, mapFile)) !=NULL) {
    rowCounter++;
  }
  maze.dimensions.x = rowCounter;
    
  rewind(mapFile);    
  
  maze.cells = malloc((columnCounter*rowCounter)*sizeof(Cell));

int currentIndex = 0;
  while ((c = fgetc(mapFile)) != EOF) {
    if (c!=10) {
      
      Cell newCell;
      newCell.C_Type = c;
      newCell.Cell_Pos.x = currentIndex/maze.dimensions.x;
      newCell.Cell_Pos.y = currentIndex%maze.dimensions.y;
    
      if (newCell.Cell_Pos.y<100){
        newCell.Cell_Team = Team1;
	if (c=='#'){
	  newCell.C_Type = 'w';
	}
      } else {
        newCell.Cell_Team = Team2;
	if (c=='#'){
	  newCell.C_Type = 'W';
	}
      }
      
      newCell.p=NULL;
      maze.cells[currentIndex] = newCell;
      // *o=NULL;  //FIX FOR ITEMS
      
      if (c=='j'){
        maze.numOfJails[0]++;
      }else if(c=='J'){
        maze.numOfJails[1]++;   
      }else if(c=='h'){
        maze.numOfHomes[0]++;
      }else if(c=='H'){
        maze.numOfHomes[1]++;   
      }

      if (c==' '){
        maze.numFloor++;
      } else {
	maze.numWall++;
      }
     
      currentIndex++;
    
    } else{
   }
  } 
  fclose(mapFile);
  return maze;
}
  


extern char*
dumpMap() {

  char* fullMap; 

  int x = (maze.dimensions.y*maze.dimensions.x);
  int i;
  for (i = 0; i < (x);i++) {
 
    int c =  maze.cells[i].C_Type;
    char print = (char) c;

    fullMap[i] = print;
    /*    
    if ((i%maze.dimensions.y) == 0){
      printf("\n%c",print);
    }else {
      printf("%c",print);      
    }
    */

  }
  fflush(stdout);
  return fullMap;
}




