#pragma once

#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

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


