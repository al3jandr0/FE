/******************************************************************************
 * Copyright (C) 2011 by Jonathan Appavoo, Boston University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <strings.h>
#include <errno.h>
#include <pthread.h>

#include "protocol.h"
#include "protocol_utils.h"
#include "protocol_client.h"
#include "client_types.h"
#include "protocol_event_msg.h"

GameData gamedata;
GameMaze themaze;
PlayerInfo playerdata;
pthread_mutex_t client_data_mutex;
pthread_mutex_t client_maze_mutex;

typedef struct
{
    Proto_Session rpc_session;
    Proto_Session event_session;
    pthread_t EventHandlerTid;
    Proto_MT_Handler session_lost_handler;
    Proto_MT_Handler base_event_handlers[PROTO_MT_EVENT_BASE_RESERVED_LAST
                                         - PROTO_MT_EVENT_BASE_RESERVED_FIRST
                                         - 1];
} Proto_Client;

extern Proto_Session *
proto_client_rpc_session(Proto_Client_Handle ch)
{
    Proto_Client *c = ch;
    return &(c->rpc_session);
}

extern Proto_Session *
proto_client_event_session(Proto_Client_Handle ch)
{
    Proto_Client *c = ch;
    return &(c->event_session);
}

extern int
proto_client_set_session_lost_handler(Proto_Client_Handle ch, Proto_MT_Handler h)
{
    Proto_Client *c = ch;
    c->session_lost_handler = h;
}

extern int
proto_client_set_event_handler(Proto_Client_Handle ch, Proto_Msg_Types mt,
                               Proto_MT_Handler h)
{
    int i;
    Proto_Client *c = ch;

    if (mt > PROTO_MT_EVENT_BASE_RESERVED_FIRST &&
            mt < PROTO_MT_EVENT_BASE_RESERVED_LAST)
    {
        i = mt - PROTO_MT_EVENT_BASE_RESERVED_FIRST - 1;
        c->base_event_handlers[i] = h;
        return 1;
    }
    else
    {
        return -1;
    }
}

static int
proto_client_session_lost_default_hdlr(Proto_Session *s)
{
    fprintf(stderr, "Session lost...:\n");
    proto_session_dump(s);
    return -1;
}

static int
proto_client_event_null_handler(Proto_Session *s)
{
    fprintf(stderr,
            "proto_client_event_null_handler: invoked for session:\n");
    proto_session_dump(s);

    return 1;
}
/*
Broadcast Message Format Version 1

header: game_ver, game_state, count_cellinfo, count_playerinfo, count_iteminfo, extra?
        uint      long        short           short             short           int
        4 + 8 + 2 + 2 + 1 = 22 bytes

        4 + 5 bytes = 10 bytes
items: type, x,    y
       char  char  char
       3 bytes
*/
static int
proto_client_event_update_handler(Proto_Session *s)
{
    unsigned long long temp_version;
    Proto_Msg_Hdr h;

    if (proto_debug())
        fprintf(stderr,
                "proto_client_event_update_handler: invoked for session:\n");
    // proto_session_dump(s);

    bzero(&h, sizeof(h));
    proto_session_hdr_unmarshall(s, &h);

    // pthread_mutex_lock(&gameMap_clientVersion_mutex);
    if (proto_debug())
        fprintf(stderr, "server game_version = %llu\n, local game_version = %llu\n,"
                "server game_state = %d\n", h.sver.raw, gamedata.game_version, h.gstate.v0.raw);

    // check that client recieved rpc join reply
    while (1)
    {
        pthread_mutex_lock(&client_data_mutex);
        if ( gamedata.game_version != -1 )
        {
           // TODO: implement correct game update (Sol 1) 
           if (proto_client_event_msg_unmarshall_v1(s, h.blen, h.sver.raw) < 0)
              fprintf(stderr, "proto_client_event_update_handler: ERROR "
                              "proto_client_event_msg_unmarshall_v1 failed\n"); 
          // if ( h.sver.raw > gamedata.game_version )
          // {
              gamedata.game_version = h.sver.raw;
              // update all the maze
          // }
	  /* else if (h.sver.raw < gamedata.game_version) 
           {
              // update the older maze cells 
              proto_client_event_msg_unmarshall_v1( s, h.blen, h.sver.raw, 1 );
           }
           else
           {
              fprintf(stderr, "proto_client_event_update_handler: "
                              "ERROR local game version = server game verison");
           }*/
           if (proto_debug())
              fprintf(stderr, "new client (local) game_version = %llu\n", 
                                                   gamedata.game_version);

           if ( h.gstate.v0.raw != gamedata.game_state )
              gamedata.game_state = h.gstate.v0.raw; 
           pthread_mutex_unlock(&client_data_mutex);
           break;
        }
        pthread_mutex_unlock(&client_data_mutex);
    }
    return 1;
}

