/******************************************************************************
* Gurwinder Singh
* Distributed Systems Spring 2013
* CTF - Game Logic
* Team - For Example
******************************************************************************/

#include "gameMap.h"

enum { FALSE, TRUE };

#define DEBUG FALSE

#define debugPrint(...) \
    do { if (DEBUG) fprintf(stderr, __VA_ARGS__); } while (0)


#define MAX 200

// Game Info
int gamePlayingFlag = FALSE;
int playerCount = 0;

// For Player Array
struct player *head = NULL;
struct player *curr = NULL;

// For Maze
Maze maze;

Item FlagT0;
Item ShovelT0;
Item FlagT1;
Item ShovelT1;

Cell *HomeList0;
Cell *HomeList1;
Cell *JailList0;
Cell *JailList1;

/******************************************************************************
* Make Array
******************************************************************************/
struct player *create_list(int ID)
{
    debugPrint("\n creating player list with headnode as [%d]\n", ID);
    struct player *ptr = (struct player *)malloc(sizeof(struct player));
    if (NULL == ptr)
    {
        printf("\n Node creation failed \n");
        return NULL;
    }
    ptr->ID = ID;
    ptr->next = NULL;

    head = curr = ptr;
    return ptr;
}

/******************************************************************************
* Add a new Player
******************************************************************************/
struct player *add_to_list(int ID, bool add_to_end)
{
    if (NULL == head)
    {
        return (create_list(ID));
    }

    if (add_to_end)
        debugPrint("\n Adding player to end of list with id [%d]\n", ID);
    else
        debugPrint("\n Adding player to beginning of list with id [%d]\n", ID);

    struct player *ptr = (struct player *)malloc(sizeof(struct player));
    if (NULL == ptr)
    {
        printf("\n Node creation failed \n");
        return NULL;
    }
    ptr->ID = ID;
    ptr->next = NULL;

    if (add_to_end)
    {
        curr->next = ptr;
        curr = ptr;
    }
    else
    {
        ptr->next = head;
        head = ptr;
    }
    return ptr;
}

/******************************************************************************
* Find a Player
******************************************************************************/
struct player *search_in_list(int ID, struct player **prev)
{
    struct player *ptr = head;
    struct player *tmp = NULL;
    bool found = false;

    debugPrint("\n Searching the player list for ID [%d] \n", ID);

    while (ptr != NULL)
    {
        if (ptr->ID == ID)
        {
            found = true;
            break;
        }
        else
        {
            tmp = ptr;
            ptr = ptr->next;
        }
    }

    if (true == found)
    {
        if (prev)
            *prev = tmp;
        return ptr;
    }
    else
    {
        return NULL;
    }
}

/******************************************************************************
* Find a Player using FD
******************************************************************************/
struct player *search_in_list_fd(int fd, struct player **prev)
{
    struct player *ptr = head;
    struct player *tmp = NULL;
    bool found = false;

    debugPrint("\n Searching the player list for FD [%d] \n", fd);

    while (ptr != NULL)
    {
        if (ptr->FD == fd)
        {
            found = true;
            break;
        }
        else
        {
            tmp = ptr;
            ptr = ptr->next;
        }
    }

    if (true == found)
    {
        if (prev)
            *prev = tmp;
        return ptr;
    }
    else
    {
        return NULL;
    }
}

/******************************************************************************
* Remove a Player
******************************************************************************/
int delete_from_list(int ID)
{
    struct player *prev = NULL;
    struct player *del = NULL;

    debugPrint("\n Deleting player with id [%d] from list\n", ID);

    del = search_in_list(ID, &prev);
    if (del == NULL)
    {
        return -1;
    }
    else
    {
        if (prev != NULL)
            prev->next = del->next;

        if (del == curr)
        {
            curr = prev;
        }
        else if (del == head)
        {
            head = del->next;
        }
    }

    free(del);
    del = NULL;

    return 0;
}

/******************************************************************************
* Remove a Player using fd
******************************************************************************/
int delete_from_list_fd(int fd)
{
    struct player *prev = NULL;
    struct player *del = NULL;

    debugPrint("\n Deleting player with FD [%d] from list\n", fd);

    del = search_in_list_fd(fd, &prev);
    if (del == NULL)
    {
        return -1;
    }
    else
    {
        if (prev != NULL)
            prev->next = del->next;

        if (del == curr)
        {
            curr = prev;
        }
        else if (del == head)
        {
            head = del->next;
        }
    }

    free(del);
    del = NULL;

    return 0;
}

/******************************************************************************
* Print Player Array
******************************************************************************/
void print_list(void)
{
    struct player *ptr = head;

    printf("\n -------Printing player list------- \n");
    while (ptr != NULL)
    {
        printf("\n Player Id : [%d] \n Player Team : [%d]\n Player Position : [%d,%d]\n Player State : [%d]\n", ptr->ID, ptr->team, ptr->PlayerPos.x, ptr->PlayerPos.y, ptr->State);

        printf("Here is a mini view of the player:\n");


        ptr = ptr->next;
    }
    return;
}

