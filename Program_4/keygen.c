/******************************************************************************
**  Author:       Adam Wright
**  Email:        wrighada@oregonstate.edu
**  Date:         5/23/2020
**  Description:  OSU cs-344 spring 2020 Project 4. 
**
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define CHAR_SET_SIZE   27

const char *CHAR_SET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";


/* MAIN ---------------------------------------------------------------------*/

int main(int argc, char **argv)
{
    // Check for the correct number of arguments
    if (argc != 2)
    {
        fprintf(stderr, "Error: incorrect argument count to %s\n", argv[0]);
        exit(1);
    }

    int input_length = atoi(argv[1]);

    // Seed the random function
    srand(time(0));

    // Generate the key string from the character set
    int i = 0;
    while (i < input_length)
    {
        printf("%c", CHAR_SET[rand() % CHAR_SET_SIZE]);
        i++;
    }

    // End the string with a newline
    printf("\n");

    return 0;
}
