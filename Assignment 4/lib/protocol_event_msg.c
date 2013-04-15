
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
#include <stdio.h>

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


extern int 
proto_client_event_msg_unmarshall_v1( Proto_Session *s, int blen, unsigned long long ver )
{
    int c_cell, c_player, c_item, extra, ii, offset, playerId;
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
        // unmarshall cells
	for ( ii = 0; ii < c_cell; ii++ )
	{
	    // TODO: check blen 
	    if (proto_session_body_unmarshall_bytes(s, offset + 3*ii, 3, &cellinfo[0]) < 0)
	       fprintf(stderr, "proto_client_event_update_handler: unmarshall_bytes_failed\n");
	    // update local maze        
	    if (proto_debug())
	       fprintf(stderr, "proto_client_event_update_handler: cellinfo %d %s\n", ii, &cellinfo[0] );
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
	       fprintf(stderr, "proto_client_event_update_handler: player %d, id = %d\n", ii, playerId );
	    if (proto_debug())
	       fprintf(stderr, "proto_client_event_update_handler: playerinfo %d %s\n", ii, &playerinfo[0] );
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
	       fprintf(stderr, "proto_client_event_update_handler: iteminfo %d %s\n", ii, &iteminfo[0] );
	}

	return 1;
}

