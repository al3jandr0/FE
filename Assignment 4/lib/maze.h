#ifndef __DAGAME_MAZE__
#define __DAGAME_MAZE__

#define numPlayers = 200;

typedef enum{
	Team1=1,
	Team2=2
}Team;

typedef struct {
	int x;
	int y;
}Position;

typedef enum{
	CT_Floor =' ',
	CT_Wall = '#',
	CT_Jailj ='j',
	CT_JailJ ='J',
	CT_Homeh ='h',
	CT_HomeH ='H',
}Cell_Type;

typedef enum{
    Flag_Team1='f',
    Flag_Team2='F',
    Shovel='S',    
}ItemType;

typedef struct{
    itemType itType;
    Position itemPos;
    bool hasAbility;
    
}Item

typedef struct{
        Team team;
	Position PlayerPos;
	int ID;
	int PID;
        char State;
    bool holdFlag;
    bool holdShovel;
	//Object *o;  //fix for items
}Player;

/* update for player list
 typedef struct{
 Player players[numPlayers];
 }PlayerList;
 */

typedef struct{
	Cell_Type C_Type;
	Position Cell_Pos;
	Player *p;
	Team Cell_Team;
        bool containItem;
	//Object *o;  //fix for items
}Cell;

typedef struct {
	Cell *cells;
	int numFloor;
	int numWall;
	int numCells;
	int numOfJails[2];
	int numOfHomes[2];
	Position dimensions;
}Maze;

extern int  findNumHome(int team);
extern int  findNumJail(int team);
extern int  findNumWall();
extern int  findNumFloor();
extern void  findDimensions(Position *p);
extern void findCInfo(int x, int y);

extern char is_cell_occupied(int column, int row);
extern char get_cell_team(int column, int row);
extern char get_cell_type(int column, int row);
extern int get_maze_dimy();
extern int get_maze_dimx();
extern void dumpMap();
extern Maze loadMap();

#endif

