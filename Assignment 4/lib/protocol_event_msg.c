#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <strings.h>
#include <errno.h>
#include <pthread.h>

#include "net.h"
#include "protocol.h"
#include "protocol_utils.h"
#include "protocol_server.h"

#include "protocol_event_msg.h"
#include "client_types.h"
#include "./../lib/maze.h"
#include "general_utils.h"
#include <stdio.h>

// id     Team  x     y     state item  item 

extern int
proto_server_test_deltas(Deltas *d)
{
   Player p;
   Cell c;

   p.team = 1;
   p.PlayerPos.x = 1;
   p.PlayerPos.y = 2;
   p.ID  = 10;
   p.PID = 10;
   p.State = 4;
   // p.holdFlag = 1;
   // p.holdShovel = 0;

   c.C_Type = 'C';
   c.Cell_Pos.x = 1;
   c.Cell_Pos.y = 1;
   c.Cell_Team = 2;

   add_delta_player(d, &p, sizeof(Player));
   add_delta_cell(d, &c, sizeof(Cell));

   return 1;
}

extern int
proto_server_marshall_deltas(Proto_Session *s, Deltas *d)
{
   int ii, offset, extra;
   char cells[3], p_infs[6], items[3];
   LinkedList *ll;
   Cell *cval;
   Player *pval;
   //Item *ival;
 
   extra = 101;
   if (proto_debug())
      fprintf(stderr, "proto_client_event_update_handler: c_cell = %d, c_player = %d, c_citem = %d, " 
                      "extra = %d\n", d->c_size, d->p_size, d->i_size, extra);

   if (proto_session_body_marshall_int(s, d->c_size)   < 0)
      fprintf(stderr, "proto_server_test_msg: proto_session_body_marshall_int failed\n");
   if (proto_session_body_marshall_int(s, d->p_size) < 0)
      fprintf(stderr, "proto_server_test_msg: proto_session_body_marshall_int failed\n");
   if (proto_session_body_marshall_int(s, d->i_size)   < 0)
      fprintf(stderr, "proto_server_test_msg: proto_session_body_marshall_int failed\n");
   if (proto_session_body_marshall_int(s, extra)  < 0)
      fprintf(stderr, "proto_server_test_msg: proto_session_body_marshall_int failed\n");

   if (proto_debug())
      fprintf(stderr, "proto_server_test_msg: Sending:\n");

   ll = d->cell_l;
   for ( ii = 0; ii < d->c_size; ii++ )
   {
      cval = (Cell*) ll->value; 
      cells[0] = cval->C_Type;
      cells[1] = (char)cval->Cell_Pos.x;
      cells[2] = (char)cval->Cell_Pos.y;
      if (proto_debug())
         fprintf(stderr, "cell %d: #%c#, #%d#, #%d#\n", ii, cells[0], (int)cells[1], (int)cells[2] );
      proto_session_body_marshall_bytes(s, 3, &cells[0]); 
      ll = ll->next;
   }
   ll = d->player_l;
   for ( ii = 0; ii < d->p_size; ii++ )
   {
      pval = (Player*) ll->value;
      p_infs[0] = (char)pval->team; 
      p_infs[1] = (char)pval->PlayerPos.x; 
      p_infs[2] = (char)pval->PlayerPos.y; 
      p_infs[3] = (char)pval->State; 
      p_infs[4] = 'Y'; // p_infs[4] = (pval->holdFlag)? 'Y':'N'; 
      p_infs[5] = 'N'; // p_infs[5] = (pval->holdShovel)? 'Y':'N'; 
      if (proto_debug())
         fprintf(stderr, "player %d: #%d#, #%d#, #%d#, #%c#, #%c#, #%c#\n", 
            pval->PID, (int)p_infs[0], (int)p_infs[1], (int)p_infs[2] , (int)p_infs[3], p_infs[4], p_infs[5]);
      proto_session_body_marshall_int(s, ii);
      proto_session_body_marshall_bytes(s, 6, &p_infs[0]); 
      ll = ll->next;
   }
   /* TODO: erite this part when items are ready
   for ( ii = 0; ii < c_item; ii++ )
   {
      fprintf(stderr, "item %d: #%c#, #%c#, #%c#\n", ii, items[ii][0], items[ii][1], items[ii][2] );
      proto_session_body_marshall_bytes(s, 3, &items[ii][0]); 
   }*/
}

