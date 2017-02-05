/*
 * tests.c
 *
 *  Created on: 5. 2. 2017
 *      Author: zdenda
 */

#include "tests.h"

/*
 * Tests the pointer for null. If null, prints err msg and return NOK.
 * Otherwise returns OK.
 */
int assert_not_null(void *pointer, char *err_msg) {
	if(pointer == NULL) {
		printf(err_msg);
		return NOK;
	}
	return OK;
}

/*
 *
 */
void test_split_file_path1() {
	char path[255];
	char **result = NULL;
	int count = 0;
	int i = 0;
	int state = NOK;
	char * expected[] = {
			{"dir-1\0"}, {"dir-2\0"}, {"test.txt\0"}
	};

	printf("Running test_split_file_path1. ");

	strcpy(path, "dir-1/dir-2/test.txt");
	result = split_file_path(path, &count);
	state = assert_not_null((void *) result, "Path is null!");
	if(state != OK) {
		return;
	}

	if(count != 3) {
		printf("\n\tWrong number of path items! Expected: %d, actual: %d.", 3, count);
		return;
	}

	for(i = 0; i < count; i++) {
		if(strcmp(expected[i], result[i]) != 0) {
			printf("\n\tWrong path item %d! Expected: %s, actual: %s.", i, expected[i], result[i]);
			return;
		}
	}

	printf("OK.\n");
}

void run_tests() {
	// add tests calls here
	test_split_file_path1();
}

