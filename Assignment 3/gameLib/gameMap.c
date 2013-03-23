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


typedef enum{
  CT_Floor =' ',
  CT_Wall = '#',
  CT_Jailj ='j',
  CT_JailJ ='J',
  CT_Homeh ='h',
  CT_HomeH ='H',
}Cell_Type;

typedef struct {
  int x;
  int y;
}Position;

typedef enum{
  Team1=1,
  Team2=2,
}Team;

typedef struct{
  Team team;
  Position PlayerPos;
  int ID;
  int PID;
  // Object *o;// FIX FOR ITEMS
}Player;


typedef struct{
  Cell_Type C_Type;
  Position Cell_Pos;
  Player *p;
  // Object *o;// FIX FOR ITEMS
}Cell;

typedef struct {
  Cell *cells;
  int numFloor;
  int numWall;
  int numCells;
  int numOfJails[2];
  int numOfHomes[2];
  Position dimensions;
} Maze;

/*
  typedef struct{
  Player players[numPlayers];
  }PlayerList;
*/

int main()
{


  
  
  FILE *mapFile;
  int c;
  int currentIndex = 0;
  Maze maze; 


  mapFile = fopen("daGame.map", "r");
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
  printf ("This is x4row : %d and this isis y4column : %d\n", maze.dimensions.x,  maze.dimensions.y);
    
    
  fseek(mapFile, 0, SEEK_SET);
    
    
  fseek(mapFile, 0, SEEK_END);
  int fileSize = ftell(mapFile);
  fseek(mapFile, 0, SEEK_SET);
  maze.cells = malloc((columnCounter*rowCounter)*sizeof(Cell));
  // c = fgetc(mapFile);
  printf("%d== current c=%c", currentIndex, fgetc(mapFile));
  printf("%d==cell[0]",  maze.cells[0].C_Type);
  while ((c = fgetc(mapFile)) != EOF) {
    
    //  printf("%d%c", currentIndex,c);     
    if (c!=10) {
      Cell newCell;
      maze.cells[currentIndex] = newCell;
      printf("current-%d,char-%c",currentIndex,c);
      newCell.C_Type = c;
      newCell.Cell_Pos.x = currentIndex/maze.dimensions.x;
     
      newCell.Cell_Pos.y = currentIndex%maze.dimensions.y;

      
      Player *p=NULL;
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

  printf("%d==cell[0]",  maze.cells[0].C_Type);


  fclose(mapFile);
  //  printf("%d,cell0",  maze.cells[0].C_Type);
  //  printf("%d\n\n", maze.cells[1].C_Type);
  
	 /* for (int i = 0; i < (maze.dimensions.y*maze.dimensions.x)+1;i++) {
  
    int c =  maze.cells[i].C_Type;
    char print = (char) c;
    
    if ((i%maze.dimensions.y) == 0){
      printf ("\nd%d%c",i,print);
      //  printf ("%c,%d\n",print,i);
    }else {
      printf("%d%c", i,print);
      // printf ("%c(%d)", print,i);
      
      
      }
       }*/
}

