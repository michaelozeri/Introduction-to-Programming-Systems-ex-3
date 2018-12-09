/* Includes */
#include "Extensions.h"


void FreeResultsObject(ResultFile* result) {
	FreeStringArray(result->Results, result->NumberOfElements);
	free(result);
}

void FreeStringArray(char** arr, int numOfMembers) {
	for (int i = 0; i < numOfMembers; i++) {
		free(arr[i]);
	}
	free(arr);
}

int FreeThread(Thread* thread) {
	int returnCode = 0;
	if (CloseHandle(thread->Handle) == false) {
		returnCode = -1;
	}
	free(thread->Id);
	free(thread);
	return returnCode;
}

int FreeThreadArray(Thread** arr, int numOfMembers) {
	int code = 0;
	for (int i = 0; i < numOfMembers; i++) {
		if (FreeThread(arr[i]) != 0) {
			code = -1;
		}
	}
	return code;
}

int createAndValidateSortThread(Thread** sortThread,
	BufferValue ** bufferValue,
	Mutex ** mutexAnchorArray,
	Semaphore ** bufferQueueSemaphore,
	int maxNumber,
	int outputBufferSize,
	int** calcfinished,
	char* outputFilePath)
{
	(*sortThread) = initNewThread(bufferValue, mutexAnchorArray, bufferQueueSemaphore, maxNumber, outputBufferSize);

	(*sortThread)->threadParams->calcFinished = *calcfinished;

	//system call
	(*sortThread)->Handle = CreateThread(
		NULL,
		0,
		(*sortThread)->Function,
		(*sortThread)->threadParams,
		0,
		(*sortThread)->Id);
	if ((*sortThread)->Handle == NULL) {
		printf("ERROR: when creating thread\n");
		//FreeThreadArray(allThreads, i);
		return -1;
	}
	return 0;
}

int RunCalLogic(ThreadParams * threadParams)
{
	int maxNumber = threadParams->maxNumber;
	Mutex* anchorArray = threadParams->ptrToAnchorArray;
	int outputBufferSize = threadParams->outputBufferSize;
	BufferValue* bufferArray = threadParams->ptrToAnchorArray;
	Semaphore* semaphore = threadParams->ptrToSemaphore;

	for (int i = 0; i < maxNumber; i++) {
		//try to aquire anchor
		if (WaitForSingleObject(anchorArray[i].handle, 0) != WAIT_OBJECT_0) {
			continue;
		}
		debug("calc thread aquired anchor!");
		int n = i + 1;
		calcNMAndWriteToSemaphore(n, maxNumber, semaphore, bufferArray, outputBufferSize);
		//release mutex
		ReleaseMutex(anchorArray[i].handle);
	}

	return 0;
}

void calcNMAndWriteToSemaphore(int n, int maxNumber, Semaphore * semaphore, BufferValue * bufferArray, int outputBufferSize)
{
	for (int m = n + 1; m < maxNumber; m += 2) {
		int a, b, c;
		if (findGCD(n, m) == 1) { //has no common denumerator
			a = (m*m) - (n*n);
			b = 2 * m * n;
			c = (m*m) + (n*n);
			//try to aquire semaphore
			if (WaitForSingleObject(semaphore->handle, INFINITE) == WAIT_OBJECT_0) {
				//write to sortArray
				int written = 0;
				int location = 0;
				while (!written) {
					int place = location % 3;
					WaitForSingleObject(bufferArray[place].mutex->handle, 0);
					if (bufferArray[place].aquired == 0) {
						setValueToBufferValue(bufferArray, place, a, b, c, n, m);
						written = 1;
					}
					ReleaseMutex(bufferArray[place].mutex->handle);
					location++;
				}
				ReleaseSemaphore(semaphore->handle, 1, NULL); //TODO: check return value?
			}
		}
	}
}

void setValueToBufferValue(BufferValue * bufferArray, int place, int a, int b, int c, int n, int m)
{
	bufferArray[place].a = a;
	bufferArray[place].b = b;
	bufferArray[place].c = c;
	bufferArray[place].n = n;
	bufferArray[place].m = m;
}

int findGCD(int n1, int n2) {
	int  i, gcd;
	for (i = 1; i <= n1 && i <= n2; ++i)
	{
		if (n1%i == 0 && n2%i == 0)
			gcd = i;
	}
	return gcd;
}

