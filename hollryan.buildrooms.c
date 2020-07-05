#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

struct Room
{
    int num;
    char *name;
    int connections[6];
};

struct Room rooms[7];

char *roomNames[10] = {"bedroom", "dungeon", "kitchen", "hallway", "armory", "tower", "garden", "library", "baths", "temple"};
int roomOrder[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

// A function to shuffle an array. Source: https://stackoverflow.com/questions/6127503/shuffle-array-in-c
void shuffle(int *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

//////////// ROOM CREATION /////////////////

// Make a directory to hold the room files.
void makeRoomsDir()
{
    // Make a name for the directory of rooms, ending in 6 random numbers (can have leading zeroes).
    char dirName[22] = "hollryan.rooms.";
    int i;
    for (i = 0; i < 6; i++)
    {
        int randNum = rand() % 10;
        randNum = randNum + '0'; // convert to ascii char
        dirName[i + 15] = randNum;
    };
    // Create a directory with that name.
    mkdir(dirName, 0777);
    // Open it as the new working directory.
    chdir(dirName);
}

// Make a single room file in the rooms folder.
FILE *makeRoomFile(struct Room room)
{
    // create the file
    FILE *fp;
    fp = fopen(room.name, "w");
    // write room name to the file
    fputs("ROOM NAME: ", fp);
    fputs(room.name, fp);
    fputs("\n", fp);
    // write room connections to the file
    int i;
    for (i = 0; i < 7; i++)
    {
        if (room.connections[i] != -1)
        {
            fprintf(fp, "CONNECTION %d: %s\n", i, rooms[room.connections[i]].name);
        }
    }
    // write room type to the file
    if (room.num == 1)
    {
        fputs("ROOM TYPE: START_ROOM", fp);
    }
    else if (room.num == 6)
    {
        fputs("ROOM TYPE: END_ROOM", fp);
    }
    else
    {
        fputs("ROOM TYPE: MID_ROOM", fp);
    }
    // end
    fclose(fp);
    return fp;
}

// Make 7 Room structs and initialize the data.
void makeRoomStructs()
{
    // Shuffle the list of room names.
    shuffle(roomOrder, 10);
    // Instantiate 7 Room structures.
    int i;
    for (i = 0; i < 7; i++)
    {
        // Populate data for this Room in the rooms structure array.
        rooms[i].num = i;
        rooms[i].name = roomNames[roomOrder[i]];
        // fill connections with -1 to show this Room is not connected to any others.
        int j;
        for (j = 0; j < 6; j++)
        {
            rooms[i].connections[j] = -1;
        }
    }
}

///////////////////// ROOM CONNECTION ///////////////////
// All rooms must have 3-6 connections to other rooms. All connections must be two-way.

struct Room getRandomRoom()
{
    //printf("getRandomRoom() called\n");
    int randNum = rand() % 7; // 0, 1, 2, 3, 4, 5, or 6 -- the indices of the 7 rooms in room[]
    //printf("randNum = %d\n", randNum);
    //printf("getRandomRoom() returning room %d, %s\n", rooms[randNum].num, rooms[randNum].name);
    return rooms[randNum];
}

bool isGraphFull()
{
    //printf("isGraphFull() called\n");
    int i;
    for (i = 0; i < 7; i++) // iterating thru all the rooms that exist
    {
        //printf("i = %d, rooms[i].connections[2] = %d\n", i, rooms[i].connections[2]);
        if (rooms[i].connections[2] == -1) // if any room does not have a 3rd connection
        {
            //printf("isGraphFull() returning FALSE\n");
            return false;
        }
    }
    //printf("isGraphFull() returning TRUE\n");
    return true;
}

bool canAddConnectionFrom(struct Room x)
{

    //printf("canAddConnectionFrom() called with x=%d\n", x.num);
    //printf("x.connections[5] == %d\n", x.connections[5]);
    if (x.connections[5] == -1)
    {
        //printf("canAddConnectionFrom() returning TRUE\n");
        return true;
    }
    //printf("canAddConnectionFrom() returning FALSE\n");
    return false;
}

bool connectionAlreadyExists(struct Room x, struct Room y)
{
    //printf("connectionAlreadyExists() called with x=%d, y=%d\n", x.num, y.num);
    int i;
    for (i = 0; i < 6; i++) // iterating thru room x's connections
    {
        if (x.connections[i] == y.num)
        {
            //printf("connectionAlreadyExists() returning TRUE\n");
            return true;
        }
    }
    //printf("connectionAlreadyExists() returning FALSE\n");
    return false;
}

void connectRoom(struct Room x, struct Room y)
{
    //printf("connectRoom() called with x=%d, y=%d\n", x.num, y.num);
    int i;
    for (i = 0; i < 6; i++) // iterating thru room x's connections
    {
        //printf("i = %d, x.connections[i] == %d, y.num=%d\n", i, x.connections[i], y.num);
        if (rooms[x.num].connections[i] == -1)
        {
            //printf("x.connections[i] = %d and y.num = %d!\n", x.connections[i], y.num);
            rooms[x.num].connections[i] = y.num;
            //printf("now x.connections[i] = %d and y.num = %d\n", x.connections[i], y.num);
            //printf("connectRoom() returning\n");
            return;
        }
    }
}

bool isSameRoom(struct Room x, struct Room y)
{
    //printf("isSameRoom() called with x=%d, y=%d\n", x.num, y.num);
    if (x.num == y.num)
    {
        //printf("isSameRoom() returning TRUE since x.num=%d and y.num=%d\n", x.num, y.num);
        return true;
    }
    //printf("isSameRoom() returning FALSE since x.num=%d and y.num=%d\n", x.num, y.num);
    return false;
}

void addRandomConnection()
{
    //printf("addRandomConnection() called\n");
    struct Room A;
    struct Room B;
    while (true)
    {
        //printf("addRandomConnection() while(true) called\n");
        A = getRandomRoom();
        if (canAddConnectionFrom(A) == true)
        {
            //printf("canAddConnectionFrom(A) == true\n");
            break;
        }
        //printf("canAddConnectionFrom(A) == false?\n");
    }
    do
    {
        //printf("calling getRandomRoom() on B\n");
        B = getRandomRoom();
    } while (canAddConnectionFrom(B) == false || isSameRoom(A, B) == true || connectionAlreadyExists(A, B) == true);

    //printf("calling connectRoom() on A and B\n");
    connectRoom(A, B);
    connectRoom(B, A);
}

void connectRooms()
{
    int counter = 0;
    //printf("connectRooms() called\n");
    while (!isGraphFull())
    {
        /*
        // prints all the rooms
        int i;
        for (i = 0; i < 7; i++) // iterate thru rooms
        {
            printf("\n\nRoom num=%d, %s\nconnections: \n", rooms[i].num, rooms[i].name);
            int j;
            for (j = 0; j < 6; j++) // iterate thru rooms' connections
            {
                printf("%d: %d (%s)\n", j, rooms[i].connections[j], rooms[rooms[i].connections[j]].name);
            }
        }
        */

        //printf("isGraphFull() is false\n");
        addRandomConnection();
    }
}

//////////////////// MAIN /////////////////////

int main(int argc, char **argv)
{
    printf("\n---------- RUNNING ----------\n");
    /////////////////////////////////

    // Generate a random seed.
    srand(time(0));

    ///////////// MAKE ROOMS DIRECTORY /////////////
    makeRoomsDir();

    ////////////// MAKE 7 ROOM STRUCTS /////////////
    makeRoomStructs();

    ////////////// CONNECT ROOM STRUCTS /////////////
    connectRooms();

    /////////// MAKE ROOM FILES FROM STRUCTS //////////
    int i;
    for (i = 0; i < 7; i++)
    {
        //printf("i = %d\nnum=%d\nname=%s\nconnections: \n", i, rooms[i].num, rooms[i].name);
        int j;
        for (j = 0; j < 6; j++)
        {
            //printf("%d: %d (%s)\n", j, rooms[i].connections[j], rooms[rooms[i].connections[j]].name);
        }
        makeRoomFile(rooms[i]);
    }

    ////////////////////////////
    printf("---------- ENDING  ----------\n\n");
    return 0;
}
