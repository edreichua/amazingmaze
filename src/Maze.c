/* ========================================================================== */
/* File: Maze.c
 *
 * Project name: Amazing Maze
 * Group name: Apple Beaver Gold
 *
 * This file contains the functions used for solving the maze, including:
 * initMazeMap(): initialize a maze map structure that keeps track of walls
 * whereToTurn(): determine the next direction that the avatar should try to turn
 *		based on the Right-Hand Rule algorithm
 * updateMazeMap(): update the newfound information in the maze map
 * printMazeMapToFile(): print the contents of the map walls to the file MazeMap.txt
 * printMapToConsole(): print the map graphics using ASCII to the console
 * updateLocations(): update the locations of the avatars in the maze
 * freeMazeMap(): free the memory allocated the MazeMap struct.
 */
/* ========================================================================== */

// ---------------- Includes
#include "Maze.h"

pthread_mutex_t maze_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t graphics_mutex = PTHREAD_MUTEX_INITIALIZER;

MazeMap *initMazeMap(int width, int height)
{
	// verify positive non-zero parameters
	if (width <= 0 || height <= 0) {
		fprintf(stderr, "Invalid width or height for initMazeMap()\n"
						"(negative number or 0 as parameter).\n");
		return NULL;
	}

	MazePoint *mazepoint;
	// allocate memory for each column
	MazePoint ***map = (MazePoint***) calloc(width, sizeof(MazePoint**));

	for (int i = 0; i < width; i++) {
		// allocate memory for spaces in each column (create rows)
		map[i] = (MazePoint**) calloc(height, sizeof(MazePoint*));
		for (int j = 0; j < height; j++) {
			// allocate space for the maze
			mazepoint = (MazePoint*) calloc(1, sizeof(MazePoint));
			
			// initialize all directions as unknown
			mazepoint->north = -1;
			mazepoint->south = -1;
			mazepoint->west = -1;
			mazepoint->east = -1;
			
			// if at border, then establish a wall
			if (i == 0) 		{ mazepoint->west = 0; }  // west border
			if (i == width - 1) { mazepoint->east = 0; }  // east border
			if (j == 0) 		{ mazepoint->north = 0; } // north border
			if (j == height -1) { mazepoint->south = 0; } // south border
			
			// establish a coordinate. add this point to the map
			mazepoint->coordinate.x = i;
			mazepoint->coordinate.y = j;
			map[i][j] = mazepoint;
		}
	}
	MazeMap *mazeMap = (MazeMap*) calloc(1, sizeof(MazeMap));
	mazeMap->map = map;
	mazeMap->width = width;
	mazeMap->height = height;
	return mazeMap;
}


int whereToTurn(int old_dir, int blocked, XYPos* coordinate, \
		MazePoint*** map)
{
	/*** verify quality of parameters ***/
	if (old_dir != M_WEST && old_dir != M_SOUTH && old_dir != M_EAST && \
			old_dir != M_NORTH && old_dir != M_NULL_MOVE && old_dir != -1) {
		fprintf(stderr, "Invalid old direction provided to whereToTurn.\n");
		return -2;
	}

	if (blocked != 0 && blocked != 1) {
		fprintf(stderr, "Invalid 'blocked' value to whereToTurn.\n");
		return -2;
	}

	if (coordinate == NULL) {
		fprintf(stderr, "NULL passed as coordinate to whereToTurn.\n");
		return -2;
	}

	if (map == NULL) {
		fprintf(stderr, "NULL map passed to whereToTurn.\n");
		return -2;
	}
	/*** Parameter quality verification completed ***/

	int x = coordinate->x;
	int y = coordinate->y;

	switch (old_dir) {
		case -1:  // if this is the avatar's first move
			return M_EAST;

		case M_NORTH:  // avatar last tried to move north
			if (blocked) {
				if (map[x][y]->west != 0) return M_WEST;
				if (map[x][y]->south != 0) return M_SOUTH;
				return M_EAST;
			} else {
				if (map[x][y]->east != 0) return M_EAST;
				if (map[x][y]->north != 0) return M_NORTH;
				if (map[x][y]->west != 0) return M_WEST;
				return M_SOUTH;
			}

		case M_WEST:  // avatar last tried to move west
			if (blocked) {
				if (map[x][y]->south != 0) return M_SOUTH;
				if (map[x][y]->east != 0) return M_EAST;
				return M_NORTH;
			} else {
				if (map[x][y]->north != 0) return M_NORTH;
				if (map[x][y]->west != 0) return M_WEST;
				if (map[x][y]->south != 0) return M_SOUTH;
				return M_EAST;
			}

		case M_SOUTH:  // avatar last tried to move south
			if (blocked) {
				if (map[x][y]->east != 0) return M_EAST;
				if (map[x][y]->north != 0) return M_NORTH;
				return M_WEST;
			} else {
				if (map[x][y]->west != 0) return M_WEST;
				if (map[x][y]->south != 0) return M_SOUTH;
				if (map[x][y]->east != 0) return M_EAST;
				return M_NORTH;
			}

		case M_EAST:  // avatar last tried to move east
			if (blocked) {
				if (map[x][y]->north != 0) return M_NORTH;
				if (map[x][y]->west != 0) return M_WEST;
				return M_SOUTH;
			} else {
				if (map[x][y]->south != 0) return M_SOUTH;
				if (map[x][y]->east != 0) return M_EAST;
				if (map[x][y]->north != 0) return M_NORTH;
				return M_WEST;
			}

		case M_NULL_MOVE:
			return M_NULL_MOVE;

		default:
			fprintf(stderr, "Invalid prev_direction.\n");
			return M_NULL_MOVE;
	}
}


