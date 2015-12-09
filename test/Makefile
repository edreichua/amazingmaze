# Filename: Makefile
# Description: The make file is to build up the Unit Testing file for Amazing Maze
CC = gcc
CFLAGS = -Wall -pedantic -std=c11
EXEC = AmazingTest
SRCDIR=../src/
OBJS = AmazingTest.o AmazingClient.o Maze.o -lpthread
SRCS = AmazingTest.c $(SRCDIR)AmazingClient.c $(SRCDIR)Maze.c 
$(EXEC): $(OBJS)
		$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)
$(OBJS): $(SRCS)
		$(CC) $(CFLAGS) -c $(SRCS)
debug: $(SRCS)
		$(CC) $(CFLAGS) -g -ggdb -c $(SRCS)
		$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS)
clean:
		rm -f *~
		rm -f *#
		rm -f *.o
		rm -f *.log
		rm -f MazeMap
		rm -f AMStartup
		rm -f AmazingTest
		cd $(SRCDIR); rm -f *~; rm -f *#; rm -f *.o;