/******************************************************************************
* Prints the cells around a player - mini map
******************************************************************************/
int print_cells_arroud(int id)
{
    int x, y, i, j, N;
    struct player *ptr = NULL;

    N = 4;
    if (gamePlayingFlag == TRUE)
    {

        ptr = search_in_list(id, NULL);

        if (ptr == NULL)
        {
            fprintf(stderr, "@print_cells_arround: ERROR: search_in_list(id = %d) returned NULL\n", id);
            return -1;
        }

        x = ptr->PlayerPos.x;
        y = ptr->PlayerPos.y;

        for (i = y - N; i < y + N; i++)
        {
            for (j = x - N; j < x + N; j++)
            {
                if ( i < 0 )
                    printf("x");
                else if ( i >= 200 )
                    printf("x");
                else if (j < 0)
                    printf("x");
                else if ( j >= 200 )
                    printf("x");
                else if ( i == y && j == x)
                    printf("p");
                else
                    printf("%c", cellInfo(i , j).C_Type );
            }
            printf("\n");
        }
        fflush(stdout);
    }
    return 1;
}

/******************************************************************************
* Location in Maze
******************************************************************************/
int getPos(int col, int row)
{
    return row * maze.dimensions.y + col;
}

/*
//extern int
int findNumHome(int par)
{
    if (par == 1)
    {
        return maze.numOfHomes[0];
    }
    else if (par == 2)
    {
        return maze.numOfHomes[1];
    }
    else
    {
        return -1;
    }
}

//extern int
int findNumJail(int par)
{
    if (par == 1)
    {
        return maze.numOfJails[0];
    }
    else if (par == 2)
    {
        return maze.numOfJails[1];
    }
    else
    {
        return -1;
    }
}

//extern int
int findNumWall()
{
    return maze.numWall;
}

//extern int
int findNumFloor()
{
    return maze.numFloor;
}


//extern void
void findDimensions(Position *pos)
{
    pos->x = maze.dimensions.x;
    pos->y = maze.dimensions.y;
}

//extern void
void findCInfo(int column, int row)
{
    int pos;
    if (row == 0 && column == 0)
    {
        pos = 0;
    }
    else if (row == 0)
    {
        pos = column;
    }
    else if (column == 0)
    {
        pos = row * maze.dimensions.y;
    }
    else
    {
        pos = row * MAX + column;
    }
    maze.cells[pos];
    printf("\nCell Info for (%d,%d)-(column,row) is:\nCell Type is '%c',  Cell Team is %d, ", column, row, maze.cells[pos].C_Type, maze.cells[pos].Cell_Team);
    if (maze.cells[pos].p == NULL)
    {
        printf("Cell Is Not Occupied\n");
    }
    else
    {
        printf("Cell Is Occupied\n");
    }
}
*/

/******************************************************************************
* Returns Cell Information
******************************************************************************/
Cell cellInfo(int row, int column)
{
    return maze.cells[ row * maze.dimensions.y + column ];
}

/******************************************************************************
* Starts the Game
******************************************************************************/
int startGame()
{

    if (gamePlayingFlag == FALSE)
    {
        printf("Starting the Game\n");

        HomeList0 = malloc(MAX * sizeof(Cell));
        HomeList1 = malloc(MAX * sizeof(Cell));
        JailList0 = malloc(MAX * sizeof(Cell));
        JailList1 = malloc(MAX * sizeof(Cell));

        printf("Loading the Map\n");

        if (loadMap() < 0)
            return -1;

        // Add in Flag and Shovel for T0
        resetItem(d, FlagT0, 0);

        resetItem(d, ShovelT0, 0);

        // Add in Flag and Shovel for T1
        resetItem(d, FlagT1, 1);

        resetItem(d, ShovelT1, 1);

        // Now Game is Playing
        gamePlayingFlag = TRUE;

        return 1;
    }
    else
        printf("Cannot start the Game yet.\n");

    // Error.
    return -1;
}

/******************************************************************************
* Fully Stops the Game
******************************************************************************/
int stopGame()
{
    int x;

    if (gamePlayingFlag == TRUE)
    {
        int i = 0, ret = 0;

        struct player *ptr = NULL;

        for (i = 0; i < playerCount; i++)
        {
            ret = delete_from_list(i);

            if (ret != 0)
            {
                debugPrint("\n delete [val = %d] failed, no such element found\n", i);
            }
            else
            {
                debugPrint("\n delete [val = %d]  passed \n", i);
            }
        }

        // Free everything
        free(HomeList0);
        free(HomeList1);
        free(JailList0);
        free(JailList1);

        // Clean up the maze
        maze.numFloor = 0;
        maze.numWall = 0;
        maze.numCells = 0;
        maze.numOfJails[0] = 0;
        maze.numOfJails[1] = 0;
        maze.numOfHomes[0] = 0;
        maze.numOfHomes[1] = 0;
        maze.dimensions.x = -1;
        maze.dimensions.y = -1;
        free(maze.cells);

        // Reset the Counter
        playerCount = 0;

        // Note Playing Game now.
        gamePlayingFlag = FALSE;

        return 1;
    }

    // Error
    return -1;

}

