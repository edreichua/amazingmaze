/* ========================================================================== */
/* File: AmazingTest.c
 *
 * Project name: AmazingTest
 * Group name: Apple Beaver Gold
 *
 * This file contains the unit testing 
 *
 * ========================================================================== */

#define _GNU_SOURCE
#include "../src/common.h"			// common functionality
#include "../src/amazing.h"			// Macros and constants for Amazing Maze
#include "../src/AmazingClient.h"	// avatar fun
#include "../src/Maze.h"				// maze map  and center avatar

int threads;


// ---------------- Private prototypes
int TestCreateAvatarStruct();
int TestinitMazeMap();
int TestwhereToTurn();
int TestfreeMazeMap();
int TestprintMazeMapToFile();
int TestupdateMazeMap();
int TestprintMapToConsole();
int TestupdateLocations();


/* ========================================================================== */
int main(void){
	
	int count = 0;
	if(TestinitMazeMap()){
		printf("Test function initMazeMap successfully.\n\n");
		count ++;
	}else{
		printf("Test function initMazeMap fail.\n\n");
	}
		
	if(TestwhereToTurn()){
		printf("Test function whereToTurn successfully.\n\n");
		count++;
	}else{
		printf("Test function whereToTurn fail.\n\n");
	}
	
	if(TestupdateMazeMap()){
		printf("Test function updateMazeMap successfully.\n\n");
		count ++;
	}else{
		printf("Test function updateMazeMap fail.\n\n");
	}
	
	if(TestprintMapToConsole()){
		printf("Test function printMapToConsole successfully.\n\n");
		count ++;
	}else{
		printf("Test function printMapToConsole fail.\n\n");
	}
	
	if(TestupdateLocations()){
		printf("Test function updateLocations successfully.\n\n");
		count ++;
	}else{
		printf("Test function updateLocations fail.\n\n");
	}
	
	if(TestprintMazeMapToFile()){
		printf("Test function printMazeMapToFile successfully.\n\n");
		count++;
	}else{
		printf("Test function printMazeMapToFile fail.\n\n");
	}
	
	
	if(TestfreeMazeMap()){
		printf("Test function freeMazeMap successfully.\n\n");
		count++;
	}else{
		printf("Test function freeMazeMap fail.\n\n");
	}
	
	if(TestCreateAvatarStruct()){
		printf("Test function CreateAvatarStruct successfully.\n");
		count ++;
	}else{
		printf("Test function CreateAvatarStruct fail.\n");
	}
	
	if(count==8){
		printf("All tests in unit testing passed!\n");
	}
	
	return 0;
}

int TestinitMazeMap(){
	int MazeWidth, MazeHeight; 
	MazeWidth = MazeHeight = -1;
	MazeMap* mazemap= initMazeMap(MazeWidth,MazeHeight);
	
	if(!mazemap){
		printf("Test initMazeMap for invalid parameters is successful.\n");
	}
	
	free(mazemap);
	MazeWidth = MazeHeight = 16;
	mazemap = initMazeMap(MazeWidth,MazeHeight);
	
	if(mazemap){
		printf("Test initMazeMap for valid parameters successfully.\n");
	}
	
	free(mazemap);
	return 1;
}

int TestwhereToTurn(){
	int MazeWidth, MazeHeight; 
	MazeWidth = MazeHeight = 16;
	MazeMap* mazemap = initMazeMap(MazeWidth,MazeHeight);

	XYPos* coordinate = (XYPos*) calloc(AM_MAX_AVATAR, sizeof(XYPos));
	coordinate->x = 5;
	coordinate->y = 6;
	printf("Test invalid previous direction for whereToTurn function\n");
	if (whereToTurn(7, 0, coordinate,mazemap->map)==-2){
		printf("Test invalid previous direction successfully for whereToTurn function\n");
	}else{
		printf("Test invalid previous direction fail for whereToTurn function\n");
		return 0;
	}
	
	printf("Test invalid blocked value for whereToTurn function\n");
	if (whereToTurn(0, 7, coordinate,mazemap->map)==-2){
		printf("Test invalid blocked value successfully for whereToTurn function\n");
	}else{
		printf("Test invalid blocked value fail for whereToTurn function\n");
		return 0;
	}
	
	free(coordinate);
	coordinate = NULL;
	
	printf("Test NULL as coordinate for whereToTurn function\n");
	if (whereToTurn(0, 0, NULL,mazemap->map)==-2){
		printf("Test NULL as coordinate successfully for whereToTurn function\n");
	}else{
		printf("Test NULL as coordinate fail for whereToTurn function\n");
		return 0;
	}
	
	coordinate = (XYPos*) calloc(AM_MAX_AVATAR, sizeof(XYPos));
	coordinate->x = 5;
	coordinate->y = 6;
	
	freeMazeMap(mazemap);
	mazemap = NULL;
	
	printf("Test NULL as MazeMap for whereToTurn function\n");
	if (whereToTurn(0, 0, coordinate,NULL)==-2){
		printf("Test NULL as MazeMap successfully for whereToTurn function\n");
	}else{
		printf("Test NULL as MazeMap fail for whereToTurn function\n");
		return 0;
	}
	free(coordinate);
	coordinate = NULL;
	
	return 1;
}

