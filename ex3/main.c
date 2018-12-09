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
	int calcFinished = 0;

	//create semaphore for quque
	Semaphore* bufferQueueSemaphore = (Semaphore*)malloc(sizeof(Semaphore));
	if (bufferQueueSemaphore == NULL) {
		printf("ERROR: creating semaphore struct\n");
		return -1;
	}
	//create semaphore with size of outputbuffer
	bufferQueueSemaphore->handle = CreateSemaphore(
		NULL,
		SEMAPHORE_INITIAL_VALUE,
		outputBufferSize, "bufferQueueSemaphore");
	if (bufferQueueSemaphore->handle == NULL) {
		printf("ERROR: creating semaphore handle\n");
		return -1;
	}

	//create maxNumber array
	Mutex* mutexAnchorArray = (Mutex*)malloc(sizeof(Mutex)*maxNumber);
	for (int i = 0; i < maxNumber; i++)
	{
		mutexAnchorArray[i].handle = CreateMutex(
			NULL, //can be set to null from recitation
			true, //TODO: thread calling create process should be its initial owner?
			NULL); // its possible to use null but we need to make sure not to lose handle
		if (mutexAnchorArray[i].handle == NULL) {
			printf("ERROR: creating mutex anchor array\n");
			return -1;
		}
	}

	//create output buffer array
	BufferValue* output_buffer = (BufferValue*)malloc(sizeof(BufferValue)*outputBufferSize);
	if (output_buffer == NULL) {
		printf("ERROR: creating output_bufer memory\n");
		return -1;
	}
	for (int i = 0; i < outputBufferSize; i++) {
		output_buffer->mutex = CreateMutex(NULL, true, NULL);
		if (output_buffer->mutex == NULL) {
			printf("ERROR: creating bufferd value mutex\n");
			return -1;
		}
	}

	//create calculation threads
	Thread** calcThreads = (Thread**)malloc(sizeof(Thread*));
	if (calcThreads == NULL) {
		printf("ERROR: Memory allocation failed for calc threads array\n");
		//FreeResultsObject(testFile);
		return -1;
	}

	//create sortThread
	Thread* sortThread = (Thread*)malloc(sizeof(Thread));
	if (sortThread == NULL) {
		printf("ERROR: when creating thread\n");
		return -1;
	}
	if (createAndValidateSortThread(&sortThread, &output_buffer, &mutexAnchorArray, &bufferQueueSemaphore, maxNumber, outputBufferSize, &calcFinished, outputFilePath)) {
		printf("ERROR: when creating thread\n");
		return -1;
	}

	//create calculation threads
	for (int i = 0; i < numOfComputationThreads; i++) {
		calcThreads[i] = initNewThread(&output_buffer, &mutexAnchorArray, &bufferQueueSemaphore, maxNumber, outputBufferSize);
		if (calcThreads[i] == NULL) {
			//FreeResultsObject(testFile);
			FreeThreadArray(calcThreads, i);
			return -1;
		}
		calcThreads[i]->Function = CalculationThreadFunc;
	}
	CreateAllCalculationThreads(calcThreads, numOfComputationThreads);

	/* Wait  for calc threads to finish*/
	for (int i = 0; i < numOfComputationThreads; i++) {
		//system call
		calcThreads[i]->WaitCode = WaitForSingleObject(calcThreads[i]->Handle, INFINITE);
		if (calcThreads[i]->WaitCode != WAIT_OBJECT_0) {
			printf("ERROR: when waiting for calc Threads\n");
			FreeThreadArray(calcThreads, numOfComputationThreads);
			return -1;
		}
	}

	//setCalcFinished to true
	calcFinished = 1;

	//wait for sort thread to finish writing to file
	sortThread->WaitCode = WaitForSingleObject(sortThread->Handle, INFINITE);
	if (sortThread->WaitCode != WAIT_OBJECT_0) {
		printf("ERROR: when waiting for sort Thread\n");
		//FreeThreadArray(calcThreads, numOfComputationThreads);
		return -1;
	}

	//DWORD exit_code = 1;
	///* Get Exit Code */
	//char** results = (char**)malloc(sizeof(char*) * numOfComputationThreads);
	//for (int i = 0; i < numOfComputationThreads; i++) {
	//	if (GetExitCodeThread(calcThreads[i]->Handle, &(calcThreads[i]->ExitCode)) == 0) {
	//		printf("ERROR: when getting thread exit code\n");
	//		return -1;
	//	}
	//}

	if (FreeThreadArray(calcThreads, outputFilePath) != 0) {
		printf("ERROR: when closing thread handles");
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