/******************************************************************************
* Reset the Game for Next Round
******************************************************************************/
int resetGame(Deltas *d)
{
    printf("Starting the Game\n");

    HomeList0 = malloc(MAX * sizeof(Cell));
    HomeList1 = malloc(MAX * sizeof(Cell));
    JailList0 = malloc(MAX * sizeof(Cell));
    JailList1 = malloc(MAX * sizeof(Cell));

    printf("Loading the Map\n");

    if (loadMap() < 0)
        return -1;

    // Add in Flag and Shovel for T0
    resetItem(d, FlagT0, 0);
    add_delta_item(d, &FlagT0, sizeof(Item));

    resetItem(d, ShovelT0, 0);
    add_delta_item(d, &ShovelT0, sizeof(Item));


    // Add in Flag and Shovel for T1
    resetItem(d, FlagT1, 1);
    add_delta_item(d, &FlagT1, sizeof(Item));

    resetItem(d, ShovelT1, 1);
    add_delta_item(d, &ShovelT1, sizeof(Item));

    int i;
    struct player *ptr = NULL;
    Position *newPlayerPos = NULL;

    for ( i = 0; i < playerCount; ++i)
    {
        ptr = search_in_list(i, NULL);

        if (NULL == ptr)
        {
        }
        else
        {
            newPlayerPos = findFreeHome(ptr->team);

            if (newPlayerPos->x != NULL && newPlayerPos->y != NULL)
            {
                ptr->PlayerPos = *newPlayerPos;
            }
            add_delta_player(d, ptr, sizeof(struct player));
        }
    }

    // Now Game is Playing
    gamePlayingFlag = TRUE;

    return 1;
}

/******************************************************************************
* Load the Map
******************************************************************************/
int loadMap()
{
    FILE *mapFile;
    if ((mapFile = fopen("./../lib/daGame.map", "r")) == NULL)
    {
        perror ("Cannot Open daGame.map. ");
        return -1;
    }

    int c;
    int columnCounter = 0;
    while ((c = fgetc(mapFile)) != 10)
    {
        columnCounter++;
    }
    maze.dimensions.y = columnCounter;

    int rowCounter = 1;
    char s[columnCounter + 2];

    while ((fgets(s, columnCounter + 2, mapFile)) != NULL)
    {
        rowCounter++;
    }
    maze.dimensions.x = rowCounter;

    rewind(mapFile);

    maze.cells = malloc((columnCounter * rowCounter) * sizeof(Cell));

    int currentIndex = 0;

    while ((c = fgetc(mapFile)) != EOF)
    {
        if (c != 10)
        {
            Cell newCell;

            newCell.C_Type = c;

            newCell.Cell_Pos.x = currentIndex / maze.dimensions.x;
            newCell.Cell_Pos.y = currentIndex % maze.dimensions.y;

            if (newCell.Cell_Pos.y < (MAX / 2))
            {
                newCell.Cell_Team = Team1;
            }
            else
            {
                newCell.Cell_Team = Team2;
            }

            newCell.p = NULL;

            maze.cells[currentIndex] = newCell;

            Item newItem;

            if (c == 's' || c == 'S' || c == 'F' || c == 'f')
            {
                newItem.itType = c;
            }
            else if (c == '#')
            {
                maze.numWall++;
            }
            else if (c == ' ')
            {
                maze.numFloor++;
            }
            else if (c == 'j')
            {
                JailList0[maze.numOfJails[0]] = newCell;
                maze.numOfJails[0]++;
            }
            else if (c == 'J')
            {
                JailList1[maze.numOfJails[1]] = newCell;
                maze.numOfJails[1]++;
            }
            else if (c == 'h')
            {
                HomeList0[maze.numOfHomes[0]] = newCell;
                maze.numOfHomes[0]++;
            }
            else if (c == 'H')
            {
                HomeList1[maze.numOfHomes[1]] = newCell;
                maze.numOfHomes[1]++;
            }

            currentIndex++;
        }
    }
    fclose(mapFile);

    return 1;
}

/******************************************************************************
* Print map to console
******************************************************************************/
void dumpMap()
{
    int x = (maze.dimensions.y * maze.dimensions.x);
    int i = 0;

    for (i = 0; i < (x); i++)
    {
        int c =  maze.cells[i].C_Type;
        char print = (char) c;

        if ((i % maze.dimensions.y) == 0)
        {
            printf("\n%c", print);
        }
        else
        {
            printf("%c", print);
        }
    }

    return;
}

/******************************************************************************
* Cell Flag check
******************************************************************************/
int CellContainFlag(Cell c, Item it)
{
    if ((c.Cell_Pos.x == it.ItemPos.x) && (c.Cell_Pos.y == it.ItemPos.y))
    {
        debugPrint("Cell at pos x has item: %d\n", c.Cell_Pos.x);
        debugPrint("Cell at pos y has item: %d\n", c.Cell_Pos.y);
        return 1;
    }
    else
    {
        debugPrint("Cell at pos x not contain an item: %d\n", c.Cell_Pos.x);
        debugPrint("Cell at pos y not contain an item: %d\n", c.Cell_Pos.y);
        return 0;
    }
}

