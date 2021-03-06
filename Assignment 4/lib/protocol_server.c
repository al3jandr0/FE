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

#include "net.h"
#include "protocol.h"
#include "protocol_utils.h"
#include "protocol_server.h"
#include "server_types.h"
#include "protocol_event_msg.h"
#include "general_utils.h"
#include "./../lib/maze.h"

#define PROTO_SERVER_MAX_EVENT_SUBSCRIBERS 1024

Server_GameData server_gameData;
pthread_mutex_t server_data_mutex;
pthread_spinlock_t spinlock;

static
char *gameReplyMsg[] = { "Not your turn yet!\n",  // 0
                         "Not a valid move!\n",   // 1
                         NULL,                    // 2
                         NULL,                    // 3
                         NULL,                    // 4
                         NULL
                       };                  // 5

pthread_mutex_t game_mutex;
pthread_mutex_t gameMapVersion_mutex;
Proto_StateVersion gameMapVersion;

struct
{
    FDType   RPCListenFD;
    PortType RPCPort;


    FDType             EventListenFD;
    PortType           EventPort;
    pthread_t          EventListenTid;
    pthread_mutex_t    EventSubscribersLock;
    int                EventLastSubscriber;
    int                EventNumSubscribers;
    FDType             EventSubscribers[PROTO_SERVER_MAX_EVENT_SUBSCRIBERS];
    Proto_Session      EventSession;
    pthread_t          RPCListenTid;
    Proto_MT_Handler   session_lost_handler;
    Proto_MT_Handler   base_req_handlers[PROTO_MT_REQ_BASE_RESERVED_LAST -
                                         PROTO_MT_REQ_BASE_RESERVED_FIRST - 1];
} Proto_Server;

extern PortType proto_server_rpcport(void)
{
    return Proto_Server.RPCPort;
}
extern PortType proto_server_eventport(void)
{
    return Proto_Server.EventPort;
}
extern Proto_Session *
proto_server_event_session(void)
{
    return &Proto_Server.EventSession;
}

extern int
proto_server_set_session_lost_handler(Proto_MT_Handler h)
{
    Proto_Server.session_lost_handler = h;
}

extern int
proto_server_set_req_handler(Proto_Msg_Types mt, Proto_MT_Handler h)
{
    int i;

    if (mt > PROTO_MT_REQ_BASE_RESERVED_FIRST &&
            mt < PROTO_MT_REQ_BASE_RESERVED_LAST)
    {
        i = mt - PROTO_MT_REQ_BASE_RESERVED_FIRST - 1;
        Proto_Server.base_req_handlers[i] = h;
        return 1;
    }
    else
    {
        return -1;
    }
}


static int
proto_server_record_event_subscriber(int fd, int *num)
{
    int rc = -1;

    pthread_mutex_lock(&Proto_Server.EventSubscribersLock);

    if (Proto_Server.EventLastSubscriber < PROTO_SERVER_MAX_EVENT_SUBSCRIBERS
            && Proto_Server.EventSubscribers[Proto_Server.EventLastSubscriber]
            == -1)
    {
        Proto_Server.EventSubscribers[Proto_Server.EventLastSubscriber] = fd;
        *num = Proto_Server.EventLastSubscriber;
        Proto_Server.EventLastSubscriber++;
        Proto_Server.EventNumSubscribers++;
        rc = 1;
    }
    else
    {
        int i;
        for (i = 0; i < PROTO_SERVER_MAX_EVENT_SUBSCRIBERS; i++)
        {
            if (Proto_Server.EventSubscribers[i] == -1)
            {
                Proto_Server.EventSubscribers[i] = fd;
                Proto_Server.EventNumSubscribers++;
                *num = i;
                rc = 1;
            }
        }
    }

    pthread_mutex_unlock(&Proto_Server.EventSubscribersLock);

    return rc;
}