static void *
proto_client_event_dispatcher(void *arg)
{
    Proto_Client *c;
    Proto_Session *s;
    Proto_Msg_Types mt;
    Proto_MT_Handler hdlr;
    int i;

    pthread_detach(pthread_self());

    c = (Proto_Client *)arg;
    s = &(c->event_session);

    for (;;)
    {
        if (proto_session_rcv_msg(s) == 1)
        {
            mt = proto_session_hdr_unmarshall_type(s);
            if (mt > PROTO_MT_EVENT_BASE_RESERVED_FIRST &&
                    mt < PROTO_MT_EVENT_BASE_RESERVED_LAST)
            {
                i = mt - PROTO_MT_EVENT_BASE_RESERVED_FIRST - 1;
                hdlr = c->base_event_handlers[i];
                if (hdlr(s) < 0) goto leave;
            }
        }
        else
        {
            c->session_lost_handler(s);
            goto leave;
        }
    }
leave:
    close(s->fd);
    return NULL;
}

extern int
proto_client_init(Proto_Client_Handle *ch)
{
    Proto_Msg_Types mt;
    Proto_Client *c;

    c = (Proto_Client *)malloc(sizeof(Proto_Client));
    if (c == NULL) return -1;
    bzero(c, sizeof(Proto_Client));

    proto_client_set_session_lost_handler(c,
                                          proto_client_session_lost_default_hdlr);

    // initialize local game state
    pthread_mutex_lock(&client_data_mutex);
    playerdata.id = -1;
    gamedata.game_state = -1;
    gamedata.game_version = -1;
    pthread_mutex_unlock(&client_data_mutex);

    for (mt = PROTO_MT_EVENT_BASE_RESERVED_FIRST + 1;
            mt < PROTO_MT_EVENT_BASE_RESERVED_LAST; mt++)
        proto_client_set_event_handler(c, mt, proto_client_event_null_handler);

    proto_client_set_event_handler(c, PROTO_MT_EVENT_BASE_UPDATE, proto_client_event_update_handler);

    *ch = c;
    return 1;
}

int
proto_client_connect(Proto_Client_Handle ch, char *host, PortType port)
{
    Proto_Client *c = (Proto_Client *)ch;

    if (net_setup_connection(&(c->rpc_session.fd), host, port) < 0)
        return -1;

    if (net_setup_connection(&(c->event_session.fd), host, port + 1) < 0)
        return -2;

    if (pthread_create(&(c->EventHandlerTid), NULL,
                       &proto_client_event_dispatcher, c) != 0)
    {
        fprintf(stderr,
                "proto_client_init: create EventHandler thread failed\n");
        perror("proto_client_init:");
        return -3;
    }

    return 0;
}

static void
marshall_mtonly(Proto_Session *s, Proto_Msg_Types mt)
{
    Proto_Msg_Hdr h;

    bzero(&h, sizeof(h));
    h.type = mt;
    proto_session_hdr_marshall(s, &h);
};

