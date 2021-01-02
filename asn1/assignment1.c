#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

// Written by William Ngo


int main(int argc, char **argv) {

	if (argc != 2)
		{
			printf("Invalid arguments\n");
			exit(0);
		}

        pid_t pid, parent_pid, child_1_pid, child_1_dot_1_pid, child_2_pid;
        pid = fork();   // creating child1
        if (pid > 0) {  // we are in the parent
                wait(NULL);     // waiting for child1 process to finish execution
                pid = fork();   // creating child2
                if (pid > 0) {
                        wait(NULL);	// waiting for child2 process to finish execution
                }
                if (pid == 0) {
						parent_pid = getppid();
						child_2_pid = getpid();
                    	printf("parent (PID %d) created child_2 (PID %d)\n", parent_pid, child_2_pid);
						char pid_as_string[20];	// just long array so we don't have to use malloc or anything...
						sprintf(pid_as_string, "%d", child_2_pid);	
						char* pid_pointer = pid_as_string;

						printf("child_2 (PID %d) is calling an external program external_program.out and leaving child_2..\n", child_2_pid);
						execl("external_program.out", "external_program.out", pid_pointer, NULL);	// passing the PID to external program and executing it
						// cuts program execution by destroying child_2 :'(
                }
        }

        if (pid == 0) { // we are in the child
				parent_pid = getppid();
				child_1_pid = getpid();
				printf("parent process (PID %d) created child_1 (PID %d) \n", parent_pid, child_1_pid);
				printf("parent (PID %d) is waiting for child_1 (PID %d) to complete before creating child_2\n", parent_pid, child_1_pid);
				
                pid = fork();   // creating child_1.1
                if (pid > 0) {  // we are in child_1 still
						wait(NULL);
						printf("child_1 (PID %d) is now complete\n", child_1_pid);
                }
                if (pid == 0) { // we are in child_1.1
						child_1_pid = getppid();
						child_1_dot_1_pid = getpid();
						printf("child_1 (PID %d) created child_1.1 (PID %d)\n", child_1_pid, child_1_dot_1_pid);
                }
        }
	return 0;
}