static
void *
proto_server_event_listen(void *arg)
{
    int fd = Proto_Server.EventListenFD;
    int connfd;

    if (net_listen(fd) < 0)
    {
        exit(-1);
    }

    for (;;)
    {
        connfd = net_accept(fd);
        if (connfd < 0)
        {
            fprintf(stderr, "Error: EventListen accept failed (%d)\n", errno);
        }
        else
        {
            int i;
            fprintf(stderr, "EventListen: connfd=%d -> ", connfd);

            if (proto_server_record_event_subscriber(connfd, &i) < 0)
            {
                fprintf(stderr, "oops no space for any more event subscribers\n");
                close(connfd);
            }
            else
            {
                fprintf(stderr, "subscriber num %d\n", i);
            }
        }
    }
}

void
proto_server_post_event(void)
{
    int i;
    int num;

    pthread_mutex_lock(&Proto_Server.EventSubscribersLock);

    i = 0;
    num = Proto_Server.EventNumSubscribers;
    while (num > 0)
    {
        Proto_Server.EventSession.fd = Proto_Server.EventSubscribers[i];
        if (Proto_Server.EventSession.fd != -1)
        {
            num--;
            if (proto_session_send_msg(&Proto_Server.EventSession, 0) < 0)
            {
                // must have lost an event connection
                close(Proto_Server.EventSession.fd);
                Proto_Server.EventSubscribers[i] = -1;
                Proto_Server.EventNumSubscribers--;
                Proto_Server.session_lost_handler(&Proto_Server.EventSession);
            }
            // FIXME: add ack message here to ensure that game is updated
            // correctly everywhere... at the risk of making server dependent
            // on client behaviour  (use time out to limit impact... drop
            // clients that misbehave but be carefull of introducing deadlocks
        }
        i++;
    }
    proto_session_reset_send(&Proto_Server.EventSession);
    pthread_mutex_unlock(&Proto_Server.EventSubscribersLock);
}

// Broadcasts changes in the game to all the Subscribers
int
doUpdateClientsGame(Deltas *d, Server_GameData* gameinfo)
{
    Proto_Session *s;
    Proto_Msg_Hdr hdr;
    int X, Y;
    char* fmaze;
    char test[6] = {'1','2','3','4','5','6'};

    if (proto_debug())
        fprintf(stderr, "doUpdateClientsGame called\n"); 

    bzero(&hdr, sizeof(hdr));
    s = proto_server_event_session();
    // prepare header
    hdr.type = PROTO_MT_EVENT_BASE_UPDATE;
    hdr.sver.raw = gameinfo->version;
    hdr.gstate.v0.raw = gameinfo->state;
    hdr.gstate.v1.raw = gameinfo->trs;
    proto_session_hdr_marshall(s, &hdr);

    // prepare body. TEST
    proto_server_marshall_deltas(s, d); // proto_server_test_msg(s);
    // send update to all subscribers
    proto_server_post_event();

    // save game map to log
    // fmaze = formatmaze();
    // X = dimx;
    // Y = dimy;
    logMaze( &test[0], 6, gameinfo->version ); 
    // free(fmaze); 
    return 1;
}

static void *
proto_server_req_dispatcher(void *arg)
{
    Proto_Session s;
    Proto_Msg_Types mt;
    Proto_MT_Handler hdlr;
    int i;
    unsigned long arg_value = (unsigned long) arg;

    pthread_detach(pthread_self());

    proto_session_init(&s);

    s.fd = (FDType) arg_value;

    fprintf(stderr, "proto_rpc_dispatcher: %p: Started: fd=%d\n",
            pthread_self(), s.fd);

    for (;;)
    {
        if (proto_session_rcv_msg(&s) == 1)
        {
            mt = proto_session_hdr_unmarshall_type(&s);
            if (mt > PROTO_MT_REQ_BASE_RESERVED_FIRST &&
                    mt < PROTO_MT_REQ_BASE_RESERVED_LAST)
            {
                i = mt - PROTO_MT_REQ_BASE_RESERVED_FIRST - 1;
                hdlr = Proto_Server.base_req_handlers[i];
                if (hdlr(&s) < 0) goto leave;
            }
        }
        else
        {
            goto leave;
        }
    }
leave:
    Proto_Server.session_lost_handler(&s);
    close(s.fd);
    return NULL;
}

