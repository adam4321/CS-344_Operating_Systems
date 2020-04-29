/******************************************************************************
**  Author:      Adam Wright
**  Email:       wrighada@oregonstate.edu
**  Date:        4/18/2020
**  Description: Program 2 for spring 2020 OSU cs-344. This program opens up
**               the room files generated by wrighada.buildrooms and then
**               uses them to form the layout of a maze which this program
**               allows the user to navigate.
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <pthread.h>


/* Global Constants */
#define ROOM_COUNT  7
#define CONNECT_MAX 6

/* Definition for the bool type */
typedef enum
{
    false,
    true
} bool;

/* Definition for Room struct */
struct Room
{
    char *name;
    char *type;
    int connect_count;
    struct Room *out_connect[CONNECT_MAX];
};


/* Array to hold the created rooms */
struct Room *Room_Arr[ROOM_COUNT];

/* Array to hold the name of the newest directory that contains my prefix */
char newestDirName[256];

/* Variable to hold the start room's index in Room_Arr */
int start_index;

/* File to hold the currently requested time */
char* time_file = "currentTime.txt";

/* Lock for switching threads to display the time */
pthread_mutex_t time_lock = PTHREAD_MUTEX_INITIALIZER;


/* FUNCTION DECLARATIONS ----------------------------------------------------*/

void Get_Newest_Dir();
void Init_Room_Arr(struct Room *Room_Arr[]);
void Fill_Room_Arr(struct Room *Room_Arr[]);
void Game_Prompt();
void Free_Memory();


/* MAIN ---------------------------------------------------------------------*/

int main()
{
    /* In game variables */
    int i;
    int j;
    int step_count = 0;

    /* Search the current directory for the newest subdirectory */
    Get_Newest_Dir();
    printf("Newest entry found is: %s\n", newestDirName);

    /* Create and fill an array of struct rooms with the newest room data */
    Init_Room_Arr(Room_Arr);
    Fill_Room_Arr(Room_Arr);


    for (i = 0; i < ROOM_COUNT; i++)
    {
        printf("Name %d: %s\t", i, Room_Arr[i]->name);
        printf("Type: %s\n", Room_Arr[i]->type);
    }


    printf("Start index: %d\n", start_index);

    /* Run the game loop */
    do
    {
        break;
    } while (true);




    Free_Memory();

    return 0;
}


/* FUNCTION DEFINITIONS -----------------------------------------------------*/

/* Function that finds the most recent room directory */
void Get_Newest_Dir()
{
  int newestDirTime = -1; /* Modified timestamp of newest subdir examined */
  char targetDirPrefix[32] = "wrighada.rooms."; /* Prefix we're looking for */
  memset(newestDirName, '\0', sizeof(newestDirName)); /* newest directory that contains my prefix */

  DIR *dirToCheck; /*Holds the directory we're starting in */
  struct dirent *fileInDir; /* Holds the current subdir of the starting dir */
  struct stat dirAttributes; /* Holds information we've gained about subdir */

  dirToCheck = opendir("."); /* Open up the directory this program was run in */

    if (dirToCheck > 0) /* Make sure the current directory could be opened */
    {
        while ((fileInDir = readdir(dirToCheck)) != NULL) /* Check each entry in dir */
        {
            if (strstr(fileInDir->d_name, targetDirPrefix) != NULL) /* If entry has prefix */
            {
                stat(fileInDir->d_name, &dirAttributes); /*Get attributes of the entry */

                if ((int)dirAttributes.st_mtime > newestDirTime) /* If this time is bigger */
                {
                    newestDirTime = (int)dirAttributes.st_mtime;
                    memset(newestDirName, '\0', sizeof(newestDirName));
                    strcpy(newestDirName, fileInDir->d_name);
                }
            }
        }
    }

  closedir(dirToCheck); /* Close the directory we opened */
}


/* Initialize heap memory for the struct room array */
void Init_Room_Arr(struct Room *Room_Arr[])
{
    int i = 0;
    int j = 0;

    /* Create space on the heap for the struct rooms array */
    for (i = 0; i < ROOM_COUNT; i++)
    {
        Room_Arr[i] = malloc(sizeof(struct Room));

        Room_Arr[i]->name = malloc(16 * sizeof(char));
        
        Room_Arr[i]->type = malloc(16 * sizeof(char));
        

        /* Set all of the connection array values to 0 */
        for (j = 0; j < CONNECT_MAX; j++)
        {
            Room_Arr[i]->out_connect[j] = calloc(16, sizeof(char));
        }
    }
}


/* Fills the room array with data from most recent directory */
void Fill_Room_Arr(struct Room *Room_Arr[])
{
    DIR *newest_dir = opendir(newestDirName);
    struct dirent *file_in_dir;
    char *file_name;
    FILE *room_file;
    char line[255];
    int file_num = 0;

    /* Iterate through the directory and grab the filenames */
    while (file_in_dir = readdir(newest_dir))
    {
        /* Iterate through the room files and populate the struct room data */
        if (file_in_dir->d_name[0] != '.')
        {
            /* Move into the directory and open each non-hidden file */
            chdir(newestDirName);
            file_name = file_in_dir->d_name;
            room_file = fopen(file_name, "r");

            /* Fill the receiving char array with null terminators */
            memset(line, '\0', sizeof(line));

            /* Split each line to pull the target data */
            while (fgets(line, sizeof(line), room_file))
            {
                /* Split each line on whitespaces */
                strtok(line, " ");
                char *token_1 = strtok(NULL, " ");
                char *token_2 = strtok(NULL, " ");

                /* Remove the final characters */
                token_1[strlen(token_1) - 1] = '\0';
                token_2[strlen(token_2) - 1] = '\0';

                /* Assign the room name */
                if (strstr(token_1, "NAME"))
                {
                    strcpy(Room_Arr[file_num]->name, token_2);
                }
                /* Assign the room type */
                if (strstr(token_1, "TYPE"))
                {
                    strcpy(Room_Arr[file_num]->type, token_2);

                    /* Find the starting room */
                    if (strcmp(token_2, "START_ROOM") == 0)
                    {
                        start_index = file_num;
                    }
                }
                if (strstr(token_1, "CONNECT"))
                {

                }
            }
            
            file_num++;
            fclose(room_file);
        }
    }

    closedir(newest_dir);
}


/* Print the game menu */
void Game_Prompt()
{
    printf("CURRENT LOCATION: \n");
    printf("POSSIBLE CONNECTIONS: \n");
    printf("WHERE TO? >");
}


/* Free the array of room structs */
void Free_Memory()
{
    int i;
    int j;
    
    for (i = 0; i < ROOM_COUNT; i++)
    {
     
        free(Room_Arr[i]->name);

        free(Room_Arr[i]->type);

        for (j = 0; j < CONNECT_MAX; j++)
        {
            free(Room_Arr[i]->out_connect[j]);
        }
        
        free(Room_Arr[i]);
    }
}
