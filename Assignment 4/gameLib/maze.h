#ifndef __DAGAME_MAZE__
#define __DAGAME_MAZE__

#define MAX = 200;
//extern Maze maze;

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
    int Type;

} Item;

typedef struct
{
    Cell_Type C_Type;
    Position Cell_Pos;
    Player *p;
    Team Cell_Team;
    Item *o;
    int occupied;
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

extern int  findNumHome(int team);
extern int  findNumHome(int team);
extern int  findNumWall();
extern int  findNumFloor();
extern void  findDimensions(Position *p);
extern void findCInfo(int x, int y);


#endif
