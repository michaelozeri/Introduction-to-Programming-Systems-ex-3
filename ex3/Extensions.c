/* Includes */
#include "Extensions.h"

void FreeProcessObject(Process* process) {
	free(process->ExitCode);
	free(process->ProcessInformation);
	free(process);
}

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
	free(thread->p_thread_params->Command);
	free(thread->p_thread_params->ExpectedResultPath);
	free(thread->p_thread_params->ResultsPath);
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

int createAndValidateSortThread(Thread** sortThread)
{
	(*sortThread) = InitNewThread();
	//system call
	(*sortThread)->Handle = CreateThread(
		NULL,
		0,
		(*sortThread)->Function,
		(*sortThread)->p_thread_params,
		0,
		(*sortThread)->Id);
	if ((*sortThread)->Handle == NULL) {
		printf("Error when creating thread\n");
		//FreeThreadArray(allThreads, i);
		return -1;
	}
}

int RunCalLogic(COMMAND_THREAD_params_t * p_params)
{
	int maxNumber = p_params->maxNumber;
	int currentNumber = 0;
	while (currentNumber < maxNumber) {
		//try to aquire anchor

		//calculateNumber
		//try to aquire semaphore
		//write to sortArray
		currentNumber++;
	}
	return 0;
}

int CompareResultsFiles(char* expectedResultPath, char* resultsPath) {
	ResultFile* expectedResults = ReadFileContents(expectedResultPath);
	int i;
	if (expectedResults == NULL) {
		printf("Couldnt read expected results path");
		return -1;
	}

	ResultFile* results = ReadFileContents(resultsPath);
	if (results == NULL) {
		printf("Couldnt read results path");
		FreeResultsObject(expectedResults);
		return -1;
	}

	if (expectedResults->NumberOfElements != results->NumberOfElements) {
		FreeResultsObject(expectedResults);
		FreeResultsObject(results);
		return -2;
	}

	for (i = 0; i < expectedResults->NumberOfElements; i++) {
		if (strcmp(expectedResults->Results[i], results->Results[i]) != 0) {
			FreeResultsObject(results);
			return -2;
		}
	}

	FreeResultsObject(results);
	return 0;
}

ResultFile* ReadFileContents(char *path) {
	int diff = 1;
	char **output = (char**)malloc(1 * sizeof(char*));
	if (output == NULL)
	{
		printf("Memory allocation failed.\n");
		return NULL;
	}

	int i = 0;
	size_t currentSize = 4;
	errno_t retval;
	FILE *p_stream;
	char p_line[100] = { 0 };
	char *p_ret_str;
	// Open file
	retval = fopen_s(&p_stream, path, "r");
	if (0 != retval) {
		printf("Failed to open file.\n");
		free(output);
		return NULL;
	}

	// Read lines
	p_ret_str = fgets(p_line, 100, p_stream);
	while (NULL != p_ret_str) {
		// Allocated memory for current line
		output[i] = (char*)malloc(100 * sizeof(char));
		if (output[i] == NULL) {
			printf("Memory allocation failed.\n");
			FreeStringArray(output, i);
			return NULL;
		}

		snprintf(output[i], strlen(p_line) + 1, p_line);
		p_ret_str = fgets(p_line, 100, p_stream);
		i++;
		if (i * sizeof(char*) >= currentSize) {
			currentSize *= 2;
			// Need to allocate more memory
			output = (char**)realloc(output, currentSize);
			if (output == NULL) {
				printf("Memory allocation failed.\n");
				FreeStringArray(output, i);
				return NULL;
			}
		}
	}

	if (i * sizeof(char*) < currentSize) {
		// We have empty allocated memory
		output = (char**)realloc(output, i * sizeof(char*));
		if (output == NULL) {
			printf("Memory allocation failed.\n");
			FreeStringArray(output, i);
			return NULL;
		}
	}
	ResultFile* results = (ResultFile*)malloc(sizeof(ResultFile));
	if (results == NULL) {
		printf("Memory allocation failed.\n");
		FreeStringArray(output, i);
		return NULL;
	}

	results->Results = output;
	results->NumberOfElements = i;
	results->TotalSize = i * sizeof(char*);
	// Close file
	retval = fclose(p_stream);
	if (0 != retval) {
		printf("Failed to close file.\n");
		FreeResultsObject(results);
		return NULL;
	}

	return results;
}

