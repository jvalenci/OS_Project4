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

int main(int argc, char **argv){

	struct timespec start, end;
    long long unsigned int diff;

    //create 1G file to test  
    fprintf(stdout, "%s\n", "Please wait test file is being generated..." );
    system("head -c 1G </dev/urandom > /tmp/testFile");
    fprintf(stdout, "%s\n", "Test file has been created.");

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