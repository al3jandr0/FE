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




extern Maze
loadMap()
{
  FILE *mapFile;
  if((mapFile = fopen("daGame.map", "r"))==NULL) {
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
      } else {
	newCell.Cell_Team = Team2;
      }
      newCell.p=NULL;
      maze.cells[currentIndex] = newCell;
      // *o=NULL;  //FIX FOR ITEMS
      
      if (c=='#'){
	maze.numWall++;
      }else {
	maze.numFloor++;
      }
      
      if (c=='j'){
	maze.numOfJails[0]++;
      }else if(c=='J'){
	maze.numOfJails[1]++;   
      }else if(c=='h'){
	maze.numOfHomes[0]++;
      }else if(c=='H'){
	maze.numOfHomes[1]++;	
      }
     
      currentIndex++;
    
    } else{
    }
  } 
  fclose(mapFile);
  return maze;
}

extern void
dumpMap() {
  int x = (maze.dimensions.y*maze.dimensions.x);
  for (int i = 0; i < (x);i++) {
 
    int c =  maze.cells[i].C_Type;
    char print = (char) c;
    
    if ((i%maze.dimensions.y) == 0){
      printf("\n%c",print);
    }else {
      printf("%c",print);      
    }
  }
}

int main()
{  
  loadMap();
  //dumpMap();
  printf("\nhey\n");  
  Position p;
  findDimensions(&p);
  printf("   p.x-%d, p.y-%d  ", p.x, p.y);

  printf("    home1--%d     ",findNumHome(1));
  printf("    wall--%d     ",findNumWall() );
  printf("    floor--%d     ", findNumFloor());
  findCInfo(0,0);
findCInfo(1,0);
findCInfo(1,1);
findCInfo(0,1);
findCInfo(2,0);
findCInfo(2,1);
findCInfo(0,2);
findCInfo(1,2);

} 
