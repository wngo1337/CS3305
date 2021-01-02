#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

// Code completed by William Ngo

int x, y, z; 

void *sum(){
	z=y+x;
	return NULL;
}

int main(){
	pid_t child;
	pthread_t thread;

	int forkCheck, threadCheck;

    // Initializing the global variables
	x=10, y=20, z=0;

	// create child process
	forkCheck = fork();
	// check for child creation error
	if (forkCheck < 0) {	// fork() returns -1 on fail	
		printf("main function: errno number is %d\n", forkCheck);
	}
	// in parent:
	if (forkCheck > 0) {
		// wait for child, and print value of z
		wait(NULL);
		printf("Using a fork(), the value of z in the parent process is: %d\n", z); //value of z after the fork process won't change
	
		// create thread, wait for it to complete, then print value of z
		threadCheck = pthread_create(&thread, NULL, sum, NULL);
		pthread_join(thread, NULL);	// wait for the thread that executes sum function to finish
		printf("Using a thread, the value of z is: %d\n", z); //value of z after passing to the thread will change
	}

	// in child:
	if (forkCheck == 0) {
		child = getpid();
		// sum x and y and store it in z
		z = x + y;
		// print value of z
		printf("Using a fork(), the value of z in the child process is: %d\n", z); 
	}

	return 0;
}

