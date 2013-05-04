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
#include <assert.h>
#include "./../lib/client_types.h"

#define STRLEN 81

typedef enum
{
    FALSE = 0,
    TRUE = 1
} Bool;

int connectFLAG = FALSE;

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

// for debuging
int print_client_data() 
{
   pthread_mutex_lock(&client_data_mutex);

   fprintf(stdout, "Game:\n   version: %llu\n   state: %d\n"
                   "Player:\n   id: %d\n   team: %d\n   x: %d\n   y: %d\n   flag: %c\n   shovel: %c\n",
                   gamedata.game_version, gamedata.game_state, playerdata.id, playerdata.team, 
                   playerdata.x, playerdata.y, playerdata.state, playerdata.flag, playerdata.shovel);

   pthread_mutex_unlock(&client_data_mutex);
   return 1;
}

static int
clientInit(Client *C)
{
    bzero(C, sizeof(Client));

    // initialize the client protocol subsystem
    if (proto_client_init(&(C->ph)) < 0)
    {
        fprintf(stderr, "client: main: ERROR initializing proto system\n");
        connectFLAG = FALSE;

        //return -1;
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

void
initGlobal(int port, char *host)
{
    bzero(&globals, sizeof(globals));
    strncpy(globals.host, host, strlen(host));
    globals.port = port;
}

int
startConnection(Client *C, char *host, PortType port, Proto_MT_Handler h)
{
    if (globals.host[0] != 0 && globals.port != 0)
    {
        if (proto_client_connect(C->ph, host, port) != 0)
        {
            fprintf(stderr, "failed to connect\n");
            connectFLAG = FALSE;

            //return -1;
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

char *specialPrompt(int menu)
{
    int max = 20;
    int c ;
    char *cmdInputs = (char *)malloc(max); // allocate buffer

    if (cmdInputs == 0) quit();

    if (menu)
        fprintf(stdout, "\ndaGame> ");

    fflush(stdout);

    while (TRUE)   // skip leading whitespace
    {
        c = getchar();
        if (c == NULL) break; // end of file
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
        if (/*isspace(c) || */c == '\n') // at end, add terminating zero
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
    char **tokens;


    char temp[400];
    strcpy (temp, cmdInputs);

    tokens = str_split(temp, ' ');


    int o;

    for (o = 0; * (tokens + o); o++)
    {
        if (proto_debug())
            printf("%d - %s\n", o, *(tokens + o));
    }

    if (proto_debug())
        printf("%d\n", o);

    if (tokens)
    {
        if (connectFLAG == FALSE)
        {
            if (strcmp(*(tokens + 0), "connect") == 0 && o == 3)
            {
                connectFLAG = TRUE;
                /*
                if (proto_debug())
                   printf("IP - %s  \nPORT - %d\n", *(tokens + 1), atoi(*(tokens + 2)));

                initGlobal(atoi(*(tokens + 2)), *(tokens + 1));

                if (proto_debug())
                   printf("INITGLOBAL - GOOD\n");

                if (clientInit(&c) < 0)
                {
                   fprintf(stderr, "ERROR: clientInit failed\n");
                   connectFLAG = FALSE;
                   //return -1;
                }
                if (proto_debug())
                   printf("CLIENTINIT- GOOD\n");
                // ok startup our connection to the server

                if (startConnection(&c, globals.host, globals.port, update_event_handler) < 0)
                {
                   fprintf(stderr, "ERROR: startConnection failed\n");
                   connectFLAG = FALSE;
                   //return -1;
                }
                if (proto_debug())
                   printf("CONNECTION - GOOD\n");
                   */
            }
            else
            {
                fprintf(stderr, "%s\n", "Error using connect usage - connect <IP> <PORT>.");
            }
        }
        else if ((strcmp(*(tokens + 0), "connect") == 0) && connectFLAG == TRUE)
        {
            fprintf(stderr, "You are connected.\n");

            int j;
            for (j = 0; * (tokens + j); j++)
            {
                free(*(tokens + j));
            }
            free(tokens);

            return cmdInputs;
        }
    }

    int j;
    for (j = 0; * (tokens + j); j++)
    {
        free(*(tokens + j));
    }
    free(tokens);

    if (proto_debug())
        printf("The command is input is - %s\n", cmdInputs);

    return cmdInputs;
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

int combine(short A, short B)
{
    return A << 16 | B;
}

short getA(int C)
{
    return C >> 16;
}

short getB(int C)
{
    return C & 0xFFFF;
}

int doCMDS(Client *C, char *cmdInput)
{
    int rc = 1;

    char **tokens;
    int k = 0;
    tokens = str_split(cmdInput, ' ');

    if (tokens)
    {
        for (k = 0; * (tokens + k); k++)
        {
            if (proto_debug())
                printf("%d - %s\n", k, *(tokens + k));
        }
    }

    if (proto_debug())
        printf("Total commands - %d\n", k);

    if (connectFLAG == FALSE)
    {
        if (strcmp(*(tokens + 0), "connect") != 0)
        {
            printf("%s\n", "Please do a connect first - connect <IP> <PORT>");
            return rc;
        }
    }

    if (tokens)
    {
        if ( strcmp(*(tokens + 0), "pickup") == 0 )
        {
            if (k == 2)
            {
                if (strcmp(*(tokens + 1), "shovel") == 0)
                   rc = proto_item_action(C->ph, 'S', 'p'); 
                else if (strcmp(*(tokens + 1), "flag") == 0)
                   rc = proto_item_action(C->ph, 'F', 'p'); 
                else
                   printf("%s\n", "Error - usage of pickup: pickup <shovel|flag>");
            }
	    else 
               printf("%s\n", "Error - usage of pickup: pickup <shovel|flag>");
            return rc;
        }
        if ( strcmp(*(tokens + 0), "drop") == 0 )
        {
            if (k == 2)
            {
                if (strcmp(*(tokens + 1), "shovel") == 0)
                   rc = proto_item_action(C->ph, 'S', 'd'); 
                else if (strcmp(*(tokens + 1), "flag") == 0)
                   rc = proto_item_action(C->ph, 'F', 'd'); 
                else
                   printf("%s\n", "Error - usage of drop: drop <shovel|flag>");
            }
	    else 
               printf("%s\n", "Error - usage of drop: drop <shovel|flag>");
            return rc;
        }
        if ( strcmp(*(tokens + 0), "move") == 0 )
        {
            if (k == 2)
            {
                if (strcmp(*(tokens + 1), "U") == 0)
                   rc = proto_client_move(C->ph, 'U'); 
                else if (strcmp(*(tokens + 1), "D") == 0)
                   rc = proto_client_move(C->ph, 'D'); 
                else if (strcmp(*(tokens + 1), "L") == 0)
                   rc = proto_client_move(C->ph, 'L'); 
                else if (strcmp(*(tokens + 1), "R") == 0)
                   rc = proto_client_move(C->ph, 'R'); 
                else
                   printf("%s\n", "Error - usage of move: move <U|D|L|R>");
            }
	    else 
                printf("%s\n", "Error - usage of move: move <U|D|L|R>");
            return rc;
        }
        if ( strcmp(*(tokens + 0), "join") == 0 )
        {
            rc = proto_client_hello(C->ph); 
            return rc;
        }
        if ( strcmp(*(tokens + 0), "printdata") == 0 )
        {
            rc = print_client_data(); 
            return rc;
        }
        if ( strcmp(*(tokens + 0), "connect") == 0 )
        {
            //rc = doRPCCmd(C, 'h');
            //return rc;
            //rc = proto_client_hello(C->ph);
            //printf("hello: rc=%x\n", rc);
            //if (rc > 0) game_process_reply(C);
            if (connectFLAG == TRUE)
            {
                //fprintf(stderr, "Are are already connected.");
            }
            return rc;
        }

        else if (strcmp(*(tokens + 0), "numhome") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;
            if (k == 2)
            {
                rc = proto_client_numhome(C->ph, atoi(*(tokens + 1)));

                if ((strcmp(*(tokens + 1), "1") == 0) || (strcmp(*(tokens + 1), "2") == 0 ))
                    printf("The number of home cells that team %s has - %d\n", *(tokens + 1), rc);
                else
                    printf("%s\n", "Error - usage of numhome: numhome <1|2>");
            }
            else
                printf("%s\n", "Error - usage of numhome: numhome <1|2>");

            rc = 1;

            return rc;
        }

        else if (strcmp(*(tokens + 0), "numjail") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc

            if (k == 2)
            {
                rc = proto_client_numjail(C->ph, atoi(*(tokens + 1)));

                if (strcmp(*(tokens + 1), "1") == 0 || strcmp(*(tokens + 1), "2") == 0 )
                    printf("The number of jail cells that team %s has - %d\n", *(tokens + 1), rc);
                else
                    printf("%s\n", "Error - usage of numjail: numjail <1|2>");
            }
            else
                printf("%s\n", "Error - usage of numjail: numjail <1|2>");
            rc = 1;

            return rc;
        }

        else if (strcmp(*(tokens + 0), "numwall") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;
            rc = proto_client_numwall(C->ph);
            printf("The number of wall cells - %d\n", rc);

            rc = 1;

            return rc;
        }

        else if (strcmp(*(tokens + 0), "numfloor") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;
            //rc = proto_client_numﬂoor(C->ph);

            rc = proto_client_numfloor(C->ph);
            printf("The number of floor cells - %d\n", rc);
            rc = 1;

            return rc;
        }

        else if (strcmp(*(tokens + 0), "dim") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;
            rc = proto_client_dim(C->ph);

            short x = getA(rc);
            short y = getB(rc);

            printf("The dimensions of the maze - %hd by %hd\n", x, y);

            rc = 1;

            return rc;
        }

        else if (strcmp(*(tokens + 0), "cinfo") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;

            if (k == 3)
            {
                rc = proto_client_cinfo(C->ph, atoi(*(tokens + 1)), atoi(*(tokens + 2)));

                if (atoi(*(tokens + 1)) > 0 && atoi(*(tokens + 2)) > 0 )
                {
                    if (proto_debug())
                        printf("%s\n", "Good");

                    int cell = (rc >> (8 * 1)) & 0xff;
                    printf("The cell is a - %c\n", (char) cell);

                    int team = (rc >> (8 * 2)) & 0xff;
                    printf("The team is - %c\n", (char) team);

                    int occupied = (rc >> (8 * 3)) & 0xff;
                    printf("Is the cell occupied - %c\n", (char) occupied);
                }
                else
                    printf("%s\n", "Error - usage of cinfo: cinfo <X> <Y>");

            }
            else
                printf("%s\n", "Error - usage of cinfo: cinfo <X> <Y>");

            rc = 1;

            return rc;

        }

        else if (strcmp(*(tokens + 0), "dump") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;
            rc = proto_client_dump(C->ph);

            return rc;
        }

        else if (strcmp(*(tokens + 0), "debug") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;
            //rc = proto_client_goodbye(C->ph);

            if (k == 2)
            {
                if (strcmp(*(tokens + 1), "on") == 0)
                    proto_debug_on();
                else if (strcmp(*(tokens + 1), "off") == 0)
                    proto_debug_off();
                else
                    printf("%s\n", "Error - usage of debug: debug <on|off>");
            }
            else
                printf("%s\n", "Error - usage of debug: debug <on|off>");

            return rc;
        }

        else if (strcmp(*(tokens + 0), "quit") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;
            rc = proto_client_goodbye(C->ph);
            exit(0);
        }

        else
        {
            fprintf(stdout, "Unkown Command\n");
        }
    }

    int j = 0;
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

        //printf("%s\n", longcommand);

        if (strlen(longcommand) > 1)
        {
            rc = doCMDS(C, longcommand);
        }
        else
        {
            //c = longcommand[0];
            //if (c != 0)
            //  rc = docmd(C, c);
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
    fprintf(stdout, "USAGE: %s <port|<<host port> [shell] [gui]>>\n"
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

    // initalize client types structs defined in ./lib/client_types.h
    //gamedata
    themaze.maze = NULL;
    themaze.rows = 1;
    themaze.columns = 2;
}

int
main(int argc, char **argv)
{
    Client c;

    // shell(&c);
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

    return 0;
}

