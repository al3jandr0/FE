#include "general_utils.h"
#include <stdlib.h>

extern int add_delta_player(Deltas *d, void *p, int size)
{
   d->player_l = list_add( d->player_l, p, size );
   d-> p_size++;
   return 1;
}

extern int add_delta_cell(Deltas *d, void *p, int size)
{
   d->cell_l = list_add( d->cell_l, p, size );
   d-> c_size++;
   return 1;
}

extern int add_delta_item(Deltas *d, void *p, int size)
{
   d->item_l = list_add( d->item_l, p, size );
   d-> i_size++;
   return 1;
}

extern int clean_deltas(Deltas *d)
{
   list_delete( d->player_l );
   list_delete( d->cell_l );
   list_delete( d->item_l );

   init_deltas(d);
   return 1;
}
extern int init_deltas( Deltas *d)
{
   d->player_l = NULL;
   d->cell_l   = NULL;
   d->item_l   = NULL;
   d->p_size = 0;
   d->c_size = 0;
   d->i_size = 0;
   return 1;
}

// TODO: add error checks
static LinkedList *list_add( LinkedList *list, void *p, int size)
{
   LinkedList *l = (LinkedList*) malloc( sizeof(LinkedList) );
   l->value = malloc( size );
   memcpy( l->value, p, size );
   l->next  = list;
   return l;
}

static void list_delete( LinkedList *il )
{
   LinkedList *tmp;
   while ( il !=NULL )
   {
      tmp = il;
      il = il->next;
      free(tmp->value);
      free(tmp);
   }
}

