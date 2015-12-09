/* ========================================================================== */
/* File: Maze.h
 *
 * Project name: Amazing Maze
 * Group name: Apple Beaver Gold
 *
 * This file contains the function prototypes used in Maze.h */
/* ========================================================================== */

#include "common.h"			// common functionality
#include "amazing.h"		// Macros and constants for Amazing Maze


/* Parameters:
 * (1) int width: the numbers of columns of the maze. x dimension.
 * (2) int height: the number of rows of the maze. y dimension.
 *
 * Description: Allocate memory for and initialize a two dimensional
 * array of MazePoint structs, which keeps track of what the four
 * directions' status is at each MazePoint. A direction can be:
 * 		1 if the direction is known to be not a wall (an open path)
 * 		0 if the direction is known to be a wall.
 * 		-1 if the direction has not been visited yet.
 *
 * Return: MazeMap *MazeMap: a pointer to a MazePoint struct that has
 * 	a map of MazePoint structs, and the map's height and width. */
MazeMap *initMazeMap(int width, int height);


/* Parameters:
 * (1) int old_dir: The last direction that the avatar tried.
 * (2) int blocked: A boolean for if the avatar was not able to move last turn.
 *		the value of this variable should be 0 or 1.
 * (3) XYPos coordinate: An XYPos struct for the avatar's location in the maze.
 * (4) MazePoint*** map: A pointer to a 2D array of MazePoint's.
 *
 * Description: Determines the next direction for the avatar to move using the
 * right-hand-rule algorithm: Try to keep the wall on the right-hand-side.
 * Do not try to move into walls avatars already know are there. Returns
 * different directions based on if the avatar's last move was successful.
 *
 * Return: the direction that the avatar should try to move next. */
int whereToTurn(int prev_direction, int blocked, XYPos* coordinate, \
		MazePoint*** map);


/* Parameters:
 * (1) MazePoint *MazeMap: A pointer to a 2D array of MazePoint's.
 * (2) XYPos old_pos: The avatar's old position before its most recent move.
 * (3) XYPos new_pos: The avatar's new position after its most recent move.
 * (4) int move_dir: The direction that the avatar had tried to move.
 *
 * Description: Updates the MazeMap with the information revealed by
 * the last avatar's attempted move.
 * If the avatar did not move successfully, the direction is marked as a wall.
 * If the avatar did move successfully, the direction is marked as open.
 *
 * Return: MazePoint MazeMap: a pointer to the updated MazeMap. */
MazeMap *updateMazeMap(MazeMap *MazeMap, XYPos *old_pos, \
		XYPos *new_pos, int move_dir);


/* Parameters:
 * (1) int MazeWidth: the width of the maze (a positive int)
 * (2) int MazeHeight: the height of the maze (a positive int)
 * (3) XYPos **locations: the pointer to the array of avatar positions
 * (4) int nAvatars: the number of avatars (int between 2 and 9 inclusive)
 * (5) int numMoves: the number of moves made so far (a positive int)
 * Description: Prints a map graphic to the console. Clears at the beginning
 * of the function so that the map visual is easier to see and understand.
 * Also prints the number of turns so far.
 * Return: Success code: 1 if successful, 0 if not successful */
int printMapToConsole(int MazeWidth, int MazeHeight, XYPos **locations,\
		int nAvatars, int numMoves);


/* Parameters:
 * (1) XYPos **locations: the pointer to the array of avatar positions
 * (2) XYPos *my_location: the pointer to this avatar's position
 * (3) int ID: the ID of this avatar
 * Description: update the array of avatar locations.
 * Return: Success code: 1 if successful, 0 if not successful */
int updateLocations(XYPos** locations, XYPos* my_location, int ID);


/* Parameters:
 * (1) MazePoint ***MazeMap:
 * (2) int width: the width of the provided maze (a positive int)
 * (3) int height: the height of the provided maze (a positive int)
 * Description: Prints the status of walls for each spot in the maze
 * 	to MazeMap.txt. Used for testing purposes. Not included in final version.
 * Return: Success code: 1 if successful, 0 if not successful */
int printMazeMapToFile(MazeMap *MazeMap);


/* Parameters:
 * (1) MazeMap *MazeMap: 
 * Description: Frees the memory allocated to the MazeMap struct.
 * Return: Success code: 1 if successful, 0 if not successful */
int freeMazeMap(MazeMap* MazeMap);

