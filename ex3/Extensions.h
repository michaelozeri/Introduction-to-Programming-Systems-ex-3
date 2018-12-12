#pragma once

/* Includes */
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include "Parallel.h"
#include "Extensions.h"
/* Constants */

#define SEMAPHORE_INITIAL_VALUE 1
#define SEMAPHORE_MAX_VALUE 1
#define DEBUG_ON 1
#define PRINTNM 1
#define DEBUG_BUFFER_SIZE 300

/* Struct Declerations */
/* ResultFile is a struct representing a read file
*	Members:
*			Results- an array of string representing the content of the file, where every line of the file is a member of the array.
*			NumberOfElements - the number of lines in the read file
*			TotalSize- The total size of the array
*/
typedef struct ResultFile {
	char **Results;
	int NumberOfElements;
	int TotalSize;
} ResultFile;

/* ThreadParams is a struct representing the params given to a command thread
*	Members:
*			Command- the command to run by the process of the thread
*			ResultsPath - the path to the output file created by the process
*			ExpectedResultPath- the path to the expected results file to compare with the actual output file created by the processs
*			ReturnCode - the code returned by the thread.
*/
typedef struct ThreadParams
{
	int maxNumber;
	int outputBufferSize;
	DWORD ReturnCode;
	void* ptrToAnchorArray;
	void* ptrToOutputBufferArray;
	Semaphore* ptrToSemaphore;
	int* calcFinished;
	char* filePath;
} ThreadParams;

/* Thread is a struct representing a new thread
*	Members:
*			Id - thread id
*			Handle - thread handle
*			WaitCode - the code we got from waiting on the thread
*			ExitCode- the exit code of the thread
*			ReturnValue - the value returned by the last function done on the thread
*			p_thread_params - the arguments for the thread
*/
typedef struct Thread {
	HANDLE Handle;
	DWORD* Id;
	DWORD WaitCode;
	DWORD ExitCode;
	BOOL ReturnValue;
	LPTHREAD_START_ROUTINE Function;
	ThreadParams *threadParams;
} Thread;

/*
* this is the struct that represents a value to insert into the output_buffer
*/
typedef struct BufferValue {
	int a;
	int b;
	int c;
	int n;
	int m;
	int aquired;
	Mutex* mutex;
}BufferValue;

/* Functions Declerations */
/* FreeResultsObject will free all allocated resources in ResultFile Struct
* Arguments:
*		result - the result struct to free
* Returns:
*		void
*/
void FreeResultsObject(ResultFile* result);

/* FreeStringArray will free all allocated resources in a strings array
* Arguments:
*		arr - the array of strings to free
*		numOfMembers - the number of strings in the array
* Returns:
*		void
*/
void FreeStringArray(char** arr, int numOfMembers);

/* GetThreadFromLine will return a thread struct object from a test file line
* Returns:
*		Thread struct representing the new thread
*/
Thread* defineNewThread(BufferValue** bufferValue, Mutex** mutexAnchorArray, Semaphore** bufferQueueSemaphore, int maxNumber, int outputBufferSize);

/*
this function creates the threads stored in allThreads then waits for them to finish and prints results into the file
given by pathToResultsFile
Parameters:
allThreads = the array of threads to run
numberOfThreads - the size of allThreads
pathToResultsFile - path To Results File
*/
int CreateAndRunAllCalculationThreads(Thread** allThreads, int numberOfThreads);


/*
this function frees the thread array and releases mem allocated
Parameters:
arr - the Thread array to be released
numOfMembers - the size of arr
Returns: 0 on success else -1;
*/
int FreeThreadArray(Thread** arr, int numOfMembers);

/*
this function prints the bufferarray of results into the path give nby the path
Parameters:
filePath - the file path print into
bufferArray - the array from which to take values to print to the file
outputBufferSize - size of the bufferArray
*/
int printResults(char* filePath, BufferValue* bufferArray, int outputBufferSize);

/*
this function runs the main logic of calculation threads
parameters:
threadParams - the parameters of the thread to use
*/
int RunCalLogic(ThreadParams* threadParams);

/*
this function takes an n as an anchor and runs on maxNumber array to calculate a,b,c and save them into the output_buffer
is called by calculation threads
parameters:
n - the anchor number to start from
maxNumber - the arg given to the program
semaphore - the pointer to the semaphore of output_buffer
bufferArray - pointer to output_buffer array
outputBufferSize - the size of output_buffer
*/
void calcNMAndWriteToSemaphore(int n, int maxNumber, Semaphore * semaphore, BufferValue * bufferArray, int outputBufferSize);

/*
this function saves the values given by a,b,c,n,m into the bufferArray at location place
parameters:
bufferArray - pointer to output_buffer to save the values into
place - location to save in bufferArray (output_buffer) the values
a,c,b,n,m - values to be written
*/
void setValueToBufferValue(BufferValue * bufferArray, int place, int a, int b, int c, int n, int m);

/*
this function fids greatest common divisor of n1,n2
parameters:
n1,n2 - the numbers which to find GCD
*/
int findGCD(int n1, int n2);

/*
this is the main logic of the sort thread to be run
parameters:
params - parameters for the thread to run initialized at main function
*/
int RunLogicSortThread(ThreadParams* params);

/*
comperator function to give for qsort function in sort thread
parameters:
elem1,elem2 - the two values to compare in the BufferValue array
*/
int compareBufferValues(const void * elem1, const void * elem2);

/*
this function is used by the sort thread, it reads a value from the output_buffer (bufferArray) and up(semaphore)
parameters:
semaphore - the array semaphore
bufferArray - the buffer array to read from
outputBufferSize - size of output_buffer
outputBufferEmpty - return argument to set if the output_buffer is empty
*/
BufferValue* readValueFromOutputBufferAndUpSemaphore(Semaphore* semaphore, BufferValue* bufferArray, int outputBufferSize, int* outputBufferEmpty);

/*
prints "DEBUG:" before the string if macro DEBUG_ON is enabled
parameters:
str - the string to debug
*/
void debug(char* str);

/*
prints "DEBUG:" before the string if macro DEBUG_ON is enabled
parameters:
str - the string to debug
*/
void debug2(char* str ,int param1, int param2);

/*
prints "DEBUG:" before the string if macro DEBUG_ON is enabled
parameters:
str - the string to debug
*/
void debug1(char* str,int param1);

/*
prints "ERROR:" before the string 
parameters:
str - the string to print error log for
*/
void error(char* str);


