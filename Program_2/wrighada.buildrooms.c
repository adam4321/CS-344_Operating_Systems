/******************************************************************************
**  Author:      Adam Wright
**  Email:       wrighada@oregonstate.edu
**  Date:        4/18/2020
**  Description: Program 2 Room generating program for spring 2020 OSU cs-344. 
**               This program generates a directory with 7 room definition
**               files within it. It creates a new directory of randomly
**               assigned rooms each time that it is run.
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>


/* Global Constants */
#define ROOM_MAX    10
#define ROOM_COUNT  7
#define CONNECT_MIN 3
#define CONNECT_MAX 6

/* Definition for the bool type */
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
    struct Room *out_connect[CONNECT_MAX];
};

/* Array to hold the created rooms */
struct Room *Room_Arr[ROOM_COUNT];

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


/* FUNCTION DECLARATIONS ----------------------------------------------------*/

void CreateConnections(struct Room *room_arr[]);
bool IsGraphFull(struct Room *room_arr[]);
void AddRandomConnection(struct Room *room_arr[]);
struct Room* GetRandomRoom(struct Room *room_arr[]);
bool CanAddConnectionFrom(struct Room *x);
bool ConnectionAlreadyExists(struct Room *x, struct Room *y);
void ConnectRoom(struct Room *x, struct Room *y);
bool IsSameRoom(struct Room *x, struct Room *y);


/* MAIN ---------------------------------------------------------------------*/

int main()
{
    /* Seed the random function */
    srand(time(NULL));
    
    /* Create a blank array for the directory name result and clear it */
    char Dir_Name[32];
    memset(Dir_Name, '\0', sizeof(Dir_Name));

    /* Retrieve the current process id and create the directory */
    int pid = getpid();
    sprintf(Dir_Name, "./wrighada.rooms.%d", pid);

    /* Check that the directory doesn't exist, and if not create it */
    struct stat st= {0};
    if (stat(Dir_Name, &st) == -1)
    {
        /* Create the directory with the needed permissions */
        mkdir(Dir_Name, 0755);
    }
    
    /* Create an array of 0-9 to select the rooms */
    int Room_Select[ROOM_MAX];
    int i = 0;
    while (i < ROOM_MAX)
    {
        Room_Select[i] = i;
        i++;
    }

    /* Randomize the numbers in the Room selection array using Fisher–Yates shuffle Algorithm */
    int j;
    for (i = (ROOM_MAX - 1); i > 0; i--)
    {
        j = rand() % (i+1);

        /* Swap the values */
        int temp = Room_Select[i];
        Room_Select[i] = Room_Select[j];
        Room_Select[j] = temp;
    }

    /* Fill [Room_Arr] with struct Room* using the order of the random indices in Room_Select */
    for (i = 0; i < ROOM_COUNT; i++)
    {
        Room_Arr[i] = malloc(sizeof(struct Room));
   
        /* Set the random room name */
        Room_Arr[i]->name = room_names[Room_Select[i]];

        /* Set the room type */
        if (i == 0)
        {
            Room_Arr[i]->type = room_types[0];
        }
        else if (i == (ROOM_COUNT - 1))
        {
            Room_Arr[i]->type = room_types[1];
        }
        else
        {
            Room_Arr[i]->type = room_types[2];
        }

        /* Set the connection count */
        Room_Arr[i]->connect_count = 0;

        /* Set all of the connection array values to 0 */
        for (j = 0; j < CONNECT_MAX; j++)
        {
            Room_Arr[i]->out_connect[j] = NULL;
        }
    }


    /* Pass the array of room pointers to have the connections created */
    CreateConnections(Room_Arr);


    /* Print out the connected rooms to 7 files */
    for (i = 0; i < ROOM_COUNT; i++)
    {
        /* Create a blank string with the room name for the file name */
        char File_Name[64];
        memset(File_Name, '\0', sizeof(File_Name));
        sprintf(File_Name, "./%s/%s_room", Dir_Name, Room_Arr[i]->name);

        /* Create a new file */
        FILE *new_file = fopen(File_Name, "w+");
        fprintf(new_file, "ROOM NAME: %s\n", Room_Arr[i]->name);
        
        /* Print the connections to the file */
        j = 0;
        while (j < Room_Arr[i]->connect_count)
        {
            fprintf(new_file, "CONNECTION %d: %s\n", j, Room_Arr[i]->out_connect[j]->name);
            j++;
        }
        
        /* Print the room's type */
        fprintf(new_file, "ROOM TYPE: %s", Room_Arr[i]->type);
        
        /* Close the output file */
        fclose(new_file);
    }

    /* Free the created rooms */
    for (i = 0; i < ROOM_COUNT; i++)
    {
        free(Room_Arr[i]);
    }

    return 0;
}


/* FUNCTION DEFINITIONS -----------------------------------------------------*/

/* Create all connections in graph */
void CreateConnections(struct Room *room_arr[])
{   
    /* Iterate through all of the created room structs and create the connections graph */
    while (IsGraphFull(room_arr) == false)
    {
        AddRandomConnection(room_arr);
    }
}


/* Returns true if all rooms have 3 to 6 outbound connections, false otherwise */
bool IsGraphFull(struct Room *room_arr[])
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
    return true;
}


/* Adds a random, valid outbound connection from a Room to another Room */
void AddRandomConnection(struct Room *room_arr[])
{
    struct Room *A;
    struct Room *B;

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

    ConnectRoom(A, B);  /* Add this connection to the real variables, */
    ConnectRoom(B, A);  /* because this A and B will be destroyed when this function terminates */
}


/* Returns a random Room, does NOT validate if connection can be added */
struct Room *GetRandomRoom(struct Room *room_arr[])
{   
    /* Set i to an int from 0 - 6 and select the i'th room from the array of structs */
    int i = rand() % ROOM_COUNT;
    return room_arr[i];
}


/* Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise */
bool CanAddConnectionFrom(struct Room *x)
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
bool ConnectionAlreadyExists(struct Room *x, struct Room *y)
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
void ConnectRoom(struct Room *x, struct Room *y)
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
bool IsSameRoom(struct Room *x, struct Room *y)
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
