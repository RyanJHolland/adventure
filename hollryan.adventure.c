#define _GNU_SOURCE
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

// Adapted from code provided in the module notes: https://repl.it/@cs344/34completeexamplec#main.c

struct Room
{
    int num;
    char *name[10];
    char *connections[6][10];
    int type; // start = 0, mid = 1, end = 2
};

struct Room rooms[7];

// Returns path to most recently modified directory in the working dir.
char *findLatestDirectory(char *path, char *prefix)
{
    struct stat dirStat;
    char directoryName[256];
    char *latestDirName;

    // Open the directory
    DIR *currDir = opendir(path);
    struct dirent *aDir;
    time_t lastModifTime;
    int i = 0;

    /* The data returned by readdir() may be overwritten by subsequent calls to readdir() for the same directory stream. So we will copy the name of the directory to the variable directoryName
  */
    while ((aDir = readdir(currDir)) != NULL)
    {
        // Use strncmp to check if the directory name matches the prefix
        if (strncmp(prefix, aDir->d_name, strlen(prefix)) == 0)
        {
            stat(aDir->d_name, &dirStat);
            // Check to see if this is the directory with the latest modification time
            if (i == 0 || difftime(dirStat.st_mtime, lastModifTime) > 0)
            {
                lastModifTime = dirStat.st_mtime;
                memset(directoryName, '\0', sizeof(directoryName));
                strcpy(directoryName, aDir->d_name);
            }
            i++;
        }
    }
    latestDirName = malloc(sizeof(char) * (strlen(directoryName) + 1));
    strcpy(latestDirName, directoryName);

    closedir(currDir);
    return latestDirName;
}

/* Create and return a string for the file path by concatenating the directory name with the file name */
char *getFilePath(char *directoryName, char *fileName)
{
    char *filePath = malloc(strlen(directoryName) + strlen(fileName) + 2);
    memset(filePath, '\0', strlen(directoryName) + strlen(fileName) + 2);
    strcpy(filePath, directoryName);
    strcat(filePath, "/");
    strcat(filePath, fileName);
    return filePath;
}

// This figures out what kind of line we're looking at: Room Name, Room Type, or Connections.
// Then it populates the Room struct for this room with that data.
int parseLine(char *lineBuffer, struct Room *room)
{
    // iterate thru chunks of the line
    char *token;
    token = strtok(lineBuffer, " "); // get first chunk
    if (!strcmp(token, "ROOM"))      // meaning this chunk says "ROOM"
    {
        //printf("token is %s and matched ROOM\n", token);
        token = strtok(NULL, " "); // get next chunk
        //printf("token is now %s\n", token);
        if (!strcmp(token, "NAME:"))
        {
            //printf("token is %s and matched NAME:\n", token);
            token = strtok(NULL, " ");
            //printf("token is now %s\n", token);
            token[strlen(token) - 1] = '\0'; // we need to remove the "\n" at the end of the name
            //printf("token is now %s\n", token);
            //room->name = token;
            memcpy(room->name, token, 8);
            //printf("X Name: %s\n", room->name);
        }
        else // must be room TYPE, not room NAME
        {
            //printf("token is %s and matched ELSE:\n", token);
            token = strtok(NULL, " ");
            //printf("token is now %s\n", token);
            if (!strcmp(token, "START_ROOM"))
            {
                room->type = 0;
            }
            else if (!strcmp(token, "MID_ROOM"))
            {
                room->type = 1;
            }
            else if (!strcmp(token, "END_ROOM"))
            {
                room->type = 2;
            }
            //printf("Type: %d\n", room->type);
        }
    }
    else // we are looking at a connection
    {
        //printf("token reached CONNECTION and is %s\n", token);
        token = strtok(NULL, " "); // step from CONNECTION to the # of the connection
        //printf("step to connection number. token is now %s\n", token);
        int connectionNum = atoi(&token[0]); // ascii char to int (leaving out the colon)
        //printf("connectionNum = %d\n", connectionNum);
        token = strtok(NULL, " "); // step to the name of the connected room
        //printf("step to name of connected room. token is now %s\n", token);
        token[strlen(token) - 1] = '\0'; // remove its trailing newline
        memcpy(room->connections[connectionNum], token, 8);
        //printf("Connection #%d: %s\n", connectionNum, room->connections[connectionNum]);
    }

    return 0;
}

int readFile(char *filePath, struct Room *room)
{
    // Open the file
    FILE *roomFile = fopen(filePath, "r");
    char lineBuffer[60];
    // read each line to buffer
    while (fgets(lineBuffer, 60, roomFile) != NULL)
    {
        // parse the line and save its data into the room struct
        parseLine(lineBuffer, room);
    };
    fclose(roomFile);

    return (0);
}

