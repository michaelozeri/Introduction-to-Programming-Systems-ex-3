#pragma once

#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct Mutex {
	HANDLE handle;

	char* Command;
	char* ResultsPath;
	char* ExpectedResultPath;
	DWORD ReturnCode;
	bool isCrashed;
	void* ptrToAnchorArray;
	void* ptrToOutputBufferArray;
}Mutex;


