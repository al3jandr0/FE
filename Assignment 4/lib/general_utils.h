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

extern int add_delta_player(Deltas *d, void *p, int size);
extern int add_delta_cell(Deltas *d, void *p, int size);
extern int add_delta_item(Deltas *d, void *p, int size);


extern int clean_deltas(Deltas *d);
extern int init_deltas( Deltas *d);
static LinkedList *list_add( LinkedList *list, void *p, int size);
static void list_delete( LinkedList *il );

#endif
