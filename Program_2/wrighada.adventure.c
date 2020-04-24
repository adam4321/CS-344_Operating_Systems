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


/* Global Constants */
#define ROOM_COUNT  7
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
    struct Room* out_connect[CONNECT_MAX];
};

/* Array holding the 3 types of the rooms */
char* room_types[3] = 
{
    "START_ROOM",
    "END_ROOM",
    "MID_ROOM"
};


int main(void)
{
    
    

    return 0;
}
