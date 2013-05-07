#include "general_utils.h"
#include <stdlib.h>
#include "stdio.h"

double get_time()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return ((double)(tv.tv_sec + tv.tv_usec*1.0e-6));
}

int modulus(int a, int b)
{
   // a % b = (a%b+b)%b
   return (a%b+b)%b;
}

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

extern int logMaze( char* fmaze, int maze_size, unsigned long long ver )
{
    char filename[30];
    FILE *file;
    int rc;

    snprintf(&filename[0], sizeof(filename), "./Mazelog/VERSION%llu", ver );
    file = fopen( &filename[0], "w" );
    if (file == NULL)
       fprintf(stderr, "@logMaze: ERROR fopen failed");
    rc = fwrite( fmaze, sizeof(char), maze_size, file );
    if (ferror(file))
       perror("@logMaze: ERROR when wrtiting to file. "); 
    rc = fclose(file);
    if (rc != 0)
       perror("@logMaze: ERROR when closign file file. "); 

    return 1;
}


extern int log_RPC_time( double diff )
{
    char filename[30], strtime[30];
    FILE *file;
    int rc;

    file = fopen( "./Mazelog/RPCtime", "a" );
    if (file == NULL)
       fprintf(stderr, "@logRPC: ERROR fopen failed");
    sprintf( &strtime[0], "%F\n", diff );
    rc = fwrite( strtime, sizeof(char), strlen(strtime), file );
    if (ferror(file))
       perror("@logRPC: ERROR when wrtiting to file. ");
    rc = fclose(file);
    if (rc != 0)
       perror("@logRPC: ERROR when closign file file. ");

    return 1;
}