// all rpc's are assume to only reply only with a return code in the body
// eg.  like the null_mes
static int
do_generic_dummy_rpc(Proto_Client_Handle ch, Proto_Msg_Types mt)
{
    int rc;
    Proto_Session *s;
    Proto_Client *c = ch;

    s = &(c->rpc_session);
    // marshall

    marshall_mtonly(s, mt);
    rc = proto_session_rpc(s);

    if (rc == 1)
    {
        proto_session_body_unmarshall_int(s, 0, &rc);
    }
    else
    {
        c->session_lost_handler(s);
        close(s->fd);
    }

    return rc;
}

static int
do_join_game_rpc(Proto_Client_Handle ch, Proto_Msg_Types mt)
{
    int rc, X, Y, ii, size;
    Proto_Session *s;
    Proto_Client *c = ch;
    Proto_Msg_Hdr h;

    if (proto_debug())
        fprintf(stderr, "do_join_game_rpc started.\n");

    // prepare msessage
    s = &(c->rpc_session);
    bzero(&h, sizeof(h));
    h.type = mt;
    proto_session_hdr_marshall(s, &h);

    if (proto_debug())
        fprintf(stderr, "do_move_rpc:\n   pId: %d\n", playerdata.id);

    if (proto_session_body_marshall_int(s, playerdata.id) < 0)
        fprintf(stderr, "do_join_game_rpc: proto_session_body_marshall_int failed. "
                "Not enough available sbufer space\n");
    // sned message
    rc = proto_session_rpc(s);
    
    // process reply: pID, xdim, ydim, maze
    if (rc == 1)
    {
        proto_session_hdr_unmarshall(s, &h);
        if (proto_session_body_unmarshall_int(s, 0, &rc) < 0)
            fprintf(stderr, "do_join_game_rpc: proto_session_body_unmarshall_int failed\n");
        if (rc < 0) 
        {
           if (proto_debug())
              fprintf(stderr, "do_join_game_rpc: returned player id = %d\n", rc);
           return rc;
        }    

        if (proto_session_body_unmarshall_int(s, sizeof(int), &X) < 0)
            fprintf(stderr, "do_join_game_rpc: proto_session_body_unmarshall_int failed\n");
        if (proto_session_body_unmarshall_int(s, 2*sizeof(int), &Y) < 0)
            fprintf(stderr, "do_join_game_rpc: proto_session_body_unmarshall_int failed\n");

        // initialize game version and state
        pthread_mutex_lock(&client_data_mutex);
        playerdata.id = rc;
        gamedata.game_state = h.gstate.v0.raw;
        gamedata.game_version = h.sver.raw;

        themaze.rows = Y;
        themaze.columns = X;
        size = X*Y;
        themaze.maze = (char*) malloc( size*sizeof(char) + 1 );
        themaze.cell_version = (unsigned long long*) malloc(size*sizeof(unsigned long long));

        if (proto_session_body_unmarshall_bytes(s, 3*sizeof(int), size, themaze.maze) < 0)
            fprintf(stderr, "do_join_game_rpc: proto_session_body_unmarshall_bytes failed\n");
        for (ii = 0; ii < size; ii++)
            themaze.cell_version[ii] = h.sver.raw;
        themaze.maze[X*Y] = 0;
        if (proto_debug())
            fprintf(stderr, "do_join_game_rpc: unmarshalled response\n" 
                    "   game version = %llu\n game state = %d\n  player id = %d\n   dimx = %d\n"
                    "   dimy = %d\n", h.sver.raw, h.gstate.v0.raw, rc, X, Y);
        pthread_mutex_unlock(&client_data_mutex);
    }
    else
    {
        c->session_lost_handler(s);
        close(s->fd);
    }
    return rc;
}

