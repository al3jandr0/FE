
#ifndef SERVER_TYPES_H
#define SERVER_TYPES_H

/* Game Client Variables */

typedef struct {
   int state;
   unsigned long long version;
   unsigned long long trs; // transaction number
} Server_GameData;

// Server_GameData
//   state 
//      -1 nothing
//       0 player have joined. game has not started
//       1 game started

extern Server_GameData server_gameData;

extern pthread_mutex_t server_data_mutex;
extern pthread_spinlock_t server_data_spinlock;
// pthread_spin_unlock(&spinlock);
// thread_spin_unlock(&spinlock);

#endif
