#ifndef __DAGAME_MAZE__
#define __DAGAME_MAZE__
"MAZE struct, player struct"
#define numPlayers=200;

typedef struct {
  Cell *cells;
  int numFloor;
  int numWall;
  int numCells;
  int numOfJails[2];
  int numOfHomes[2];
  int x,y;
}Maze;


typedef enum{
  CT_Floor =" ";
  CT_Wall = "#";
  CT_Jailj ="j";
  CT_JailJ ="J";
  CT_Homeh ="h";
  CT_HomeH ="H";
}Cell_Type;

typedef struct{
  Cell_Type C_Type;
  Position Cell_Pos;
  Player *p;
  Object *o;
}Cell;

typedef struct {
  int x;
  int y;
}Position;

//PLAYER STRUCT

typedef struct{
  Player players[numPlayers];
}PlayerList;

typedef struct{
  Team team;
  Position PlayerPos;
  int ID;
  int PID;
  Object *o;
}Player;

typedef enum{
  Team1=1;
  Team2=2;
}Team;