static
void *
proto_server_rpc_listen(void *arg)
{
    int fd = Proto_Server.RPCListenFD;
    unsigned long connfd;
    pthread_t tid;

    if (net_listen(fd) < 0)
    {
        fprintf(stderr, "Error: proto_server_rpc_listen listen failed (%d)\n", errno);
        exit(-1);
    }

    for (;;)
    {
        connfd = net_accept(fd);
        if (connfd < 0)
        {
            fprintf(stderr, "Error: proto_server_rpc_listen accept failed (%d)\n", errno);
        }
        else
        {
            pthread_create(&tid, NULL, &proto_server_req_dispatcher,
                           (void *)connfd);
        }
    }
}

extern int
proto_server_start_rpc_loop(void)
{
    if (pthread_create(&(Proto_Server.RPCListenTid), NULL,
                       &proto_server_rpc_listen, NULL) != 0)
    {
        fprintf(stderr,
                "proto_server_rpc_listen: pthread_create: create RPCListen thread failed\n");
        perror("pthread_create:");
        return -3;
    }
    return 1;
}

static int
proto_session_lost_default_handler(Proto_Session *s)
{
    fprintf(stderr, "Session lost...:\n");
    proto_session_dump(s);
    return -1;
}

static int
proto_server_mt_null_handler(Proto_Session *s)
{
    int rc = 1;
    Proto_Msg_Hdr h;

    fprintf(stderr, "proto_server_mt_null_handler: invoked for session:\n");
    proto_session_dump(s);

    // setup dummy reply header : set correct reply message type and
    // everything else empty
    bzero(&h, sizeof(h));
    h.type = proto_session_hdr_unmarshall_type(s);
    h.type += PROTO_MT_REP_BASE_RESERVED_FIRST;
    proto_session_hdr_marshall(s, &h);

    // setup a dummy body that just has a return code
    proto_session_body_marshall_int(s, 0xdeadbeef);

    rc = proto_session_send_msg(s, 1);

    return rc;
}

static int
proto_server_mt_join_game_handler(Proto_Session *s)
{
    int rc, player, dimy, dimx, msg_playerId; 
    Proto_Msg_Hdr h;
    Deltas *d;
    Server_GameData gameinfo;

    player = 1;
    char dummy_maze[] = {'1','2','3','4','5','6','7','8','9'};

    if (proto_debug())
       fprintf(stderr, "proto_server_mt_join_game_handler: invoked for session:\n");

    // read incoming message
    proto_session_body_unmarshall_int(s, 0, &msg_playerId);
    if (proto_debug())
        fprintf(stderr, "proto_server_mt_join_ganme_handler: Recieved:\n"
                        "    pId: %d\n", msg_playerId);

    bzero(&h, sizeof(s));
    h.type = proto_session_hdr_unmarshall_type(s);
    h.type += PROTO_MT_REP_BASE_RESERVED_FIRST;
    d = (Deltas*) malloc( sizeof(Deltas) );
    init_deltas( d );

    pthread_mutex_lock(&server_data_mutex);

    proto_server_test_deltas(d); // fills deltas with harcoded data for testing
    dimx = 3; // dimx = get_maze_dimx();
    dimy = 3; // dimy = get_maze_dimy();
    // rc = add_player(d);
    // fprintf(stderr, "proto_server_mt_join_game_handler: game logic returned id = %d\n", rc);
    // if (rc > 0)
    // {
       server_gameData.version++;
       h.gstate.v0.raw = gameinfo.state = server_gameData.state = 1; // temporary test value
       // call fimy and dimx
       // get the maze
    //   h.gstate.v0.raw = gameinfo.state = server_gameData.state = gamestate();
    // }
    h.sver.raw = gameinfo.version = server_gameData.version;
    h.gstate.v1.raw = gameinfo.trs = ++server_gameData.trs;
    proto_session_hdr_marshall(s, &h);

    // if (rc > 0)
    doUpdateClientsGame(d, &gameinfo);
    clean_deltas(d);
    free(d);

    pthread_mutex_unlock(&server_data_mutex);

    // prepare relpy body: pID, xdim, ydim, maze
    if (proto_session_body_marshall_int(s, player) < 0) // rc instead of player
        fprintf(stderr, "proto_server_mt_join_game_handler: "
                "proto_session_body_marshall_int failed\n");
    if (proto_session_body_marshall_int(s, dimx) < 0 )
        fprintf(stderr, "proto_server_mt_join_game_handler: "
                "proto_session_body_marshall_int failed\n");
    if (proto_session_body_marshall_int(s, dimy) < 0 )
        fprintf(stderr, "proto_server_mt_join_game_handler: "
                "proto_session_body_marshall_int failed\n");
    if (proto_session_body_marshall_bytes(s, dimx*dimy, &dummy_maze[0]) < 0)
        fprintf(stderr, "proto_server_mt_join_game_handler: "
                "proto_session_body_marshall_bytes failed\n");

    return proto_session_send_msg(s,1);
}

