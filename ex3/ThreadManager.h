#pragma once


/*
* A simplified API for creating threads.
* Input Arguments :
*p_start_routine : A pointer to the function to be executed by the thread.
*     This pointer represents the starting address of the thread.
*     The function has to have this specific signature :
*DWORD WINAPI FunctionName(LPVOID lpParam);
*With FunctionName being replaced with the function's name.
*   p_thread_parameters: A(void *) to a variable to be passed to the thread.
* Output Arguments :
*p_thread_id : A pointer to a variable that receives the thread identifier.
*     If this parameter is NULL, the thread identifier is not returned.
* Return :
*If the function succeeds, the return value is a handle to the new thread.
*   If the function fails, the return value is NULL.
*   To get extended error information, call GetLastError.
* Notes :
*This function is just a wrapper for CreateThread.
*/
HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine, LPVOID p_thread_parameters, LPDWORD p_thread_id);