int TestupdateMazeMap(){

	int MazeWidth, MazeHeight; 
	MazeWidth = MazeHeight = 16;
	MazeMap* mazemap = initMazeMap(MazeWidth,MazeHeight);
	
	XYPos* oldpos = (XYPos*) calloc(AM_MAX_AVATAR, sizeof(XYPos));
	oldpos->x = 5;
	oldpos->y = 6;
	
	XYPos* newpos = (XYPos*) calloc(AM_MAX_AVATAR, sizeof(XYPos));
	newpos->x = 5;
	newpos->y = 6;
	
	printf("Test NULL mazemap for function updateMazeMap\n");
	if (!updateMazeMap(NULL, oldpos, newpos, 0)){
		printf("Test NULL mazemap for function updateMazeMap successfully\n");
	}else{
		printf("Test NULL mazemap for function updateMazeMap fail\n");
		free(oldpos); free(newpos); freeMazeMap(mazemap);
		return 0;
	}
	
	printf("Test invalid old_pos parameter\n");
	if (!updateMazeMap(mazemap, NULL, newpos, 0)){
		printf("Test invalid old_pos parameter for function updateMazeMap successfully\n");
	}else{
		printf("Test invalid old_pos parameter for function updateMazeMap fail\n");
		free(oldpos); free(newpos); freeMazeMap(mazemap);
		return 0;
	}
	
	
	printf("Test invalid new_pos parameter\n");
	if (!updateMazeMap(mazemap, oldpos, NULL, 0)){
		printf("Test invalid new_pos parameter for function updateMazeMap successfully\n");
	}else{
		printf("Test invalid new_pos parameter for function updateMazeMap fail\n");
		free(oldpos); free(newpos); freeMazeMap(mazemap);
		return 0;
	}
	
	printf("Test invalid direction\n");
	if (!updateMazeMap(mazemap, oldpos, newpos, 7)){
		printf("Test invalid dir parameter for function updateMazeMap successfully\n");
	}else{
		printf("Test invalid dir parameter for function updateMazeMap fail\n");
		free(oldpos); free(newpos); freeMazeMap(mazemap);
		return 0;
	}
	
	free(oldpos); free(newpos); freeMazeMap(mazemap);
	return 1;
}

int TestprintMapToConsole(){
	
	XYPos* pos = (XYPos*) calloc(AM_MAX_AVATAR, sizeof(XYPos));
	pos->x = 5;
	pos->y = 6;

	printf("Test invalid MazeWidth for printMapToConsole\n");
	if (!printMapToConsole(-1,5,&pos,3,1)){
		printf("Test invalid MazeWidth parameter for function printMapToConsole successfully\n");
	}else{
		printf("Test invalid MazeWidth parameter for function printMapToConsole fail\n");
		free(pos); 
		return 0;
	}
	
	printf("Test invalid MazeHeight for printMapToConsole\n");
	if (!printMapToConsole(5,-5,&pos,3,1)){
		printf("Test invalid MazeHeight parameter for function printMapToConsole successfully\n");
	}else{
		printf("Test invalid MazeHeight parameter for function printMapToConsole fail\n");
		free(pos); 
		return 0;
	}
	
	printf("Test invalid locations for printMapToConsole\n");
	if (!printMapToConsole(5,5,NULL,3,1)){
		printf("Test invalid locations parameter for function printMapToConsole successfully\n");
	}else{
		printf("Test invalid locations parameter for function printMapToConsole fail\n");
		free(pos); 
		return 0;
	}
	
	printf("Test invalid nAvatars for printMapToConsole\n");
	if (!printMapToConsole(5,5,&pos,30,1)){
		printf("Test invalid nAvatars parameter for function printMapToConsole successfully\n");
	}else{
		printf("Test invalid nAvatars parameter for function printMapToConsole fail\n");
		free(pos); 
		return 0;
	}
	
	printf("Test invalid nummoves for printMapToConsole\n");
	if (!printMapToConsole(5,5,&pos,3,-1)){
		printf("Test invalid nummoves parameter for function printMapToConsole successfully\n");
	}else{
		printf("Test invalid nummoves parameter for function printMapToConsole fail\n");
		free(pos); 
		return 0;
	}
	
	free(pos);
	return 1;
}

