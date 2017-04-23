#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

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
	//create 1G file to test  
    fprintf(stdout, "%s\n", "Please wait test file is being generated..." );
    // system("head -c 1G </dev/urandom > /tmp/testFile");
    fprintf(stdout, "%s\n\n", "Test file has been created.");

}

void getUserInputAndValidate(int* input){
	while(scanf("%d", input) < 0){
    	fprintf(stderr, "%s\n", "Enter a number." );
    }
    if(!(*input >= 1 && *input <= 5 || *input == 0)){
    	fprintf(stderr, "%s\n", "Number has to be inbetween 1 and 5 or 0");;
    	getUserInputAndValidate(input);
    }
}

int main(int argc, char **argv){

	struct timespec start, end;
    long long unsigned int diff;
    int userInput = 0;

    createFile();
    printSelections();
    getUserInputAndValidate(&userInput);
    
    while( userInput != 0){
    	printf("%s\n", "works" );
    	printSelections();
    	getUserInputAndValidate(&userInput);
    }

	//gets the starting time before system calls with error checking
    if (clock_gettime(CLOCK_REALTIME, &start) == -1) {
        printf("Error reading the time for the start of the system call loop.");
    }


	//gets the ending time after system calls with error checking
    if (clock_gettime(CLOCK_REALTIME, &end) == -1) {
        printf("Error reading the time for the end of the system call loop.");
    }

	diff = 1000 * (end.tv_sec - start.tv_sec);
	printf("%llu \n", diff );

	return 0;
}