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
#include <sys/stat.h>
#include <signal.h>


#define BUF_SIZE    2049    // Maximum number of characters supported
#define MAX_ARGS     512    // Maximum number of commands supported

bool is_fg_only = false;        // Bool to hold the state of foreground only mode


/* FUNCTION DECLARATIONS ----------------------------------------------------*/

void free_memory(char *user_input, int arg_count, char *arg_arr[]);
char *get_input();
void print_arr(int arg_count, char *arg_arr[]);
void term_bg_procs(pid_t pid_count, pid_t bg_pid_arr[]);
void tstp_handler(int tstp_sig);
void change_dir(char **arg_arr);
void get_status(int status);
void file_err_msg(char *cur_file, int open_file, int file_dsc);


/* MAIN ---------------------------------------------------------------------*/

int main()
{
    int arg_count = 0;              // Value to hold the number of tokens entered
    char *current_arg;              // String to hold each token to be parsed
    char *arg_arr[MAX_ARGS];        // Array of strings to hold CLI arguments
    int status = 0;                 // Exit status of last foreground process
    bool bg_mode = false;           // Boolean flag holding state of background mode
    int cur_status = 0;             // State of the most recent exit status
    pid_t cur_pid;                  // The current background process
    pid_t pid_count = 0;
    pid_t bg_pid_arr[MAX_ARGS];     // Array of pids of all background processes
    char *in_file;                  // Input file pointer
    char *out_file;                 // Output file pointer

    // Signal handler structs
    struct sigaction catch_sig_int = {0};
    struct sigaction catch_sig_tstp = {0};

    // SIGINT handler
    catch_sig_int.sa_handler = SIG_IGN;
    sigaction(SIGINT, &catch_sig_int, NULL);

    // SIGTSTP handler
    catch_sig_tstp.sa_handler = &tstp_handler;
    sigfillset(&catch_sig_tstp.sa_mask);
    catch_sig_tstp.sa_flags = SA_RESTART;
    sigaction(SIGTSTP, &catch_sig_tstp, NULL);


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
            get_status(cur_status);
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
            else if (strstr(current_arg, "$$") != NULL)
            {
                // Pull the shell's current process id
                pid_t shell_pid = getpid();

                // Convert the pid to a string and assign to current_arg
                char str_pid[12];
                sprintf(str_pid, "%d", shell_pid);
                current_arg = strdup(str_pid);
            }

            // Enter the string into the arguments array and advance the arg
            arg_arr[arg_count] = strdup(current_arg);

            // Advance to the next user input token
            arg_count++;
            arg_arr[arg_count] = NULL;
            current_arg = strtok(NULL, " \n");
        }


        // // COMMENT OUT BEFORE RELEASE !! print array for testing
        // print_arr(arg_count, arg_arr);


        // Check for and process change directory
        if (strcmp(arg_arr[0], "cd") == 0)
        {
            change_dir(arg_arr);
            free_memory(user_input, arg_count, arg_arr);

            // Run prompt loop again
            continue;
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
        // Fork a new child process and execute it
        else
        {
            cur_pid = fork();

            // Failed fork - should never happen!!
            if (cur_pid == -1)
            {
                cur_status = 1;
                fprintf(stderr, "Error: process not started.\n");
                term_bg_procs(pid_count, bg_pid_arr);
                break;
            }
            // Successfully forked child process 
            else if (cur_pid == 0)
            {
                if (bg_mode == false)
                {
                    catch_sig_int.sa_handler = SIG_DFL;
                    sigaction(SIGINT, &catch_sig_int, NULL);
                }
                // Open input file for redirection
                if (in_file != NULL)
                {
                    int open_file = open(in_file, O_RDONLY);

                    // Check for success and print message on error
                    file_err_msg(in_file, open_file, 0)
                    close(open_file);
                }
                // Open output file for redirection
                if (out_file != NULL)
                {
                    int open_file = open(out_file, O_WRONLY|O_CREAT|O_TRUNC, 0644);

                    // Check for success and print message on error
                    file_err_msg(out_file, open_file, 1)
                    close(open_file);
                }

                if (execvp(arg_arr[0], arg_arr))
                {
                    printf("%s: no such file or directory\n", arg_arr[0]);
                    fflush(stdout);
                    exit(1);
                }
                else
                {
                    if (bg_mode == false)
                    {
                        cur_pid = waitpid(cur_pid, &cur_status, 0);

                        if (WIFSIGNALED(cur_status))
                        {
                            printf(" terminated by signal %d\n", cur_status);
                            fflush(stdout);
                        }
                    }
                    
                }
                
            }
            // Continuing parent process
            else
            {
                if (bg_mode == false)
                {
                    cur_pid = waitpid(cur_pid, &cur_status, 0);

                    if (WIFSIGNALED(cur_status))
                    {
                        printf(" terminated by signal %d\n", cur_status);
                        fflush(stdout);
                    }
                }
                if (bg_mode == true)
                {
                    waitpid(cur_pid, &cur_status, WNOHANG);

                    bg_pid_arr[pid_count] = cur_pid;

                    printf("background pid is %d\n", cur_pid);
                    fflush(stdout);
                    pid_count++;
                }
            }
        }
        


        cur_pid = waitpid(-1, &cur_status, WNOHANG);

        while (cur_pid > 0)
        {
            if (WIFEXITED(cur_status))
            {
                printf("background pid %d is done: exit value %d\n", cur_pid, cur_status);
                fflush(stdout);
            }
            else
            {
                printf("background pid %d is done: terminated by signal %d\n", cur_pid, cur_status);
                fflush(stdout);
            }
            
            cur_pid = waitpid(-1, &cur_status, WNOHANG);
        }

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
        fflush(stdout);
        i++;
    }

    printf("\n");
}


// End all background processes
void term_bg_procs(pid_t pid_count, pid_t bg_pid_arr[])
{
    pid_t i = 0;

    while (i < pid_count)
    {
        kill(bg_pid_arr[i], SIGTERM);
        i++;
    }
}


// Handler to catch (^Z) TSTP signal and switch foreground only mode
void tstp_handler(int tstp_sig)
{
    // Switch to foreground only and print message
    if (is_fg_only == false)
    {
        is_fg_only = true;
        write(STDOUT_FILENO, "\nEntering foreground-only mode (& is now ignored)\n: ", 52);
        fflush(stdout);
    }
    // Switch back to normal mode and print message
    else
    {
        is_fg_only = false;
        write(STDOUT_FILENO, "\nExiting foreground-only mode\n: ", 32);
        fflush(stdout);
    }
} 


// Print the exit value or the terminating signal
void get_status(int cur_status)
{
    // Prints the exit status of the last foreground process
    if (WIFEXITED(cur_status))
    {
        printf("exit value %i\n", WEXITSTATUS(cur_status));
    }
    // Prints the terminating signal of the last foreground process
    else
    {
        printf("terminated by signal %i", cur_status);
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
            printf("%s: no such file or directory.\n", arg_arr[1]);
        }
    }
}


// Function to print file open and close messages
void file_err_msg(char *cur_file, int open_file, int file_dsc)
{
    // Pick input or output for the message 
    if (file_dsc == 0)
    {
        char *file_type = "input";
    }
    else if (file_dsc == 1)
    {
        char *file_type = "output";
    }
    

    if(open_file == -1)
    {
        fprintf(stderr, "cannot open file %s for %s\n", cur_file, file_type);
        fflush(stderr);
        exit(1);
    }
    else if(dup2(open_file, file_dsc) == -1)
    {
        fprintf(stderr, "error in redirecting %s\n", file_type);
        fflush(stderr);
        exit(1);
    }
}