/******************************************************************************
* Home Winning Flag check
******************************************************************************/
int checkHomeForFlags(Item Flag_Team1, Item Flag_Team2, int team)
{
    int counter = 0;
    int p = 0;

    if (team == 0)
    {
        printf("%s\n", "Team 1");

        for (p = 0; p < maze.numOfHomes[0]; p++)
        {
            if ((CellContainFlag(HomeList0[p], Flag_Team1) == 1) || (CellContainFlag(HomeList0[p], Flag_Team2) == 1))
            {
                printf("Item is at : %d\n", p);
                counter++;
            }
        }
        if (counter > 1)
        {
            printf("%s\n", "Winning Condition");
            return 1;
        }
        else
        {
            printf("%s\n", "No Winning Condition");
            return 0;
        }

    }

    if (team == 1)
    {
        printf("%s\n", "Team 2");

        for (p = 0; p < maze.numOfHomes[1]; p++)
        {
            if ((CellContainFlag(HomeList1[p], Flag_Team1) == 1) || (CellContainFlag(HomeList1[p], Flag_Team2) == 1))
            {
                counter++;
            }
        }
        if (counter > 1)
        {
            return 1;
        }
        else
        {
            return 0;
        }

    }
}


/******************************************************************************
* Check the status of the game
******************************************************************************/
int gameStat()
{
    int p;
    int checker = 0;

    // Not Currently playing
    if (gamePlayingFlag == FALSE)
        return -1;


    //return 3; // Team 1 wins
    for (p = 0; p < maze.numOfJails[1]; p++)
    {
        if (JailList1[p].occupied == 1)
        {
            checker++;
        }
    }

    if (checker == 0  &&  flag0 == TRUE && flag2 == TRUE)
        return 3;


    checker = 0;

    //return 4; // Team 2 wins
    for (p = 0; p < maze.numOfJails[0]; p++)
    {
        if (JailList0[p].occupied == 1)
        {
            checker++;
        }
    }

    if (checker == 0  &&  flag0 == TRUE && flag2 == TRUE)
        return 4;


    // Normal
    return 1;
}

/******************************************************************************
* Find a Free Home cell based on team number
******************************************************************************/
Position *findFreeHome(int team)
{
    debugPrint("Trying to find location for team %d\n", team);

    int p = 0;

    Position *newPos = NULL;

    newPos = malloc(sizeof(Position));

    if (team == 0)
    {
        debugPrint("%s\n", "Team 0");

        for (p = 0; p < maze.numOfHomes[0]; p++)
        {
            if (HomeList0[p].occupied != team)
            {
                newPos->x = HomeList0[p].Cell_Pos.x;
                newPos->y = HomeList0[p].Cell_Pos.y;
                HomeList0[p].occupied = team;
                debugPrint("Free Location for team %d found at %d, %d\n", team, newPos->x, newPos->y);
                return newPos;
            }
        }
    }

    if (team == 1)
    {
        debugPrint("%s\n", "Team 1");
        for (p = 0; p < maze.numOfHomes[1]; p++)
        {
            if (HomeList1[p].occupied != team)
            {
                newPos->x = HomeList1[p].Cell_Pos.x;
                newPos->y = HomeList1[p].Cell_Pos.y;
                HomeList1[p].occupied = team;
                debugPrint("Free Location for team %d found at %d, %d\n", team, newPos->x, newPos->y);
                return newPos;
            }
        }
    }

    newPos->x = NULL;
    newPos->y = NULL;

    debugPrint("Free Location for team %d found at %d, %d\n", team, NULL, NULL);

    return newPos;
}

/******************************************************************************
* Pick up an Item
******************************************************************************/
int pickUpItem(int playerID, Deltas *d)
{
    struct player *ptr = NULL;

    Item *itPtr = NULL;
    int pos;

    ptr = search_in_list(playerID, NULL);

    pos = getPos(ptr->PlayerPos.y, ptr->PlayerPos.x);

    if (maze.cells[pos].containItem == TRUE)
    {
        itPtr->ItemPos.x = ptr->PlayerPos.x;
        itPtr->ItemPos.y = ptr->PlayerPos.y;

        ptr->i = itPtr;

        maze.cells[pos].containItem = FALSE;

        add_delta_player(d, ptr, sizeof(struct player));
        add_delta_cell(d, &(maze.cells[pos]), sizeof(Cell));
        add_delta_item(d, &itPtr, sizeof(Item));

        return 1;
    }

    // Error
    return -1;
}

