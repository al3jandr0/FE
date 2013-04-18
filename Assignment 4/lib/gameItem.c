
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <strings.h>
#include <errno.h>
#include <pthread.h>

#include "maze.h"
#include "general_utils.h"

//playerArray[MAX];

//playerArray[playerID]->holdShovel = TRUE;



extern int
pickUpItem(int playerID, Deltas *d){
    if(Cell.containItem==true){
        
        Item->itemPos->x=playerArray[playerID]->PlayerPos->x;
        Item.itemPos->y=playerArray[playerID]->PlayerPos->y;
        
        if(Item->itType==Shovel){
            Item->hasAbility=true;
           playerArray[playerID].holdShovel=true;
      
        
        }else if(Item->itType==Flag_Team1)||(Item->itType==Flag_Team2){
            playerArray[playerID]->holdFlag=true;
            Item->hasAbility=false;
        }
        
        Cell.containItem=false;
        add_delta_player(d, playerArray[playerID], sizeof(Player));
        add_delta_cell(d, Cell, sizeof(Cell));
        add_delta_item(d, item, sizeof(item));
    return 1;
    }else{
        return -1;
    }
}

extern int
breakWall(int x, int y, Deltas *d){
    
    Cell CheckCell =maze[x][y];
    
    
    if(CheckCell->C_Type==CT_Wall)    {
       
       // Player->PlayerPos->x=CheckCell->Cell_Pos->x;
       
      //  Player->PlayerPos->y=CheckCell->Cell_Pos->y;
       
        CheckCell->C_type=CT_Floor;
       
        Player->holdShovel=false;
       
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

extern int
resetItem(Deltas *d, Item it){
    
    if(it->itType=Shovel)
    {
        //position to the base
       
    }
    else if(itemType=Flag_team1)
    {
        
        //position at the part of team1
        }
    else if(itemType=Flag_team2)
    {
            
    //position at the part of team2
    }
    return 1;
    else
    {
    return -1;
    }
}


extern int
int dropItem(int playerID, Deltas *d, Item it){
if(Cell->C_Type==CT_Floor)
{
    
    /*
     
     if(playerarray[id]-> holdFlag == TRUE)
     {
     playerarray[id]-> item  == False
     
     }
    
    */
    
   // playerArray[playerID
    //Cell->Cell_Pos->x=it->itemPos->x;
    //Cell->Cell_Pos->y=it->itemPos->y;
    if(playerArray[playerID]->holdFlag==TRUE)
    {
        playerArray[playerID]->holdFlag=FALSE;
        
    }
    else if(playerArray[playerID]->holdShover=TRUE){
        playerArray[playerID]->holdShovel=FALSE;
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