MazeMap *updateMazeMap(MazeMap *MazeMap, XYPos* old_pos, \
		XYPos* new_pos, int move_dir)
{
	pthread_mutex_lock(&maze_mutex);
	/*** verify quality of parameters ***/
	if (MazeMap == NULL) {
		fprintf(stderr, "NULL MazeMap passed to updateMazeMap.\n");
		pthread_mutex_unlock(&maze_mutex);
		return NULL;
	}
	if (old_pos == NULL || new_pos == NULL) {
		fprintf(stderr, "NULL position value(s) passed to updateMazeMap.\n");
		pthread_mutex_unlock(&maze_mutex);
		return NULL;
	}
	if (move_dir == -1) {  // avatar's first time moving
		pthread_mutex_unlock(&maze_mutex);
		return MazeMap;
	}
	if (move_dir != M_WEST && move_dir != M_SOUTH && move_dir != M_EAST 
			&& move_dir != M_NORTH && move_dir != M_NULL_MOVE) {
		fprintf(stderr, "Invalid move_dir provided to updateMazeMap.\n");
		pthread_mutex_unlock(&maze_mutex);
		return NULL;
	}

	int width = MazeMap->width;
	int height = MazeMap->height;
	MazePoint ***map = MazeMap->map;
	if (width <= 0 || height <= 0) {
		fprintf(stderr, "Invalid width or height passed to updateMazeMap.\n");
		return NULL;
	}
	/*** Parameter quality verification completed ***/

	// variable abbreviations
	int oldX = old_pos->x;
	int oldY = old_pos->y;
	int newX = new_pos->x;
	int newY = new_pos->y;

	// if the avatar had not moved and tried to move...
	// mark that side as a wall
	if (oldX == newX && oldY == newY) {
		switch (move_dir) {
			case M_NORTH:  // ... north
				map[oldX][oldY]->north = 0;
				if (oldY > 0)
					map[oldX][oldY - 1]->south = 0;
				break;
			case M_WEST:  // ... west
				map[oldX][oldY]->west = 0;
				if (oldX > 0)
					map[oldX - 1][oldY]->east = 0;
				break;
			case M_EAST:  // ... east
				map[oldX][oldY]->east = 0;
				if (oldX < width - 1)
					map[oldX + 1][oldY]->west = 0;
				break;
			case M_SOUTH:  // ... south
				map[oldX][oldY]->south = 0;
				if (oldY < height - 1)
					map[oldX][oldY + 1]->north = 0;
				break;
			case M_NULL_MOVE:  // ... stayed still
				break;
			default:
				fprintf(stderr, "Invalid prev_direction.\n");
				break;
		}

	} else {  // if the avatar had moved...
		// mark that side as accessible
		if (oldX == newX && oldY > newY) {  // ...north
			map[oldX][oldY]->north = 1;
			map[newX][newY]->south = 1;
		
		} else if (oldX == newX && oldY < newY) {  // ...south
			map[oldX][oldY]->south = 1;
			map[newX][newY]->north = 1;
		
		} else if (oldX > newX && oldY == newY) {  // ...west
			map[oldX][oldY]->west = 1;
			map[newX][newY]->east = 1;

		} else if (oldX < newX && oldY == newY) {  // ...east
			map[oldX][oldY]->east = 1;
			map[newX][newY]->west = 1;
		}
	}
	pthread_mutex_unlock(&maze_mutex);
	return MazeMap;
}


