#include "Parallel.h"

Semaphore * CreateBufferQueueSemaphore(int outputBufferSize)
{
	Semaphore * sem = (Semaphore*)malloc(sizeof(Semaphore));
	if (sem == NULL) {
		error("creating semaphore struct");
		return NULL;
	}
	//create semaphore with size of outputbuffer
	sem->handle = CreateSemaphore(
		NULL,
		SEMAPHORE_INITIAL_VALUE,
		outputBufferSize, "bufferQueueSemaphore");
	if (sem->handle == NULL) {
		error("creating semaphore handle");
		return NULL;
	}

	return sem;
}

Mutex * CreateMutexAnchorArray(int maxNumber)
{
	Mutex* mutexAnchorArray = (Mutex*)malloc(sizeof(Mutex)*maxNumber);
	if (mutexAnchorArray == NULL) {
		error("allcating mem for mutexAnchorArray");
		return NULL;
	}
	for (int i = 0; i < maxNumber; i++)
	{
		mutexAnchorArray[i].handle = CreateMutex(
			NULL, // can be set to null from recitation
			false, // thread calling create Mutex should be its initial owner?
			NULL); // its possible to use null but we need to make sure not to lose handle
		if (mutexAnchorArray[i].handle == NULL) {
			error("creating mutex anchor array");
			return NULL;
		}
		mutexAnchorArray[i].locked = 0;
	}
	return mutexAnchorArray;
}

BufferValue * CreateOutputBuffer(int outputBufferSize)
{
	BufferValue* output_buffer = (BufferValue*)malloc(sizeof(BufferValue)*outputBufferSize);
	if (output_buffer == NULL) {
		error("creating output_bufer memory");
		return NULL;
	}
	for (int i = 0; i < outputBufferSize; i++) {
		output_buffer[i].mutex = (Mutex*)malloc(sizeof(Mutex));
		if (output_buffer[i].mutex == NULL) {
			error("creating bufferd value mutex memory");
			return NULL;
		}
		output_buffer[i].mutex->handle = CreateMutex(NULL, true, NULL);
		if (output_buffer[i].mutex->handle == NULL) {
			error("creating bufferd value mutex");
			return NULL;
		}
		//init for marking as not aquired
		output_buffer[i].aquired = 0;
	}
	return output_buffer;
}
