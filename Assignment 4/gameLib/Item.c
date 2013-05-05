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

enum { FALSE, TRUE };

#define DEBUG FALSE

#define debugPrint(...) \
do { if (DEBUG) fprintf(stderr, __VA_ARGS__); } while (0)

int gamePlayingFlag = FALSE;

int playerCount = 0;

#define MAX 200

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

typedef enum
{
    None = 0,
    Flag_Team1 = 1,
    Flag_Team2 = 2,
    Shovel = 3
}Item_Type;

typedef struct
{
    Item_Type itType;
    Position ItemPos;
    //int hasAbility;
} Item;

typedef struct
{
    Team team;
    Position PlayerPos;
    int ID;
    int State;
    Item *i;
} Player;

typedef struct
{
    Cell_Type C_Type;
    Position Cell_Pos;
    Player *p;
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

struct player
{
    Team team;
    Position PlayerPos;
    int ID;
    int State;
    Item *i;
    struct player *next;
};



struct player *head = NULL;
struct player *curr = NULL;

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

Maze maze;

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


int getPos(int col, int row)
{
    int pos;
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
}
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
int startGame()
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
        
        loadMap();
        
        gamePlayingFlag = TRUE;
        return 1;
        
    }
    
    else
        printf("Cannot start the Game yet.\n");
    
    return -1;
}

Position *findFreeHome(int team)
{
    
    printf("Trying to find location for team %d\n", team);
    
    int p = 0;
    
    Position *newPos = NULL;
    
    newPos = malloc(sizeof(Position));
    
    if (team == 1)
    {
        printf("%s\n", "Team 0");
        
        for (p = 2; p < maze.numOfHomes[0]; p++)
        {
            if (HomeList0[p].occupied != 1)
            {
                newPos->x = HomeList0[p].Cell_Pos.x;
                newPos->y = HomeList0[p].Cell_Pos.y;
                HomeList0[p].occupied = 1;
                printf("Free Location for team %d found at %d, %d\n", team, newPos->x, newPos->y);
                return newPos;
                //break;
            }
        }
    }
    if (team == 1)
    {
       
        for (p = 0; p < maze.numOfHomes[1]; p++)
        {
            if (HomeList1[p].occupied != 1)
            {
                newPos->x = HomeList1[p].Cell_Pos.x;
                newPos->y = HomeList1[p].Cell_Pos.y;
                HomeList1[p].occupied = 1;

                return newPos;
                //break;
            }
        }
    }
    
    newPos->x = 1;
    newPos->y = 1;
    
   
    
    
    return newPos;
}


    


int dimX()
{
    return maze.dimensions.x;
}

int dimY()
{
    return maze.dimensions.y;
}






int pickUpItem(int playerID/* Deltas *d */)
{   struct player *ptr = NULL;
   // Cell possibleItemCell;
    Item *itPtr = NULL;
    int pos;
    
    ptr=search_in_list(playerID, NULL);
    
   
    pos=getPos(ptr->PlayerPos.y, ptr->PlayerPos.x);
    
    //maze.cells[pos].Cell_Pos.x=ptr->PlayerPos.x;
    //maze.cells[pos].Cell_Pos.y=ptr->PlayerPos.y;
    if (maze.cells[pos].containItem == TRUE)
    {
        
        itPtr->ItemPos.x = ptr->PlayerPos.x;
        itPtr->ItemPos.y = ptr->PlayerPos.y;
        
            
            ptr->i=itPtr;
        
                
        maze.cells[pos].containItem = FALSE;/*
        add_delta_player(d, playerArray[playerID], sizeof(Player));
        add_delta_cell(d, Cell, sizeof(Cell));
        add_delta_item(d, item, sizeof(item));
                             */
        return 1;
    }
    else
    {
        return -1;
    }
}





