#pragma once

// IO_Thread.h

#ifndef __COMMAND_THREAD_H__
#define __COMMAND_THREAD_H__

/* Includes */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <tchar.h> 
#include <stdbool.h>
#include "Extensions.h"

// Function Declarations -------------------------------------------------------

/*
* Math thread. This thread adds up to numbers.
*/
DWORD WINAPI CalculationThreadFunc(LPVOID lpParam);

/*
* this is the sortThread main logic will be set to the sort thread Fucntion
*/
DWORD WINAPI sortThreadFunc(LPVOID lpParam);

#endif // !__COMMAND_THREAD_H__