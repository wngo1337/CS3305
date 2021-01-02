#include <stdlib.h>

#include "scheduling.h"

/*
Code written by William Ngo
/*

##  Round-Robin Scheduling Algorithm  ##

	Parameters:-

	queue: 1D int array containing the CPU burst time for every process in a ready queue
	np: number of processes (length of `queue` array)
	tq: time quantum for the RR algorithm


	Returns:-

	rr_result struct pointer, where
	rr_result -> order: 1D int array, containing the execution order of processes (i.e. `(rr_result -> order)[0]` is the process number of the first process)
		** hint: p1 is always the first item in order of selection in round-robin and FCFS
    rr_result -> order_n: int, representing the size of the `order` array, described above ^
	rr_result -> turnarounds: 1D int array, containing the turnaround time for each process (i.e. `(rr_result -> turnarounds)[0]` is the turnaround time for p1)
*/
rr_result *rr(int *queue, int np, int tq)
{
	rr_result *result = malloc(sizeof(rr_result));
	result->np = np;
	result->turnarounds = malloc(sizeof(int) * np);	// compute all turnaround times and put in here...
	
	// remember to malloc/realloc for order array because we don't know what size it will be
	
	/* 
	while there are still processes to compute, loop over all processes with CPU burst remaining.
	
	turnaround time = completion time - start time
	we can get the start times by making an array full of -1, and -1 means the process didn't start yet
	Otherwise, the starting time is the current time
	*/
	
	int isDone = 0;	// Note to self: 0 = false, 1 and other ints = true
	int elapsedTime = 0;	
	int *order = NULL;	// need dynamic memory since we don't know when all processes will finish
	
	int order_n = 0;	// increment every time a process gets the CPU
	int remainingBurstTime[np];	// we will need the original burst times later, so make a copy for processing
	int startTime[np];
	int completionTime[np];	// hopefully defaults all array values to 0... Oops they don't. I'll fix that
	int turnaroundTime[np];

	int j = 0;
	for (j = 0; j < np; j++) {	// cleaning up arrays to avoid unintended stuff
		startTime[j] = -1;	// default start value to overwrite
		remainingBurstTime[j] = queue[j]; // copying burst times since we will be using these to keep track of progress
		completionTime[j] = 0;
	}
	
	int i = 0;
	while (!isDone) {	
		isDone = 1;	// Will only exit if there are no remaining processes
		for (i = 0; i < np; i++) {
			if (startTime[i] == -1) {	// the process hasn't started yet
				startTime[i] = elapsedTime;
			}
			if (remainingBurstTime[i] > 0) {	// Process i still has more to compute
				isDone = 0; // Tell "OS" to go through the queue again
				order_n = order_n + 1;	// counting how many things were processed
				order = realloc(order, order_n * sizeof(int));
				order[order_n - 1] = i;	// STUPID OUT OF BOUND INDEX
				
				if (remainingBurstTime[i] <= tq) {	// Case 1: Process i will finish computing this cycle
					elapsedTime = elapsedTime + remainingBurstTime[i];	// Finished at end or before time quantum
					completionTime[i] = completionTime[i] + elapsedTime;
					remainingBurstTime[i] = 0;	// Process is finished. Don't count it in the loop anymore...
				}
				
				else {	// Case 2: Process i will not finish in time -> runs the full time quantum			
					elapsedTime = elapsedTime + tq;
					remainingBurstTime[i] = remainingBurstTime[i] - tq;
				}							
			 }		 
		}
		if (isDone) {	// should only be true (1) if all processes finished
			break;
		}
	}
	
	for (i = 0; i < np; i++) {	// calculate turnaround times from our start/end time arrays
		turnaroundTime[i] = completionTime[i] - startTime[i];
	}

	// assign the results to the struct before freeing
	result->order = malloc(order_n * sizeof(int));	// finally know the size of the order array
	
	int k;
	for (k = 0; k < order_n; k++) {	// copy over saved process order from dynamic order array
		(result->order)[k] = order[k];
	}
	
	for (k = 0; k < np; k++) {	// copy over turnaround times
		(result->turnarounds)[k] = turnaroundTime[k];
	}
	
	result->order_n = order_n;

	
	free(order);
	return result;
}

