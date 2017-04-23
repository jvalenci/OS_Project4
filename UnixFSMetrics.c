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

unsigned long long int rdtsc(void)
{
   unsigned long long int x;
   unsigned a, d;
 
   __asm__ volatile("rdtsc" : "=a" (a), "=d" (d));
 
   return ((unsigned long long)a) | (((unsigned long long)d) << 32);;
}

//prints the test choices
void printSelections(){
	fprintf(stdout, "%s\n", "choose which test you want to perform. Enter 0 to exit" );
    fprintf(stdout, "%s\n", "1. How big is the block size used by the file system to read data?" );
    fprintf(stdout, "%s\n", "2. During a sequential read of a large file, how much data is prefetched by the file system?" );
    fprintf(stdout, "%s\n", "3. How big is the file cache?" );
    fprintf(stdout, "%s\n", "4. How many direct pointers are in the inode?" );
    fprintf(stdout, "%s\n", "5. All of the above" );
}

//creates a 1G file to test
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

void discard_junk () 
{
  char c;
  while((c = getchar()) != '\n' && c != EOF)
    ;
}

void getUserInputAndValidate(int* input){
	while(scanf("%d", input) != 1){
    	fprintf(stderr, "%s\n", "Enter a number." );
		discard_junk();
    }
    if(!(*input >= 1 && *input <= 5 || *input == 0)){
    	fprintf(stderr, "%s\n", "Number has to be inbetween 1 and 5 or 0");
    	getUserInputAndValidate(input);
    }
}

void firstTest(){
	struct timespec start, end;
    long long unsigned int diff;
	int file = 0;
	int sizePowerOf2 = 1;
	int i;
	ssize_t readCount;

	char *buffer = (char *)malloc(sizePowerOf2);

	file = open("/tmp/testFile.txt", O_RDONLY);
	assert(file > 0);

	printf("%s\n", "1st test:");
	printf("%s\n", "read size \t time");


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

	    // printf("%llu\n",readCount );

	    diff = 1000 * (end.tv_sec - start.tv_sec);

		printf("%d \t %llu\n", sizePowerOf2, diff);
		lseek(file, rand() % INT_MAX, SEEK_SET);
	}
	free(buffer);
	close(file);
}

void secondTest(){

}

void thirdTest(){

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
    			fprintf(stderr, "%s\n", "I do not recognize this selections.");
    	}

    	//reprompt and prime userinput
    	printSelections();
    	getUserInputAndValidate(&userInput);
    }

	return 0;
}