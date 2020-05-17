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

bool is_fg_only = false;    // Bool to hold the state of foreground only mode


/* FUNCTION DECLARATIONS ----------------------------------------------------*/

void catchSIGINT(int signo);
void change_dir(char **arg_arr);
void get_status(int status);
char *get_input();
void free_memory(char *user_input);


/* MAIN ---------------------------------------------------------------------*/

int main()
{
    char *arg_arr[MAX_ARGS];                            // Array of strings to hold CLI arguments
    char *in_file = NULL;                               // Input file pointer
    char *out_file = NULL;                              // Output file pointer


    
    // Start the shell and keep it running
    while (true)
    {
        // Print the command line and get the input
        char *user_input = get_input();

        // Skip any comments or blank lines
        if (strncmp(user_input, "#", 1) == 0 || strncmp(user_input, " ", 1) == 0)
        {
            continue;
        }

        // // Check for and exit command
        // if (strcmp(arg_arr[0], "exit") == 0)
        // {
        //     // Free memory


        //     exit(EXIT_SUCCESS);
        // }


        printf("%s", user_input);
        free_memory(user_input);
        return 0;
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


// Print the command line and read the input
char *get_input()
{
    // Print the command line
    printf(": ");
    fflush(stdout);

    // String for CLI input
    char *input_buf = malloc(BUF_SIZE * sizeof(char));

    // Clear out the buffer
    memset(input_buf, '\0', sizeof(input_buf));

    // Read the CLI input
    fgets(input_buf, BUF_SIZE, stdin);

    return input_buf;
}


// Free the alocated memory
void free_memory(char *user_input)
{
    // Free the user input string
    free(user_input);
}
