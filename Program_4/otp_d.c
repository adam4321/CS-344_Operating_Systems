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

    while (1)
    {
        if (num_connects >= MAX_CONNECTS)
        {
            fprintf(stderr, "All connections used");
            continue;
        }

        // Accept a connection, blocking if one is not available until one connects
        sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
        establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
        sleep(2);

        if (establishedConnectionFD < 0)
        {
            error("ERROR on accept");
            continue;
        }

        spawn_pid = fork();

        if (spawn_pid == -1)
        {
            fprintf(stderr, "New connection had a failed process startup");
            close(establishedConnectionFD);
            continue;
        }
        else if (spawn_pid == 0)
        {
            num_connects++;

            // Get the message from the client and display it
            memset(buffer, '\0', 256);
            charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
            if (charsRead < 0) error("ERROR reading from socket");

            printf("SERVER: I received this from the client: \"%s\"\n", buffer);

            // Tokenize the request string and pull out the mode and user
            char *token = strtok(buffer, " ");
            char *mode = strdup(token);
            printf("Mode: %s\n", mode);


            token = strtok(NULL, " ");
            char *user = strdup(token);
            printf("User: %s\n", user);


            // SERVER POST MODE -----------------------------------------------
            if (strcmp(mode, "post") == 0)
            {
                // If in post mode then pull out the message from the request
                token = strtok(NULL, "");
                char *message = strdup(token);
                printf("Message: %s\n", message);


                free(message);
            }


            // SERVER GET MODE ------------------------------------------------

            if (strcmp(mode, "get") == 0)
            {



            }

            // Send a Success message back to the client
            charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
            if (charsRead < 0) error("ERROR writing to socket");

            close(establishedConnectionFD); // Close the existing socket which is connected to the client
            free(mode);
            free(user);
            num_connects--;
            exit(0);
        }
        else
        {
			waitpid(spawn_pid, &status, 0);
        }
    }

	close(listenSocketFD); // Close the listening socket

	return 0; 
}


/* FUNCTION DEFINITIONS -----------------------------------------------------*/

// Error function used for reporting issues
void error(const char *msg)
{ 
    perror(msg);
    exit(1);
}
