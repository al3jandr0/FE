
#ifndef CLIENT_TYPES_H
#define CLIENT_TYPES_H

/* Game Client Variables */

typedef struct {
   int game_state;
   unsigned long long game_version;
} GameData;

typedef struct {
   int rows;
   int columns;
   char* maze;
   unsigned long long *cell_version;
} GameMaze;

typedef struct {
   int id;
   int team;
   int x;
   int y;
   char state;
   char flag;
   char shovel;
} PlayerInfo;

extern GameData gamedata;
extern GameMaze themaze;
extern PlayerInfo playerdata;

extern pthread_mutex_t client_data_mutex;
extern pthread_mutex_t client_maze_mutex;
extern pthread_spinlock_t client_data_spinlock;
#endif
