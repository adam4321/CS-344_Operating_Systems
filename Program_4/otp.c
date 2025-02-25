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


#define CHAR_SET_SIZE   27          // The length of the character set
#define MAX_BUFFER_SIZE 75000       // Maximum network message size

const char *CHAR_SET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";


/* FUNCTION DECLARATIONS -----------------------------------------------------*/

void error(const char *msg);
int file_read_test(char *name, char *buffer, int size);
void encrypt_msg(char *msg_str, char *plaintext_str, char *key_str);
void decrypt_msg(char *msg_str, char *cipher_str, char *key_str);


/* MAIN ---------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
	int socketFD;                       // Holds the sending socket
    int portNumber;                     // Holds the port number in argv[4 or 5]
    int charsWritten;                   // Holds the number of characters written to stream
    int charsRead;                      // Holds the number of characters returned
	struct sockaddr_in serverAddress;   // Holds the server's address
	struct hostent *serverHostInfo;     // Holds the server's information
	char buffer[MAX_BUFFER_SIZE];       // Holds the message to be sent to the server
    
    
    // Check the arguments passed on the command line
	if (argc == 5)
    {
        // 5 argument call should have get as the second argument
        if (strcmp(argv[1], "get") != 0)
        {
            fprintf(stderr,"USAGE: otp post user plaintext key port\n");
            fprintf(stderr,"USAGE: otp get user key port\n");
            exit(2); 
        }
        // Get the port number, convert to an integer from a string
        else { portNumber = atoi(argv[4]); }
    }
    else if(argc == 6)
    {
        // 6 argument call should have post as the second argument
        if (strcmp(argv[1], "post") != 0)
        {
            fprintf(stderr,"USAGE: otp post user plaintext key port\n");
            fprintf(stderr,"USAGE: otp get user key port\n");
            exit(2); 
        }
        // Get the port number, convert to an integer from a string
        else { portNumber = atoi(argv[5]); }
    }
    // Calls to otp can only have 5 or 6 arguments
    else
    {
        fprintf(stderr, "Incorrect number of arguments to otp\n");
        fprintf(stderr,"USAGE: otp post user plaintext key port\n");
        fprintf(stderr,"USAGE: otp get user key port\n");
        exit(2);
    }


    // CLIENT POST MODE -------------------------------------------------------
    if (strcmp(argv[1], "post") == 0)
    {
        // Clear out the buffer array
        memset(buffer, '\0', sizeof(buffer));
        
        // Create 3 strings
        char plaintext_str[MAX_BUFFER_SIZE];
        char key_str[MAX_BUFFER_SIZE];
        char msg_str[MAX_BUFFER_SIZE];
        memset(msg_str, '\0', sizeof(MAX_BUFFER_SIZE));
        
        // Read in the files and test their contents for bad characters
        long plaintext_len = file_read_test(argv[3], plaintext_str, MAX_BUFFER_SIZE);
        long key_len = file_read_test(argv[4], key_str, MAX_BUFFER_SIZE);

        // Test that the key file is equal of longer than plaintext 
        if (plaintext_len > key_len)
        {
            fprintf(stderr, "The key file %s is too short\n", argv[4]);
            exit(1);
        }

        // Call the function to encrypt the plaintext string
        encrypt_msg(msg_str, plaintext_str, key_str);

        // Assemble the post message string
        strcat(buffer, argv[1]);
        strcat(buffer, " ");
        strcat(buffer, argv[2]);
        strcat(buffer, " ");
        strcat(buffer, msg_str);

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


    // CLIENT GET MODE --------------------------------------------------------
    if (strcmp(argv[1], "get") == 0)
    {
        // Clear out the buffer array and fill with the request mode and the user
        memset(buffer, '\0', sizeof(buffer));
        strcat(buffer, argv[1]);
        strcat(buffer, " ");
        strcat(buffer, argv[2]);
        
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

        // Get return message from server
        memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
        charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
        if (charsRead < 0)
        {
            error("CLIENT: ERROR reading from socket");
            exit(2);
        }

        // Check for no directory error
        if (strstr(buffer, "The user directory") != NULL) { exit(1); }

        // Check for no file error
        if (strstr(buffer, "No file for") != NULL) { exit(1); }

        // Read in the key file
        char key_str[MAX_BUFFER_SIZE];
        memset(key_str, '\0', sizeof(MAX_BUFFER_SIZE));
        long key_len = file_read_test(argv[3], key_str, MAX_BUFFER_SIZE);

        // Array for the decrypted message
        char final_msg[MAX_BUFFER_SIZE];
        memset(final_msg, '\0', sizeof(MAX_BUFFER_SIZE));

        // Decrypt the message and print it to stdout
        decrypt_msg(final_msg, buffer, key_str);
        printf("%s\n", final_msg);
    }

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


// Read in the file / test the characters / return it's length
int file_read_test(char *name, char *buffer, int size)
{  
    FILE *fptr = fopen(name, "r");
    int i;

    // Check that the input files are open
    if (!fptr)
    {
        fprintf(stderr, "%s cannot be opened\n", name);
        exit(1);
    }

    // Clear the buffer and read the file contents into buffer
    memset(buffer, '\0', size);
    fgets(buffer, size - 1, fptr);
    buffer[strcspn(buffer, "\n")] = '\0';
    int length = strlen(buffer);

    // Test the string in buffer for bad characters
    for (i = 0; i < length; i++)
    {
        // The string should only have capital letters or a space
        if (!isupper(buffer[i]) && buffer[i] != ' ') {
            fprintf(stderr, "%s contains bad characters\n", name);
            fclose(fptr);
            exit(1);
        }
    }

    // Close the file and return the string length
    fclose(fptr);
    return length;
}


// Function to encrypt a post mode message
void encrypt_msg(char *msg_str, char *plaintext_str, char *key_str)
{
    int plain_text_len = strlen(plaintext_str);
    int i;

    // Loop through the plaintext string
    for (i = 0; i < plain_text_len; i++)
    {
        // Find the position in the alphabet of the plaintext character
        int msg_idx = strchr(CHAR_SET, plaintext_str[i]) - CHAR_SET;

        // Add the position in the alphabet of the key character
        msg_idx += strchr(CHAR_SET, key_str[i]) - CHAR_SET;

        // Clock arithmetic to wrap around Z
        msg_idx %= CHAR_SET_SIZE;

        // Insert the encrypted character
        msg_str[i] = CHAR_SET[msg_idx];
    }

    // Remove the newline from the end of the string
    msg_str[i] = '\0';
}


// Function to decrypt a get mode message
void decrypt_msg(char *msg_str, char *cipher_str, char *key_str)
{
    int cipher_len = strlen(cipher_str);
    int i;

    // Loop through the ciphertext string
    for (i = 0; i < cipher_len; i++)
    {
        // Find the position in the alphabet of the ciphertext character
        int msg_idx = strchr(CHAR_SET, cipher_str[i]) - CHAR_SET;
        
        // Subtract the position in the alphabet of the key character
        msg_idx -= (strchr(CHAR_SET, key_str[i]) - CHAR_SET);

        // If below Z then add the char set size
        if (msg_idx < 0)
        {
            msg_idx += CHAR_SET_SIZE;
        }
        // If positive, it wraps around Z so mod by char set size
        else
        {
            msg_idx %= CHAR_SET_SIZE;
        }
        
        // Insert the encrypted character
        msg_str[i] = CHAR_SET[msg_idx];
    }

    // Remove the newline from the end of the string
    msg_str[i] = '\0';
}
