/* Filename: AmazingClient.c
 *
 * Project name: Amazing Maze
 * Group name: Apple Beaver Gold
 *
 * This file contains 
 */

#define _GNU_SOURCE
// ---------------- Includes
#include "common.h"			// common functionality
#include "amazing.h"		// Macros and constants for Amazing Maze
#include "AmazingClient.h"	// avatar fun
#include "Maze.h"		// maze map and algorithms

// ---------------- Globals
extern int threads;
pthread_mutex_t log_mutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
pthread_mutex_t locations_mutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
pthread_mutex_t message_mutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
pthread_mutex_t avatar_mutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;

int numMoves = 0;
uint32_t goalx = 0;
uint32_t goaly = 0;

/* ==================================   Functions   ================================== */

void* AvatarFun(void* arg)
{
 	int sockfd;
    struct sockaddr_in servaddr;
    AvatarData* data = (AvatarData*) arg;

	struct timespec tim, tim2;
  	tim.tv_sec = 0;
   	tim.tv_nsec = 50000;

 	// Create a socket for the client
    // If sockfd<0 there was an error in the creation of the socket
    if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
    	perror("Problem in creating the socket");	
        exit(2);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(data->IPadd);
    servaddr.sin_port =  htons(data->MazePort); //convert to big-endian order
    
    // Connection of the client to the socket 
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Problem in connecting to the server");
        exit(4);
    }
 	
 	// Send AM_AVATAR_READY msg for each avatar
    struct AM_Message readymsg, recvmsg;
	readymsg.type = htonl(AM_AVATAR_READY);
    readymsg.avatar_ready.AvatarID = htonl(data->AvatarID);
    send(sockfd, &readymsg, sizeof(readymsg), 0);

  	int old_dir = -1, new_dir = -1;
    int blocked = 0;
    XYPos* old_pos = (XYPos*) calloc(1, sizeof(XYPos));
    XYPos* new_pos = (XYPos*) calloc(1, sizeof(XYPos));
    int exitvalue = 6;

	// receive the first message from the server
    recv(sockfd, &recvmsg, sizeof(recvmsg), 0);
    old_pos->x = -1;
    old_pos->y = -1;
    
    new_pos->x = ntohl(recvmsg.avatar_turn.Pos[data->AvatarID].x);
    new_pos->y = ntohl(recvmsg.avatar_turn.Pos[data->AvatarID].y);
    int turnId = ntohl(recvmsg.avatar_turn.TurnId);

    // set goal vertex
	if (data->AvatarID == 0) {
		goalx = new_pos->x;
		goaly = new_pos->y;
	}
	
	while (1) {
		// if it's this avatar's turn
		if (data->AvatarID == turnId) {
			if(pthread_mutex_trylock(&avatar_mutex) != 0) nanosleep(&tim , &tim2);
			if (new_pos->x == goalx && new_pos->y == goaly) {
				new_dir = M_NULL_MOVE;
			} else {
				// figure out how to move
				new_dir = whereToTurn(old_dir, blocked, new_pos, data->MazeMap->map);
			}
			// ask to move 
			int ask = askToMoveAvatar(sockfd, new_dir, data->AvatarID);
			if (ask == 0) {
				cleanup(old_pos, new_pos, data, sockfd, exitvalue);
				exit(9);
			}
			numMoves++;
		}
		
		// Receive response from Server
		if (recv(sockfd, &recvmsg, sizeof(recvmsg), 0) < 0) {
			fprintf(stderr, "error receiving message from server.\n");
			exit (14);
		}

		if (ntohl(recvmsg.type) == AM_MAZE_SOLVED) {		// solved the maze
			for (int i = 0; i < data->nAvatars; i++) {
				new_pos->x = goalx;
				new_pos->y = goaly;
				updateLocations(data->locations, new_pos, turnId);
			}
			if (data->AvatarID == 0) {
				printMapToConsole(data->MazeMap->width, data->MazeMap->height, 
						data->locations, data->nAvatars, numMoves);
				printf("Maze Solved!\n");

				char *solved = (char*) calloc(100, sizeof(char));
				sprintf(solved, "Maze solved for %d avatars and %d "
						"difficulty in %d moves.\n",
						ntohl(recvmsg.maze_solved.nAvatars),
						ntohl(recvmsg.maze_solved.Difficulty),
						ntohl(recvmsg.maze_solved.nMoves));
				int writesolved = writeToLog(data->filename, solved);
				free(solved);
				if (writesolved == 0) {
					cleanup(old_pos, new_pos, data, sockfd, exitvalue);
					exit(10);
				}
		
				char *success = (char*) calloc(50, sizeof(char));
				sprintf(success, "Success code: %u\n",
						ntohl(recvmsg.maze_solved.Hash));
				int writesuccess = writeToLog(data->filename, success);
				free(success);
				if (writesuccess == 0) {
					cleanup(old_pos, new_pos, data, sockfd, exitvalue);
					exit(11);
				}
			}
			cleanup(old_pos, new_pos, data, sockfd, exitvalue);
			return NULL;

		} else if (ntohl(recvmsg.type) == AM_TOO_MANY_MOVES) {	// too many moves
			if (data->AvatarID == 0) {
				printf("Ran out of moves :(\n");
				int writemoves = writeToLog(data->filename, "Ran out of moves\n");
				if (writemoves == 0) {
					cleanup(old_pos, new_pos, data, sockfd, exitvalue);
					exit(12);
				}
			}
			cleanup(old_pos, new_pos, data, sockfd, exitvalue);
			return NULL;

		} else if (ntohl(recvmsg.type) == AM_AVATAR_TURN) {		// regular turn
			old_pos->x = new_pos->x;
			old_pos->y = new_pos->y;
			new_pos->x = ntohl(recvmsg.avatar_turn.Pos[data->AvatarID].x);
			new_pos->y = ntohl(recvmsg.avatar_turn.Pos[data->AvatarID].y);

			updateLocations(data->locations, new_pos, data->AvatarID);

		} else if (!IS_AM_ERROR(ntohl(recvmsg.type))) {  // error message
			if (data->AvatarID == 0) {
				fprintf(stderr, "Error: message type.\n");
				int writeerror = writeToLog(data->filename, "Error reading message type.\n");
				if (writeerror == 0) {
					cleanup(old_pos, new_pos, data, sockfd, exitvalue);
					exit(13);
				}
			}
			cleanup(old_pos, new_pos, data, sockfd, exitvalue);
			exit(6);
		} else {
			printf("this is the recvmsg type: %u\n", ntohl(recvmsg.type));
			fprintf(stderr, "Undefined error receiving turn message from server.\n");
			cleanup(old_pos, new_pos, data, sockfd, exitvalue);
			exit(6);
		}
		
		if (data->AvatarID == turnId) {
			int check = updateLog(data->filename, data->AvatarID, new_dir, 
					data->nAvatars, data->locations);
			if (check == 0) {
				cleanup(old_pos, new_pos, data, sockfd, exitvalue);
				exit(8);
			}

			// update blocked
			if (new_pos->x == old_pos->x && new_pos->y == old_pos->y) {
				blocked = 1;
			} else {
				blocked = 0;
			}
			
			data->MazeMap = updateMazeMap(data->MazeMap, old_pos, new_pos, new_dir);
			if (!(data->MazeMap)) {
				fprintf(stderr, "Error updating the MazeMap.\n");
				exit(7);
			}
			printMapToConsole(data->MazeMap->width, data->MazeMap->height, 
					data->locations, data->nAvatars, numMoves);
		}
		turnId = ntohl(recvmsg.avatar_turn.TurnId);
		old_dir = new_dir;
		pthread_mutex_unlock(&avatar_mutex);
	}
}


