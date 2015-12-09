#!/bin/bash
#
# ABATS.sh
#
# Description: Test AMStartup and save input into a log file 
# The bash script will run the following tests:
#
# 1. Clean build AMStartup
# 2. Test input parameter
# 3. Unit Testing
# 4. Test maze at level 0, level 1, level 2, level 3 and level 4 with 3 avatars
#
#

# 0. Specify the log file, data directory and the final index file
logfile="./`date +"%a_%b_%d_%T_%Y"`.testlog"

echo "Test started at: `date +"%a_%b_%d_%T_%Y"`" >> $logfile
echo "Operating system and host name: `uname -a`" >> $logfile
echo "Start testing Amazing Maze" >> $logfile
printf "\n\n" >> $logfile

# 1. Clean build the AMStartup
echo "1. make clean and build AMStartup" >> $logfile
make clean &>> $logfile
make AMStartup &>> $logfile
if [ $? -ne 0 ]; then
	echo "build failed" >> $logfile
	exit 1
fi
echo "build AMStartup successfully" >> $logfile
printf "\n\n" >> $logfile

# 2. Clean build test
echo "2. Build AmazingTest" >> $logfile
cd ./test
make clean &>> ../$logfile
make AmazingTest &>> ../$logfile
if [ $? -ne 0 ]; then
	echo "build failed" >> ../$logfile
	exit 1
fi
echo "build AmazingTest successfully" >> ../$logfile
printf "\n\n" >> ../$logfile

cd - > /dev/null


# 3. Test input parameters
echo "testing input parameters. see NOTE in readme.txt" >> $logfile
echo "3.1 Test invalid number of avatars" >> $logfile
echo "./AMStartup -n 0" >> $logfile
printf "Output: " >> $logfile
./AMStartup -n 0 2>> $logfile
if [ $? -eq 0 ]; then
	echo "test invalid number of avatars successfully" >> $logfile
else
	echo "test invalid number of avatars fail" >> $logfile
	exit 1
fi
printf "\n\n" >> $logfile

echo "3.2 Test invalid difficulty level" >> $logfile
echo "./AMStartup -d 1000" >> $logfile
printf "Output: ">> $logfile
./AMStartup -d 1000 2>> $logfile
if [ $? -eq 0 ]; then
	echo "test invalid difficulty level successfully" >> $logfile
else
	echo "test invalid difficulty level fail" >> $logfile
	exit 1
fi
printf "\n\n" >> $logfile

echo "3.3 Test invalid hostname" >> $logfile
echo "./AMStartup -h invalid.cs.dartmouth.edu" >> $logfile
printf "Output: ">> $logfile
./AMStartup -h invalid@cs.dartmouth.edu 2>> $logfile
if [ $? -eq 0 ]; then
	echo "test invalid hostname failed" >> $logfile
else
	echo "test invalid hostname successfully" >> $logfile
fi
printf "\n\n" >> $logfile


# 4. Unit Testing
echo "4. Run Unit Testing" >> $logfile
echo "./AmazingTest" >> $logfile
cd ./test
./AmazingTest &>> ../$logfile

cd - > /dev/null


# 5. Test AMStartup
echo "4.1 Test AMStartup at level 0 with 3 avatars" >> $logfile
echo "./AMStartup -d 0 -n 3 -h pierce.cs.dartmouth.edu" >> $logfile
./AMStartup -d 0 -n 3 -h pierce.cs.dartmouth.edu 2>> $logfile

if [ $? -eq 0 ]; then
	echo "Test AMStartup at level 0 with 3 avatars successfully" >> $logfile
else
	echo "Test AMStartup at level 0 with 3 avatars fail" >> $logfile
	exit 1
fi
printf "\n\n" >> $logfile


echo "4.2 Test AMStartup at level 1 with 3 avatars" >> $logfile
echo "./AMStartup -d 1 -n 3 -h pierce.cs.dartmouth.edu" >> $logfile
./AMStartup -d 1 -n 3 -h pierce.cs.dartmouth.edu 2>> $logfile

if [ $? -eq 0 ]; then
	echo "Test AMStartup at level 1 with 3 avatars successfully" >> $logfile
else
	echo "Test AMStartup at level 1 with 3 avatars fail" >> $logfile
	exit 1
fi
printf "\n\n" >> $logfile


echo "4.3 Test AMStartup at level 2 with 3 avatars" >> $logfile
echo "./AMStartup -d 2 -n 3 -h pierce.cs.dartmouth.edu" >> $logfile
./AMStartup -d 2 -n 3 -h pierce.cs.dartmouth.edu 2>> $logfile

if [ $? -eq 0 ]; then
	echo "Test AMStartup at level 2 with 3 avatars successfully" >> $logfile
else
	echo "Test AMStartup at level 2 with 3 avatars fail" >> $logfile
	exit 1
fi
printf "\n\n" >> $logfile


echo "4.4 Test AMStartup at level 3 with 3 avatars" >> $logfile
echo "./AMStartup -d 3 -n 3 -h pierce.cs.dartmouth.edu" >> $logfile
./AMStartup -d 3 -n 3 -h pierce.cs.dartmouth.edu 2>> $logfile

if [ $? -eq 0 ]; then
	echo "Test AMStartup at level 3 with 3 avatars successfully" >> $logfile
else
	echo "Test AMStartup at level 3 with 3 avatars fail" >> $logfile
	exit 1
fi
printf "\n\n" >> $logfile

echo "4.5 Test AMStartup at level 4 with 3 avatars" >> $logfile
echo "./AMStartup -d 4 -n 3 -h pierce.cs.dartmouth.edu" >> $logfile
./AMStartup -d 4 -n 3 -h pierce.cs.dartmouth.edu 2>> $logfile

if [ $? -eq 0 ]; then
	echo "Test AMStartup at level 4 with 3 avatars successfully" >> $logfile
else
	echo "Test AMStartup at level 4 with 3 avatars fail" >> $logfile
	exit 1
fi
printf "\n\n" >> $logfile

printf "\nAll Tests passed!\n" >> $logfile

# 6. Clean up
make clean
printf "\n\n" >> $logfile

echo "Test ended at: `date +"%a_%b_%d_%T_%Y"`" >> $logfile

exit 0