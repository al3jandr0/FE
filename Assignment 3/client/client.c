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
#include <string.h>
#include "../lib/types.h"
#include "../lib/protocol_client.h"
#include "../lib/protocol_utils.h"

#define STRLEN 81

typedef enum
{
    FALSE = 0,
    TRUE = 1
} Bool;

struct Globals
{
    char host[STRLEN];
    PortType port;
} globals;


typedef struct ClientState
{
    int data;
    Proto_Client_Handle ph;
} Client;

static int
clientInit(Client *C)
{
    bzero(C, sizeof(Client));

    // initialize the client protocol subsystem
    if (proto_client_init(&(C->ph)) < 0)
    {
        fprintf(stderr, "client: main: ERROR initializing proto system\n");
        return -1;
    }
    return 1;
}


static int
update_event_handler(Proto_Session *s)
{
    Client *C = proto_session_get_data(s);

    fprintf(stderr, "%s: called", __func__);
    return 1;
}


int
startConnection(Client *C, char *host, PortType port, Proto_MT_Handler h)
{
    if (globals.host[0] != 0 && globals.port != 0)
    {
        if (proto_client_connect(C->ph, host, port) != 0)
        {
            fprintf(stderr, "failed to connect\n");
            return -1;
        }
        proto_session_set_data(proto_client_event_session(C->ph), C);
#if 0
        if (h != NULL)
        {
            proto_client_set_event_handler(C->ph, PROTO_MT_EVENT_BASE_UPDATE,
                                           h);
        }
#endif
        return 1;
    }
    return 0;
}


void quit() // write error message and quit
{
    fprintf(stderr, "Memory Exhausted\n");
    exit(1);
}

//returns the size of a character array using a pointer to the first element of the character array
int size(char *ptr)
{
    //variable used to access the subsequent array elements.
    int offset = 0;
    //variable that counts the number of elements in your array
    int count = 0;

    //While loop that tests whether the end of the array has been reached
    while (*(ptr + offset) != '\0')
    {
        //increment the count variable
        ++count;
        //advance to the next element of the array
        ++offset;
    }
    //return the size of the array
    return count;
}

char *specialPrompt(int menu)
{
    int max = 20;
    int c ;
    char *cmdInputs = (char *)malloc(max); // allocate buffer

    if (cmdInputs == 0) quit();

    if (menu)
        fprintf(stderr, "\ndaGame> ");

    fflush(stdout);

    while (true)   // skip leading whitespace
    {
        c = getchar();
        if (c == EOF) break; // end of file
        if (!isspace(c))
        {
            ungetc(c, stdin);
            break;
        }
    }

    int i = 0;
    while (1)
    {
        c = getchar();
        if (c == EOF) // at end, add terminating zero
        {
            cmdInputs[i] = 0;
            break;
        }
        cmdInputs[i] = c;
        if (i == max - 1) // buffer full
        {
            max = max + max;
            cmdInputs = (char *)realloc(cmdInputs, max); // get a new and larger buffer
            if (cmdInputs == 0) quit();
        }
        i++;
    }

    //printf("The command is %s\n", cmdInputs);

    char **tokens;

    tokens = str_split(cmdInputs, ' ');

    if (tokens)
    {
        if (connectFLAG == FALSE)
        {
            if (strcmp(*(tokens + 0), "connect") == 0)
            {
                connectFLAG = TRUE;
                initGlobal(atoi(*(tokens + 2)), *(tokens + 1))
                
                if (clientInit(&c) < 0)
                {
                    fprintf(stderr, "ERROR: clientInit failed\n");
                    return -1;
                }

                // ok startup our connection to the server
                if (startConnection(&c, globals.host, globals.port, update_event_handler) < 0)
                {
                    fprintf(stderr, "ERROR: startConnection failed\n");
                    return -1;
                }
            }
            else
            {
                fprintf(stderr, "%s\n", "Error using connect.");
            }
        }
    }

    int j;
    for (j = 0; * (tokens + j); j++)
    {
        free(*(tokens + j));
    }
    free(tokens);

    return cmdInputs;
}


char **str_split(char *a_str, const char a_delim)
{
    char **result    = 0;
    size_t count     = 0;
    char *tmp        = a_str;
    char *last_comma = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char *) * count);

    if (result)
    {
        size_t idx  = 0;
        char *token = strtok(a_str, " ");

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, " ");
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

void tokenize(char *s, char delim)
{
    char *olds = s;
    char olddelim = delim;
    while (olddelim && *s)
    {
        while (*s && (delim != *s)) s++;
        *s ^= olddelim = *s; // olddelim = *s; *s = 0;
        cb(olds);
        *s++ ^= olddelim; // *s = olddelim; s++;
        olds = s;
    }
}

int
prompt(int menu)
{
    static char MenuString[] = "\nclient> ";
    int ret;
    int c = 0;

    if (menu) printf("%s", MenuString);
    fflush(stdout);
    c = getchar();
    return c;
}


// FIXME:  this is ugly maybe the speration of the proto_client code and
//         the game code is dumb
int
game_process_reply(Client *C)
{
    Proto_Session *s;

    s = proto_client_rpc_session(C->ph);

    fprintf(stderr, "%s: called %p\n", __func__, s);

    return 1;
}