/******************************************************************************
* Reset the Items
******************************************************************************/
int resetItem(Deltas *d, Item it, int team)
{
    int row, col, pos;
    int dx = dimX();
    int dy = dimY();

    debugPrint("it.ItemPos.x before: %d\n", it.ItemPos.x);
    debugPrint("it.ItemPos.y before: %d\n", it.ItemPos.y);

    if (it.itType == 3)
    {

        Position *NewPos = NULL;
        NewPos = findFreeHome(team);
        pos = getPos(NewPos->y, NewPos->x);

        debugPrint("it.ItemPos.x before: %d\n", it.ItemPos.x);
        debugPrint("it.ItemPos.y before: %d\n", it.ItemPos.y);

        it.ItemPos.x = NewPos->x;
        it.ItemPos.y = NewPos->y;

        debugPrint("it.ItemPos.x after: %d\n", it.ItemPos.x);
        debugPrint("it.ItemPos.y after: %d\n", it.ItemPos.y);
        debugPrint("maze.cells[pos].Cell_Pos.x: %d\n", maze.cells[pos].Cell_Pos.x);
        debugPrint("maze.cells[pos].Cell_Pos.y: %d\n", maze.cells[pos].Cell_Pos.y);

        maze.cells[pos].Cell_Pos.x = it.ItemPos.x;
        maze.cells[pos].Cell_Pos.y = it.ItemPos.y;

        maze.cells[pos].containItem = TRUE;

        maze.cells[pos].item = it;

        add_delta_cell(d, &(maze.cells[pos]), sizeof(Cell));
        add_delta_item(d, &it, sizeof(Item));

        return 1;
    }
    else if (it.itType = 1) //position at the part of team1
    {


        do
        {
            row = (rand() % (dx / 2) + 0);
            col = (rand() % (dy / 2) + 0);

            debugPrint("row: %d\n", row);
            debugPrint("col %d\n", col);
            debugPrint("dx: %d\n", dx);
            debugPrint("dy %d\n", dy);

            pos = getPos(col, row);

            debugPrint("it.ItemPos.x stays: %d\n", it.ItemPos.x);
            debugPrint("it.ItemPos.y stays: %d\n", it.ItemPos.y);
        }
        while (maze.cells[pos].C_Type != CT_Floor);



        it.ItemPos.x = row;
        it.ItemPos.y = col;

        maze.cells[pos].Cell_Pos.x = it.ItemPos.x;
        maze.cells[pos].Cell_Pos.y = it.ItemPos.y;
        maze.cells[pos].item = it;
        maze.cells[pos].containItem = TRUE;

        add_delta_cell(d, &(maze.cells[pos]), sizeof(Cell));
        add_delta_item(d, &it, sizeof(Item));

        return 1;

    }
    else if (it.itType = 2)//position at the part of team2
    {
        do
        {
            row = (rand()  % dx + (dx / 2));
            col = (rand()  % dy + (dy / 2));

            pos = getPos(col, row);
        }
        while (maze.cells[pos].C_Type != CT_Floor);

        it.ItemPos.x = row;
        it.ItemPos.y = col;

        maze.cells[pos].Cell_Pos.x = it.ItemPos.x;
        maze.cells[pos].Cell_Pos.y = it.ItemPos.y;
        maze.cells[pos].item = it;
        maze.cells[pos].containItem = TRUE;

        add_delta_cell(d, &(maze.cells[pos]), sizeof(Cell));
        add_delta_item(d, &it, sizeof(Item));

        return 1;
    }

    //Error
    return -1;
}

/******************************************************************************
* Drop the Item
******************************************************************************/
int dropItem(int playerID, Deltas *d, Item it)
{
    struct player *ptr = NULL;

    Item *itPtr = NULL;
    int pos;

    ptr = search_in_list(playerID, NULL);

    if (ptr->i->itType != 0)
    {
        pos = getPos(ptr->PlayerPos.y, ptr->PlayerPos.x);
        maze.cells[pos].item = *(ptr->i);

        maze.cells[pos].Cell_Pos.x = ptr->i->ItemPos.x;
        maze.cells[pos].Cell_Pos.y = ptr->i->ItemPos.y;

        it.ItemPos.x = maze.cells[pos].Cell_Pos.x;
        it.ItemPos.y = maze.cells[pos].Cell_Pos.y;

        ptr->i->itType = 0;

        add_delta_player(d, ptr, sizeof(struct player));
        add_delta_cell(d, &(maze.cells[pos]), sizeof(Cell));
        add_delta_item(d, &it, sizeof(Item));

        return 1;
    }

    // Error
    return -1;
}

/******************************************************************************
* Break a Wall
******************************************************************************/
int breakWall(int playerID, int x, int y, Deltas *d)
{
    struct player *ptr = NULL;

    int pos;

    ptr = search_in_list(playerID, NULL);

    pos = getPos(ptr->PlayerPos.y, ptr->PlayerPos.x);

    if ( ptr->i == NULL )
        return -1;

    if ((ptr->i->itType == Shovel) && (maze.cells[pos].C_Type == CT_Wall))
    {

        ptr->PlayerPos.x = x;
        ptr->PlayerPos.y = y;

        maze.cells[pos].C_Type = CT_Floor;

        resetItem(d, *(ptr->i), ptr->team);
        ptr->i = NULL;

        add_delta_player(d, ptr, sizeof(struct player));
        add_delta_cell(d, &(maze.cells[pos]), sizeof(Cell));

        return 1;

    }

    // Error
    return -1;
}

