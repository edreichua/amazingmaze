/* Filename: AmazingClient.h
 *
 * Project name: Amazing Maze
 * Group name: Apple Beaver Gold
 *
 * This file contains the function prototypes used in the program.
 */
 
// ---------------- Globals
extern pthread_mutex_t mutex1, mutex2;
extern uint32_t goalx;
extern uint32_t goaly;


// ---------------- Private prototypes

/* Parameters:
 * (1) struct AvatarData *avatar_data: a package for all the information
 *		the avatars will need. The user needs to have already allocated
 *		memory space for this struct before the user calls this function.
 * (2) int AvatarID: the ID of this avatar. This parameter is the only
 *		parameter in the AvatarData package that is different for each avatar.
 * (3) int nAvatars: the number of avatars in the maze. 2-10 inclusive.
 * (4) int Difficulty: the difficulty level of the maze. 0-9 inclusive.
 * (5) char *ipadd: the IP address of the server.
 * (6) uint32_t MazePort: the port number determined in AMStartup.c
 * (7) MazeMap *MazeMap: the pointer to a MazeMap struct that contains the
 *		width and height of the maze and the MazePoint ***map
 * (8) char *filename: the name of the file to which the avatars
 *		will be writing. This file must exist, and the user must have
 *		writing rights to this file.
 * (9) XYPos **locations: a pointer to an array of XYPos structs.
 *		This list has a length of AM_MAX_AVATAR and keeps track of the
 *		position of each avatar in the maze. The user must have already
 *		allocated space for this array and established this pointer before
 *		the user calls this function.
 * Description: Fills the given avatar_data struct with the information
 * 		that the avatars will need to solve the maze and communicate with
 *		each other. */
void CreateAvatarStruct(AvatarData* avatar_data, int AvatarId, \
		int nAvatars, int Difficulty, char* ipadd, uint32_t MazePort, \
		MazeMap *MazeMap, char* filename, XYPos** locations);


/* Parameters: 
 * (1) void *arg: The struct package of data that each avatar should have
 * 	access to.
 * Description: The "main" function for each avatar.
 *	Create a socket for each avatar thread.
 *	Communicate with the server via sending and receiving messages
		and with other avatars via access to the same structs:
			MazeMap* MazeMap and XYPos** locations
 *  Destroy the threads once they have received a "win" or "error" or
		"max move" message and return. */
void* AvatarFun(void* arg);


/* Parameters:
 * (1) int sockfd: the socket ID of this avatar thread
 * (2) int direction: the direction that this avatar is trying to move.
 * (3) int AvatarId: the ID of the avatar asking to move
 * Description: Sends AM_AVATAR_MOVE message to server for a given avatar
 * 	to move a given direction.
 * Return: Status code: 1 if successful, 0 if not successful,
 * 	-1 if thread problem. */
int askToMoveAvatar(int sockfd, int direction, int AvatarId);


/* Parameters:
 * (1) char *filename: the name of the log file to write to
 * (2) char *string: the string to write to the file
 * Description: Locks the log mutex, opens the file, writes the string,
 *	close the file, and unlock the log mutex. Multipurpose for notes to log.
 * Note: Need the log_mutex global.
 * Return: Status code: 1 if successful, 0 if not successful,
 * 	-1 if thread problem. */
int writeToLog(char* filename, char* string);


/* Parameters:
 * (1) XYPos* old_pos: the pointer to the old position of this avatar
 * (2) XYPos* new_pos: the pointer to the new position of this avatar
 * (3) AvatarData* data: the package of data that this avatar has
 * (4) int sockfd: the socket ID of this avatar thread
 * (5) int exitvalue: the value to exit on
 * Description: Free the memory and decrement the number of threads running.
	Close the socket for this avatar and exit this avatar's thread. */
void cleanup(XYPos* old_pos, XYPos* new_pos, AvatarData* data, int sockfd, \
		int exitvalue);


/* Parameters:
 * (1) char *filename: the name of the log file to write to
 * (2) int ID: the ID of the avatar who just tried to move
 * (3) int direction: the direction that the avatar with ID tried to move
 * (4) int nAvatars: the number of avatars in the maze
 * (5) XYPos **locations: a pointer to the array of avatar locations
 * Description: Writes to the file where avatar ID tried to move and 
 *	where all the avatars are now.
 * Return: Success code: 1 if successful, 0 if not successful,
 *	-1 if thread problem */
int updateLog(char *filename, int ID, int direction, int nAvatars,
		XYPos **locations);