static int
do_move_rpc(Proto_Client_Handle ch, Proto_Msg_Types mt, char data)
{
    int rc, temp_version;
    Proto_Session *s;
    Proto_Client *c = ch;
    Proto_Msg_Hdr h;

    // prepare message: pID, move_command
    s = &(c->rpc_session);
    bzero(&h, sizeof(h));
    h.type = mt;
    proto_session_hdr_marshall(s, &h);

    if (proto_debug())
        fprintf(stderr, "do_move_rpc:\n   pId: %d\n   move: #%c#\n", playerdata.id, data);

    if (proto_session_body_marshall_int(s, playerdata.id) < 0)
        fprintf(stderr, "do_move_rpc: proto_session_body_marshall_int failed. "
                "Not enough available sbufer space\n");
    if (proto_session_body_marshall_char(s, data) < 0)
        fprintf(stderr, "do_move_rpc: proto_session_body_marshall_char failed. "
                "Not enough available sbufer space\n");
    rc = proto_session_rpc(s);
 
    // process reply
    if (rc == 1)
    {
        proto_session_hdr_unmarshall(s, &h);
        // keep client synchronized
        /*while (1)
        {
            pthread_mutex_lock(&client_data_mutex);
            temp_version = gamedata.game_version
            pthread_mutex_unlock(&client_data_mutex);
            if ( h.sver.raw <= gamedata.game_version )
               break;
        }*/

        if (proto_session_body_unmarshall_int(s, 0, &rc) < 0)
            fprintf(stderr, "do_move_rpc: proto_session_body_unmarshall_int failed\n");
        if (proto_debug())
            fprintf(stderr, "do_move_rpc: unmarshalled response rc = %d, game version = %llu, game state = %d \n",
                             rc, h.sver.raw, h.gstate.v0.raw);
    }
    else
    {
        c->session_lost_handler(s);
        close(s->fd);
    }
    return rc;
}

static int
do_item_action_rpc(Proto_Client_Handle ch, Proto_Msg_Types mt, char item, char action)
{
    int rc;
    Proto_Session *s;
    Proto_Client *c = ch;
    Proto_Msg_Hdr h;

    // prepare message
    s = &(c->rpc_session);
    bzero(&h, sizeof(h));
    h.type = mt;
    proto_session_hdr_marshall(s, &h);

    if (proto_debug())
        fprintf(stderr, "do_item_action: pId: %d, item: #%c#, action = #%c#\n", playerdata.id, item, action);

    if (proto_session_body_marshall_int(s, playerdata.id) < 0)
        fprintf(stderr, "do_item_action_rpc: proto_session_body_marshall_int failed. "
                "Not enough available sbufer space\n");
    if (proto_session_body_marshall_char(s, item) < 0)
        fprintf(stderr, "do_item_action: proto_session_body_marshall_char failed. "
                "Not enough available sbufer space\n");
    if (proto_session_body_marshall_char(s, action) < 0)
        fprintf(stderr, "do_item_action: proto_session_body_marshall_char failed. "
                "Not enough available sbufer space\n");
    rc = proto_session_rpc(s);
 
    // process response
    if (rc == 1)
    {
        proto_session_hdr_unmarshall(s, &h);
        // keep client synchronized
        /*while (1)
        {
            pthread_mutex_lock(&client_data_mutex);
            temp_version = gamedata.game_version
            pthread_mutex_unlock(&client_data_mutex);
            if ( h.sver.raw <= gamedata.game_version )
               break;
        }*/
        if (proto_session_body_unmarshall_int(s, 0, &rc) < 0)
            fprintf(stderr, "do_item_action: proto_session_body_unmarshall_int failed\n");
        if (proto_debug())
            fprintf(stderr, "do_item_action: unmarshalled response rc = %d, game version = %llu, game state = %d \n",
                             rc, h.sver.raw, h.gstate.v0.raw);
    }
    else
    {
        c->session_lost_handler(s);
        close(s->fd);
    }
    return rc;
}