static int
proto_server_mt_move_handler(Proto_Session *s)
{
    int rc, player_id;
    char move;
    int dummy_reply = 1;
    Proto_Msg_Hdr h;
    Deltas *d;
    Server_GameData gameinfo;

    if (proto_debug())
       fprintf(stderr, "proto_server_mt_move_handler: invoked for session:\n");

    // Read rpc message: pID, direction
    proto_session_body_unmarshall_int(s, 0, &player_id);
    proto_session_body_unmarshall_char(s, sizeof(int), &move);
    if (proto_debug())
        fprintf(stderr, "proto_server_mt_move_handler: Recieved:\n"
                        "    pId: %d\n    move #%c#\n", player_id, move);

    bzero(&h, sizeof(h));
    h.type = proto_session_hdr_unmarshall_type(s);
    h.type += PROTO_MT_REP_BASE_RESERVED_FIRST;
    d = (Deltas*) malloc( sizeof(Deltas) );
    init_deltas( d );

    pthread_mutex_lock(&server_data_mutex);

    // rc = moveLogic();
    if (proto_debug())
        fprintf(stderr, "proto_server_mt_move_handler: Send: game logic_reply: %d\n", dummy_reply);

    // if (rc > 0)
    // {
       server_gameData.version++;
       h.gstate.v0.raw = gameinfo.state = server_gameData.state = 1; // temporary test value
       // h.gstate.v0.raw = gameinfo.state = server_gameData.state = gamestate();
    // }
    h.sver.raw = gameinfo.version = server_gameData.version;
    h.gstate.v1.raw = gameinfo.trs = ++server_gameData.trs;
    proto_session_hdr_marshall(s, &h);
    proto_session_body_marshall_int(s, dummy_reply); // rc

    // if (rc > 0)
    // doUpdateClientsGame(d, &gameinfo);
    clean_deltas(d);
    free(d);

    pthread_mutex_unlock(&server_data_mutex);

    return proto_session_send_msg(s, 1);
}

static int
proto_server_mt_item_action_handler(Proto_Session *s)
{
    int rc, player_id;
    char item, action;
    int dummy_reply = 1;
    Proto_Msg_Hdr h;
    Deltas *d;
    Server_GameData gameinfo;

    if (proto_debug())
       fprintf(stderr, "proto_server_mt_item_action_handler: invoked for session:\n");

    // Read rpc message: pID, item, action 
    proto_session_body_unmarshall_int(s, 0, &player_id);
    proto_session_body_unmarshall_char(s, sizeof(int), &item);
    proto_session_body_unmarshall_char(s, sizeof(int)+sizeof(char), &action);
    if (proto_debug())
        fprintf(stderr, "proto_server_mt_move_handler: Recieved:\n"
                        "    pId: %d\n    item #%c#\n    action #%c#\n", player_id, item, action);

    bzero(&h, sizeof(h));
    h.type = proto_session_hdr_unmarshall_type(s);
    h.type += PROTO_MT_REP_BASE_RESERVED_FIRST;
    d = (Deltas*) malloc( sizeof(Deltas) );
    init_deltas( d );

    pthread_mutex_lock(&server_data_mutex);

    // rc = gamelogic move();
    if (proto_debug())
        fprintf(stderr, "proto_server_mt_move_handler: Send: game logic_reply: %d\n", dummy_reply);

    // if (rc > 0)
    // {
       server_gameData.version++;
       h.gstate.v0.raw = gameinfo.state = server_gameData.state = 1; // temporary test value
       // h.gstate.v0.raw = gameinfo.state = server_gameData.state = gamestate();
    // }
    h.sver.raw = gameinfo.version = server_gameData.version;
    h.gstate.v1.raw = gameinfo.trs = ++server_gameData.trs;
    proto_session_hdr_marshall(s, &h);
    proto_session_body_marshall_int(s, dummy_reply); // rc

    // if (rc > 0)
    // doUpdateClientsGame(d, &gameinfo);
    clean_deltas(d);
    free(d);

    pthread_mutex_unlock(&server_data_mutex);

    return proto_session_send_msg(s, 1);
}

