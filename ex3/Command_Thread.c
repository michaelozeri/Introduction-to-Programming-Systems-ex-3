#pragma once
/* Includes */
#include "Command_Thread.h"
#include "Extensions.h"


DWORD WINAPI CalculationThreadFunc(LPVOID lpParam) {
	ThreadParams* params;

	/* Check if lpParam is NULL */
	if (NULL == lpParam) {
		printf("Did not recieve thread params\n");
		return -1;
	}

	params = (ThreadParams *)lpParam;
	RunCalLogic(params);
	return params->ReturnCode;
}

DWORD WINAPI sortThreadFunc(LPVOID lpParam)
{
	ThreadParams* threadParams;
	/* Check if lpParam is NULL */
	if (NULL == lpParam) {
		printf("Did not recieve thread params\n");
		return -1;
	}
	threadParams = (ThreadParams *)lpParam;
	RunLogicSortThread(threadParams);
	return 0;
}
