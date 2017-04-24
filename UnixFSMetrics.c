#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <limits.h>

//prints the test choices for the prompt
void printSelections(){
	fprintf(stdout, "%s\n", "choose which test you want to perform. Enter 0 to exit" );
    fprintf(stdout, "%s\n", "1. How big is the block size used by the file system to read data?" );
    fprintf(stdout, "%s\n", "2. During a sequential read of a large file, how much data is prefetched by the file system?" );
    fprintf(stdout, "%s\n", "3. How big is the file cache?" );
    fprintf(stdout, "%s\n", "4. How many direct pointers are in the inode?" );
    fprintf(stdout, "%s\n", "5. All of the above" );
}

//creates a 3G file to run various test and allowing plenty of room.
void createFile(){
	int file;
	file = open("/tmp/testFile.txt", O_RDONLY);
	if(file < 0){
		fprintf(stdout, "%s\n", "Please wait test file is being generated..." );
		system("head -c 3G </dev/urandom > /tmp/testFile.txt");	
		fprintf(stdout, "%s\n\n", "Test file has been created.");
	}
	else{
		printf("%s\n", "testFile already exists.");
	}
	close(file);
}

//cleans the stdin of junk. Used in the input validation.
void discard_junk () 
{
  char c;
  while((c = getchar()) != '\n' && c != EOF)
    ;
}

//validates the user input. user input can only be 1-5 or 0 to quit.
void getUserInputAndValidate(int* input){
	while(scanf("%d", input) != 1){
    	fprintf(stderr, "%s\n", "Enter a number 1-5 or 0." );
		discard_junk();
    }
    if(!(*input >= 1 && *input <= 5 || *input == 0)){
    	fprintf(stderr, "%s\n", "Number has to be between 1 and 5 or 0");
    	getUserInputAndValidate(input);
    }
}

//gradually increase the buffer size by powers of 2 to measure how long each read would take.
//At the same time I'm repositioning the file desc pointer to a random position, where the OS hasn't cached blocks yet
//and force a more accurate blocksize read by avoiding misreads of prefetched data.
void firstTest(){
	struct timespec start, end;
    long long unsigned int diff;
	int file = 0;
	int sizePowerOf2 = 1;
	ssize_t readCount;

	char *buffer = (char *)malloc(sizePowerOf2);
	assert(buffer);

	file = open("/tmp/testFile.txt", O_RDONLY);
	assert(file > 0);

	printf("%s\n", "1st test:");
	printf("%-17s %s\n", "readSize (bytes)", "time in nanoseconds");

	for (sizePowerOf2 = 2; sizePowerOf2 < INT_MAX/2; sizePowerOf2 *= 2){

		buffer = (char *) realloc(buffer, sizePowerOf2);

		//gets the starting time before system calls with error checking
	    if (clock_gettime(CLOCK_REALTIME, &start) == -1) {
	        printf("Error reading the time for the start of the system call loop.");
	    }

		readCount = read(file, buffer, sizePowerOf2);

		//gets the ending time after system calls with error checking
	    if (clock_gettime(CLOCK_REALTIME, &end) == -1) {
	        printf("Error reading the time for the end of the system call loop.");
	    }
	    assert(readCount > -1);

	    diff = 1000000000 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;

		printf("%-17d %llu\n", sizePowerOf2, diff);
		lseek(file, rand() % INT_MAX, SEEK_SET);
	}

	//clean up
	free(buffer);
	close(file);
}

//NOTE: running this method will take a long time. It was needed to have accurate prefetched data
//I'm keep my buffer at size 1024, which is 1KB, so I can see where the slow downs are in the amount of time it takes to perform the reads.
//While my reads perform extremely fast, meaning the data is already prefetched. The slow downs indicated that it need to refill
//the buffer of prefetched data. This will read the entire 3G file 1KB at a time.
void secondTest(){
	struct timespec start, end;
    long long unsigned int diff;
	int file = 0;
	int i;
	ssize_t readCount;
	int bufferCount = 1024;

	char *buffer = (char *)malloc(bufferCount);
	assert(buffer);

	file = open("/tmp/testFile.txt", O_RDONLY);
	assert(file > 0);

	printf("%s\n", "2nd test:");
	printf("%-17s %s\n", "byte read", "time in nanoseconds");

	for (i = 0; readCount != 0 ; i += bufferCount){

		//gets the starting time before system calls with error checking
	    if (clock_gettime(CLOCK_REALTIME, &start) == -1) {
	        printf("Error reading the time for the start of the system call loop.");
	    }

		readCount = read(file, buffer, bufferCount);

		//gets the ending time after system calls with error checking
	    if (clock_gettime(CLOCK_REALTIME, &end) == -1) {
	        printf("Error reading the time for the end of the system call loop.");
	    }
	    assert(readCount > -1);

	    diff = 1000000000 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;

		printf("%-17d %llu\n", i + bufferCount, diff);
	}

	//clean up
	free(buffer);
	close(file);
}

void thirdTest(){
	const int READS_PER_SIZE = 3;

	struct timespec start, end;
    long long unsigned int diff;
	int file = 0;
	int readSize = 1;
	int readIter = 0;
	ssize_t readCount;

	char *buffer = (char *)malloc(readSize);

	file = open("/tmp/testFile.txt", O_RDONLY);
	assert(file > 0);

	printf("%s\n", "3rd test:");
	printf("%-17s %s\n", "readSize (bytes)", "time in nanoseconds");

	// increase the buffer/read size by powers of 2 and for each size,
	// read the same part of the file multiple times to evaluate the cache performance.
	for (readSize = 2; readSize < INT_MAX/2; readSize *= 2) {
		
		buffer = (char *) realloc(buffer, readSize);

		for (readIter = 0; readIter < READS_PER_SIZE; readIter++) {

			//gets the starting time before system calls with error checking
		    if (clock_gettime(CLOCK_REALTIME, &start) == -1) {
		        printf("Error reading the time for the start of the system call loop.");
		    }

			readCount = read(file, buffer, readSize);

			//gets the ending time after system calls with error checking
		    if (clock_gettime(CLOCK_REALTIME, &end) == -1) {
		        printf("Error reading the time for the end of the system call loop.");
		    }
		    assert(readCount > -1);

		    diff = 1000000000 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;

			printf("%-17d %llu\n", readSize, diff);
		}

		// keep reading from the beginning of the file
		lseek(file, 0, SEEK_SET);
	}

	// clean up
	free(buffer);
	close(file);
}

void fourthTest(){

}

void allTests(){
	firstTest();
	secondTest();
	thirdTest();
	fourthTest();
}

int main(int argc, char **argv){

    int userInput = 0;
    time_t t;

    srand((unsigned) time(&t));

    createFile();
    printSelections();
    getUserInputAndValidate(&userInput);
    
    while( userInput != 0){
    	switch(userInput){
    		case 1:
    			firstTest();
    			break;
    		case 2:
    			secondTest();
    			break;
    		case 3:
    			thirdTest(); 
    			break;
    		case 4:
    			fourthTest();
    			break;
    		case 5:
    			allTests();
    			break;
    		default:
    			fprintf(stderr, "%s\n", "I do not recognize this selection.");
    	}

    	//reprompt and prime userinput
    	printSelections();
    	getUserInputAndValidate(&userInput);
    }

	return 0;
}