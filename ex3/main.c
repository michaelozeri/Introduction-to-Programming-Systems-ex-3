/*
this is the program fro ex3 of מבוא לתכנון מערכות
By:
Michael ozeri - 302444229
Omer Machluf - 200892917
*/

#include "Extensions.h"
#include "Command_Thread.h"
#include "Parallel.h"

int FreeAllGlobals(int exitstatus);

//global variables
int * calcFinished;
Semaphore* bufferQueueSemaphore;
Mutex* mutexAnchorArray;
BufferValue* output_buffer;
Thread** calcThreads;
Thread* sortThread;

int maxNumber;
int numOfComputationThreads;
int outputBufferSize;
char* outputFilePath;

int main(int argc, char** argv) {
	if (argc < 5) {
		error("no enough arguments");
		return -1;
	}

	maxNumber = atoi(argv[1]);
	numOfComputationThreads = atoi(argv[2]);
	outputBufferSize = atoi(argv[3]);
	outputFilePath = argv[4];
	calcFinished = (int*)malloc(sizeof(int));
	if (calcFinished == NULL) {
		error("creating calcFinished");
		return -1;
	}
	*calcFinished = 0;

	if (numOfComputationThreads > 100 || outputBufferSize > 100 || maxNumber > 1000) {
		error("wrong arguments inserted");
		return FreeAllGlobals(-1);
	}

	printf("max number: %d\nnumber of calcthreads: %d\noutput Buffer Size: %d\noutput file path: %s\n", maxNumber, numOfComputationThreads, outputBufferSize, outputFilePath);

	debug("after parsing arguments");

	//create semaphore for quque
	bufferQueueSemaphore = CreateBufferQueueSemaphore(outputBufferSize);
	if (bufferQueueSemaphore == NULL) {
		error("creating buffer quque semaphore");
		return FreeAllGlobals(-1);
	}
	debug("after creating semaphore");

	//create maxNumber array
	mutexAnchorArray = CreateMutexAnchorArray(maxNumber);
	if (mutexAnchorArray == NULL) {
		error("creating mutexAnchorArray");
		return FreeAllGlobals(-1);
	}
	debug("after creating anchor array");

	//create output buffer array
	output_buffer = CreateOutputBuffer(outputBufferSize);
	if (output_buffer == NULL) {
		error("creating output_buffer");
		return FreeAllGlobals(-1);
	}
	debug("after creating output buffer array");

	//create sortThread
	sortThread = defineNewThread(&output_buffer, &mutexAnchorArray, &bufferQueueSemaphore, maxNumber, outputBufferSize);
	if (sortThread == NULL) {
		error("creating new sort thread failed");
		return FreeAllGlobals(-1);
	}
	sortThread->Function = sortThreadFunc;
	sortThread->threadParams->calcFinished = calcFinished;
	sortThread->threadParams->filePath = outputFilePath;
	//system call
	sortThread->Handle = CreateThread(
		NULL,
		0,
		sortThread->Function,
		sortThread->threadParams,
		0,
		sortThread->Id);
	if (sortThread->Handle == NULL) {
		error("when creating thread\n");
		return FreeAllGlobals(-1);
	}

	debug("after creating sort thread");

	//create calculation threads
	calcThreads = (Thread**)malloc(sizeof(Thread*));
	if (calcThreads == NULL) {
		error("memory allocation failed for calc threads array");
		return FreeAllGlobals(-1);
	}

	debug("after creating calculation threads array");

	//create calculation threads
	for (int i = 0; i < numOfComputationThreads; i++) {
		calcThreads[i] = defineNewThread(&output_buffer, &mutexAnchorArray, &bufferQueueSemaphore, maxNumber, outputBufferSize);
		if (calcThreads[i] == NULL) {
			error("creating new thread failed");
			return FreeAllGlobals(-1);
		}
		calcThreads[i]->Function = CalculationThreadFunc;
	}
	CreateAndRunAllCalculationThreads(calcThreads, numOfComputationThreads);

	debug("after running calculation threads");

	/* Wait  for calc threads to finish*/
	for (int i = 0; i < numOfComputationThreads; i++) {
		//system call
		calcThreads[i]->WaitCode = WaitForSingleObject(calcThreads[i]->Handle, INFINITE);
		if (calcThreads[i]->WaitCode != WAIT_OBJECT_0) {
			error("when waiting for calc Threads, exit code is bad");
			return FreeAllGlobals(-1);
		}
	}

	//setCalcFinished to true
	*calcFinished = 1;

	debug("set calcFinished to 1 by main");

	//wait for sort thread to finish writing to file
	sortThread->WaitCode = WaitForSingleObject(sortThread->Handle, INFINITE);
	if (sortThread->WaitCode != WAIT_OBJECT_0) {
		error("when waiting for sort Thread");
		return FreeAllGlobals(-1);
	}

	debug("finished waiting to sort thread");

	return FreeAllGlobals(0);
}


int FreeAllGlobals(int exitstatus) {
	if (calcFinished != NULL) {
		free(calcFinished);
	}
	if (bufferQueueSemaphore != NULL) {
		CloseHandle(bufferQueueSemaphore->handle);
		free(bufferQueueSemaphore);
	}
	//TODO: why u no working
	/*if (mutexAnchorArray != NULL) {
		for (int i = 0; i < maxNumber; i++)
		{
			if (mutexAnchorArray[i].handle != NULL) {
				CloseHandle(mutexAnchorArray[i].handle);
			}
			free(mutexAnchorArray+i);
		}
	}*/
	/*if (output_buffer != NULL) {
		for (int i = 0; i < outputBufferSize; i++)
		{
			CloseHandle(output_buffer[i].mutex->handle);
			free(output_buffer[i].mutex);
			free(output_buffer + i);
		}
	}*/
	if (calcThreads != NULL) {
		FreeThreadArray(calcThreads,numOfComputationThreads);
	}
	if (sortThread != NULL) {
		FreeThread(sortThread);
	}
	return exitstatus;
}

