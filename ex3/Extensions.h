#pragma once

/* Includes */
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include "Parallel.h"
#include "ThreadManager.h"
#include "Extensions.h"
/* Constants */

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
Thread* initNewThread(BufferValue** bufferValue, Mutex** mutexAnchorArray, Semaphore** bufferQueueSemaphore, int maxNumber, int outputBufferSize);

/*
this function creates the threads stored in allThreads then waits for them to finish and prints results into the file
given by pathToResultsFile
Parameters:
allThreads = the array of threads to run
numberOfThreads - the size of allThreads
pathToResultsFile - path To Results File
*/
int CreateAllCalculationThreads(Thread** allThreads, int numberOfThreads);

/*
this function checks the resultCode of each process and prints it into the file
which path is given
Parameters:
all Threads - the array of threads to print the result into the file
numberOfThreads - the size of allThreads array
pathToResultsFile - the path to the result file to print into
Returns: 0 on success else -2
*/
int PrintResults(Thread** allThreads, int numberOfThreads, char* pathToResultsFile);

/*
this function frees the thread array and releases mem allocated
Parameters:
arr - the Thread array to be released
numOfMembers - the size of arr
Returns: 0 on success else -1;
*/
int FreeThreadArray(Thread** arr, int numOfMembers);

int createAndValidateSortThread(Thread** sortThread, BufferValue** bufferValue, Mutex** mutexAnchorArray, Semaphore** bufferQueueSemaphore, int maxNumber, int outputBufferSize, int* calcfinished,char* outputFilePath);

int RunCalLogic(ThreadParams* p_params);

void calcNMAndWriteToSemaphore(int n, int maxNumber, Semaphore * semaphore, BufferValue * bufferArray, int outputBufferSize);

void setValueToBufferValue(BufferValue * bufferArray, int place, int a, int b, int c, int n, int m);

int findGCD(int n1, int n2);

int RunLogicSortThread(ThreadParams* params);

int compareBufferValues(const void * elem1, const void * elem2);

BufferValue* readValueFromOutputBufferAndUpSemaphore(Semaphore* semaphore, BufferValue* bufferArray, int outputBufferSize, int* outputBufferEmpty);

int printResults(char* filePath, BufferValue* bufferArray, int outputBufferSize);
