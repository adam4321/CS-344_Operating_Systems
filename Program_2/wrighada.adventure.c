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

/* Definition for the Room struct */
struct Room
{
    char *name;
    char *type;
    int connect_count;
    char *out_connect[CONNECT_MAX];
};

/* Global Variables */
struct Room *Room_Arr[ROOM_COUNT];      /* Array to hold the created rooms */
char newestDirName[256];                /* Array to hold the name of the newest directory */
int start_index;                        /* Variable to hold the start room's index in Room_Arr */
char* time_file = "currentTime.txt";    /* File to hold the currently requested time */
pthread_mutex_t time_lock;              /* Lock for switching threads to display the time */


/* FUNCTION DECLARATIONS ----------------------------------------------------*/

void Get_Newest_Dir();
void Init_Room_Arr(struct Room *Room_Arr[]);
void Fill_Room_Arr(struct Room *Room_Arr[]);
int Get_Room_Index(char *Room);
void Free_Memory();


/* MAIN ---------------------------------------------------------------------*/

int main()
{
    /* In game variables */
    int i;
    int j;
    struct Room *current_room;
    int current_index;
    int step_count = 0;
    int rooms_visited[255];
    char input_buffer[255];
    bool room_match = false;
    

    /* Search the current directory for the newest subdirectory */
    Get_Newest_Dir();

    /* Create and fill an array of struct rooms with the newest room data */
    Init_Room_Arr(Room_Arr);
    Fill_Room_Arr(Room_Arr);


    printf("\n");
    printf("Newest entry found is: %s\n", newestDirName);

    for (i = 0; i < ROOM_COUNT; i++)
    {
        printf("Name %d: %s\t", i, Room_Arr[i]->name);
        printf("Type: %s\n", Room_Arr[i]->type);
        printf("Connect Count: %d\n", Room_Arr[i]->connect_count);
        for (j = 0; j < CONNECT_MAX; j++)
        {
            printf("Connection %d: %s\n", j, Room_Arr[i]->out_connect[j]);
        }
    }

    printf("Start index: %d\n\n", start_index);



    /* Enter the starting room into the visited array */
    rooms_visited[step_count] = start_index;

    /* Set the struct room pointer to the starting room */
    current_room = Room_Arr[start_index];

    /* Begin the game loop */
    do
    {
        printf("CURRENT LOCATION: %s\n", current_room->name);
        printf("POSSIBLE CONNECTIONS:");

        i = 0;

        while (i < current_room->connect_count - 1)
        {
            printf(" %s,", current_room->out_connect[i]);
            i++;
        }


        printf(" %s.\n", current_room->out_connect[i]);
        printf("WHERE TO? >");


        memset(input_buffer, '\0', sizeof(input_buffer));
        scanf("%254s", input_buffer);
        printf("\n");

        if (strcmp(input_buffer, "time") == 0)
        {


            printf("\n");
        }
        else
        {
            i = 0;
            room_match = false;

            while (i < current_room->connect_count)
            {
                

                if (strcmp(input_buffer, current_room->out_connect[i]) == 0)
                {
                    room_match = true;
                    step_count++;
                    rooms_visited[step_count] = Get_Room_Index(current_room->out_connect[i]);

                    /* Update the index of the current room */
                    current_index = rooms_visited[step_count];

                    /* Set the struct room pointer to the current room */
                    current_room = Room_Arr[current_index];

                    if (strcmp(current_room->type, "END_ROOM") == 0)
                    {
                        printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
                        printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", step_count + 1);
                        
                        j = 0;
                        while (j < step_count + 1)
                        {
                            printf("%s\n", Room_Arr[rooms_visited[j]]->name);
                            j++;
                        }

                        /* Free the heap alocated objects */
                        Free_Memory();

                        /* End the program and return success signal */
                        return 0;
                    }
                }

                i++;
            }
            if (room_match == false)
            {
                printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
            }
        }

    } while (true); 
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

        Room_Arr[i]->connect_count = 0;

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
    int i;

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

            i = 0;
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
                else if (strstr(token_1, "TYPE"))
                {
                    strcpy(Room_Arr[file_num]->type, token_2);

                    /* Find the starting room and store the value */
                    if (strcmp(token_2, "START_ROOM") == 0)
                    {
                        start_index = file_num;
                    }
                }
                /* Assign the connected rooms and connection count */
                else
                {
                    strcpy(Room_Arr[file_num]->out_connect[i], token_2);
                    Room_Arr[file_num]->connect_count++;
                    i++;
                }
            }
            
            file_num++;
            fclose(room_file);
        }
    }

    closedir(newest_dir);
}


/* Returns the index of a struct room or -1 if it isn't found */
int Get_Room_Index(char *str)
{
    int i = 0;

    while (i < ROOM_COUNT)
    {
        if (strcmp(Room_Arr[i]->name, str) == 0)
        {
            return i;
        }

        i++;
    }

    /* This should not be reached because a room will be found */ 
    return -1;
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