static int
proto_server_mt_leave_game_handler(Proto_Session *s)
{
    Proto_Msg_Hdr h;
    int rc, msg_playerId;
    int dummy_reply = 1;
    Deltas *d;
    Server_GameData gameinfo;

    if (proto_debug())
        fprintf(stderr, "proto_server_mt_leave_game_handler: invoked for session:\n");

    proto_session_body_unmarshall_int(s, 0, &msg_playerId);
    if (proto_debug())
        fprintf(stderr, "proto_server_mt_leave_ganme_handler: Recieved:\n"
                        "    pId: %d\n", msg_playerId);

    bzero(&h, sizeof(h));
    h.type = proto_session_hdr_unmarshall_type(s);
    h.type += PROTO_MT_REP_BASE_RESERVED_FIRST;
    d = (Deltas*) malloc( sizeof(Deltas) );
    init_deltas( d );

    pthread_mutex_lock(&server_data_mutex);

    // rc = gamelogic remove player
    if (proto_debug())
        fprintf(stderr, "proto_server_mt_leave_game_handler: remove player reply:%d\n", 1);

    // if (rc > 0)
    // {
       server_gameData.version++;
       h.gstate.v0.raw = gameinfo.state = server_gameData.state = 1; // temporary test value
       // h.gstate.v0.raw = gameinfo.state = server_gameData.state = gamestate();
    // }
    h.sver.raw = gameinfo.version = server_gameData.version;
    h.gstate.v1.raw = gameinfo.trs = ++server_gameData.trs;
    proto_session_hdr_marshall(s, &h);
    proto_session_body_marshall_int(s, dummy_reply); // rc

    // if (rc > 0)
    // doUpdateClientsGame(d, &gameinfo);
    clean_deltas(d);
    free(d);

    pthread_mutex_unlock(&server_data_mutex);

    return proto_session_send_msg(s, 1);
}

static int proto_server_mt_numhome_handler(Proto_Session *s)
{
    int numhome, rc, teamNo;
    Proto_Msg_Hdr h;

    if (proto_debug()) {
        fprintf(stderr, "proto_server_mt_move_handler: invoked for session:\n");
        //proto_session_dump(s);
    }

    proto_session_body_unmarshall_int(s, 0, &teamNo);

    if (proto_debug())
        fprintf(stderr, "proto_server_mt_numhome_handler: requested teamNo = %d\n", teamNo);

    numhome = findNumHome(teamNo); //numhome = 5; 
    //pthread_mutex_lock(&game_mutex); //pthread_mutex_unlock(&game_mutex);

    if (proto_debug())
        fprintf(stderr, "proto_server_mt_numhome_handler: maze.c func returned = %d\n", numhome); 

    // create replay message
    bzero(&h, sizeof(h));
    h.type = proto_session_hdr_unmarshall_type(s);
    h.type += PROTO_MT_REP_BASE_RESERVED_FIRST;
    proto_session_hdr_marshall(s, &h);
 
    if (proto_session_body_marshall_int(s, numhome) < 0 )
        fprintf(stderr, "proto_server_mt_numhome_handler: "
                "proto_session_body_marshall_bytes failed\n");

    rc = proto_session_send_msg(s, 1);
    // TODO: return failed if an error occours
    return rc;
}