// marshalls a body's message with hardcoded info fro debuging
extern int
proto_server_test_msg(Proto_Session *s )
{
   int extra, ii, offset, c_cell, c_player, c_item;

   char cells[2][3]  = {{'a','0','1'},{'b','2','3'}};
   char items[1][3]  = {{'s','4','5'}};
   char p_infs[2][6] = {{'A','B','C','D','E','F'},{'G','H','I','J','K','L'}};

   c_cell   = sizeof(cells)  / 3;
   c_item   = sizeof(items)  / 3;
   c_player = sizeof(p_infs) / 6;
   extra    = 101;

   fprintf(stderr, "proto_client_event_update_handler: c_cell = %d, c_player = %d, c_citem = %d, " 
                   "extra = %d\n", c_cell, c_player, c_item, extra);

   if (proto_session_body_marshall_int(s, c_cell)   < 0)
      fprintf(stderr, "proto_server_test_msg: proto_session_body_marshall_int failed\n");
   if (proto_session_body_marshall_int(s, c_player) < 0)
      fprintf(stderr, "proto_server_test_msg: proto_session_body_marshall_int failed\n");
   if (proto_session_body_marshall_int(s, c_item)   < 0)
      fprintf(stderr, "proto_server_test_msg: proto_session_body_marshall_int failed\n");
   if (proto_session_body_marshall_int(s, extra)  < 0)
      fprintf(stderr, "proto_server_test_msg: proto_session_body_marshall_int failed\n");

   fprintf(stderr, "proto_server_test_msg: Sending:\n");
   for ( ii = 0; ii < c_cell; ii++ )
   {
      fprintf(stderr, "cell %d: #%c#, #%c#, #%c#\n", ii, cells[ii][0], cells[ii][1], cells[ii][2] );
      proto_session_body_marshall_bytes(s, 3, &cells[ii][0]); 
   }
   for ( ii = 0; ii < c_player; ii++ )
   {
      fprintf(stderr, "player %d: #%c#, #%c#, #%c#, #%c#, #%c#, #%c#\n", 
            ii, p_infs[ii][0], p_infs[ii][1], p_infs[ii][2] , p_infs[ii][3], p_infs[ii][4], p_infs[ii][5]);
      proto_session_body_marshall_int(s, ii);
      proto_session_body_marshall_bytes(s, 6, &p_infs[ii][0]); 
   }
   for ( ii = 0; ii < c_item; ii++ )
   {
      fprintf(stderr, "item %d: #%c#, #%c#, #%c#\n", ii, items[ii][0], items[ii][1], items[ii][2] );
      proto_session_body_marshall_bytes(s, 3, &items[ii][0]); 
   }
}

inline void 
update_mazecell( int ver, int x, int y, char disp, int special )
{
   int I;
   I = x*themaze.rows + y;
   if (special)
   {
      if ( themaze.cell_version[I] < ver )
         themaze.maze[I] = disp;
   }
   else
      themaze.maze[I] = disp;
}

