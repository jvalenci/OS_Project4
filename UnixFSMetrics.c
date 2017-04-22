#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

unsigned long long int rdtsc(void)
{
   unsigned long long int x;
   unsigned a, d;
 
   __asm__ volatile("rdtsc" : "=a" (a), "=d" (d));
 
   return ((unsigned long long)a) | (((unsigned long long)d) << 32);;
}

int main(int argc, char **argv){
	unsigned long long a, b;
	struct timespec start, end;
    long long unsigned int diff;

	//gets the starting time before system calls with error checking
    if (clock_gettime(CLOCK_REALTIME, &start) == -1) {
        printf("Error reading the time for the start of the system call loop.");
    }

	sleep(10);


	//gets the ending time after system calls with error checking
    if (clock_gettime(CLOCK_REALTIME, &end) == -1) {
        printf("Error reading the time for the end of the system call loop.");
    }

	diff = 1000 * (end.tv_sec - start.tv_sec);
	printf("%llu \n", diff );
	
	return 0;
}