static int
do_leave_game_rpc(Proto_Client_Handle ch, Proto_Msg_Types mt)
{
    int rc;
    Proto_Session *s;
    Proto_Client *c = ch;
    Proto_Msg_Hdr h;

    if (proto_debug())
       fprintf(stderr, "do_leave_rpc\n");

    s = &(c->rpc_session);
    bzero(&h, sizeof(h));

    marshall_mtonly(s, mt);
    rc = proto_session_rpc(s);

    if (rc == 1)
    {
        proto_session_hdr_unmarshall(s, &h);
        // keep client synchronized
        /*while (1)
        {
            pthread_mutex_lock(&client_data_mutex);
            temp_version = gamedata.game_version
            pthread_mutex_unlock(&client_data_mutex);
            if ( h.sver.raw <= gamedata.game_version )
               break;
        }*/

        if (proto_session_body_unmarshall_int(s, 0, &rc) < 0)
            fprintf(stderr, "do_leave_game: proto_session_body_unmarshall_int failed\n");
        if (proto_debug())
            fprintf(stderr, "do_leave_game: unmarshalled response rc = %d, game version = %llu, game state = %d \n",
                            rc, h.sver.raw, h.gstate.v0.raw);
    }
    else
    {
        c->session_lost_handler(s);
        close(s->fd);
    }

    return rc;
}

// Assigment 3 rpc
static int
do_numhome_rpc(Proto_Client_Handle ch, Proto_Msg_Types mt, int data)
{
    int rc;
    Proto_Session *s;
    Proto_Client *c = ch;
    Proto_Msg_Hdr h;

    if (proto_debug())
       fprintf(stderr, "do_numhome_rpc (team = %d)\n", data);

    s = &(c->rpc_session);
    bzero(&h, sizeof(h));
    h.type = mt;
    proto_session_hdr_marshall(s, &h);

    if (proto_session_body_marshall_int(s, data) < 0)
        fprintf(stderr,
                "get_numHome_rpc: proto_session_body_marshall_char failed. "
                "Not enough available sbufer space\n");

    rc = proto_session_rpc(s);
    if (rc == 1) {
        if(proto_session_body_unmarshall_int(s, 0, &rc) < 0)
            fprintf(stderr, "do_numhome_rpc: proto_session_body_unmarshall_bytes failed\n");
        if (proto_debug())
            fprintf(stderr, "do_numhome_rpc: unmarshalled response rc = %d \n", rc);
    }
    else {
        c->session_lost_handler(s);
        close(s->fd);
    }
    return rc;
}

static int
do_numfloor_rpc(Proto_Client_Handle ch, Proto_Msg_Types mt)
{
    int rc;
    Proto_Session *s;
    Proto_Client *c = ch;
    Proto_Msg_Hdr h;

    if (proto_debug())
       fprintf(stderr, "do_numfloor_rpc: invoked for session \n");

    s = &(c->rpc_session);
    bzero(&h, sizeof(h));
    h.type = mt;
    proto_session_hdr_marshall(s, &h);

    rc = proto_session_rpc(s);
    if (rc == 1) {
        if(proto_session_body_unmarshall_int(s, 0, &rc) < 0)
            fprintf(stderr, "do_numfloor_rpc: proto_session_body_unmarshall_bytes failed\n");
        if (proto_debug())
           fprintf(stderr, "do_numfloor_rpc: unmarshalled response rc = %d \n", rc);
    }
    else {
        c->session_lost_handler(s);
        close(s->fd);
    }
    return rc;
}

static int
do_numjail_rpc(Proto_Client_Handle ch, Proto_Msg_Types mt, int data)
{
    int rc;
    Proto_Session *s;
    Proto_Client *c = ch;
    Proto_Msg_Hdr h;

    if (proto_debug())
       fprintf(stderr, "do_numjail_rpc\n");

    s = &(c->rpc_session);
    bzero(&h, sizeof(h));
    h.type = mt;
    proto_session_hdr_marshall(s, &h);

    if (proto_session_body_marshall_int(s, data) < 0)
        fprintf(stderr,
                "get_numHome_rpc: proto_session_body_marshall_char failed. "
                "Not enough available sbufer space\n");

    rc = proto_session_rpc(s);
    if (rc == 1) {
        if(proto_session_body_unmarshall_int(s, 0, &rc) < 0)
            fprintf(stderr, "do_numjail_rpc: proto_session_body_unmarshall_bytes failed\n");
        if (proto_debug())
            fprintf(stderr, "do_numjail_rpc: unmarshalled response rc = %d \n", rc);
    }
    else {
        c->session_lost_handler(s);
        close(s->fd);
    }
    return rc;
}

