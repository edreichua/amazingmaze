readme.txt
Apple Beaver Gold
CS50: Amazing Maze


FILE ORGANIZATION:
project (main directory)
	src (source code)
		amazing.h
		AmazingClient.c
		AmazingClient.h
		AMStartup.c
		common.h
		Maze.c
		Maze.h
	test (directory for test files)
		AmazingTest.c
		Makefile (for the unit testing program)
	results (directory of log files results)
	ABATS.sh
	Makefile (for the main program AMStartup)
	readme.txt

	
HOW TO BUILD:
cd into the the project directory and type in the terminal "make". Hit Enter.
If there is any error with the clock skew, make again.

If the user wishes to run the unit testing program, call ABATS.sh in the main
directory.

HOW TO RUN:
The main program:
cd into the project directory and type AMStartup. The command may be followed
by three optional parameters. If a parameter is entered, please use the
corresponding option (-d, -n, or -h).
(1) -n: the number of avatars, a number from 2 through AM_MAX_AVATAR, inclusive
	example: -n 8
(2) -d: the difficulty level, a number from 0 through AM_MAX_DIFFICULTY, inclusive
	example: -d 2
(3) -h: the host name
	example: -h pierce.cs.dartmouth.edu
NOTE:
If the user enters an invalid difficulty level, the program defaults the
difficulty level to 0. If the user enters an invalid number of avatars,
the program defaults the number of avatars to 2. If the user does not specify
a hostname, the program defaults the hostname to pierce.cs.dartmouth.edu. The
program then runs normally. However, if the user enters a hostname, but it is
invalid, then the program will quit.

The test script:
cd into the project directory and run ABATS.sh. This script will clean build
the main program and the testing program. This script will also check invalid
parameters to the main program as well as run the unit testing program.
Then, the script will print a log of its results in the project directory.
DO NOT run ABATS.sh from a directory other than the main project directory as
the builds will not be successful.

ASSUMPTIONS:
- If the user enters in a non-number for the number of avatars, then the program
will default the number of avatars to 2. If the user enters in a non-number for
the difficulty level, then the program will default the difficulty level to 0.
This is because of the nature of the atoi() function, which returns 0 if the
parameter is not a number.