static int proto_server_mt_numjail_handler(Proto_Session *s)
{
    int numjail, teamNo, rc;
    Proto_Msg_Hdr h;

    if (proto_debug()) {
        fprintf(stderr, "proto_server_mt_move_handler: invoked for session:\n");
        //proto_session_dump(s);
    }

    // read msg here
    proto_session_body_unmarshall_int(s, 0, &teamNo);

    if (proto_debug())
        fprintf(stderr, "proto_server_mt_numjail_handler: requested teamNo = %d\n", teamNo);

    numjail = findNumJail(teamNo);//numjail = 8; // call mze.c numjail(teamNo) func

    if (proto_debug())
        fprintf(stderr, "proto_server_mt_numjail_handler: maze.c::func returned = %d\n", numjail);

    // create replay message
    bzero(&h, sizeof(h));
    h.type = proto_session_hdr_unmarshall_type(s);
    h.type += PROTO_MT_REP_BASE_RESERVED_FIRST;
    proto_session_hdr_marshall(s, &h);
    
    if (proto_session_body_marshall_int(s, numjail) < 0 )
        fprintf(stderr, "proto_server_mt_numjail_handler: "
                "proto_session_body_marshall_bytes failed\n");

    rc = proto_session_send_msg(s, 1);
    // TODO: return failed if an error occours
    return rc;
}

static int proto_server_mt_numwall_handler(Proto_Session *s)
{
    int numwall, rc;
    Proto_Msg_Hdr h;

    if (proto_debug()) {
        fprintf(stderr, "proto_server_mt_numwall_handler: invoked for session:\n");
        //proto_session_dump(s);
    }

    numwall = 54321; // call mze.c numjail(teamNo) func

    if (proto_debug())
        fprintf(stderr, "proto_server_mt_numwall_handler: maze.c::func returned = %d\n", numwall);

    // create replay message
    bzero(&h, sizeof(h));
    h.type = proto_session_hdr_unmarshall_type(s);
    h.type += PROTO_MT_REP_BASE_RESERVED_FIRST;
    proto_session_hdr_marshall(s, &h);

    if (proto_session_body_marshall_int(s, numwall) < 0 )
        fprintf(stderr, "proto_server_mt_numwall_handler: "
                "proto_session_body_marshall_bytes failed\n");

    rc = proto_session_send_msg(s, 1);
    // TODO: return failed if an error occours
    return rc;
}

static int proto_server_mt_numfloor_handler(Proto_Session *s)
{
    int numfloor, rc;
    Proto_Msg_Hdr h;

    if (proto_debug()) {
        fprintf(stderr, "proto_server_mt_numfloor_handler: invoked for session:\n");
        //proto_session_dump(s);
    }

    numfloor = findNumFloor();//numfloor = 12345; // call mze.c numjail(teamNo) func

    if (proto_debug())
        fprintf(stderr, "proto_server_mt_numfloor_handler: maze.c::func returned = %d\n", numfloor);

    // create replay message
    bzero(&h, sizeof(h));
    h.type = proto_session_hdr_unmarshall_type(s);
    h.type += PROTO_MT_REP_BASE_RESERVED_FIRST;
    proto_session_hdr_marshall(s, &h);

    if (proto_session_body_marshall_int(s, numfloor) < 0 )
        fprintf(stderr, "proto_server_mt_numfloor_handler: "
                "proto_session_body_marshall_bytes failed\n");

    rc = proto_session_send_msg(s, 1);
    // TODO: return failed if an error occours
    return rc;
}

static int proto_server_mt_dim_handler(Proto_Session *s)
{
    int dimx, dimy, rc;
    Proto_Msg_Hdr h;

    if (proto_debug()) {
        fprintf(stderr, "proto_server_mt_dim_handler: invoked for session:\n");
        //proto_session_dump(s);
    }

    //dimx = 1;//dimx = 0x0000AAAA; // call mze.c numjail(teamNo) func
    //dimy = 2;//dimy = 0x0000BBBB; // call mze.c numjail(teamNo) func
    dimx = get_maze_dimx();
    dimy = get_maze_dimy();

    if (proto_debug())
        fprintf(stderr, "proto_server_mt_dim_handler: maze.c::func returned x=%d, y=%d\n", dimx, dimy);

    // create replay message
    bzero(&h, sizeof(h));
    h.type = proto_session_hdr_unmarshall_type(s);
    h.type += PROTO_MT_REP_BASE_RESERVED_FIRST;
    proto_session_hdr_marshall(s, &h);

    if (proto_session_body_marshall_int(s, dimx) < 0 )
        fprintf(stderr, "proto_server_mt_dim_handler: "
                "proto_session_body_marshall_bytes failed\n");

    if (proto_session_body_marshall_int(s, dimy) < 0 )
        fprintf(stderr, "proto_server_mt_dim_handler: "
                "proto_session_body_marshall_bytes failed\n");

    rc = proto_session_send_msg(s, 1);
    // TODO: return failed if an error occours
    return rc;
}

