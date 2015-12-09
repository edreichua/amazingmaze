/* ========================================================================== */
/* File: common.h
 *
 * Project name: Amazing Maze
 * Group name: Apple Beaver Gold
 *
 * This file contains the common defines and data structures.
 *
 */
/* ========================================================================== */
#ifndef COMMON_H
#define COMMON_H

// ---------------- Prerequisites e.g., Requires "math.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <getopt.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <dirent.h>                          // scandir, alphasort
#include <sys/stat.h>                        // stat functionality
#include "amazing.h"

// ---------------- Constants
#define MAXLEN 100 // maximum number of characters in a file line or name
#define MAXTRY 10  // maximum number of tries to connect to server

// ---------------- Structures/Types

/* A spot in the maze with four directions.
 * The directions are set to one of three integers:
 *	1 if the direction is known to be not a wall (an open path)
 *  0 if the direction is known to be a wall.
 *  -1 if the direction is unknown  */
typedef struct MazePoint
{
	int west;
    int east; 
    int north;
    int south;
    struct XYPos coordinate;
} MazePoint;

/* The MazePoint*** map with its associated width and height */
typedef struct MazeMap
{
	int width;
	int height;
	MazePoint ***map;
} MazeMap;

/* Data shared by the avatars. */
typedef struct AvatarData {
	int AvatarID;
	int nAvatars;
	int Difficulty;
	char IPadd[MAXLEN];
	uint32_t MazePort;
	MazeMap *MazeMap;
	char filename[MAXLEN];
    XYPos** locations;
} AvatarData;

// ---------------- Public Variables
extern pthread_mutex_t log_mutex;
extern pthread_mutex_t locations_mutex;
extern pthread_mutex_t maze_mutex;
extern pthread_mutex_t message_mutex;
extern pthread_mutex_t graphics_mutex;

// ---------------- Prototypes/Macros
int IsDir(const char *path);

#endif // COMMON_H