Thread * initNewThread(BufferValue ** bufferValue, Mutex ** mutexAnchorArray, Semaphore ** bufferQueueSemaphore, int maxNumber, int outputBufferSize)
{
	Thread* newThread = (Thread*)malloc(sizeof(Thread));
	if (newThread == NULL) {
		error("Memory allocation failed for new Thread");
		return NULL;
	}

	newThread->threadParams = (ThreadParams*)malloc(sizeof(ThreadParams));
	if (newThread->threadParams == NULL) {
		error("Memory allocation failed for thread parameters");
		FreeThread(newThread);
		return NULL;
	}

	//set parameters of thread
	newThread->threadParams->maxNumber = maxNumber;
	newThread->threadParams->outputBufferSize = outputBufferSize;
	newThread->threadParams->ptrToAnchorArray = *mutexAnchorArray;
	newThread->threadParams->ptrToOutputBufferArray = *bufferValue;
	newThread->threadParams->ptrToSemaphore = *bufferQueueSemaphore;

	newThread->Id = (DWORD*)malloc(sizeof(DWORD));
	if (newThread->Id == NULL) {
		error("Memory allocation failed for thread ID");
		FreeThread(newThread);
		return NULL;
	}
	/*newThread->threadParams->calcFinished = (int*)malloc(sizeof(int));
	if (newThread->threadParams->calcFinished == NULL) {
		error("creating mem for calcFinished");
		return NULL;
	}

	return newThread;*/
}

int RunLogicSortThread(ThreadParams* params) {
	int maxNumber = params->maxNumber;
	int outputBufferSize = params->outputBufferSize;
	BufferValue* bufferArray = params->ptrToOutputBufferArray;
	Semaphore* semaphore = params->ptrToSemaphore;
	int outputSize = maxNumber * maxNumber;
	BufferValue* outArrayFinal = (BufferValue*)malloc(sizeof(BufferValue)*(outputSize));
	int calcFinished = 0;
	int outputBufferEmpty = 0;
	while (!calcFinished && !outputBufferEmpty) {
		//read value and up semaphore
		BufferValue* val = readValueFromOutputBufferAndUpSemaphore(semaphore, bufferArray, outputBufferSize, &outputBufferEmpty);
		//insert into outArrayFinal ending
		outArrayFinal[outputSize - 1].a = val->a;
		outArrayFinal[outputSize - 1].b = val->b;
		outArrayFinal[outputSize - 1].c = val->c;
		outArrayFinal[outputSize - 1].n = val->n;
		outArrayFinal[outputSize - 1].m = val->m;
		//sort array
		qsort(outArrayFinal, outputSize, sizeof(BufferValue), compareBufferValues);
		//check outputempty
		calcFinished = *(params->calcFinished);
		if (calcFinished) {
			printf("DEBUG: calcFinished changed to 1!\n");
		}
	}
	printResults(params->filePath, outArrayFinal, outputSize);
	return 0;
}

int compareBufferValues(const void * elem1, const void * elem2)
{
	BufferValue* val1 = (BufferValue*)elem1;
	BufferValue* val2 = (BufferValue*)elem2;
	int n1 = val1->n;
	int n2 = val2->n;
	int m1 = val1->n;
	int m2 = val2->n;
	if (n1 > n2) {
		return  -1;
	}
	else if (n1 == n2) {
		if (m1 > m2) {
			return -1;
		}
		else if (m1 == m2) {
			return 0;
		}
		else {
			return 1;
		}
	}
	else {
		return 1;
	}
}

BufferValue * readValueFromOutputBufferAndUpSemaphore(Semaphore * semaphore, BufferValue * bufferArray, int outputBufferSize, int * outputBufferEmpty)
{
	BufferValue* retVal = (BufferValue*)malloc(sizeof(BufferValue));
	int found = 0;
	for (int i = 0; i < outputBufferSize; i++) {
		WaitForSingleObject(bufferArray[i].mutex->handle, 0);
		if (bufferArray[i].aquired == 1) {
			retVal->a = bufferArray[i].a;
			retVal->b = bufferArray[i].b;
			retVal->c = bufferArray[i].c;
			retVal->n = bufferArray[i].n;
			retVal->m = bufferArray[i].m;
			bufferArray[i].aquired = 0;
		}
		ReleaseMutex(bufferArray[i].mutex->handle);
		if (found) {
			break;
		}
	}
	ReleaseSemaphore(semaphore->handle, 1, NULL);
	if (!found) {
		printf("DEBUG: finished for loop and didnt find any object\n");
		*outputBufferEmpty = 1;
	}
	return retVal;
}

int printResults(char* filePath, BufferValue* bufferArray, int outputBufferSize) {
	int i;
	FILE* file = NULL;
	int retVal = fopen_s(&file, filePath, "w+");
	if (file == NULL) {
		printf("ERROR: opening file!\n");
		return -2;
	}
	for (i = 0; i < outputBufferSize; i++) {
		char buffer[20];
		retVal = sprintf_s(buffer, 20, "%d,%d,%d", bufferArray[i].a, bufferArray[i].b, bufferArray[i].c);
		if (retVal == 0) {
			printf("ERROR: inserting text when writing to file");
			return -2;
		}
		fprintf(file, "%s\n", buffer);
	}
	fclose(file);
	return 0;
}

void debug(char * str)
{
	if (DEBUG_ON) {
		printf("DEBUG: %s\n", str);
	}
}

void error(char * str)
{
	printf("ERROR: %s\n", str);
}