static int proto_server_mt_cinfo_handler(Proto_Session *s)
{
    int in_x, in_y, rc;
    char cell, team, occupied;
    Proto_Msg_Hdr h;

    if (proto_debug()) {
        fprintf(stderr, "proto_server_mt_cinfo_handler: invoked for session:\n");
        //proto_session_dump(s);
    }

    // read msg here
    proto_session_body_unmarshall_int(s, 0, &in_x);
    proto_session_body_unmarshall_int(s, sizeof(int), &in_y);

    if (proto_debug()) 
        fprintf(stderr, "proto_server_mt_numjail_handler: requested cellinfo( %d, %d )\n", in_x, in_y);
    
    //cell = '#';//cell = (char)0xCC; // call mze.c numjail(teamNo) func
    //team = '1';//team = (char)0xDD;
    //occupied = 'Y';//occupied = (char)0xEE;

    cell = get_cell_type( in_x, in_y );
    team = get_cell_team( in_x, in_y );
    occupied = is_cell_occupied( in_x, in_y );
    //findCInfo(in_x, in_y);

    if (proto_debug())
        fprintf(stderr, "proto_server_mt_numjail_handler: maze.c::func returned\n"
                "     cell='%c', team='%c', occupied='%c'\n", cell, team, occupied);

    // create replay message
    bzero(&h, sizeof(h));
    h.type = proto_session_hdr_unmarshall_type(s);
    h.type += PROTO_MT_REP_BASE_RESERVED_FIRST;
    proto_session_hdr_marshall(s, &h);

    if (proto_session_body_marshall_char(s, cell) < 0 )
        fprintf(stderr, "proto_server_mt_cinfo_handler: "
                "proto_session_body_marshall_bytes failed\n");

    if (proto_session_body_marshall_char(s, team) < 0 )
        fprintf(stderr, "proto_server_mt_cinfo_handler: "
                "proto_session_body_marshall_bytes failed\n");

    if (proto_session_body_marshall_char(s, occupied) < 0 )
        fprintf(stderr, "proto_server_mt_cinfo_handler: "
                "proto_session_body_marshall_bytes failed\n");

    rc = proto_session_send_msg(s, 1);
    // TODO: return failed if an error occours
    return rc;
}

static int proto_server_mt_dump_handler(Proto_Session *s)
{
    int rc;
    Proto_Msg_Hdr h;

    if (proto_debug()) {
        fprintf(stderr, "proto_server_mt_dump_handler: invoked for session:\n");
        //proto_session_dump(s);
    }

    dumpMap();

    // create replay message
    bzero(&h, sizeof(h));
    h.type = proto_session_hdr_unmarshall_type(s);
    h.type += PROTO_MT_REP_BASE_RESERVED_FIRST;
    proto_session_hdr_marshall(s, &h);

    rc = 1;
    if (proto_session_body_marshall_int(s, rc) < 0 )
        fprintf(stderr, "proto_server_mt_dump_handler: "
                "proto_session_body_marshall_bytes failed\n");

    rc = proto_session_send_msg(s, 1);
    // TODO: return failed if an error occours
    return rc;
}