// print info on rooms
void printRooms()
{
    // print the now-populated Room structs
    int i;
    for (i = 0; i < 7; i++)
    {
        printf("num: %d\n", rooms[i].num);
        printf("name: %s\n", rooms[i].name);
        printf("type: %d\n", rooms[i].type);
        int j = 0;
        while (j < 6)
        {
            if (strlen(rooms[i].connections[j]) > 0)
            {
                printf("connection %d: %s\n", j, rooms[i].connections[j]);
            }
            j++;
        }
        printf("\n");
    }
}

void printConnections(int currentRoom)
{
    printf("POSSIBLE CONNECTIONS: ");
    int j = 0;
    printf("%s", rooms[currentRoom].connections[j]);
    while (j < 6)
    {
        j++;
        if (strlen(rooms[currentRoom].connections[j]) > 1)
        {
            printf(", %s", rooms[currentRoom].connections[j]);
        }
    }
    printf(".\n");
}

int main(void)
{
    //printf("\n------- starting ----------\n");
    // Find the most recent rooms folder
    struct dirent *file; // directory iterator cursor thingy
    char *latestDir = findLatestDirectory(".", "hollryan.rooms.");
    DIR *d = opendir(latestDir);
    file = readdir(d); // calling this twice skips over the . and .. files, which we want to ignore.
    file = readdir(d);

    int roomsCounter = 0;

    while ((file = readdir(d)) != NULL)
    {
        char *filePath = getFilePath(latestDir, file->d_name);
        //printf("filePath = %s, roomsCounter = %d\n", filePath, roomsCounter);
        readFile(filePath, &rooms[roomsCounter]);
        rooms[roomsCounter].num = roomsCounter;
        //printf("Name: %s, Type: %d\n", rooms[roomsCounter].name, rooms[roomsCounter].type);
        roomsCounter++;
    }

    //printRooms();

    // get the starting room
    int i, currentRoom;
    for (i = 0; i < 7; i++)
    {
        if (rooms[i].type == 0)
        {
            currentRoom = rooms[i].num;
        }
    }

    // play the game
    int steps = 0;
    bool match = false;
    int path[256];
    int x;
    for (x = 0; x < 256; x++)
    {
        path[x] = -1;
    }

    while (true)
    {
        printf("\nCURRENT LOCATION: %s\n", rooms[currentRoom].name);

        // check loss condition
        if (steps == 256)
        {
            printf("You took too long, and got busted by the guards!\n");
            return 0;
        }

        // check victory condition
        if (rooms[currentRoom].type == 2)
        {
            printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
            printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
            int i = 0;
            while (path[i] > -1)
            {
                printf("%s\n", rooms[path[i]].name);
                i++;
                /*
                if (path[i] > -1)
                {
                    printf(", ");
                }
                else
                {
                    printf(". ");
                }
                */
            }
            printf("\n\n");
            return 0;
        }

        // No victory or loss. Continue the game:
        printConnections(currentRoom);
        printf("WHERE TO? >");
        char input[10];
        fgets(input, sizeof(input), stdin);
        // input has a trailing '\n' which we must remove
        input[strlen(input) - 1] = '\0';

        //printf("You chose: %s\n", input);
        //printf("rooms[i].connections[0] = %s\n", rooms[currentRoom].connections[0]);

        // check if input matches possible move locations
        int i = 0;
        while (i < 6) // for each possible connection
        {
            if (strlen(rooms[currentRoom].connections[i]) > 0) // for each actual connection
            {
                int same = strcmp(rooms[currentRoom].connections[i], input); // check if input matches it
                if (same == 0)
                {
                    //printf("currentRoom = %d, rooms[currentRoom].num = %d, path[steps] = %d, steps = %d\n", currentRoom, rooms[currentRoom].num, path[steps], steps);
                    match = true; // match found
                    // move to that room
                    int j;
                    for (j = 0; j < 7; j++)
                    {
                        if (strcmp(rooms[j].name, input) == 0)
                        {
                            currentRoom = rooms[j].num;
                        }
                    }
                    path[steps] = currentRoom; // log our path
                    steps++;                   // increment counter of moves made
                    //printf("currentRoom = %d, rooms[currentRoom].num = %d, path[steps] = %d, steps = %d\n", currentRoom, rooms[currentRoom].num, path[steps], steps);
                    break;
                }
                //printf(", %s", rooms[currentRoom].connections[i]);
            }
            i++;
        }

        // bad input
        if (match == false)
        {

            printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
        }
    }
    //printf("\n------- ending ------------\n");
    return 0;
}