/******************************************************************************
* Add a new Player
******************************************************************************/
int addPlayer(int fd, Deltas *d)
{
    struct player *newPlayer = NULL;

    int i;

    int temp = 0;

    struct player *ptr = NULL;

    if (playerCount < MAX)
    {

    }
    else
    {
        for (i = 0; i < playerCount; i++)
        {
            ptr = search_in_list(i, NULL);

            if (NULL == ptr)
            {
            }
            else
            {
                temp++;
            }
        }
    }

    if (temp < MAX)
    {
        debugPrint("%s\n", "\nAdding Player!");

        add_to_list(playerCount, true);

        newPlayer = search_in_list(playerCount, NULL);

        newPlayer->team = playerCount % 2;

        newPlayer->FD = fd;

        Position *newPlayerPos = NULL;

        newPlayerPos = findFreeHome(playerCount % 2);

        if (newPlayerPos->x != NULL && newPlayerPos->y != NULL)
        {
            newPlayer->PlayerPos = *newPlayerPos;

            newPlayer->ID = playerCount;

            Item tempItem;

            newPlayer->i = NULL;

            add_delta_player(d, newPlayer, sizeof(struct player));

            return playerCount++;
        }
    }

    return -1;

}

/******************************************************************************
* Remove a Player
******************************************************************************/
int removePlayer (int playerID, Deltas *d)
{
    struct player *newPlayer = NULL;
    newPlayer = search_in_list(playerID, NULL);

    if (newPlayer == NULL)
        return -1;

    Cell temp = cellInfo((newPlayer->PlayerPos.y) + 0, (newPlayer->PlayerPos.x) - 0);

    int ret = delete_from_list(playerID);

    if (ret != 0)
    {
        debugPrint("\n Delete [player id = %d] failed, no such element found\n", playerID);
        return -1;
    }
    else
    {
        debugPrint("\n Delete [player id = %d] - Done. \n", playerID);

        add_delta_cell(d, &temp, sizeof(Cell));

        return 1;
    }
}

/******************************************************************************
* Remove a Player using the fd
******************************************************************************/
int removePlayerFD(int fd, Deltas *d)
{
    struct player *newPlayer = NULL;
    newPlayer = search_in_list_fd(fd, NULL);

    if (newPlayer == NULL)
        return -1;

    Cell temp = cellInfo((newPlayer->PlayerPos.y) + 0, (newPlayer->PlayerPos.x) - 0);

    int ret = delete_from_list_fd(playerID);

    if (ret != 0)
    {
        debugPrint("\n Delete [player fd = %d] failed, no such element found\n", playerID);
        return -1;
    }
    else
    {
        debugPrint("\n Delete [player fd = %d] - Done. \n", playerID);

        add_delta_cell(d, &temp, sizeof(Cell));

        return 1;
    }
}

/******************************************************************************
* Jail a Player
******************************************************************************/
int jailPlayer(struct player *tempPlayer)
{
    int p = 0;

    if (tempPlayer->team == Team1)
    {
        for (p = 0; p < maze.numOfJails[0]; p++)
        {
            if (JailList0[p].occupied > 0)
            {
                tempPlayer->PlayerPos.x = JailList0[p].Cell_Pos.x;
                tempPlayer->PlayerPos.y = JailList0[p].Cell_Pos.y;

                JailList0[p].occupied = tempPlayer->team;

                tempPlayer->State = -1;

                return 1;
            }
        }
    }

    if (tempPlayer->team == Team2)
    {
        for (p = 0; p < maze.numOfJails[1]; p++)
        {
            if (JailList1[p].occupied > 0)
            {
                tempPlayer->PlayerPos.x = JailList1[p].Cell_Pos.x;
                tempPlayer->PlayerPos.y = JailList1[p].Cell_Pos.y;

                JailList1[p].occupied = 1;

                tempPlayer->State = -1;

                return 1;
            }
        }
    }

    return -1;
}

/******************************************************************************
* Tag Checker
******************************************************************************/
void tagCheck(struct player *tempPlayer)
{
    Team tagger = tempPlayer->team;

    int k;

    struct player *ptr = NULL;

    for (k = 0; k < MAX; k++)
    {
        ptr = search_in_list(k, NULL);

        if (NULL == ptr)
        {
            debugPrint("\n Search [playerID = %d] failed, no such element found\n", k);
        }
        else
        {
            debugPrint("\n Search passed [playerID = %d] and checking if she is tagging [playerID = %d]\n", ptr->ID, tempPlayer->ID);

            if (ptr->team != tagger)
            {
                if ((ptr->PlayerPos.x ==  tempPlayer->PlayerPos.x) && (ptr->PlayerPos.y ==  tempPlayer->PlayerPos.y))
                {
                    if (ptr->PlayerPos.x < MAX / 2 && tagger == Team1)
                        jailPlayer(tempPlayer);
                    else
                        jailPlayer(ptr);
                }
            }
        }

    }

    return;
}

