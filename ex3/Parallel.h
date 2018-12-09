#pragma once

#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

#define SEMAPHORE_INITIAL_VALUE 1
#define SEMAPHORE_MAX_VALUE 1

typedef struct Mutex {
	HANDLE handle;
}Mutex;


typedef struct Semaphore {
	HANDLE handle;
}Semaphore;


