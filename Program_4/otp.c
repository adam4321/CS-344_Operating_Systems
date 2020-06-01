/******************************************************************************
**  Author:       Adam Wright
**  Email:        wrighada@oregonstate.edu
**  Date:         5/23/2020
**  Description:  OSU cs-344 spring 2020 Project 4. This program is the client
**                which interacts with the server otp_d. It can connect in 
**                either get or post mode. It asks otp_d to either store or 
**                retrieve messages for a given user.
**
**  Program get:  otp post user plaintext key port
**  Example call: otp post Ben plaintext1 mykey 57111
**  Program post: otp get user key port
**  Example call: otp get Ben mykey 57111
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define CHAR_SET_SIZE   27

const char *CHAR_SET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";


/* FUNCTION DECLARATIONS -----------------------------------------------------*/

void error(const char *msg);
int file_test(FILE *infile);
void encrypt_msg(char *msg, char *plain_text, char *key);
void decrypt_msg(char *msg, char *plain_text, char *key);


/* MAIN ---------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[256];
    
    // Check usage & args
	if (argc == 5)
    {
        if (strcmp(argv[1], "get") != 0)
        {
            fprintf(stderr,"USAGE: otp post user plaintext key port\n");
            fprintf(stderr,"USAGE: otp get user key port\n");
            exit(2); 
        }
        else
        {
            portNumber = atoi(argv[4]);  // Get the port number, convert to an integer from a string
        }
    }
    else if(argc == 6)
    {
        if (strcmp(argv[1], "post") != 0)
        {
            fprintf(stderr,"USAGE: otp post user plaintext key port\n");
            fprintf(stderr,"USAGE: otp get user key port\n");
            exit(2); 
        }
        else
        {
            portNumber = atoi(argv[5]); // Get the port number, convert to an integer from a string
        }
    }
    else
    {
        fprintf(stderr, "Incorrect number of arguments to otp\n");
        exit(2);
    }


    if (strcmp(argv[1], "get") == 0)
    {

        memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array

        int i;
        for (i = 1; i < 4; i++)
        {
            strcat(buffer, argv[i]);
            strcat(buffer, " ");
        }

        // Set up the server address struct
        memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
        serverAddress.sin_family = AF_INET; // Create a network-capable socket
        serverAddress.sin_port = htons(portNumber); // Store the port number
        serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
        if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(2); }
        memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

        // Set up the socket
        socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
        if (socketFD < 0) error("CLIENT: ERROR opening socket");
        
        // Connect to server
        if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
        {
            error("CLIENT: ERROR connecting");
        }

        // Send message to server
        charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
        if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
        if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");
    }


    if (strcmp(argv[1], "post") == 0)
    {
        memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
        strcat(buffer, argv[1]);
        strcat(buffer, " ");
        strcat(buffer, argv[2]);
        strcat(buffer, " ");

        FILE *fptr1 = fopen(argv[3], "r");
        FILE *fptr2 = fopen(argv[4], "r");
        printf("%s %s\n", argv[3], argv[4]);
        long plaintext_len;
        long key_len;

        if (!fptr1 || !fptr2)
        {
            fprintf(stderr, "Input file cannot be opened\n");
            exit(1);
        }
        
        plaintext_len = file_test(fptr1);
        if (plaintext_len == -1)
        {
            fprintf(stderr, "Bad characters in %s\n", argv[3]);
            exit(1);
        }

        key_len = file_test(fptr2);
        if (plaintext_len == -1)
        {
            fprintf(stderr, "Bad characters in %s\n", argv[4]);
            exit(1);
        }
        

        // char *input_msg = malloc()
        

        if (plaintext_len > key_len)
        {
            fprintf(stderr, "The key file must be equal or longer than plaintext\n");
            exit(1);
        }


        fclose(fptr1);
        fclose(fptr2);

        // Set up the server address struct
        memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
        serverAddress.sin_family = AF_INET; // Create a network-capable socket
        serverAddress.sin_port = htons(portNumber); // Store the port number
        serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
        if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(2); }
        memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

        // Set up the socket
        socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
        if (socketFD < 0) error("CLIENT: ERROR opening socket");
        
        // Connect to server
        if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
        {
            error("CLIENT: ERROR connecting");
        }

        // Send message to server
        charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
        if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
        if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");
    }

	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0)
    {
        error("CLIENT: ERROR reading from socket");
        exit(2);
    }
	printf("CLIENT: I received this from the server: \"%s\"\n", buffer);


    // Close the socket
	close(socketFD);

	return 0;
}


/* FUNCTION DEFINITIONS -----------------------------------------------------*/

// Error function used for reporting issues
void error(const char *msg)
{
    perror(msg);
    exit(2);
}


// Check the size of the passed in files
int file_test(FILE *infile)
{
    int cur_char;
    int count = 0;
    
    cur_char = fgetc(infile);

    while (cur_char != EOF && cur_char != '\n')
    {
        if (!isupper(cur_char) || cur_char != ' ')
        {
            return -2;
        }

        cur_char = fgetc(infile);
        count++;
    }

    return count;
}


// Function to encrypt a post mode message
void encrypt_msg(char *msg, char *plain_text, char *key)
{
    int plain_text_len = strlen(plain_text);
    int key_len = strlen(key);

    int i;
    for (i = 0; i < plain_text_len; i++)
    {
        int msg_idx = strchr(CHAR_SET, plain_text[i]) - CHAR_SET;
        msg_idx += strchr(CHAR_SET, key[i]) - CHAR_SET;
        msg_idx %= CHAR_SET_SIZE;

        msg[i] = CHAR_SET[msg_idx];
    }
}


// Function to decrypt a get mode message
void decrypt_msg(char *msg, char *plain_text, char *key)
{
    int msg_len = strlen(msg);
    int key_len = strlen(key);

    int i;
    for (i = 0; i < msg_len; i++)
    {
        int msg_idx = strchr(CHAR_SET, msg[i]) - CHAR_SET;
        msg_idx += strchr(CHAR_SET, key[i]) - CHAR_SET;
        if (msg_idx < 0)
        {
            msg_idx += CHAR_SET_SIZE;
        }
        else
        {
            msg_idx %= CHAR_SET_SIZE;
        }
        
        plain_text[i] = CHAR_SET[msg_idx];
    }
}
