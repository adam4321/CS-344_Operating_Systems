/******************************************************************************
**  Author:      Adam Wright
**  Email:       wrighada@oregonstate.edu
**  Date:        4/18/2020
**  Description: Program 2 Room generationg program for spring 2020 OSU cs-344. 
**               This program generates a directory with 7 room definition
**               files within it. It creates a new directory of randomly
**               assigned rooms each time that it is run.
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>


/* Global Constants */
#define ROOM_MAX    10
#define ROOM_COUNT  7
#define CONNECT_MIN 3
#define CONNECT_MAX 6

/* Definition for bool type */
typedef enum
{
    false,
    true
} bool;

/* Definition for Room struct */
struct Room
{
    char* name;
    char* type;
    int connect_count;
    struct Room* out_connect[CONNECT_MAX];
};

/* Room arrays */
char* room_names[ROOM_MAX] = 
{
    "Cheese",
    "Dinosaur",
    "Key",
    "Passage",
    "Stick",
    "Map",
    "Rats",
    "Mummy",
    "Gate",
    "Clock"
};

char* room_types[3] = 
{
    "START_ROOM",
    "END_ROOM",
    "MID_ROOM"
};

char* file_names[ROOM_COUNT] = 
{
    "AAAA_room",
    "BBBB_room",
    "CCCC_room",
    "DDDD_room",
    "EEEE_room",
    "FFFF_room",
    "GGGG_room"
};


/* FUNCTION DECLARATIONS ----------------------------------------------------*/

bool IsGraphFull();
void AddRandomConnection(struct Room* room_arr[]);
struct Room* GetRandomRoom(struct Room* room_arr[]);
bool CanAddConnectionFrom(struct Room* x);
bool ConnectionAlreadyExists(struct Room* x, struct Room* y);
void ConnectRoom(struct Room* x, struct Room* y);
bool IsSameRoom(struct Room* x, struct Room* y);


/* MAIN ---------------------------------------------------------------------*/

int main()
{
    /* Seed the random function */
    srand(time(0));
    
    /* Create a blank array for the directory name result and clear it */
    char Dir_Name[32];
    memset(Dir_Name, '\0', sizeof(Dir_Name));

    /* Retrieve the current process id and create the directory */
    int pid = getpid();
    sprintf(Dir_Name, "wrighada.rooms.%d", pid);

    /* Check that the directory doesn't exist, and if not create it */
    struct stat st= {0};
    if (stat(Dir_Name, &st) == -1)
    {
        /* Create the directory with the needed permissions */
        mkdir(Dir_Name, 0755);
    }
    

    

    return 0;
}


/* FUNCTION DEFINITIONS -----------------------------------------------------*/

/* Create all connections in graph */
void CreateConnections(struct Room* room_arr[])
{   
    while (IsGraphFull(room_arr) == false)
    {
        AddRandomConnection(room_arr);
    }
}


/* Returns true if all rooms have 3 to 6 outbound connections, false otherwise */
bool IsGraphFull(struct Room* room_arr[])
{
    int i = 0;

    /* Iterate through the array of rooms and check if each has reached the min connections */ 
    while (i <  ROOM_COUNT)
    {
        if (room_arr[i]->connect_count < CONNECT_MIN)
        {
            return false;
        }
        else
        {
            i++;
        }
    }
}


/* Adds a random, valid outbound connection from a Room to another Room */
void AddRandomConnection(struct Room* room_arr[])
{
    struct Room* A;  /* Maybe a struct, maybe global arrays of ints */
    struct Room* B;

    while (true)
    {
        A = GetRandomRoom(room_arr);

        if (CanAddConnectionFrom(A) == true)
        break;
    }

    do
    {
        B = GetRandomRoom(room_arr);
    }
    while (CanAddConnectionFrom(B) == false || IsSameRoom(A, B) == true || ConnectionAlreadyExists(A, B) == true);

    ConnectRoom(A, B);  /* TODO: Add this connection to the real variables, */
    ConnectRoom(B, A);  /* because this A and B will be destroyed when this function terminates */
}


/* Returns a random Room, does NOT validate if connection can be added */
struct Room* GetRandomRoom(struct Room* room_arr[])
{   
    /* Set i to an int from 0 - 6 and select the i'th room from the array of structs */
    int i = rand() % ROOM_COUNT;
    return room_arr[i];
}


/* Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise */
bool CanAddConnectionFrom(struct Room* x)
{
    if (x->connect_count < CONNECT_MAX)
    {
        return true;
    }
    else
    {
        return false;
    }
}


/* Returns true if a connection from Room x to Room y already exists, false otherwise */
bool ConnectionAlreadyExists(struct Room* x, struct Room* y)
{
    int i = 0;

    /* Iterate through the possible number of connections */
    while (i < CONNECT_MAX)
    {
        /* If it hits 0, then no connection was found */
        if (x->out_connect[i] == 0)
        {
            return false;
        }
        /* If the name properties match, then a connection already exists */
        else if (strcmp(x->out_connect[i]->name, y->name) == 0)
        {
            return true;
        }
        else
        {
            i++;
        }
    }
    /* No match found, then no connection exists */
    return false;
}


/* Connects Rooms x and y together, does not check if this connection is valid */
void ConnectRoom(struct Room* x, struct Room* y)
{
    int i = 0;

    /* Loop until the first open position */
    while ((i < CONNECT_MAX) && (x->out_connect[i] != 0))
    {
        i++;
    }
    
    /* Add y to x's list at the first open position in the array */
    x->out_connect[i] = y;
    x->connect_count++;
}


/* Returns true if Rooms x and y are the same Room, false otherwise */
bool IsSameRoom(struct Room* x, struct Room* y)
{
    /* Room names are unique, so compare them to find if the rooms are the same */
    if (strcmp(x->name, y->name) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