static int
do_numwall_rpc(Proto_Client_Handle ch, Proto_Msg_Types mt)
{
    int rc;
    Proto_Session *s;
    Proto_Client *c = ch;
    Proto_Msg_Hdr h;

    if (proto_debug())
       fprintf(stderr, "do_numwall_rpc\n");

    s = &(c->rpc_session);
    bzero(&h, sizeof(h));
    h.type = mt;
    proto_session_hdr_marshall(s, &h);

    rc = proto_session_rpc(s);
    if (rc == 1) {
        if(proto_session_body_unmarshall_int(s, 0, &rc) < 0)
           fprintf(stderr, "get_numwall_rpc: proto_session_body_unmarshall_bytes failed\n");
        if (proto_debug())
           fprintf(stderr, "do_numwall_rpc: unmarshalled response rc = %d \n", rc);
    }
    else {
        c->session_lost_handler(s);
        close(s->fd);
    }
    return rc;
}

static int
do_dim_rpc(Proto_Client_Handle ch, Proto_Msg_Types mt)
{
    int rc, x, y;
    Proto_Session *s;
    Proto_Client *c = ch;
    Proto_Msg_Hdr h;

    if (proto_debug())
        fprintf(stderr, "do_dim_rpc\n");

    s = &(c->rpc_session);
    bzero(&h, sizeof(h));
    h.type = mt;
    proto_session_hdr_marshall(s, &h);

    rc = proto_session_rpc(s);
    if (rc == 1) {
        if (proto_session_body_unmarshall_int(s, 0, &x) < 0)
            fprintf(stderr, "do_dim_rpc: proto_session_body_unmarshall_bytes failed\n");
        if (proto_session_body_unmarshall_int(s, sizeof(int), &y) < 0)
            fprintf(stderr, "do_dim_rpc: proto_session_body_unmarshall_bytes failed\n");

        if (proto_debug())
            fprintf(stderr, "do_dim_rpc: unmarshalled response x = %X \n", x);
        if (proto_debug())
            fprintf(stderr, "do_dim_rpc: unmarshalled response y = %X \n", y);
  
       rc = (x << 16) | y;

       if (proto_debug())
          fprintf(stderr, "do_dim_rpc: return value = %X \n", rc);
    }
    else {
        c->session_lost_handler(s);
        close(s->fd);
    }
    return rc;
}

static int
do_cinfo_rpc(Proto_Client_Handle ch, Proto_Msg_Types mt, int x, int y)
{
    int rc;
    char cell, team, occupied;
    Proto_Session *s;
    Proto_Client *c = ch;
    Proto_Msg_Hdr h;

    s = &(c->rpc_session);
    bzero(&h, sizeof(h));
    h.type = mt;
    proto_session_hdr_marshall(s, &h);

    if (proto_debug())
       fprintf(stderr, "do_cinfo_rpc: cinfo(%d, %d) \n", x, y);

    if (proto_session_body_marshall_int(s, x) < 0)
        fprintf(stderr,
                "do_cinfo_rpc: proto_session_body_marshall_char failed. "
                "Not enough available sbufer space\n");

    if (proto_session_body_marshall_int(s, y) < 0)
        fprintf(stderr,
                "do_cinfo_rpc: proto_session_body_marshall_char failed. "
                "Not enough available sbufer space\n");

    rc = proto_session_rpc(s);
    if (rc == 1) {
        // unmarshall
        if (proto_session_body_unmarshall_char(s, 0, &cell) < 0)
             fprintf(stderr, "do_cinfo_rpc: proto_session_body_unmarshall_bytes failed\n");
        if (proto_session_body_unmarshall_char(s, 1, &team) < 0)
             fprintf(stderr, "do_cinfo_rpc: proto_session_body_unmarshall_bytes failed\n");
        if (proto_session_body_unmarshall_char(s, 2, &occupied) < 0)
             fprintf(stderr, "do_cinfo_rpc: proto_session_body_unmarshall_bytes failed\n");

        if (proto_debug())
             fprintf(stderr, "do_cinfo_rpc: unmarshalled response cell = %X, %c \n", cell & 0xFF, cell);
        if (proto_debug())
             fprintf(stderr, "do_cinfo_rpc: unmarshalled response team = %X, %c \n", team & 0xFF, team);
        if (proto_debug())
             fprintf(stderr, "do_cinfo_rpc: unmarshalled response occupied = %X, %c \n", occupied & 0xFF, occupied);

        rc = 0;
        rc =((cell & 0xFF)<<8) | ((team & 0xFF)<<16) | ((occupied & 0xFF)<<24);

        if (proto_debug())
           fprintf(stderr, "do_cinfo_rpc: return value = %X \n", rc);
    }
    else {
        c->session_lost_handler(s);
        close(s->fd);
    }
    return rc;
}

