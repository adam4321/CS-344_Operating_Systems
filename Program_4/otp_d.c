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


#define MAX_CONNECTS     5


/* FUNCTION DECLARATIONS -----------------------------------------------------*/

void error(const char *msg);


/* MAIN ---------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[256];
	struct sockaddr_in serverAddress, clientAddress;
    pid_t spawn_pid;
    int status = 0;
    int num_connects = 0;

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

        // PROJECT REQUIREMENT THAT THE SERVER SLEEP FOR 2 SECONDS AFTER CONNECTION ACCTEPTANCE
        sleep(2);

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
            // Increment the number of connections
            num_connects++;

            // Get the message from the client and display it
            memset(buffer, '\0', 256);
            charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
            if (charsRead < 0) error("ERROR reading from socket");

            printf("SERVER: I received this from the client: \"%s\"\n", buffer);

            // Tokenize the request string and pull out the mode
            char *token = strtok(buffer, " ");
            char *mode = strdup(token);

            // printf("Mode: %s\n", mode);

            // Pull out the user token
            token = strtok(NULL, " ");
            char *user = strdup(token);

            // printf("User: %s\n", user);


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

                // printf("Message: %s", message);

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
                // Assemble the file name from the username and oldest 
                char file_name[255];
                memset(file_name, '\0', sizeof(file_name));


                FILE *ciphertext = fopen(file_name, "r");




                fclose(ciphertext);
            }

            // Send a Success message back to the client
            charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
            if (charsRead < 0) error("ERROR writing to socket");

            // Close and free the alocated resources
            close(establishedConnectionFD); // Close the existing socket which is connected to the client
            free(mode);
            free(user);
            num_connects--;
            exit(0);
        }
        // Parent process awaits the completion of it's children
        else { waitpid(spawn_pid, &status, 0); }
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
