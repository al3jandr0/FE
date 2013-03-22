
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define STRLEN 81

typedef enum
{
    FALSE = 0,
    TRUE = 1
} Bool;

int connectFLAG = FALSE;

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
        fprintf(stderr, "\ndaGame> ");

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
    //char **tokens;

    //tokens = str_split(cmdInputs, ' ');

    //if (tokens)
    {
        if (connectFLAG == FALSE)
        {
            if (strcmp(strtok (cmdInputs, " "), "connect") == 0)
            {
                connectFLAG = TRUE;
                /* initGlobal(atoi(*(tokens + 2)), *(tokens + 1));

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
                 }*/
            }
            else
            {
                fprintf(stderr, "%s\n", "Error using connect.");
            }
        }
    }

    /*
        int j;
        for (j = 0; * (tokens + j); j++)
        {
            free(*(tokens + j));
        }
        free(tokens);
    */

    printf("The command is input is - %s\n", cmdInputs);

    return cmdInputs;
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


int doCMDS(char *cmdInput)
{
    int rc = 1;

    char **tokens;

    tokens = str_split(cmdInput, ' ');

    if (tokens)
    {
        int k = 0;
        for (k = 0; * (tokens + k); k++)
        {
            printf("%d - %s\n", k, *(tokens + k));
        }
    }

    if (strcmp(*(tokens + 0), "connect") == 0 && connectFLAG == FALSE)
    {
        printf("%s\n", "Please do a connect first");
        return -1;
    }


    if (tokens)
    {
        if ( strcmp(*(tokens + 0), "connect") == 0 )
        {
            //rc = doRPCCmd(C, 'h');
            //return rc;
            // rc = proto_client_hello(C->ph);
            printf("hello: rc=%x\n", rc);
            //if (rc > 0) game_process_reply(C);
        }

        else if (strcmp(*(tokens + 0), "numhome") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;
            //rc = proto_client_numhome(C->ph, *(tokens + 1));

            if ((strcmp(*(tokens + 1), "1") == 0) || (strcmp(*(tokens + 1), "2") == 0 ))
                printf("The number of home cells that team %s has - %d\n", *(tokens + 1), rc);
            else
                printf("%s\n", "Error - usage of numhome: numhome <1|2>");
        }

        else if (strcmp(*(tokens + 0), "numjail") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc
            //rc = proto_client_numjail(C->ph, *(tokens + 1));
            if (*(tokens + 1))
            {
                if (strcmp(*(tokens + 1), "1") == 0 || strcmp(*(tokens + 1), "2") == 0 )
                    printf("The number of jail cells that team %s has - %d\n", *(tokens + 1), rc);
                else
                    printf("%s\n", "Error - usage of numjail: numjail <1|2>");
            }
            else
                printf("%s\n", "Error - usage of numjail: numjail <1|2>");
        }

        else if (strcmp(*(tokens + 0), "numwall") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;
            //rc = proto_client_numwall(C->ph);
            printf("The number of wall cells - %d\n", rc);
        }

        else if (strcmp(*(tokens + 0), "numﬂoor") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;
            //rc = proto_client_numﬂoor(C->ph);
            printf("The number of floor cells - %d\n", rc);
        }

        else if (strcmp(*(tokens + 0), "dim") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;
            //rc = proto_client_dim(C->ph);

            short x = getA(rc);
            short y = getB(rc);

            printf("The dimensions of the maze - %hd by %hd\n", x, y);
        }

        else if (strcmp(*(tokens + 0), "cinfo") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;
            //rc = proto_client_cinfo(C->ph, *(tokens + 1), *(tokens + 2));

            if (*(tokens + 1) && *(tokens + 2))
            {
                if (atoi(*(tokens + 1)) > 0 && atoi(*(tokens + 2)) > 0 )
                    printf("%s\n", "Good");
                else
                    printf("%s\n", "Error - usage of cinfo: cinfo <X> <Y>");

            }
            else
                printf("%s\n", "Error - usage of cinfo: cinfo <X> <Y>");

            int cell = (rc >> (8 * 1)) & 0xff;
            printf("The cell is a - %c\n", (char) cell);

            int team = (rc >> (8 * 2)) & 0xff;
            printf("The team is - %c\n", (char) team);

            int occupied = (rc >> (8 * 3)) & 0xff;
            printf("Is the cell occupied - %c\n", (char) occupied);

        }

        else if (strcmp(*(tokens + 0), "dump") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;
            //rc = proto_client_dump(C->ph);
        }

        else if (strcmp(*(tokens + 0), "debug") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;
            //rc = proto_client_goodbye(C->ph);

            if (*(tokens + 1))
            {
                if (strcmp(*(tokens + 1), "on") == 0)
                    //proto_debug_on();
                    printf("%s\n", "Error - usage of debug: debug <on|off>");

                else if (strcmp(*(tokens + 1), "off") == 0)
                    // proto_debug_off();
                    printf("%s\n", "Error - usage of debug: debug <on|off>");

                else
                    printf("%s\n", "Error - usage of debug: debug <on|off>");
            }
            else
                printf("%s\n", "Error - usage of debug: debug <on|off>");

        }

        else if (strcmp(*(tokens + 0), "quit") == 0)
        {
            //rc = docmd(C, 'q');
            //return rc;
            //rc = proto_client_goodbye(C->ph);
            exit(0);
        }

        else
        {
            fprintf(stderr, "Unkown Command\n");
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

void *
shell(void)
{
    char c;
    int rc;
    int menu = 1;
    char *longcommand;

    while (1)
    {
        (longcommand = specialPrompt(menu));

        printf("The command is - %s\n", longcommand);

        if (size(longcommand) > 1)
        {
            rc = doCMDS(longcommand);
        }
        else
        {
            c = longcommand[0];
            //if (c != 0)
            //rc = docmd(C, c);
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

int
main(int argc, char **argv)
{

    shell();

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

