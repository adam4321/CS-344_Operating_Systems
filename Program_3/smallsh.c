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
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>


#define BUF_SIZE    2049    // Maximum number of characters supported
#define ARG_CNT     513     // Maximum number of commands supported


/* FUNCTION DECLARATIONS ----------------------------------------------------*/

void get_status(int status);
void change_dir(char **arg_arr);


/* MAIN ---------------------------------------------------------------------*/

int main()
{
    
    // Start the shell and keep it running
    while (true)
    {
        printf(": ");
        fflush(stdout);
        fflush(stderr);


    }

    return 0;
}


/* FUNCTION DEFINITIONS -----------------------------------------------------*/

// Print the exit value or the terminating signal
void get_status(int status)
{
    if (WIFEXITED(status))
    {
        printf("exit value %i\n", WEXITSTATUS(status));
    }
    else
    {
        printf("terminated by signal %i", status);
    }
    
}


// Implementation of the built-in cd for changing directories
void change_dir(char **arg_arr)
{
    char *dir;

    // If no directory is requested then change to user home directory
    if (arg_arr[1] == 0)
    {
        dir = getenv("HOME");
        chdir(dir);
    }
    // If a directory argument exists, then change to that directory
    else
    {
        dir = arg_arr[1];
        chdir(dir);
    }
    // If failed, then print an error
    if (chdir(dir) != 0)
    {
        printf("Invalid directory name.\n");
    }
}