/******************************************************************************
* Move Player
******************************************************************************/
int movePlayer (int playerID, Deltas *d, char c)  //['U', 'D', 'L', 'R']
{
    int x, y;
    printf("\ngamePlayingFlag - %d\n", gamePlayingFlag);

    if (gamePlayingFlag == TRUE)
    {
        struct player *ptr = NULL;
        Cell tempCell_U, tempCell_D, tempCell_L, tempCell_R;

        printf("@%s: Looking for player - %d \n", __FUNCTION__, playerID);
        ptr = search_in_list(playerID, NULL);

        if (NULL == ptr)
        {
            debugPrint("\n@%s: Search [playerID = %d] failed, no such element found\n", __FUNCTION__, playerID);
            return -1;
        }
        else
        {
            debugPrint("\n@%s: Search passed [playerID = %d]\n", __FUNCTION__, ptr->ID);
        }

        x = ptr->PlayerPos.x;
        y = ptr->PlayerPos.y;

        // jail check
        if (ptr->State == 0)
        {
            if ( (y < 200) && (y > 0) )  tempCell_U = cellInfo( y + 1, x + 0);
            if ( (y < 200) && (y > 0) )  tempCell_D = cellInfo( y - 1, x - 0);
            if ( (x < 200) && (x > 0) )  tempCell_L = cellInfo( y - 0, x - 1);
            if ( (x < 200) && (x > 0) )  tempCell_R = cellInfo( y + 0, x + 1);

            debugPrint("\nTrying to do a %c\n", c);
            debugPrint("tempCell_U: type - %c\tlocation - %d, %d\n", tempCell_U.C_Type, tempCell_U.Cell_Pos.x, tempCell_U.Cell_Pos.y);
            debugPrint("tempCell_D: type - %c\tlocation - %d, %d\n", tempCell_D.C_Type, tempCell_D.Cell_Pos.x, tempCell_D.Cell_Pos.y);
            debugPrint("tempCell_L: type - %c\tlocation - %d, %d\n", tempCell_L.C_Type, tempCell_L.Cell_Pos.x, tempCell_L.Cell_Pos.y);
            debugPrint("tempCell_R: type - %c\tlocation - %d, %d\n", tempCell_R.C_Type, tempCell_R.Cell_Pos.x, tempCell_R.Cell_Pos.y);

            if ((c == 'U' || c == 'u' || c == '1'))
            {
                breakWall(playerID, y + 1, x, d);
            }
            if ((c == 'D' || c == 'd' || c == '2'))
            {
                breakWall(playerID, y - 1, x, d);
            }
            if ((c == 'L' || c == 'l' || c == '3'))
            {
                breakWall(playerID, y, x - 1, d);
            }
            if ((c == 'R' || c == 'r' || c == '4'))
            {
                breakWall(playerID, y, x + 1, d);
            }

            // IDid move check
            if ( (c == 'U' || c == 'u' || c == '1') &&
                    tempCell_U.occupied != ptr->team   &&
                    (tempCell_U.C_Type != CT_Wall && tempCell_U.C_Type != CT_Jailj && tempCell_U.C_Type != CT_JailJ))
            {
                ptr->PlayerPos.y--;

                /*
                pickUpItem(playerID, d);

                if (ptr->i != NULL)
                    if (ptr->i->itType == 3)
                        dropItem(playerID, d, *(ptr->i));
                */

                tempCell_U.occupied = ptr->team;

                // tagging check
                tagCheck(ptr);

                add_delta_player(d, ptr, sizeof(struct player));
                print_cells_arroud(playerID);
                return 1;
            }

            if ( (c == 'D' || c == 'd' || c == '2') &&
                    tempCell_D.occupied != ptr->team   &&
                    (tempCell_D.C_Type != CT_Wall && tempCell_D.C_Type != CT_Jailj && tempCell_D.C_Type != CT_JailJ))
            {
                ptr->PlayerPos.y++;

                /*
                pickUpItem(playerID, d);

                if (ptr->i != NULL)
                    if (ptr->i->itType == 3)
                        dropItem(playerID, d, *(ptr->i));
                */

                tempCell_D.occupied = ptr->team;

                // tagging check
                tagCheck(ptr);

                add_delta_player(d, ptr, sizeof(struct player));
                print_cells_arroud(playerID);
                return 1; // move made
            }

            if ( (c == 'L' || c == 'l' || c == '3') &&
                    tempCell_L.occupied != ptr->team   &&
                    (tempCell_L.C_Type != CT_Wall && tempCell_L.C_Type != CT_Jailj && tempCell_L.C_Type != CT_JailJ))
            {
                ptr->PlayerPos.x--;

                /*
                pickUpItem(playerID, d);

                if (ptr->i != NULL)
                    if (ptr->i->itType == 3)
                    {
                        dropItem(playerID, d, *(ptr->i));
                    }
                */

                tempCell_L.occupied = ptr->team;

                // tagging check
                tagCheck(ptr);

                add_delta_player(d, ptr, sizeof(struct player));
                print_cells_arroud(playerID);
                return 1; // move made
            }

            if ( (c == 'R' || c == 'r' || c == '4') &&
                    tempCell_R.occupied != ptr->team   &&
                    (tempCell_R.C_Type != CT_Wall && tempCell_R.C_Type != CT_Jailj && tempCell_R.C_Type != CT_JailJ))
            {
                ptr->PlayerPos.x++;

                /*
                pickUpItem(playerID, d);

                if (ptr->i != NULL)
                    if (ptr->i->itType == 3)
                    {
                        dropItem(playerID, d, *(ptr->i));
                    }
                */

                tempCell_R.occupied = ptr->team;

                // tagging check
                tagCheck(ptr);

                add_delta_player(d, ptr, sizeof(struct player));
                print_cells_arroud(playerID);
                return 1; // move made
            }
            return -1; // did not move
        }
        else
            return -1; // In jail
    }
    return -1; // game not started
}