int askToMoveAvatar(int sockfd, int direction, int AvatarID)
{	
	if (pthread_mutex_trylock(&message_mutex) != 0) return -1;
	
	if (direction != M_WEST && direction != M_SOUTH && direction != M_EAST 
			&& direction != M_NORTH && direction != M_NULL_MOVE) {
		fprintf(stderr, "Invalid move requested.\n");
		pthread_mutex_unlock(&message_mutex);
		return 0;
	}

	struct AM_Message movemsg;
	movemsg.type = htonl(AM_AVATAR_MOVE);
	movemsg.avatar_move.AvatarID = htonl(AvatarID);

	movemsg.avatar_move.Direction = htonl(direction);

	if (send(sockfd, &movemsg, sizeof(movemsg), 0) < 0) {
		fprintf(stderr, "Error sending AM_AVATAR_MOVE message.\n");
		return 0;
	}
	pthread_mutex_unlock(&message_mutex);
	return 1;
}


void CreateAvatarStruct(AvatarData *avatar_data, int AvatarID, \
		int nAvatars, int Difficulty, char *ipadd, uint32_t MazePort, \
		MazeMap *MazeMap, char *filename, XYPos** locations)
{
 	avatar_data->AvatarID = AvatarID;
	avatar_data->nAvatars = nAvatars;
	avatar_data->Difficulty = Difficulty;
	strcpy(avatar_data->IPadd, ipadd);
	avatar_data->MazePort = MazePort;
	avatar_data->MazeMap = MazeMap;
	strcpy(avatar_data->filename, filename);
	avatar_data->locations = locations;
}


int writeToLog(char *filename, char *string)
{
	if (pthread_mutex_trylock(&log_mutex) != 0) return -1;

	FILE *file = fopen(filename, "a");  // open file
	if (!file) {
		fprintf(stderr, "Error opening log file.\n");
		return 0;
	}
	fprintf(file, string);				// write to file
	fclose(file);						// close file
	pthread_mutex_unlock(&log_mutex);  // unlock
	return 1;
}


int updateLog(char *filename, int ID, int direction, int nAvatars,
		XYPos **locations)
{
	if(pthread_mutex_trylock(&log_mutex) != 0) return -1;

	FILE *file = fopen(filename, "a");  // open file
	if (!file) {
		fprintf(stderr, "Error opening log file.\n");
		return 0;
	}

	// this avatar has moved
	char *move = (char*) calloc(MAXLEN, sizeof(char));
	sprintf(move, "Avatar %d tries to move by direction %d\n", 
			ID, direction);
	fprintf(file, move);
	free(move);

	// where are other avatars now?
	for (int i = 0; i < nAvatars; i++) {
		char *position = (char*) calloc(MAXLEN, sizeof(char));
		if (locations[i] != NULL) {
			sprintf(position, "Avatar %d is now at x: %d and y: %d\n", i, locations[i]->x, locations[i]->y);
			fprintf(file, position);
		}
		free(position);
	}
	fprintf(file, "\n");
	
	fclose(file);  // close file
	pthread_mutex_unlock(&log_mutex);  // unlock
	return 1;
}


void cleanup(XYPos* old_pos, XYPos* new_pos, AvatarData* data, \
			int sockfd, int exitvalue)
{
	free(old_pos);
	free(new_pos);
	free(data);
	threads--;
	close(sockfd);
	pthread_exit(&exitvalue);
}
	