int doCMDS(Client *C, char *cmdInput)
{
    int rc = 1;

    char **tokens;

    tokens = str_split(cmdInputs, ' ');

    if (tokens)
    {
        if ( strcmp(*(tokens + 0), "connect") == 0 )
        {
            //rc = doRPCCmd(C, 'h');
            //return rc;
            rc = proto_client_hello(C->ph);
            printf("hello: rc=%x\n", rc);
            if (rc > 0) game_process_reply(C);
        }

        else if (strcmp(*(tokens + 0), "numhome") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;
            rc = proto_client_numhome(C->ph, *(tokens + 1));

        }

        else if (strcmp(*(tokens + 0), "numjail") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc
            rc = proto_client_numjail(C->ph, *(tokens + 1));
        }

        else if (strcmp(*(tokens + 0), "numwall") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;
            rc = proto_client_numwall(C->ph;
        }

                                  else if (strcmp(*(tokens + 0), "numﬂoor") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;
            rc = proto_client_numﬂoor(C->ph);
        }

        else if (strcmp(*(tokens + 0), "dim") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;
            rc = proto_client_dim(C->ph);
        }

        else if (strcmp(*(tokens + 0), "cinfo") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;
            rc = proto_client_cinfo(C->ph, *(tokens + 1), *(tokens + 2));
        }

        else if (strcmp(*(tokens + 0), "dump") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;
            rc = proto_client_dump(C->ph);
        }

        else if (strcmp(*(tokens + 0), "quit") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;
            rc = proto_client_goodbye(C->ph);
        }

        else
        {
            fprintf(stderr, "\nUnkown Command\n");
        }
    }

    int j;
    for (j = 0; * (tokens + j); j++)
    {
        free(*(tokens + j));
    }
    free(tokens);

    return rc;
}

int
doRPCCmd(Client *C, char c)
{
    int rc = -1;

    switch (c)
    {
    case 'h':
    {
        rc = proto_client_hello(C->ph);
        printf("hello: rc=%x\n", rc);
        if (rc > 0) game_process_reply(C);
    }
    break;
    case 'm':
        scanf("%c", &c);
        rc = proto_client_move(C->ph, c);
        break;
    case 'g':
        rc = proto_client_goodbye(C->ph);
        break;
    default:
        printf("%s: unknown command %c\n", __func__, c);
    }
    // NULL MT OVERRIDE ;-)
    printf("%s: rc=0x%x\n", __func__, rc);
    if (rc == 0xdeadbeef) rc = 1;
    return rc;
}

int
doRPC(Client *C)
{
    int rc;
    char c;

    printf("enter (h|m<c>|g): ");
    scanf("%c", &c);
    rc = doRPCCmd(C, c);

    printf("doRPC: rc=0x%x\n", rc);

    return rc;
}


int
docmd(Client *C, char cmd)
{
    int rc = 1;

    switch (cmd)
    {
    case 'd':
        proto_debug_on();
        break;
    case 'D':
        proto_debug_off();
        break;
    case 'r':
        rc = doRPC(C);
        break;
    case 'q':
        rc = -1;
        break;
    case '\n':
        rc = 1;
        break;
    default:
        printf("Unkown Command\n");
    }
    return rc;
}

void *
shell(void *arg)
{
    Client *C = arg;
    char c;
    int rc;
    int menu = 1;
    char *longcommand;

    while (1)
    {
        (longcommand = specialPrompt(menu));

        if (size(longcommand) > 1)
        {
            rc = doCMDS(C, longcommand);
        }
        else
        {
            c = longcommand[0];
            if (c != 0)
                rc = docmd(C, c);
        }

        if (rc < 0) break;
        if (rc == 1) menu = 1; else menu = 0;


        /*
        if ((c = prompt(menu)) != 0) rc = docmd(C, c);
        if (rc < 0) break;
        if (rc == 1) menu = 1; else menu = 0;
        */
    }

    fprintf(stderr, "terminating\n");
    fflush(stdout);
    return NULL;
}

void
usage(char *pgm)
{
    fprintf(stderr, "USAGE: %s <port|<<host port> [shell] [gui]>>\n"
            "  port     : rpc port of a game server if this is only argument\n"
            "             specified then host will default to localhost and\n"
            "             only the graphical user interface will be started\n"
            "  host port: if both host and port are specifed then the game\n"
            "examples:\n"
            " %s 12345 : starts client connecting to localhost:12345\n"
            " %s localhost 12345 : starts client connecting to locaalhost:12345\n",
            pgm, pgm, pgm, pgm);

}

void
initGlobal(int port, char **host)
{
    bzero(&globals, sizeof(globals));
    strncpy(globals.host, host, strlen(host) - 1);
    globals.port = port;
}

void
initGlobals(int argc, char **argv)
{
    bzero(&globals, sizeof(globals));

    if (argc == 1)
    {
        usage(argv[0]);
        exit(-1);
    }

    if (argc == 2)
    {
        strncpy(globals.host, "localhost", STRLEN);
        globals.port = atoi(argv[1]);
    }

    if (argc >= 3)
    {
        strncpy(globals.host, argv[1], STRLEN);
        globals.port = atoi(argv[2]);
    }

}

int
main(int argc, char **argv)
{
    Client c;

    shell(&c);

    /*
        initGlobals(argc, argv);

        if (clientInit(&c) < 0)
        {
            fprintf(stderr, "ERROR: clientInit failed\n");
            return -1;
        }

        // ok startup our connection to the server
        if (startConnection(&c, globals.host, globals.port, update_event_handler) < 0)
        {
            fprintf(stderr, "ERROR: startConnection failed\n");
            return -1;
        }

        shell(&c);
    */

    return 0;
}

