#pragma once

#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <wchar.h>
#include <tchar.h> 

#define SEMAPHORE_INITIAL_VALUE 1
#define SEMAPHORE_MAX_VALUE 1

/*
struct representing the mutex has a handle to the mutex
*/
typedef struct Mutex {
	HANDLE handle;
	int locked;
}Mutex;


/*
struct representing the semaphore has a handle to the semaphore
*/
typedef struct Semaphore {
	HANDLE handle;
}Semaphore;

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

/*
this function creates the semaphore for the output_buffer
parameters:
outputBufferSize - the max value for the semaphore
returns:
Semaphore* if sucess else NULL
*/
Semaphore* CreateBufferQueueSemaphore(int outputBufferSize);

/*
this function creates the anchor array.
parameters:
maxNumber - the array size
returns:
a pointer to the anchor array if success else NULL
*/
Mutex* CreateMutexAnchorArray(int maxNumber);

/*
this function creates the output buffer including mutexes inside each place
parameters:
outputBufferSize - the size of the buffer to create
returns pointer to the buffer on sucess else NULL
*/
BufferValue* CreateOutputBuffer(int outputBufferSize);