int TestupdateLocations(){
	
	XYPos* pos = (XYPos*) calloc(AM_MAX_AVATAR, sizeof(XYPos));
	pos->x = 5;
	pos->y = 6;
	
	printf("Test invalid locations parameter for updateLocations\n");
	if (!updateLocations(NULL,pos,1)){
		printf("Test invalid locations parameter for function updateLocations successfully\n");
	}else{
		printf("Test invalid locations parameter for function updateLocations fail\n");
		free(pos); 
		return 0;
	}
	
	printf("Test invalid my_locations parameter for updateLocations\n");
	if (!updateLocations(&pos,NULL,1)){
		printf("Test invalid my_locations parameter for function updateLocations successfully\n");
	}else{
		printf("Test invalid my_locations parameter for function updateLocations fail\n");
		free(pos); 
		return 0;
	}
	
	printf("Test invalid ID parameter for updateLocations\n");
	if (!updateLocations(&pos,pos,-1)){
		printf("Test invalid ID parameter for function updateLocations successfully\n");
	}else{
		printf("Test invalid ID parameter for function updateLocations fail\n");
		free(pos); 
		return 0;
	}
	free(pos);
	return 1;

}

int TestprintMazeMapToFile(){

	int MazeWidth, MazeHeight; 
	MazeWidth = MazeHeight = -1;
	MazeMap* mazemap = initMazeMap(MazeWidth,MazeHeight);
	if(!mazemap){
		printf("Test invalid input parameter (invalid MazeWidth and MazeHeight) successfully for printMazeMapToFile function\n");
	}else if(!printMazeMapToFile(mazemap)){
		printf("Test invalid input parameter (invalid MazeWidth and MazeHeight) successfully for printMazeMapToFile function\n");
	}else{
		printf("Test invalid input parameter (invalid MazeWidth and MazeHeight) fail for printMazeMapToFile function\n");
		return 0;
	}
	
	freeMazeMap(mazemap);
	return 1;

}

int TestfreeMazeMap(){
	if(!freeMazeMap(NULL)){
		printf("Test invalid free successfully for freeMazeMap function\n");
	}else{
		printf("Test invalid free successfully for freeMazeMap function\n");
		return 0;
	}
	return 1;
}

int TestCreateAvatarStruct(){
	
	int flag = 0;
		
	AvatarData* a_data = (AvatarData*) calloc(1, sizeof(AvatarData));
	int i, nAvatars, Difficulty;
	uint32_t MazePort, MazeWidth, MazeHeight;
	
	i = nAvatars = Difficulty = 2;
	MazePort = MazeWidth = MazeHeight = 16;
	MazeMap* mazemap = initMazeMap(MazeWidth,MazeHeight);
	char filename[] = "file.log";
	char hostname[] = "pierce.cs.dartmouth.edu";
	
	
	XYPos* locations  = (XYPos*) calloc(AM_MAX_AVATAR, sizeof(XYPos*));
	
	locations->x = 10;
	locations->y = 10;
	
	
    CreateAvatarStruct(a_data, i, nAvatars, Difficulty, hostname, \
		MazePort,mazemap, filename, &locations);
	
	if( a_data->AvatarID == i && a_data->nAvatars == nAvatars && a_data->Difficulty == Difficulty \
		&& !strcmp(a_data->IPadd, hostname) && a_data->MazePort == MazePort && !strcmp(a_data->filename, filename) \
		&& (*a_data->locations)->x == locations->x && (*a_data->locations)->y == locations->y){
		flag = 1;
	}
		
	freeMazeMap(mazemap);
	free(locations);
	free(a_data);
	return flag;
}