/******************************************************************************
* Format Maze for output
******************************************************************************/
void formatMaze(char *mazeOutput)
{
    int x = (maze.dimensions.y * maze.dimensions.x);

    int i;

    for (i = 0; i < x; i++)
    {

        int c =  maze.cells[i].C_Type;
        char print = (char) c;

        if ((i % maze.dimensions.y) == 0)
        {
            mazeOutput[i] = print;
        }
        else
        {
            mazeOutput[i] = print;
        }
    }

    int k;
    int temp;

    struct player *ptr = NULL;

    for (k = 0; k < MAX; k++)
    {
        ptr = search_in_list(k, NULL);

        if (NULL == ptr)
        {
            debugPrint("\n Search [playerID = %d] failed, no such element found\n", k);
        }
        else
        {
            debugPrint("\n Search passed [playerID = %d] and checking if she is tagging [playerID = %d]\n", ptr->ID, tempPlayer->ID);

            temp = getPos(ptr->PlayerPos.x, ptr->PlayerPos.y);
            mazeOutput[temp] = 'P';
        }
    }

    return;
}

/******************************************************************************
* Return x dimensions
******************************************************************************/
int dimX()
{
    return maze.dimensions.x;
}

/******************************************************************************
* Return y dimensions
******************************************************************************/
int dimY()
{
    return maze.dimensions.y;
}

/******************************************************************************
* Testing Code
******************************************************************************/
/*
void addPlayerTest(int num)
{
    int i;

    for ( i = 0; i < num; i++)
    {
        addPlayer();
    }


    print_list();

    int temp = 0;

    struct player *ptr = NULL;

    for (i = 0; i < playerCount; i++)
    {
        ptr = search_in_list(i, NULL);

        if (NULL == ptr)
        {
        }
        else
        {
            temp++;
        }
    }

    printf("\nTotal player count is - %d\n", temp);

    for ( i = 0; i <= num; i++)
    {
        removePlayer(i);
    }


    print_list();

    temp = 0;

    ptr = NULL;

    for (i = 0; i < playerCount; i++)
    {
        ptr = search_in_list(i, NULL);

        if (NULL == ptr)
        {
        }
        else
        {
            temp++;
        }
    }

    printf("\nTotal player count is - %d\n", temp);
}

void homeAndJailTest()
{
    int x = 0;

    for (x = 0; x < maze.numOfHomes[0]; x++)
    {
        printf("\nType - %c \nTeam - %d \n[%d, %d]\n", (char)HomeList0[x].C_Type, HomeList0[x].Cell_Team , HomeList0[x].Cell_Pos.x, HomeList0[x].Cell_Pos.y);
    }
    for (x = 0; x < maze.numOfHomes[1]; x++)
    {
        printf("\nType - %c \nTeam - %d \n[%d, %d]\n", (char)HomeList1[x].C_Type, HomeList1[x].Cell_Team , HomeList1[x].Cell_Pos.x, HomeList1[x].Cell_Pos.y);
    }
    for (x = 0; x < maze.numOfJails[0]; x++)
    {
        printf("\nType - %c \nTeam - %d \n[%d, %d]\n", (char)JailList0[x].C_Type, JailList0[x].Cell_Team , JailList0[x].Cell_Pos.x, JailList0[x].Cell_Pos.y);
    }
    for (x = 0; x < maze.numOfJails[1]; x++)
    {
        printf("\nType - %c \nTeam - %d \n[%d, %d]\n", (char)JailList1[x].C_Type, JailList1[x].Cell_Team , JailList1[x].Cell_Pos.x, JailList1[x].Cell_Pos.y);
    }
}

void moveTest(int num)
{
    int temp = 0;
    int move = 0;
    int i;

    for (i = 0; i < num; ++i)
    {
        temp = addPlayer();
        printf("Player id returned : %d\n", temp);
        print_list();
        move = movePlayer (temp, 'U');
        printf("The move was - %d\n", move);
        print_list();
        move = movePlayer (temp, 'D');
        printf("The move was - %d\n", move);
        print_list();
        move = movePlayer (temp, 'L');
        printf("The move was - %d\n", move);
        print_list();
        move = movePlayer (temp, 'R');
        printf("The move was - %d\n", move);
        print_list();
    }
}

int itemTest(void)
{
    Item testIt;
    testIt.itType = Flag_Team1;
    testIt.ItemPos.x = 0;
    testIt.ItemPos.y = 0;
    resetItem(testIt, 1);
}

*/
/*
int main(int argc, char const *argv[])
{
    //startGame();
    //dumpMap();
    return 0;
}
*/
