
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <strings.h>
#include <errno.h>
#include <pthread.h>

#include "maze.h"
#include "general_utils.h"

extern int
pickUpItem(int playerID, Deltas *d){
    if(Cell.containItem==true){
        
        Item->itemPos->x=Player->PlayerPos->x;
        Item.itemPos->y=Player->PlayerPos->y;
        
        if(Item->itType==Shovel){
            Item->hasAbility=true;
            Player.holdShovel=true;
      
        
        }else if(Item->itType==Flag_Team1)||(Item->itType==Flag_Team2){
            Player->holdFlag=true;
            Item->hasAbility=false;
        }
        
        Cell.containItem=false;
        add_delta_player(d, Player, sizeof(Player));
        add_delta_cell(d, Cell, sizeof(Cell));
        add_delta_item(d, item, sizeof(item));
    return 1;
    }else{
        return -1;
    }
}

extern int
breakWall(int x, int y, Deltas *d){
    if(Cell.C_Type==CT_Wall)&&(Player.holdShovel==true)
    {
       
        Player->PlayerPos->x=Cell->Cell_Pos->x;
       
        Player->PlayerPos->y=Cell->Cell_Pos->y;
       
        C_type=CT_Floor;
       
        Player->holdShovel=false;
       
        add_delta_player(d, Player, sizeof(Player));
        add_delta_cell(d, Cell, sizeof(Cell));
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
    
    if(i->itType=Shovel)
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
int dropItem(int playerID, Deltas *d, Item it)
if(cell==CT_Floor)
{
    Cell->Cell_Pos->x=it->itemPos->x;
    Cell->Cell_Pos->y=it->itemPos->y;
    
    add_delta_player(d, Player, sizeof(Player));
    add_delta_cell(d, Cell, sizeof(Cell));
    add_delta_item(d, item, sizeof(item));
    
    return 1;
}
else
{
    return -1;
}