extern int 
proto_client_event_msg_unmarshall_v1( Proto_Session *s, int blen, unsigned long long ver, int special )
{
    int c_cell, c_player, c_item, extra, ii, offset, playerId, I;
    char cellinfo[4]; // type, x, y
    char iteminfo[4]; // type, x, y
    char playerinfo[7]; // team, x, y, state, flag, item
    cellinfo[3] = 0;   // for easy printing
    iteminfo[3] = 0;   // for easy printing
    playerinfo[6] = 0; // for easy printing

	if ( blen < (4*sizeof(int)) )
	{
	    fprintf(stderr, "proto_client_event_update_handler: "
                    "message is shorter than expected (message version 1)\n");
	     return -1;
	}
	// unmarshall header 
	if (proto_session_body_unmarshall_int(s, 0, &c_cell) < 0)
	    fprintf(stderr, "proto_client_event_update_handler:"
			" proto_session_body_unmarshall_int failed\n");
	if (proto_session_body_unmarshall_int(s, sizeof(int), &c_player) < 0)
	    fprintf(stderr, "proto_client_event_update_handler:"
			" proto_session_body_unmarshall_int failed\n");
	if (proto_session_body_unmarshall_int(s, 2*sizeof(int), &c_item) < 0)
	    fprintf(stderr, "proto_client_event_update_handler:"
			" proto_session_body_unmarshall_int failed\n");
	if (proto_session_body_unmarshall_int(s, 3*sizeof(int), &extra) < 0)
	    fprintf(stderr, "proto_client_event_update_handler:"
			" proto_session_body_unmarshall_int failed\n");
 
	if (proto_debug())
           fprintf(stderr, "proto_client_event_update_handler: c_cell = %d, c_player = %d, c_citem = %d, " 
                           "extra = %d\n", c_cell, c_player, c_item, extra);

	offset = 4*sizeof(int);
	if ( blen < ( offset + 3*c_cell ) )
        {
	    fprintf(stderr, "proto_client_event_update_handler: ERROR: blen = %d," 
                            " expected buffer size of %d\n", blen, offset + 3*c_cell);
            return -1;
        }
        // unmarshall cells. x, y, cell_type
	for ( ii = 0; ii < c_cell; ii++ )
	{
	    // TODO: check blen 
	    if (proto_session_body_unmarshall_bytes(s, offset + 3*ii, 3, &cellinfo[0]) < 0)
	       fprintf(stderr, "proto_client_event_update_handler: unmarshall_bytes_failed\n");
	    // update local maze        
	    if (proto_debug())
	       fprintf(stderr, "proto_client_event_update_handler: cellinfo %d %c %d %d\n",
                               ii, cellinfo[0], (int)cellinfo[1], (int)cellinfo[2]);
            update_mazecell( ver, (int)cellinfo[1], (int)cellinfo[2], cellinfo[0], special );
	}

	offset += 3*c_cell;
	if ( blen < ( offset + 10*c_player ) )
        {
	    fprintf(stderr, "proto_client_event_update_handler: ERROR: blen = %d," 
                            " expected buffer size of %d\n", blen, offset + 10*c_cell);
            return -1;
        }
	// unmarshall player. 10 bytes
	for ( ii = 0; ii < c_player; ii++ )
	{
	    // TODO: check blen 
	    if (proto_session_body_unmarshall_int(s, offset + 10*ii, &playerId) < 0)
	       fprintf(stderr, "proto_client_event_update_handler: unmarshall_int_failed\n");
	    if (proto_session_body_unmarshall_bytes(s, offset + 10*ii + sizeof(int), 6, &playerinfo[0]) < 0)
	       fprintf(stderr, "proto_client_event_update_handler: unmarshall_bytes_failed\n");
	    if (proto_debug())
	       fprintf(stderr, "proto_client_event_update_handler: playerinfo deltaNo.%d: %d, %d, %d, %d, %c, %c, %c\n", 
                                ii, playerId, (int)playerinfo[0], (int)playerinfo[1], (int)playerinfo[2], playerinfo[3], 
                                playerinfo[4], playerinfo[5]);

            update_mazecell( ver, (int)playerinfo[1], (int)playerinfo[2], 'x', special );
            // update local player info
            if (playerdata.id == playerId)
            {
               playerdata.team = (int)playerinfo[0];
               playerdata.x = (int)playerinfo[1];
               playerdata.y = (int)playerinfo[2];
               playerdata.state = playerinfo[3];
               playerdata.flag = playerinfo[4];
               playerdata.shovel = playerinfo[5];
            }
        }

	offset += 10*c_player;
	if ( blen < ( offset + 3*c_item ) )
        {
	    fprintf(stderr, "proto_client_event_update_handler: ERROR: blen = %d," 
                            " expected buffer size of %d\n", blen, offset + 3*c_cell);
            return -1;
        }
	// unmarshall items. 3 bytes
	for ( ii = 0; ii < c_item; ii++ )
	{
	    // check blen 
	    if (proto_session_body_unmarshall_bytes(s, offset + 3*ii, 3, &iteminfo[0]) < 0)
	       fprintf(stderr, "proto_client_event_update_handler: unmarshall_bytes_failed\n");
	    // update local maze        
	    if (proto_debug())
	       fprintf(stderr, "proto_client_event_update_handler: iteminfo %d %c %d %d\n",
                               ii, iteminfo[0], (int)iteminfo[1], (int)iteminfo[2]);
            update_mazecell( ver, (int)iteminfo[1], (int)iteminfo[2], iteminfo[0], special );
	}

	return 1;
}

