#pragma once
/* Includes */
#include "Command_Thread.h"


DWORD WINAPI CalculationThreadFunc(LPVOID lpParam) {
	COMMAND_THREAD_params_t* p_params;

	/* Check if lpParam is NULL */
	if (NULL == lpParam) {
		printf("Did not recieve thread params\n");
		return -1;
	}

	p_params = (COMMAND_THREAD_params_t *)lpParam;
	RunCalLogic(p_params);
	return p_params->ReturnCode;
}

DWORD WINAPI sortThreadFunc(LPVOID lpParam)
{
	COMMAND_THREAD_params_t* p_params;
	/* Check if lpParam is NULL */
	if (NULL == lpParam) {
		printf("Did not recieve thread params\n");
		return -1;
	}
	p_params = (COMMAND_THREAD_params_t *)lpParam;
	//TODO: implement and run logic of sort thread
	return 0;
}
