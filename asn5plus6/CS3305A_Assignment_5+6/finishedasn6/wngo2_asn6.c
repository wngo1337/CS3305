// REMEMBER TO USE THE -pthread FLAG WHEN COMPILING OR IT WON'T WORK
// Would probably be easier to write to my Makefile and forget, but I didn't do it yet

/*
Assignment 5 and 6 by William Ngo.

This program will model the accounts as integers, and the clients as threads.
WILL NEED TO USE MALLOC BECAUSE DON'T KNOW THE NUMBER OF ACCOUNTS :(
Each line read by the program will consist of one client's actions.

Requests are done in random order (because of threads), so we need to lock the critical section whenever
a client is inside so that things don't get weird.

Linux supports semaphores through the mutex in pthread, so we just need to import that
*/

#include <pthread.h>	// for threads and mutex
#include <stdio.h>	// for file I/O
#include <stdlib.h>	// for converting strings to values
#include <string.h>	// for storing lines
#include <unistd.h>	// Well, I don't really know what this does, but it was used in asn4

pthread_mutex_t lock;	// give global access to the mutex 

struct lineAndAccountsStruct {	// because we can't pass multiple arguments to a pthread function???
	char *line;
	int *accounts;
};

// this function performs an entire line of client operations
void *performClientOperations(void *lineAndAccountsStruct) {	// MUST REWORK FOR THE STRUCT
	struct lineAndAccountsStruct *info = lineAndAccountsStruct;
	char *myLine = info->line;
	int *myAccounts = info->accounts;
	
	const char delim[2] = " ";
	char *word = strtok(myLine, delim);	// discarding the word containing the client number at the start
	char operationChar;
	
	int amount;
	
	for (word = strtok(NULL, delim); word != NULL; word = strtok(NULL, delim)) {
		operationChar = *word;
		if (word[0] == 'd' || word[0] == 'w') {	// single account operations
			// read the next word, which contains the account number
			word = strtok(NULL, delim);	// get the the word that contains the account number
			int tempAccountNum1 = atoi(&word[1]);	// convert the character to a number

			word = strtok(NULL, delim);	// get the dollar amount following the account 
			amount = atoi(word);			
			int trueAccountIndex1 = tempAccountNum1 - 1;	// index starts at 0, but accounts start at 1
				if (operationChar == 'd') {
					pthread_mutex_lock(&lock);	// LOCK the mutex when we want to access
						myAccounts[trueAccountIndex1] += amount;	// deposit into the account
					pthread_mutex_unlock(&lock);	// UNLOCK when done so other threads can access
				}
				else if (operationChar == 'w') {
					if (myAccounts[trueAccountIndex1] >= amount) {
						pthread_mutex_lock(&lock);
							myAccounts[trueAccountIndex1] -= amount;
						pthread_mutex_unlock(&lock);
					}
				}	
		}
		else if (word[0] == 't') {	// double account operations
			word = strtok(NULL, delim);	// gets the first account
			int tempAccountNum2 = atoi(&word[1]);
			int trueAccountIndex2 = tempAccountNum2 - 1;
			word = strtok(NULL, delim);	// gets the second account
			int tempAccountNum3 = atoi(&word[1]);
			int trueAccountIndex3 = tempAccountNum3 - 1;

			word = strtok(NULL, delim);	// gets the dollar amount of the transfer
			amount = atoi(word);
			if (myAccounts[trueAccountIndex2] >= amount) {
				pthread_mutex_lock(&lock);
					myAccounts[trueAccountIndex2] -= amount;
					myAccounts[trueAccountIndex3] += amount;
				pthread_mutex_unlock(&lock);
			}
		}	
	}	// finished all client activities
	free(word);
}

int main (int argc, char* argv[]) {
	if (argc != 2) {	// 2 args because first arg is program name...
		printf("This program requires one input file!\n");
		exit(1);
	}
	
	// Mainly used io.c from assignment 4 as a reference on I/O
	FILE *filepointer = fopen(argv[1], "r");
	if (filepointer == NULL)
	{	
		perror(argv[1]);
		exit(1);
	}
	char *buffer = NULL;	// Setting up buffer for file I/O 
	size_t length = 0;	// I don't really know why we need to know this lol
	char *someChar;
	const char delim[2] = " ";
	ssize_t nread;	// Just a type that can store negative numbers?

	// Accounts start at 1, but array would start at 0, so guess we assign to x-1
	int *accounts = malloc(sizeof(int));	// just give it any size for realloc lol
	pthread_t *clients;
	int numAccounts = 0;
	int numClients = 0;
	
	// Initialize the pthread
	pthread_mutex_init(&lock, NULL); // pthread_mutex_lock for lock, and unlock for unlock lol. Destroy when done
	
	// Initial passover of text is simply to get the number of accounts and clients so we can malloc them
	while (nread = getline(&buffer, &length, filepointer) != -1) {	
		someChar = strtok(buffer, delim);	// gets the word with the account number in it
		if (someChar[0] == 'a') {
			numAccounts += 1;
			someChar = strtok(NULL, delim);
			someChar = strtok(NULL, delim);	// should now be pointing at the balance
			accounts = realloc(accounts, sizeof(int) * numAccounts);
			accounts[numAccounts - 1] = atoi(someChar);
		}
		else if (someChar[0] == 'c') {
			numClients += 1;
		}
	}
	// already allocated memory for the accounts above, so just need to make room for client threads
	clients = malloc(sizeof(pthread_t) * numClients);

	fseek(filepointer, 0, SEEK_SET);	// reset file pointer to beginning to pass over again
	
	int pthread_check = 0;
	int pthread_count = 0;
	struct lineAndAccountsStruct *lineAndAccounts;
	while (nread = getline(&buffer, &length, filepointer) != -1) {
		if (buffer[0] == 'c') {
			
			// have to do this insanely confusing stuff because function can only take one argument... C SUCKS
			lineAndAccounts = malloc(sizeof(*lineAndAccounts));
			lineAndAccounts->line = buffer;
			lineAndAccounts->accounts = accounts;
			
			pthread_check = pthread_create(&clients[pthread_count], NULL, performClientOperations, lineAndAccounts); 
			pthread_join(clients[pthread_count], NULL);
			pthread_count += 1;
		}
	}
	fclose(filepointer);
	
	// open a new file to write to
	filepointer = fopen("assignment_6_output_file.txt", "w");
	if (filepointer == NULL) {
		exit(1);
	}
	
	for (int i = 0; i < numAccounts; i++) {
		printf("a%d b %d\n", i, accounts[i]);
		fprintf(filepointer, "a%d b %d\n", i, accounts[i]);
	}
	
	fclose(filepointer);
	free(buffer);
	free(accounts);
	free(clients);
}

