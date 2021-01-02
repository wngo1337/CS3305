#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

// Written by William Ngo

// Reading and writing end of the pipe
#define READ_END 0
#define WRITE_END 1

int main(int argc, char **argv)
{

	// If the user does not pass X, Y and Z, the program will terminate
	if (argc != 4)
	{
		printf("Invalid arguments\n");
		exit(0);
	}

	pid_t pid, parent_pid, child_pid;
	int status;
	int port[2];
	char X[20];	// We remember that parent and child get separate copies of these after the fork
	char Y[20];	
	char Z[20];
	status = pipe(port);	// creating the pipe, hopefully it works lol
	parent_pid = getpid();
	printf("A pipe is created for communication between parent (PID %d) and child\n", parent_pid); 
	pid = fork();	// create the child process
	if (pid > 0) {	// in the parent, but child created successfully
		child_pid = pid;
		printf("parent (PID %d) created a child (PID %d)\n", parent_pid, child_pid);
		
		strcpy(X, argv[1]);	// read in the command line argument
		printf("parent (PID %d) receives X = \"%s\" from the user\n", parent_pid, X);
		
		wait(NULL);	// wait until child has completed executing (writing to pipe)
		read(port[0], Y, 15);	// read the string that child child wrote to pipe

		printf("parent (PID %d) reads Y' from the pipe (Y' = \"%s\")\n", parent_pid, Y);
		strcat(X, Y);	// should contain the full string now
		printf("parent (PID %d) concatenates X and Y' to generate the string:\"%s\"\n", parent_pid, X);
	}
	
	if (pid == 0) {	// in the child
		child_pid = getpid();
		parent_pid = getppid();
		strcpy(Y, argv[2]);	// read 2nd and 3rd command line args into Y and Z respectively
		strcpy(Z, argv[3]);
		printf("child (PID %d) receives Y = \"%s\" and Z = \"%s\" from the user\n", child_pid, Y, Z);
		
		strcat(Y, Z);	// Y now contains the concatenation of the two strings
		printf("child (PID %d) concatenates Y and Z to generate Y'= \"%s\"\n", child_pid, Y);
		
		write(port[1], Y, 15);	// write the concatenated string back to the pipe for parent to read
		printf("child (PID %d) writes Y' into the pipe\n", child_pid);
	}
	return 0;
}

