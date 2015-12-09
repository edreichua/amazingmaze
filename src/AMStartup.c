/* ========================================================================== */
/* File: AMStartup.c
 *
 * Project name: Amazing Maze
 * Group name: Apple Beaver Gold
 *
 * This file contains the start up script for the Amazing Maze
 *
 * Input: ./AMStartup -n [nAvatars] -d [Difficulty] -h [Hostname]
 *
 * Command line options: -n Number of Avatars (2 to 10)
 *						 -d Difficulty level (1 to 9)
 *						 -h Hostname
 * 						 -n, -d and -h require argument
 *						 The default, if the variable is not specified, for:
 *							nAvatars is 2
 *							Difficulty is 1
 *							hostname is pierce.cs.dartmouth.edu
 * 
 * Output: If an error is encountered, the program prints the appropriate error
 * message. Otherwise, the program calls AmazingClient.c, which prints out
 * graphics for each avatar move. 
 *
 * Pseudocode:
 *
 * Special Considerations: See readme
 *
 */
/* ========================================================================== */

#define _GNU_SOURCE
// ---------------- Includes
#include "common.h"			// common functionality
#include "amazing.h"		// Macros and constants for Amazing Maze
#include "AmazingClient.h"	// avatar fun
#include "Maze.h"			// find center avatar. create maze map	


// ---------------- Structures
/* Structure for getopt_long */
static struct option longopts[] = {
	{"nAvatars",	required_argument, 0, 'n'},
	{"Difficulty",	required_argument, 0, 'd'},
	{"Hostname",	required_argument, 0, 'h'},
};

int threads = 0;

/* ========================================================================== */

int main(int argc, char * argv[])
{
	// Variables for command line options parsing
	int indexptr, nAvatars, Difficulty, n, d, h;
	char ch;
	char Hostname[MAXLEN];

	// Variables for client
	int sockfd;
    struct sockaddr_in servaddr;
    
    // Variables for initialization
   	uint32_t MazePort, MazeWidth, MazeHeight;

	// Command line parsing
	while ((ch = getopt_long(argc, argv, "n:d:h:", longopts, &indexptr)) != -1) {
		switch (ch) { 
      		case 'n': 
      			if (optarg) {
      				nAvatars = atoi(optarg); 
      				n = 1;
      			}
      			break;
       		case 'd': 
       			if (optarg) {
       				Difficulty = atoi(optarg); 
       				d = 1;
       			}
       			break;
           	case 'h':
				if (optarg) {
           	 		strcpy(Hostname,optarg); 
           	 		h = 1;
           	 	}
           		break;
      		default: 
           		exit(1);
           		break;
     	}  
     }
     
    // Check parameters
    if (!n || nAvatars < 2 || nAvatars > AM_MAX_AVATAR) {
    	nAvatars = 2;
    	perror("Number of Avatars not specified or ill-defined. It will be defaulted to 2");
    }
    if (!d || Difficulty < 0 || Difficulty > AM_MAX_DIFFICULTY) {
    	Difficulty = 0;
    	perror("Difficulty not specified or ill-defined. It will be defaulted to 0");
    }
	
    if (!h) {
		strcpy(Hostname, "pierce.cs.dartmouth.edu");
    	perror("Hostname not specified. It will be defaulted to pierce.cs.dartmouth.edu");
    }
	
	// Create a socket for the client
    // If sockfd<0 there was an error in the creation of the socket
    if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
    	perror("Problem in creating the socket");	
        exit(2);
    }

    // Creation of the server address
    memset(&servaddr, 0, sizeof(servaddr));
	struct hostent *host = gethostbyname(Hostname);
	if (host == NULL) {
		perror("Invalid hostname.");
		close(sockfd);
		exit(3);
	}
    servaddr.sin_family = AF_INET;
	struct in_addr **addr_list = (struct in_addr **) host->h_addr_list;
    servaddr.sin_addr.s_addr = inet_addr(inet_ntoa(*addr_list[0]));
    servaddr.sin_port = htons(AM_SERVER_PORT);  // convert to big-endian order

    // Connection of the client to the socket
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Problem in connecting to the server");
        close(sockfd);
        exit(4);
    }
    
    // Send AM_INIT
    struct AM_Message initmsg, recvmsg;
	initmsg.type = htonl(AM_INIT);
   	initmsg.init.nAvatars = htonl(nAvatars);
    initmsg.init.Difficulty = htonl(Difficulty);
    send(sockfd, &initmsg, sizeof(initmsg), 0);

    // Receive response from Server
    recv(sockfd, &recvmsg, sizeof(recvmsg), 0);

	if (ntohl(recvmsg.type) == AM_INIT_OK) {
    	MazePort = ntohl(recvmsg.init_ok.MazePort);
    	MazeWidth = ntohl(recvmsg.init_ok.MazeWidth);
    	MazeHeight = ntohl(recvmsg.init_ok.MazeHeight);
    } else if (IS_AM_ERROR(recvmsg.type)) {
    	perror("Error: ");
    	close(sockfd);
        exit(6);
	} else {
		fprintf(stderr, "Undefined error.\n");
		close(sockfd);
		exit(6);
	}

    // Create file to write
    char filename[MAXLEN];
    time_t curtime;
    time(&curtime);
	if (!IsDir("./results")) {
		fprintf(stderr, "Please create a results directory for the logs to be stored.\n");
		close(sockfd);
		exit(27);
	}
    sprintf(filename, "./results/Amazing_%s_%d_%d.log", getlogin(), nAvatars, Difficulty);
    FILE *wfile = fopen(filename, "w");
    fprintf(wfile, "%s, %u, %s\n", getlogin(), MazePort, ctime(&curtime));
    fclose(wfile);

	// initialize MazeMap and locations array
    XYPos** locations = (XYPos**) calloc(AM_MAX_AVATAR, sizeof(XYPos*));
	MazeMap *MazeMap = initMazeMap((int) MazeWidth, (int) MazeHeight);

    // Create threads
    pthread_t avatars[nAvatars];
    for (int i = 0; i < nAvatars; i++) {
    	AvatarData* a_data = (AvatarData*) calloc(1, sizeof(AvatarData));
    	CreateAvatarStruct(a_data, i, nAvatars, Difficulty, \
				inet_ntoa(*addr_list[0]), MazePort, MazeMap, filename, locations);
    	pthread_create(&avatars[i], NULL, AvatarFun, (void *) a_data);
		threads++;
    }

	// don't let the main program die before the threads are done
	while (threads > 0) {
		sleep(1);
	}

    // Close and free memory
    close(sockfd);
	free(locations);
	freeMazeMap(MazeMap);
	for (int i = 0; i < nAvatars; i++) {
		pthread_detach(avatars[i]);
	}
	exit(0);
}

/* Taken from file.c in Search engine labs */
int IsDir(const char *path)
{
    struct stat statbuf;                     // stat buffer

    // make sure we have a path
    if(!path) return 0;

    // if we can stat it, and it's a directory
    return (!stat(path, &statbuf) && S_ISDIR(statbuf.st_mode));
}