static int
do_dump_rpc(Proto_Client_Handle ch, Proto_Msg_Types mt)
{
    int rc;
    Proto_Session *s;
    Proto_Client *c = ch;

    if (proto_debug())
       fprintf(stderr, "do_dump_rpc \n");

    s = &(c->rpc_session);
    marshall_mtonly(s, mt);
    rc = proto_session_rpc(s);

    if (rc == 1)
    {
        proto_session_body_unmarshall_int(s, 0, &rc);
    }
    else
    {
        c->session_lost_handler(s);
        close(s->fd);
    }
    return rc;
}

extern int
proto_client_hello(Proto_Client_Handle ch)
{
    //return do_generic_dummy_rpc(ch,PROTO_MT_REQ_BASE_HELLO);
    return do_join_game_rpc(ch, PROTO_MT_REQ_BASE_HELLO);
}

extern int
proto_client_move(Proto_Client_Handle ch, char data)
{
    //return do_generic_dummy_rpc(ch,PROTO_MT_REQ_BASE_MOVE);
    return do_move_rpc(ch, PROTO_MT_REQ_BASE_MOVE, data);
}

extern int
proto_client_goodbye(Proto_Client_Handle ch)
{
    //return do_generic_dummy_rpc(ch,PROTO_MT_REQ_BASE_GOODBYE);
    return do_leave_game_rpc(ch, PROTO_MT_REQ_BASE_GOODBYE);
}

// Assignment 3
extern int
proto_client_numhome(Proto_Client_Handle ch, int teamNo)
{
    return do_numhome_rpc(ch, PROTO_MT_REQ_NUM_HOME, teamNo);
}

extern int
proto_client_numfloor(Proto_Client_Handle ch)
{
    return do_numfloor_rpc(ch, PROTO_MT_REQ_NUM_FLOOR);
}

extern int
proto_client_numjail(Proto_Client_Handle ch, int teamNo)
{
    return do_numjail_rpc(ch, PROTO_MT_REQ_NUM_JAIL, teamNo);
}

extern int
proto_client_numwall(Proto_Client_Handle ch)
{
    return do_numwall_rpc(ch, PROTO_MT_REQ_NUM_WALL);
}

extern int
proto_client_dim(Proto_Client_Handle ch)
{
    return do_dim_rpc(ch, PROTO_MT_REQ_MAP_DIM);
}

extern int
proto_client_cinfo(Proto_Client_Handle ch, int x, int y)
{
    return do_cinfo_rpc(ch, PROTO_MT_REQ_CELL_INFO, x, y);
}

extern int
proto_client_dump(Proto_Client_Handle ch)
{
    return do_dump_rpc(ch, PROTO_MT_REQ_MAP_DUMP);
}
extern int
proto_item_action(Proto_Client_Handle ch, char item, char action)
{
    return do_item_action_rpc(ch, PROTO_MT_REQ_ITEM_ACTION, item, action);
}



