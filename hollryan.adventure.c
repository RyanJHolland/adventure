#define _GNU_SOURCE
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

// Adapted from code provided in the module notes: https://repl.it/@cs344/34completeexamplec#main.c

/*This programs
* Opens the current directory
* Finds the latest directory inside the current directory that has the prefix "hollryan.rooms."
* Goes to this latest directory and proceses the file named "room_info.txt" 
* Creates a linked list with entries for each row of data found in "room_info.txt"
* When done processing the file, print all the room data stored in the linked list
*/

struct Room
{
    int num;
    char *name;
    int connections[6];
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

/* Parse the current line and create a Room with the data */
struct Room *createRoom(char *currLine)
{
    struct Room *currRoom = malloc(sizeof(struct Room));

    // The first line is the room name
    char *line = strtok(currLine, " ");
    currRoom->name = calloc(strlen(line) + 1, sizeof(char));
    strcpy(currRoom->name, line);
    /*
    // The second line is the connection 0
    line = strtok(NULL, " ");
    currRoom->lastName = calloc(strlen(line) + 1, sizeof(char));
    strcpy(currRoom->lastName, line);

    // The third line is the firstName
    line = strtok(NULL, " ");
    currRoom->firstName = calloc(strlen(line) + 1, sizeof(char));
    strcpy(currRoom->firstName, line);

    // The last line is the major
    line = strtok(NULL, "\n");
    currRoom->major = calloc(strlen(line) + 1, sizeof(char));
    strcpy(currRoom->major, line);
    // Set the next node to NULL in the newly created Room entry
    currRoom->next = NULL;
    */

    return currRoom;
}

int parseLine(char *buf, struct Room *room)
{
    int i = 0;
    int j = 0;
    char text[10];
    // initialize the text array with nulls
    while (i < 10)
    {
        text[i] = '\0';
        i++;
    }
    i = 0;

    // parse the line by copying everything after the ": " to the text array
    while (i < 60)
    {
        if (buf[i] == '\0')
        {
            //return 0;
        }
        else if (buf[i] == ':')
        {
            i = i + 2; // skip the ": " (colon and space)
            while (i < 60)
            {
                if (buf[i] == '\0')
                {
                    break;
                }
                else
                {
                    text[j] = buf[i];
                    i++;
                    j++;
                }
            }
        }
        i++;
    }
    room->name = text;
    return 0;
}

int readFile(char *filePath)
{
    // create a room to store the file
    struct Room *room;
    // Open the specified file for reading only
    FILE *roomFile = fopen(filePath, "r");
    char buf[60];
    // read each line to buffer
    while (fgets(buf, 60, roomFile) != NULL)
    {
        // parse the line and save its data into the room struct
        parseLine(buf, room);
    };
    fclose(roomFile);

    return (0);
    /*

    char *currLine = NULL;
    size_t len = 0;
    ssize_t nread;
    char *line;

    // Read all the lines in the file
    while ((nread = getline(&currLine, &len, roomFile)) != -1)
    {
        // Get the room element corresponding to the current line
        struct Room *newElem = createRoom(currLine);

        // Is this the first element in the linked list?
        if (head == NULL)
        {
            // This is the first element in the linked link
            // Set the head and the tail to this element
            head = newElem;
            tail = newElem;
        }
        else
        {
            // This is not the first element.
            // Add this element to the list and advance the tail
            tail->next = newElem;
            tail = newElem;
        }
    }
    free(currLine);
    return head;
    */
}

/*
* Print the linked list of Rooms
void printRoomList(struct Room *list)
{
    while (list != NULL)
    {
        printf("%s %s %s %s\n", list->onid, list->lastName,
               list->firstName, list->major);
        list = list->next;
    }
}
*/

int main(void)
{
    printf("\n");
    struct dirent *file; // directory iterator cursor thingy
    char *latestDir = findLatestDirectory(".", "hollryan.rooms.");
    DIR *d = opendir(latestDir);
    file = readdir(d); // calling this twice skips over the . and .. files, which we want to ignore.
    file = readdir(d);
    while ((file = readdir(d)) != NULL)
    {
        char *filePath = getFilePath(latestDir, file->d_name);
        readFile(filePath);
    }

    //printRoomList(list);
    printf("\n");
    return 0;
}
/*

int main(int argc, char **argv)
{

    struct stat stats;
    stat("hollryan.Rooms.188397/", &stats);

    // source for the stat-accessing below: https://codeforwin.org/2018/03/c-program-find-file-properties-using-stat-function.html
    struct tm dt;
    dt = *(gmtime(&stats.st_mtime));
    printf("\nModified on: %d-%d-%d %d:%d:%d\n", dt.tm_mday, dt.tm_mon, dt.tm_year + 1900,
           dt.tm_hour, dt.tm_min, dt.tm_sec);
    return 0;
}

*/