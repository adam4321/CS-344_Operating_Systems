/******************************************************************************
**  Author:      Adam Wright
**  Email:       wrighada@oregonstate.edu
**  Date:        5/6/2020
**  Description: Program 3 for spring 2020 OSU cs-344. This program implements
**               a basic bare-bones shell for Unix.
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


/* FUNCTION DECLARATIONS ----------------------------------------------------*/

void change_dir(char **arg_arr);


/* MAIN ---------------------------------------------------------------------*/

int main()
{


    return 0;
}


/* FUNCTION DEFINITIONS -----------------------------------------------------*/

void change_dir(char **arg_arr)
{
    char *dir;

    if (arg_arr[1] == NULL)
    {
        dir = getenv("HOME");
        chdir(dir);
    }
    else
    {
        dir = arg_arr[1];
    }
    
    if (chdir != NULL)
    {
        printf("Invalid directory name.\n");
    }
}