int resetItem(/*Deltas *d,*/ Item it, int team)
{   //Item *itPtr = NULL;
    int row,col,pos;
    int dx = dimX();
     int dy = dimY();
    

    //printf("it.ItemPos.x before: %d\n", it.ItemPos.x);
   // printf("it.ItemPos.y before: %d\n", it.ItemPos.y);
    
    if (it.itType == 3)
    {
   
        Position *NewPos=NULL;
        NewPos=findFreeHome(team);
        pos=getPos(NewPos->y, NewPos->x);
        
           // printf("it.ItemPos.x before: %d\n", it.ItemPos.x);
           // printf("it.ItemPos.y before: %d\n", it.ItemPos.y);
        
        it.ItemPos.x= NewPos->x;
        it.ItemPos.y= NewPos->y;
      //  it.Cell.containItem=TRUE;
     //  it.ItemPos=maze.cells[NewPos].Cell_Pos;
    // printf("it.ItemPos.x after: %d\n", it.ItemPos.x);
    // printf("it.ItemPos.y after: %d\n", it.ItemPos.y);
   // printf("maze.cells[pos].Cell_Pos.x: %d\n", maze.cells[pos].Cell_Pos.x);
   // printf("maze.cells[pos].Cell_Pos.y: %d\n", maze.cells[pos].Cell_Pos.y);
    
        maze.cells[pos].Cell_Pos.x = it.ItemPos.x;
        maze.cells[pos].Cell_Pos.y = it.ItemPos.y;
        maze.cells[pos].containItem= TRUE;
        maze.cells[pos].item = it;
       
        // it.ItemPos=NewPos;
    }
        
    // printf("it.ItemPos.x after: %d\n", it.ItemPos.x);
    // printf("it.ItemPos.y after: %d\n", it.ItemPos.y);
    // printf("maze.cells[pos].Cell_Pos.x: %d\n", maze.cells[pos].Cell_Pos.x);
    // printf("maze.cells[pos].Cell_Pos.y: %d\n", maze.cells[pos].Cell_Pos.y);
    
    
   
    else if (it.itType = 1)//position at the part of team1
    {
        
        
        do{
           
            row=(rand() %(dx/2)+0);
            col=(rand() %(dy/2)+0);
       //     printf("row: %d\n", row);
       //     printf("col %d\n", col);

     //       printf("dx: %d\n", dx);
     //       printf("dy %d\n", dy);
            
            pos= getPos(col,row);
            
     //      printf("it.ItemPos.x stays: %d\n", it.ItemPos.x);
     //      printf("it.ItemPos.y stays: %d\n", it.ItemPos.y);
       
        }while(maze.cells[pos].C_Type!=CT_Floor);
      
        
               
                it.ItemPos.x=row;
               it.ItemPos.y=col;
    
        maze.cells[pos].Cell_Pos.x=it.ItemPos.x;
        maze.cells[pos].Cell_Pos.y=it.ItemPos.y;
        maze.cells[pos].item = it;
        maze.cells[pos].containItem=TRUE;
                
        return 1;
        
    }
    else if (it.itType = 2)//position at the part of team2
    {
        do{
            row=(rand()  %dx+(dx/2));
            col=(rand()  %dy+(dy/2));

            
            pos= getPos(col,row);
            
        }while(maze.cells[pos].C_Type!=CT_Floor);
        it.ItemPos.x=row;
        it.ItemPos.y=col;
        
        maze.cells[pos].Cell_Pos.x=it.ItemPos.x;
        maze.cells[pos].Cell_Pos.y=it.ItemPos.y;
        maze.cells[pos].item = it;
        maze.cells[pos].containItem=TRUE;
        
        return 1;
    }
    
    
    else
    {
        return -1;
    }
}




int dropItem(int playerID,/* Deltas *d, */Item it)
{
    struct player *ptr = NULL;

    Item *itPtr = NULL;
    int pos;
    
    ptr=search_in_list(playerID, NULL);
    ptr=search_in_list(playerID, NULL);
    
    
    
    
  
    
    
     
    if(ptr->i->itType!=0)
    {
        pos=getPos(ptr->PlayerPos.y, ptr->PlayerPos.x);
        maze.cells[pos].item=*(ptr->i);//not sure
       
        maze.cells[pos].Cell_Pos.x=ptr->i->ItemPos.x;
        maze.cells[pos].Cell_Pos.y=ptr->i->ItemPos.y;
        it.ItemPos.x= maze.cells[pos].Cell_Pos.x;
        it.ItemPos.y=maze.cells[pos].Cell_Pos.y;
        
        ptr->i->itType=0;
        //whereDropped.containItem= TRUE;
        
    
    
    
        
   /*        add_delta_player(d, playerArray[playerID], sizeof(Player));
        add_delta_cell(d, Cell, sizeof(Cell));
        add_delta_item(d, item, sizeof(item));
     */   
        return 1;
    }
    else
    {
        return -1;
    }
}

int breakWall(int playerID,int x, int y/*, Deltas *d*/)
{
    struct player *ptr = NULL;
    
    int pos;
    
    ptr=search_in_list(playerID, NULL);
    
    
    pos=getPos(ptr->PlayerPos.y, ptr->PlayerPos.x);
    

    if ((ptr->i->itType==Shovel)&&(maze.cells[pos].C_Type==CT_Wall))
    {
        
        ptr->PlayerPos.x=x;
        ptr->PlayerPos.y=y;
        
        //  Player->PlayerPos->y=CheckCell->Cell_Pos->y;
        
        maze.cells[pos].C_Type = CT_Floor;
        
        resetItem(/*Deltas *d,*/*(ptr->i), ptr->team);
        ptr->i = NULL;
        
        
        /*
        add_delta_player(d, Player, sizeof(Player));
        add_delta_cell(d, CheckCell, sizeof(Cell));
        add_delta_item(d, item, sizeof(item));
       */ 
        return 1;
        
    }
    else
    {
        return -1;
    }
}


int main(void)
{
    
   
    
    Item testIt;
     
    

   
    startGame();
//dumpMap();
    testIt.itType=Flag_Team1;
    
    
    
    testIt.ItemPos.x=0;
    testIt.ItemPos.y=0;
resetItem(testIt, 1);
    
  
}






