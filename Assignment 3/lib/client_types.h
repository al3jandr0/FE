
#ifndef CLIENT_TYPES_H
#define CLIENT_TYPES_H

/* Game Client Variables */

typedef struct {
   int player_id;
   int game_state;
   unsigned long long game_version;
} GameData;

typedef struct {
   int rows;
   int columns;
   char* maze;
} Maze;

extern GameData gamedata;
extern Maze themaze;

extern pthread_mutex_t client_data_mutex;
extern pthread_mutex_t client_maze_mutex;
#endif
