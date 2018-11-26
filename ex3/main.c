#include <stdio.h>
#include <stdlib.h>
#include <thread>

int main(int argc, char** argv) {
	if (argc < 5) {
		printf("ERROR: no enough arguments\n");
		return -1;
	}

	int maxNumber = atoi(argv[1]);
	int numOfComputationThreads = atoi(argv[2]);
	int outputBufferSize = atoi(argv[3]);
	char* outputFilePath = argv[4];


	//create sortThread

	//create maxNumber array

	//create output buffer array

	//create threads
	

	return 0;

}