/******************************************************************************
**  Author:      Adam Wright
**  Email:       wrighada@oregonstate.edu
**  Date:        4/18/2020
**  Description: Program 2 for spring 2020 OSU cs-344. 
**
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


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
    struct Room* out_connect[6];
};

const int ROOM_COUNT = 7;


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
    srand(time(0));
    
    printf("hello world\n");

    // /* Create all connections in graph */
    // while (IsGraphFull() == false)
    // {
    //     AddRandomConnection(struct Room* room_arr[]);
    // }




    return 0;
}


/* FUNCTION DEFINITIONS -----------------------------------------------------*/

/* Returns true if all rooms have 3 to 6 outbound connections, false otherwise */
bool IsGraphFull(struct Room* room_arr[])
{
    int i = 0;

    /* Iterate through the array of rooms and check if each has reached the min connections */ 
    while (i <  ROOM_COUNT)
    {
        if (room_arr[i]->connect_count < 3)
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
    if (x->connect_count < 6)
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
    return;
}


/* Connects Rooms x and y together, does not check if this connection is valid */
void ConnectRoom(struct Room* x, struct Room* y)
{
    
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
