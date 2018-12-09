#include "Extensions.h"
#include "Command_Thread.h"
#include "Parallel.h"


int main(int argc, char** argv) {
	if (argc < 5) {
		error("no enough arguments");
		return -1;
	}

	int maxNumber = atoi(argv[1]);
	int numOfComputationThreads = atoi(argv[2]);
	int outputBufferSize = atoi(argv[3]);
	char* outputFilePath = argv[4];
	int* calcFinished = (int*)malloc(sizeof(int));
	if (calcFinished == NULL) {
		error("creating calcFinished");
		return -1;
	}
	*calcFinished = 0;

	debug("after parsing arguments");

	//create semaphore for quque
	Semaphore* bufferQueueSemaphore = (Semaphore*)malloc(sizeof(Semaphore));
	if (bufferQueueSemaphore == NULL) {
		error("creating semaphore struct");
		return -1;
	}
	//create semaphore with size of outputbuffer
	bufferQueueSemaphore->handle = CreateSemaphore(
		NULL,
		SEMAPHORE_INITIAL_VALUE,
		outputBufferSize, "bufferQueueSemaphore");
	if (bufferQueueSemaphore->handle == NULL) {
		error("creating semaphore handle");
		return -1;
	}

	debug("after creating semaphore");

	//create maxNumber array
	Mutex* mutexAnchorArray = (Mutex*)malloc(sizeof(Mutex)*maxNumber);
	for (int i = 0; i < maxNumber; i++)
	{
		mutexAnchorArray[i].handle = CreateMutex(
			NULL, //can be set to null from recitation
			true, //TODO: thread calling create process should be its initial owner?
			NULL); // its possible to use null but we need to make sure not to lose handle
		if (mutexAnchorArray[i].handle == NULL) {
			error("creating mutex anchor array");
			return -1;
		}
	}

	debug("after creating anchor array");

	//create output buffer array
	BufferValue* output_buffer = (BufferValue*)malloc(sizeof(BufferValue)*outputBufferSize);
	if (output_buffer == NULL) {
		error("creating output_bufer memory");
		return -1;
	}
	for (int i = 0; i < outputBufferSize; i++) {
		output_buffer->mutex = CreateMutex(NULL, true, NULL);
		if (output_buffer->mutex == NULL) {
			error("creating bufferd value mutex");
			return -1;
		}
	}

	debug("after creating output buffer array");

	//create calculation threads
	Thread** calcThreads = (Thread**)malloc(sizeof(Thread*));
	if (calcThreads == NULL) {
		error("memory allocation failed for calc threads array");
		//FreeResultsObject(testFile);
		return -1;
	}

	debug("after creating calculation threads array");

	//create sortThread
	Thread* sortThread = (Thread*)malloc(sizeof(Thread));
	if (sortThread == NULL) {
		error("when creating sore thread memory");
		return -1;
	}
	if (createAndValidateSortThread(&sortThread, &output_buffer, &mutexAnchorArray, &bufferQueueSemaphore, maxNumber, outputBufferSize, &calcFinished, outputFilePath)) {
		error("when creating thread");
		return -1;
	}

	//debug("after creating sort thread");

	//create calculation threads
	for (int i = 0; i < numOfComputationThreads; i++) {
		calcThreads[i] = initNewThread(&output_buffer, &mutexAnchorArray, &bufferQueueSemaphore, maxNumber, outputBufferSize);
		if (calcThreads[i] == NULL) {
			error("creating new thread failed");
			//FreeResultsObject(testFile);
			FreeThreadArray(calcThreads, i);
			return -1;
		}
		calcThreads[i]->Function = CalculationThreadFunc;
	}
	CreateAllCalculationThreads(calcThreads, numOfComputationThreads);

	debug("after running calculation threads");

	/* Wait  for calc threads to finish*/
	for (int i = 0; i < numOfComputationThreads; i++) {
		//system call
		calcThreads[i]->WaitCode = WaitForSingleObject(calcThreads[i]->Handle, INFINITE);
		if (calcThreads[i]->WaitCode != WAIT_OBJECT_0) {
			error("when waiting for calc Threads, exit code is bad");
			FreeThreadArray(calcThreads, numOfComputationThreads);
			return -1;
		}
	}

	//setCalcFinished to true
	*calcFinished = 1;

	debug("set calcFinished to 1 by main");

	//wait for sort thread to finish writing to file
	sortThread->WaitCode = WaitForSingleObject(sortThread->Handle, INFINITE);
	if (sortThread->WaitCode != WAIT_OBJECT_0) {
		error("when waiting for sort Thread");
		//FreeThreadArray(calcThreads, numOfComputationThreads);
		return -1;
	}

	debug("finished waiting to sort thread");

	//DWORD exit_code = 1;
	///* Get Exit Code */
	//char** results = (char**)malloc(sizeof(char*) * numOfComputationThreads);
	//for (int i = 0; i < numOfComputationThreads; i++) {
	//	if (GetExitCodeThread(calcThreads[i]->Handle, &(calcThreads[i]->ExitCode)) == 0) {
	//		printf("ERROR: when getting thread exit code\n");
	//		return -1;
	//	}
	//}

	/*if (FreeThreadArray(calcThreads, outputFilePath) != 0) {
		printf("ERROR: when closing thread handles");
		return -1;
	}*/

	return 0;
}


int CreateAllCalculationThreads(Thread** allThreads, int numberOfThreads) {
	for (int i = 0; i < numberOfThreads; i++) {
		//system call
		allThreads[i]->Handle = CreateThread(
			NULL,
			0,
			allThreads[i]->Function,
			allThreads[i]->threadParams,
			0,
			allThreads[i]->Id);

		if (allThreads[i]->Handle == NULL) {
			printf("ERROR: when creating thread\n");
			FreeThreadArray(allThreads, i);
			return -1;
		}
	}
	return 0;
}