int printMazeMapToFile(MazeMap *MazeMap) 
{	
	if(!MazeMap){
		fprintf(stderr, "Invalid MazeMap entered.\n");
		return 0;
	}
	if (MazeMap->width <= 0 || MazeMap->height <= 0) {
		fprintf(stderr, "Invalid width or height entered.\n");
		return 0;
	}
	
	FILE* file = fopen("MazeMap.txt", "w");
	if (!file) {
		fprintf(stderr, "Error opening file.\n");
		return 0;
	}

	// for each location in the maze map, print out the directions
	for (int i = 0; i < MazeMap->width; i++) {
		for (int j = 0; j < MazeMap->height; j++) {
			MazePoint* spot = MazeMap->map[i][j];
			fprintf(file, "%d,%d: north:%d west:%d south:%d east:%d\n", \
					i, j, spot->north, spot->west, spot->south, spot->east);

		}
	}

	fprintf(file, "\n");
	fclose(file);
	return 1;
}


int printMapToConsole(int MazeWidth, int MazeHeight, XYPos** locations, \
		int nAvatars, int numMoves)
{	
	if (pthread_mutex_trylock(&graphics_mutex) != 0) return -1;  // lock

	/*** verify quality of parameters ***/
	if (MazeWidth <= 0 || MazeHeight <= 0) {
		fprintf(stderr, "Invalid width or height passed to printMapToConsole.\n");
		pthread_mutex_unlock(&graphics_mutex);  // unlock
		return 0;
	}
	if (locations == NULL) {
		fprintf(stderr, "NULL locations list provided to printMapToConsole.\n");
		pthread_mutex_unlock(&graphics_mutex);  // unlock
		return 0;
	}
	if (nAvatars < 2 || nAvatars > AM_MAX_AVATAR) {
		fprintf(stderr, "Invalid nAvatars passed to printMapToConsole.\n");
		pthread_mutex_unlock(&graphics_mutex);  // unlock
		return 0;
	}
	if (numMoves < 0) {
		fprintf(stderr, "Invalid numMoves (negative) passed to printMapToConsole.\n");
		pthread_mutex_unlock(&graphics_mutex);  // unlock
		return 0;
	}

	for (int i = 0; i < nAvatars; i++) {
		if (locations[i] == NULL) {
			pthread_mutex_unlock(&graphics_mutex);  // unlock
			return 0;
		}
	}
	/*** Done verifying quality of parameters ***/

	system("clear\n");  // to keep graphics in same place on screen
	int avatar;
	for (int i = 0; i < MazeHeight + 2; i++) {
		for (int j = 0; j < MazeWidth + 2; j++) {
			if (i == 0 || i == MazeHeight + 1 || j == 0 || j == MazeWidth + 1) {
				// print the border of the maze
				printf("#  ");
			} else {
				avatar = -1;  // no avatars there
				for (int k = 0; k < nAvatars; k++) {
					if (locations[k]->x + 1 == j && locations[k]->y + 1 == i) {
						if (avatar == -1) {  // if no avatars there
							avatar = k;		// now one avatar there
						} else {			// already an avatar there
							avatar = -2;	// now more than one avatar there
						}
					}
				}
				if (avatar == -1) {  // no avatars here
					printf("   ");
				} else if (avatar == -2) {  // more than one avatar here
					printf("+  ");
				} else {			// only one avatar there
					printf("%d  ", avatar);
				}
			}
		}
		printf("\n");  // next row of maze
	}
	printf("Number of moves: %d\n", numMoves);
	pthread_mutex_unlock(&graphics_mutex);  // unlock
	return 1;
}


int updateLocations(XYPos** locations, XYPos* my_location, int ID) 
{
	pthread_mutex_lock(&locations_mutex);
	/*** verify quality of parameters ***/
	if (locations == NULL) {
		fprintf(stderr, "NULL locations list passed to updateLocations.\n");
		return 0;
	}
	if (my_location == NULL) {
		fprintf(stderr, "NULL my_location passed to updateLocations.\n");
		return 0;
	}
	if (ID < 0 || ID > AM_MAX_AVATAR) {
		fprintf(stderr, "Invalid ID passed to updateLocations.\n");
		return 0;
	}
	/*** Done verifying quality of parameters ***/

	locations[ID] = my_location;			// update the position
	pthread_mutex_unlock(&locations_mutex);  // unlock mutex
	return 1;
}


int freeMazeMap(MazeMap *MazeMap)
{
	/*** verify quality of parameters ***/
	if (MazeMap == NULL) {
		fprintf(stderr, "NULL MazeMap passed to freeMazeMap.\n");
		return 0;
	}
	/*** Done checking quality of parameters ***/

	for (int i = 0; i < MazeMap->width; i++) {
		MazePoint** col = MazeMap->map[i];
		for (int j = 0; j < MazeMap->height; j++) {
			free(col[j]);  // free each spot
		}
		free(col);  // free each column
	}
	free(MazeMap->map);  // free the MazePoint ***map
	free(MazeMap);  // free the MazeMap struct
	return 1;
}
