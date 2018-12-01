#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <Windows.h>
#include "Extensions.h"
#include "Command_Thread.h"
#include "Parallel.h"

int main(int argc, char** argv) {
	if (argc < 5) {
		printf("ERROR: no enough arguments\n");
		return -1;
	}

	int maxNumber = atoi(argv[1]);
	int numOfComputationThreads = atoi(argv[2]);
	int outputBufferSize = atoi(argv[3]);
	char* outputFilePath = argv[4];

	//TODO: set all pointers to params of threads (mutex arrays and output_buffer

	Thread** allThreads = (Thread**)malloc(sizeof(Thread*));
	if (allThreads == NULL) {
		printf("Memory allocation failed.\n");
		//FreeResultsObject(testFile);
		return -1;
	}

	//create maxNumber array
	Mutex* mutexArray = (Mutex*)malloc(sizeof(Mutex)*maxNumber);
	for (int i = 0; i < maxNumber; i++)
	{
		mutexArray[i].handle =  CreateMutex(
			NULL, //can be set to null from recitation
			true, //TODO: thread calling create process should be its initial owner?
			NULL); // its possible to use null but we need to make sure not to lose handle
		if (mutexArray[i].handle == NULL) {
			printf("Error creating mutex\n");
			return -1;
		}
	}

	//create output buffer array
	bufferValue* output_buffer = (bufferValue*)malloc(sizeof(bufferValue)*outputBufferSize);
	if (output_buffer == NULL) {
		printf("Error creating output_bufer memory\n");
		return -1;
	}

	//create sortThread
	Thread* sortThread = (Thread*)malloc(sizeof(Thread));
	if (sortThread == NULL) {
		printf("Error when creating thread\n");
		return -1;
	}
	if (createAndValidateSortThread(&sortThread)) {
		printf("Error when creating thread\n");
		return -1;
	}
	
	//create calculation threads
	for (int i = 0; i < numOfComputationThreads; i++) {
		allThreads[i] = InitNewThread();
		if (allThreads[i] == NULL) {
			//FreeResultsObject(testFile);
			FreeThreadArray(allThreads, i);
			return -1;
		}
		allThreads[i]->Function = CalculationThreadFunc;
	}
	CreateAllCalculationThreads(allThreads, numOfComputationThreads, outputFilePath);

	//TODO: wait for sort thread to finish?

	/* Wait  for calc threads to finish*/
	for (int i = 0; i < numOfComputationThreads; i++) {
		//system call
		allThreads[i]->WaitCode = WaitForSingleObject(allThreads[i]->Handle, INFINITE);
		if (allThreads[i]->WaitCode != WAIT_OBJECT_0) {
			printf("Error when waiting\n");
			FreeThreadArray(allThreads, numOfComputationThreads);
			return -1;
		}
	}

	DWORD exit_code = 1;
	/* Get Exit Code */
	char** results = (char**)malloc(sizeof(char*) * numOfComputationThreads);
	for (int i = 0; i < numOfComputationThreads; i++) {
		if (GetExitCodeThread(allThreads[i]->Handle, &(allThreads[i]->ExitCode)) == 0) {
			printf("Error when getting thread exit code\n");
			return -1;
		}

	}

	//TODO: sore thread exit code?

	//TODO: main print to file or sort thread prints
	PrintResults(allThreads, numOfComputationThreads, outputFilePath);

	if (FreeThreadArray(allThreads, outputFilePath) != 0) {
		printf("Error when closing thread handles");
		return -1;
	}
	return 0;
}


int CreateAllCalculationThreads(Thread** allThreads, int numberOfThreads) {
	for (int i = 0; i < numberOfThreads; i++) {
		//system call
		allThreads[i]->Handle = CreateThread(
			NULL,
			0,
			allThreads[i]->Function,
			allThreads[i]->p_thread_params,
			0,
			allThreads[i]->Id);

		if (allThreads[i]->Handle == NULL) {
			printf("Error when creating thread\n");
			FreeThreadArray(allThreads, i);
			return -1;
		}
	}
	return 0;
}

int PrintResults(Thread** allThreads, int numberOfThreads, char* pathToResultsFile) {
	int i;
	FILE* file = NULL;
	int retVal = fopen_s(&file, pathToResultsFile, "w+");
	if (file == NULL) {
		printf("Error opening file!\n");
		return -2;
	}
	for (i = 0; i < numberOfThreads; i++) {
		char buffer[100];
		retVal = sprintf_s(buffer, 100, "test #%d : %s", i + 1, TranslateExitCode(allThreads[i]));
		if (retVal == 0) {
			printf("Error");
			return -2;
		}
		fprintf(file, "%s\n", buffer);
	}
	fclose(file);

	return 0;
}