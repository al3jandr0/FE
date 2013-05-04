#ifndef GENERIC_UTILS_H
#define GENERIC_UTILS_H

typedef struct
{
   void *value;
   struct LinkedList *next;

} LinkedList;

typedef struct
{
  LinkedList *player_l;
  LinkedList *cell_l;
  LinkedList *item_l;
  int p_size; // player list size
  int c_size; // cell list size
  int i_size; // item list size

} Deltas;

/*
  d. pointer to delta
  p. pointer to any struct (Player, Cell, Item)
  size. size in bytes of the struct pointed by p. 
        Use sizeof(Player) if p points to a player struct
        Use sizeof(Cell) if p points to a Cell struct
        Use sizeof(Item) if p points to an Item`:w
 struct
*/

extern int logMaze( char* fmaze, int maze_size, unsigned long long ver );

extern int add_delta_player(Deltas *d, void *p, int size);
extern int add_delta_cell(Deltas *d, void *p, int size);
extern int add_delta_item(Deltas *d, void *p, int size);

extern int clean_deltas(Deltas *d);
extern int init_deltas( Deltas *d);
static LinkedList *list_add( LinkedList *list, void *p, int size);
static void list_delete( LinkedList *il );

#endif
