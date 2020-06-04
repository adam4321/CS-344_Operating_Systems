/******************************************************************************
**  Author:       Adam Wright
**  Email:        wrighada@oregonstate.edu
**  Date:         5/23/2020
**  Description:  OSU cs-344 spring 2020 Project 4. This program acts as a
**                server daemon and is designed to be run in the background to
**                listen to the port that was assigned at startup. It accepts
**                get and post requests from the client program otp.
**
**  Program use:  otp_d lintening_port
**  Example call: otp_d 57111 &
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <dirent.h>


#define MAX_CONNECTS     5          // Maximum number of concurrent connections
#define MAX_BUFFER_SIZE  75000      // Maximum size of a network message


/* FUNCTION DECLARATIONS -----------------------------------------------------*/

void error(const char *msg);


/* MAIN ---------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
	int listenSocketFD;                 // Holds the listening socket
    int establishedConnectionFD;        // Holds the network connection for child proc
    int portNumber;                     // Holds the port number from argv[1]
    int charsRead;                      // Holds the number of characters sent from client
	socklen_t sizeOfClientInfo;         // Holds the client socket information
	char buffer[MAX_BUFFER_SIZE];       // Holds the string received from the client
	struct sockaddr_in serverAddress;   // Holds the server address
    struct sockaddr_in clientAddress;   // Holds the client address
    pid_t spawn_pid;                    // Holds the process id of the newly forked process
    int status = 0;                     // Holds the status of the current process exit
    int num_connects = 0;               // Holds the number of current connections


	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

    // Enable the socket to begin listening
    if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
        error("ERROR on binding");
    listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections


    // Infinite loop creating child processes to manage 5 connection on the port 
    while (1)
    {
        // Support up to 5 simultaneous connections
        if (num_connects >= MAX_CONNECTS)
        {
            fprintf(stderr, "All connections currently used\n");
            continue;
        }

        // Accept a connection, blocking if one is not available until one connects
        sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
        establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept

        // Report a connection error
        if (establishedConnectionFD < 0)
        {
            error("ERROR on accept");
            continue;
        }


        // Fork a new process for the new connection
        spawn_pid = fork();

        // Verify that the child process started
        if (spawn_pid == -1)
        {
            fprintf(stderr, "New connection had a failed process startup\n");
            close(establishedConnectionFD);
            continue;
        }
        // Child process to handle a new connection
        else if (spawn_pid == 0)
        {
            // PROJECT REQUIREMENT THAT THE SERVER SLEEP FOR 2 SECONDS AFTER CONNECTION ACCTEPTANCE
            sleep(2);


            // Increment the number of connections
            num_connects++;

            // Get the message from the client and display it
            memset(buffer, '\0', sizeof(buffer));
            charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer), 0); // Read the client's message from the socket
            if (charsRead < 0) error("ERROR reading from socket");

            // Tokenize the request string and pull out the mode
            char *token = strtok(buffer, " ");
            char *mode = strdup(token);

            // Pull out the user token
            token = strtok(NULL, " ");
            char *user = strdup(token);


            // SERVER POST MODE -----------------------------------------------
            if (strcmp(mode, "post") == 0)
            {
                // If in post mode then pull out the message from the request
                token = strtok(NULL, "");
                char *message = strdup(token);
                strcat(message, "\n");

                // Check if the user's directory exists, and if not create it
                struct stat st= {0};
                if (stat(user, &st) == -1)
                {
                    // Create the directory with the needed permissions
                    mkdir(user, 0755);
                }

                // Build up the ciphertext file path from username and the current process pid
                char file_path[255];
                memset(file_path, '\0', sizeof(file_path));
                int pid = getpid();
                sprintf(file_path, "%s/%d", user, pid);

                // Create a new file for the ciphertext
                FILE *ciphertext = fopen(file_path, "w+");

                // Write the encoded message to the ciphertext file
                fprintf(ciphertext, message);

                // Print the path to the file
                printf("./%s\n", file_path);

                // Close the file and free the message
                fclose(ciphertext);
                free(message);
            }


            // SERVER GET MODE ------------------------------------------------
            if (strcmp(mode, "get") == 0)
            {
                // Assemble the directory path
                char dir_path[255];
                memset(dir_path, '\0', sizeof(dir_path));
                sprintf(dir_path, "./%s", user);

                // Assemble the file name from the username and oldest 
                char file_name[255];
                memset(file_name, '\0', sizeof(file_name));

                // Store current time to compare against older files
                int oldest_time = (int)time(NULL);

                DIR *dir_to_check; // Holds the directory we're starting in
                struct dirent *file_in_dir; // Holds the current file in the starting dir
                struct stat file_attr; // Holds information we've gained about the file

                dir_to_check = opendir(dir_path); // Open up the directory for the requested user

                // Make sure the current directory could be opened, else print an error
                if (dir_to_check != NULL)
                {
                    // Search the directory for the oldest file
                    while ((file_in_dir = readdir(dir_to_check)) != NULL) // Check each entry in dir
                    {
                        if (strstr(file_in_dir->d_name, ".") == NULL) // If entry isn't . or ..
                        {
                            // Assemble the relative path to the file
                            char cur_file_path[255];
                            memset(cur_file_path, '\0', sizeof(cur_file_path));
                            sprintf(cur_file_path, "%s/%s", dir_path, file_in_dir->d_name);

                            stat(cur_file_path, &file_attr); // Get attributes of the entry

                            if ((int)file_attr.st_mtime < oldest_time) // If this time is lower
                            {
                                // The oldest file time is stored
                                oldest_time = file_attr.st_mtime;
                                memset(file_name, '\0', sizeof(file_name));
                                strcpy(file_name, file_in_dir->d_name); // Store the directory
                            }
                        }
                    }
                    
                    closedir(dir_to_check); // Close the directory we opened

                    // Print an error if no file was found
                    if (file_name[0] != '\0')
                    {
                        // Piece together the path of the requested ciphertext file
                        char full_path[255];
                        memset(full_path, '\0', sizeof(full_path));
                        sprintf(full_path, "%s/%s", dir_path, file_name);
                        
                        // Create a buffer for the ciphertext data
                        char enc_msg[MAX_BUFFER_SIZE];
                        memset(enc_msg, '\0', sizeof(enc_msg));

                        // Read in the encoded message from the file
                        FILE *ciphertext = fopen(full_path, "r");
                        fgets(enc_msg, sizeof(enc_msg) - 1, ciphertext);
                        enc_msg[strcspn(enc_msg, "\n")] = '\0';

                        // Close and delete the ciphertext file
                        fclose(ciphertext);
                        remove(full_path);

                        // Send a Success message back to the client
                        charsRead = send(establishedConnectionFD, enc_msg, sizeof(enc_msg), 0); // Send success back
                        if (charsRead < 0) error("ERROR writing to socket");
                    }
                    else
                    {
                        // Return an error to the client
                        char no_dir[255];
                        memset(no_dir, '\0', sizeof(no_dir));

                        sprintf(no_dir, "No file for %s was found\n", user);
                        fprintf(stderr, "No file for %s was found\n", user);

                        // Send a Error message back to the client
                        charsRead = send(establishedConnectionFD, no_dir, sizeof(no_dir), 0); // Send error back
                        if (charsRead < 0) error("ERROR writing to socket");
                    }
                }
                else
                {                    
                    // Return an error to the client
                    char no_dir[255];
                    memset(no_dir, '\0', sizeof(no_dir));

                    sprintf(no_dir, "The user directory %s wasn't found\n", dir_path);
                    fprintf(stderr, "The user directory %s wasn't found\n", dir_path);

                    // Send a Error message back to the client
                    charsRead = send(establishedConnectionFD, no_dir, sizeof(no_dir), 0); // Send error back
                    if (charsRead < 0) error("ERROR writing to socket");
                }
            }

            // Close and free the alocated resources
            close(establishedConnectionFD); // Close the existing socket which is connected to the client
            free(mode);
            free(user);
            num_connects--;
            exit(0);
        }

        // Parent process awaits the completion of it's children
        else { spawn_pid = waitpid(-1, &status, WNOHANG); }
    }

    // Close the listening socket
	close(listenSocketFD);

	return 0; 
}


/* FUNCTION DEFINITIONS -----------------------------------------------------*/

// Error function used for reporting issues
void error(const char *msg)
{ 
    perror(msg);
    exit(1);
}
