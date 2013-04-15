
#ifndef SERVER_TYPES_H
#define SERVER_TYPES_H

/* Game Client Variables */

typedef struct {
   int state;
   unsigned long long version;
} Server_GameData;

extern Server_GameData server_gameData;

extern pthread_mutex_t server_data_mutex;
extern pthread_spinlock_t server_data_spinlock;
// pthread_spin_unlock(&spinlock);
// thread_spin_unlock(&spinlock);

#endif