Thread* InitNewThread() {
	Thread* newThread = (Thread*)malloc(sizeof(Thread));
	if (newThread == NULL) {
		printf("Memory allocation failed.\n");
		return NULL;
	}

	newThread->p_thread_params = (COMMAND_THREAD_params_t*)malloc(sizeof(COMMAND_THREAD_params_t));
	if (newThread->p_thread_params == NULL) {
		printf("Memory allocation failed.\n");
		FreeThread(newThread);
		return NULL;
	}

	newThread->Id = (DWORD*)malloc(sizeof(DWORD));
	if (newThread->Id == NULL) {
		printf("Memory allocation failed.\n");
		FreeThread(newThread);
		return NULL;
	}

	//newThread->p_thread_params->ExpectedResultPath = (char*)malloc(sizeof(char) * 100);
	//if (newThread->p_thread_params->ExpectedResultPath == NULL) {
	//	printf("Memory allocation failed.\n");
	//	FreeThread(newThread);
	//	return NULL;
	//}

	////set expected rusults
	//snprintf(newThread->p_thread_params->ExpectedResultPath, expectedSize + 1, "%s%s", rel, arr[1]);
	//newThread->p_thread_params->Command = (char*)malloc(sizeof(char) * 100);
	//if (newThread->p_thread_params->Command == NULL) {
	//	printf("Memory allocation failed.\n");
	//	FreeThread(newThread);
	//	return NULL;
	//}

	return newThread;
}

char* TranslateExitCode(Thread* thread) {
	int exitCode = thread->ExitCode;
	if (thread->p_thread_params->isCrashed) {
		char* crashString = (char*)malloc(sizeof(char) * 11);
		int retVal = sprintf_s(crashString, 11, "Crashed %d", exitCode);
		if (retVal == 0) {
			return "Crashed";
		}
		return crashString;
	}
	switch (exitCode) {
	case 0:
		return "Succeeded";
	case -1:
		return "Crashed";
	case -2:
		return "Failed";
	case -3:
		return "Timed Out";
	}
	return NULL;
}

int SplitLine(const char *str, char c, char*** arr) {
	int count = 1;
	int token_len = 1;
	int i = 0;
	char *p;
	char *t;
	p = str;
	while (*p != '\0') {
		if (*p == c) {
			count++;
		}
		p++;
	}
	*arr = (char**)malloc(sizeof(char*) * count);
	if (*arr == NULL) {
		printf("Memory allocation failed.\n");
		return -1;
	}
	p = str;
	while (*p != '\0') {
		if (*p == c) {
			(*arr)[i] = (char*)malloc(sizeof(char) * token_len);
			if ((*arr)[i] == NULL) {
				printf("Memory allocation failed.\n");
				return -1;
			}
			token_len = 0;
			i++;
		}
		p++;
		token_len++;
	}
	(*arr)[i] = (char*)malloc(sizeof(char) * token_len);
	if ((*arr)[i] == NULL) {
		printf("Memory allocation failed.\n");
		return -1;
	}
	i = 0;
	p = str;
	t = ((*arr)[i]);
	while (*p != '\0') {
		if (*p != c && *p != '\0') {
			*t = *p;
			t++;
		}
		else {
			*t = '\0';
			i++;
			t = ((*arr)[i]);
		}
		p++;
	}
	return count;
}

char** SplitLineArguments(const char *str) {
	char** results = (char**)malloc(sizeof(char*) * 2);
	if (results == NULL) {
		printf("Memory allocation failed.\n");
		return NULL;
	}

	char* newP = strrchr(str, ' ');
	if (newP == NULL) {
		printf("Not enough arguments given");
		return NULL;
	}

	int len = newP - str;
	results[0] = (char*)malloc(sizeof(char)*len);
	if (results[0] == NULL) {
		free(results);
		printf("Memory allocation failed.\n");
		return NULL;
	}

	snprintf(results[0], len + 1, str);
	int res1Len = strlen(str) - len;
	results[1] = (char*)malloc(sizeof(char)*res1Len);
	if (results[1] == NULL) {
		free(results[0]);
		free(results);
		printf("Memory allocation failed.\n");
		return NULL;
	}
	snprintf(results[1], res1Len + 1, newP + 1);
	return results;
}

char* ConverExeExtensionToTxt(char *orig, char *rep, char *with) {
	char *result; // the return string
	char *ins;    // the next insert point
	char *tmp;    // varies
	int len_rep;  // length of rep (the string to remove)
	int len_with; // length of with (the string to replace rep with)
	int len_front; // distance between rep and end of last rep
	int count;    // number of replacements

	// sanity checks and initialization
	if (!orig || !rep)
		return NULL;
	len_rep = strlen(rep);
	if (len_rep == 0)
		return NULL; // empty rep causes infinite loop during count
	if (!with)
		with = "";
	len_with = strlen(with);

	// count the number of replacements needed
	ins = orig;
	for (count = 0; tmp = strstr(ins, rep); ++count) {
		ins = tmp + len_rep;
	}

	tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

	if (!result)
		return NULL;

	// first time through the loop, all the variable are set correctly
	// from here on,
	//    tmp points to the end of the result string
	//    ins points to the next occurrence of rep in orig
	//    orig points to the remainder of orig after "end of rep"
	while (count--) {
		ins = strstr(orig, rep);
		len_front = ins - orig;

		snprintf(tmp, len_front + 1, orig);
		snprintf(tmp + len_front, strlen(with) + 1, with);
		orig += len_front + len_rep; // move to next "end of rep"
	}

	snprintf(tmp, strlen(result) + 1, result);
	return result;
}