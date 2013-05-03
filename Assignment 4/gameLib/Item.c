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
    int hasAbility;
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
    Item *o;
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


int pickUpItem(int playerID/* Deltas *d */)
{   struct player *ptr = NULL;
    Cell possibleItemCell;
    Item *itPtr = NULL;
    
    ptr=search_in_list(playerID, NULL);
    
   
    
    
    possibleItemCell.Cell_Pos.x=ptr->PlayerPos.x;
    possibleItemCell.Cell_Pos.y=ptr->PlayerPos.y;
    if (possibleItemCell.containItem == TRUE)
    {
        
        itPtr->ItemPos.x = ptr->PlayerPos.x;
        itPtr->ItemPos.y = ptr->PlayerPos.y;
        
            
            ptr->i=itPtr;
        
                
        possibleItemCell.containItem = FALSE;/*
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


int breakWall(struct player *ptr, int x, int y/*,Deltas *d*/)
{
    
    Cell *CheckCell;
    CheckCell->Cell_Pos.x=x;
    CheckCell->Cell_Pos.y=y;
    
    if ((CheckCell->C_Type == CT_Wall)&&(ptr->i->itType==3))
    
    {
        
        ptr->PlayerPos.x=CheckCell->Cell_Pos.x;
        
        ptr->PlayerPos.y=CheckCell->Cell_Pos.y;
        
        CheckCell->C_Type = CT_Floor;
        
        ptr->i=NULL;
       //resets the item,
        
       // add_delta_player(d, Player, sizeof(Player));
       // add_delta_cell(d, CheckCell, sizeof(Cell));
       // add_delta_item(d, item, sizeof(item));
        
        return 1;
    
    }
    else
    {
        return -1;
    }
}
/*
int resetItem(/*Deltas *d, Item it)
{
    
    if (it->itType ==3)
    {
        //position to the base
        
    }
    else if (itemType = 1)
    {
        
        //position at the part of team1
    }
    else if (itemType = 2)
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





