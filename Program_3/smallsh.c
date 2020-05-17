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

void free_memory(char *user_input);
char *get_input();
void catchSIGINT(int signo);
void change_dir(char **arg_arr);
void get_status(int status);


/* MAIN ---------------------------------------------------------------------*/

int main()
{
    int arg_count = 0;              // Value to hold the number of tokens entered
    char *current_arg;              // String to hold each token to be parsed
    char *arg_arr[MAX_ARGS];        // Array of strings to hold CLI arguments
    char *param_expand[BUF_SIZE];   // String to hold the current shell pid if $$ is found
    char *in_file;                  // Input file pointer
    char *out_file;                 // Output file pointer


    // Start the shell and keep it running
    while (true)
    {
        // Print the command line and get the input
        char *user_input = get_input();

        // Skip any comments or blank lines
        if (user_input[0] == '#' || user_input[0] == ' ' || user_input[0] == '\n')
        {
            // Free memory
            free_memory(user_input);

            // Run prompt loop again
            continue;
        }

        // Pull the first token from the user input
        current_arg = strtok(user_input, " \n");

        // Check for an exit command
        if (strcmp(current_arg, "exit") == 0)
        {
            // Free memory
            free_memory(user_input);

            // Exit the shell with a successfull status
            exit(EXIT_SUCCESS);
        }

        arg_count = 0;

        while (current_arg != NULL)
        {
            // Check for input redirection
            if (strcmp(current_arg, "<") == 0)
            {
                // Pull the next argument
                current_arg = strtok(NULL, " \n");

                // Assign the input file
                in_file = strdup(current_arg);
                current_arg = strtok(NULL, " \n");
            }
            // Check for output redirection
            else if (strcmp(current_arg, ">") == 0)
            {
                // Pull the next argument
                current_arg = strtok(NULL, " \n");

                // Assign the output file
                out_file = strdup(current_arg);
                current_arg = strtok(NULL, " \n");
            }
            else
            {
                if (strstr(current_arg, "$$") != NULL)
                {
                    int shell_pid = getpid();
                    // strcpy( , current_arg);
                    // ex
                }
            }
            
        }




        printf("%s", user_input);

        // Free the memory alocated during the current loop
        free_memory(user_input);
    }

    return 0;
}


/* FUNCTION DEFINITIONS -----------------------------------------------------*/

// Free the alocated memory
void free_memory(char *user_input)
{
    // Free the user input string
    free(user_input);
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
    // If no directory is requested then change to user home directory
    if (arg_arr[1] == NULL)
    {
        chdir(getenv("HOME"));
    }
    // If a directory argument exists, then try to change to that directory
    else
    {
        // If failed, then print an error
        if (chdir(arg_arr[1]) != 0)
        {
            printf("No such file or directory.\n");
        }
    }
}
