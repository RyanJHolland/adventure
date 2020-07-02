#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <time.h>

int i;
char *roomNames[10] = {"bedroom", "dungeon", "kitchen", "hall", "armory", "tower", "garden", "library", "baths", "temple"};

// Create a directory with the given filename.
int makeDir(char *dirName)
{
    if (mkdir(dirName, 0777) == 0)
    {
        printf("Created directory: %s\n", dirName);
        return 0;
    }
    else
    {
        printf("Error making directory! errno = %d: ", errno);
        printf(strerror(errno));
        printf("\n");
        return -1;
    }
}

// Make a room.
int makeRoom(char *dirName)
{
    int randNum = rand() % 10;
    char *roomName = roomNames[randNum];
    dirName = strcat(dirName, "/");
    fopen(strcat(dirName, roomName), "w");

}

// Make multiple rooms.
int makeRooms(char *dirName, int numRooms)
{
    for (i = 0; i < numRooms; i++)
    {
        makeRoom(dirName);
    }
}

// Run the program.
int main(void)
{
    printf("\n$$$$$$$$$$ RUNNING $$$$$$$$$$\n");

    // Generate a random seed.
    srand(time(0));

    // Make a name for the directory of rooms, ending in 6 random numbers.
    char dirName[22] = "hollryan.rooms.";
    for (i = 0; i < 6; i++)
    {
        int randNum = rand() % 10;
        randNum = randNum + '0'; // convert to ascii char
        dirName[i + 15] = randNum;
    };

    // Create a directory with that name.
    makeDir(dirName);

    // Put rooms in that directory.
    makeRooms(dirName, 7);

    printf("$$$$$$$$$$ ENDING  $$$$$$$$$$\n\n");
    return 0;
}