extern int
proto_server_init(void)
{
    int i;
    int rc;

    proto_session_init(&Proto_Server.EventSession);

    proto_server_set_session_lost_handler(
        proto_session_lost_default_handler);
    //for (i=PROTO_MT_REQ_BASE_RESERVED_FIRST+1;
    //     i<PROTO_MT_REQ_BASE_RESERVED_LAST; i++) {
    //proto_server_set_req_handler(i, proto_server_mt_null_handler);
    //proto_server_set_req_handler(i, proto_server_mt_join_game_handler);
    //}
    // set_up_actual_game rpc handlers
    proto_server_set_req_handler( PROTO_MT_REQ_BASE_HELLO, proto_server_mt_join_game_handler);
    proto_server_set_req_handler( PROTO_MT_REQ_BASE_MOVE, proto_server_mt_move_handler);
    proto_server_set_req_handler( PROTO_MT_REQ_ITEM_ACTION, proto_server_mt_item_action_handler);
    proto_server_set_req_handler( PROTO_MT_REQ_BASE_GOODBYE, proto_server_mt_leave_game_handler);

    proto_server_set_req_handler( PROTO_MT_REQ_NUM_HOME , proto_server_mt_numhome_handler  );
    proto_server_set_req_handler( PROTO_MT_REQ_NUM_JAIL , proto_server_mt_numjail_handler  );
    proto_server_set_req_handler( PROTO_MT_REQ_NUM_WALL , proto_server_mt_numwall_handler  );
    proto_server_set_req_handler( PROTO_MT_REQ_NUM_FLOOR, proto_server_mt_numfloor_handler );
    proto_server_set_req_handler( PROTO_MT_REQ_MAP_DIM  , proto_server_mt_dim_handler      );
    proto_server_set_req_handler( PROTO_MT_REQ_CELL_INFO, proto_server_mt_cinfo_handler    );
    proto_server_set_req_handler( PROTO_MT_REQ_MAP_DUMP , proto_server_mt_dump_handler     );

    pthread_mutex_lock(&server_data_mutex);
    server_gameData.version = 0;
    server_gameData.state = -2;
    server_gameData.trs = 0;
    pthread_mutex_unlock(&server_data_mutex);

    for (i = 0; i < PROTO_SERVER_MAX_EVENT_SUBSCRIBERS; i++)
    {
        Proto_Server.EventSubscribers[i] = -1;
    }
    Proto_Server.EventNumSubscribers = 0;
    Proto_Server.EventLastSubscriber = 0;
    pthread_mutex_init(&Proto_Server.EventSubscribersLock, 0);


    rc = net_setup_listen_socket(&(Proto_Server.RPCListenFD),
                                 &(Proto_Server.RPCPort));

    if (rc == 0)
    {
        fprintf(stderr, "prot_server_init: net_setup_listen_socket: FAILED for RPCPort\n");
        return -1;
    }

    Proto_Server.EventPort = Proto_Server.RPCPort + 1;

    rc = net_setup_listen_socket(&(Proto_Server.EventListenFD),
                                 &(Proto_Server.EventPort));

    if (rc == 0)
    {
        fprintf(stderr, "proto_server_init: net_setup_listen_socket: FAILED for EventPort=%d\n",
                Proto_Server.EventPort);
        return -2;
    }

    if (pthread_create(&(Proto_Server.EventListenTid), NULL,
                       &proto_server_event_listen, NULL) != 0)
    {
        fprintf(stderr,
                "proto_server_init: pthread_create: create EventListen thread failed\n");
        perror("pthread_createt:");
        return -3;
    }

    return 0;
}

// For debuging event channel update Lists of deltas
void printlist_Player( LinkedList *il)
{
   int ii = 0;
   while ( il !=NULL )
   {
      Player *val = (Player*)il->value;
      printf("Player:\n  id = %d\n  pid = %d\n  team = %d\n  x = %d\n  "
             "y = %d\n  state = %c\n", val->ID, val->PID, val->team,
             val->PlayerPos.x, val->PlayerPos.y, val->State);
      il = il->next;
      ii++;
   }
}

void printlist_Cell( LinkedList *il)
{
   int ii = 0;
   while ( il !=NULL )
   {
      Cell *val = (Cell*)il->value;
      printf("Cell:\n  type = %c\n  team = %d\n  x = %d\n  y = %d\n",
             val->C_Type, val->Cell_Team, val->Cell_Pos.x, val->Cell_Pos.y);
      il = il->next;
      ii++;
   }
}

