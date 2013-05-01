#ifndef __DAGAME_PROTOCOL_CLIENT_H__
#define __DAGAME_PROTOCOL_CLIENT_H__

//#include "net.h"
#include "protocol.h"
#include "protocol_session.h"
#include "protocol_utils.h"

extern int
proto_client_event_msg_unmarshall_v1( Proto_Session *s, int blen, unsigned long long ver, int special );

extern int
proto_server_test_msg(Proto_Session *s );
#endif




