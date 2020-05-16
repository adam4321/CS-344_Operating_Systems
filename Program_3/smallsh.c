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
#include <signal.h>


#define BUF_SIZE    2049    // Maximum number of characters supported
#define MAX_ARGS     512    // Maximum number of commands supported

bool is_fg_only = false;


/* FUNCTION DECLARATIONS ----------------------------------------------------*/

void catchSIGINT(int signo);
void change_dir(char **arg_arr);
void get_status(int status);


/* MAIN ---------------------------------------------------------------------*/

int main()
{
    char *input_buf = malloc(BUF_SIZE);     // String for CLI input
    char *arg_arr[MAX_ARGS];                // Array of strings to hold CLI arguments
    char *in_file = NULL;                   // Input file pointer
    char *out_file = NULL;                  // Output file pointer


    
    // Start the shell and keep it running
    while (true)
    {
        printf(": ");
        fflush(stdout);
        fflush(stderr);

        // Skip any comments or blank lines
        if (strncmp(input_buf, "#", 1) == 0 || strncmp(input_buf, "\0", 1) == 0)
        {
            continue;
        }

        // Check for and exit command
        if (strcmp(arg_arr[0], "exit") == 0)
        {
            // Free memory


            exit(EXIT_SUCCESS);
        }



    }

    return 0;
}


/* FUNCTION DEFINITIONS -----------------------------------------------------*/

void catchSIGINT(int signo)
{
  char* message = "SIGINT. Use CTRL-Z to Stop.\n";
  write(STDOUT_FILENO, message, 28);
}


// Print the exit value or the terminating signal
void get_status(int status)
{
    // Prints the exit status of the last foreground process
    if (WIFEXITED(status))
    {
        printf("exit value %i\n", WEXITSTATUS(status));
    }
    // Prints the terminating signal of the last foreground process
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
