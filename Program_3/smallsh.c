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


/* FUNCTION DECLARATIONS ----------------------------------------------------*/

void free_memory(char *user_input, int arg_count, char *arg_arr[]);
char *get_input();
void print_arr(int arg_count, char *arg_arr[]);
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
    int status = 0;                 // Exit status of last foreground process
    bool is_fg_only = false;        // Bool to hold the state of foreground only mode
    bool bg_mode = false;           // Boolean flag holding state of background mode
    char *in_file;                  // Input file pointer
    char *out_file;                 // Output file pointer


    // Start the shell and keep it running
    while (true)
    {
        // Print the command line and get the input
        char *user_input = get_input();
        arg_count = 0;
        bg_mode = false;

        // Skip any comments or blank lines
        if (user_input[0] == '#' || user_input[0] == ' ' || user_input[0] == '\n')
        {
            free_memory(user_input, arg_count, arg_arr);

            // Run prompt loop again
            continue;
        }


        // Pull the first token from the user input
        current_arg = strtok(user_input, " \n");

        // Check for an exit command
        if (strcmp(current_arg, "exit") == 0)
        {
            free_memory(user_input, arg_count, arg_arr);

            // Exit the shell with a successfull status
            exit(EXIT_SUCCESS);
        }
        // Print status of the last foreground process
        else if (strcmp(current_arg, "status") == 0)
        {
            get_status(status);
            free_memory(user_input, arg_count, arg_arr);

            // Run prompt loop again
            continue;
        }


        // Parse user input beyond the first command
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
                    
                }
            }

            // Enter the string into the arguments array and advance the arg
            arg_arr[arg_count] = strdup(current_arg);

            // Advance to the next user input token
            arg_count++;
            arg_arr[arg_count] = NULL;
            current_arg = strtok(NULL, " \n");
        }


        // Check for and process change directory
        if (strcmp(arg_arr[0], "cd") == 0)
        {
            change_dir(arg_arr);
        }
        // Check for and process background process request
        else if (strcmp(arg_arr[arg_count - 1], "&") == 0)
        {
            arg_arr[arg_count - 1] = "\0";

            if (is_fg_only == false)
            {
                bg_mode = true;
            }
        }


        // // COMMENT OUT BEFORE RELEASE !! print array for testing
        // print_arr(arg_count, arg_arr);

        // Free the memory alocated during the current loop
        free_memory(user_input, arg_count, arg_arr);
    }
}


/* FUNCTION DEFINITIONS -----------------------------------------------------*/

// Free the alocated memory
void free_memory(char *user_input, int arg_count, char *arg_arr[])
{
    // Free the user input string
    free(user_input);

    // Free the arguments array
    int i = 0;
    while (i < arg_count)
    {
        free(arg_arr[i]);
        i++;
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


// FOR TESTING - COMMENT OUT BEFORE RELEASE !! print the arguments array
void print_arr(int arg_count, char *arg_arr[])
{
    int i = 0;

    while (i < arg_count)
    {
        printf("%s ", arg_arr[i]);
        i++;
    }

    printf("\n");
}


// 
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
