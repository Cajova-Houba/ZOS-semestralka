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
 * Tests the pointer for null. If not null, prints err_msg and returns NOK.
 * Otherwise returns OK.
 */
int assert_null(void *pointer, char *err_msg) {
	if(pointer != NULL) {
		printf(err_msg);
		return NOK;
	}

	return OK;
}

void test_split_file_path_bad() {
	char path[255];
	char **result = NULL;
	int count = 0;
	int state = NOK;

	printf("Running test_split_file_path_bad. ");

	strcpy(path, "");
	result = split_file_path(path, &count);
	state = assert_null(result, "\n\tEmpty path isn't null!\n");
	if(state != OK) {
		return;
	}
	if(count != 0) {
		printf("\n\tWrong count returned for empty path! Expected: %d, actual: %d.\n", 0 , count);
		return;
	}


	strcpy(path, "ahoj.txt");
	result = split_file_path(path, &count);
	state = assert_null(result, "\n\tPath without root isn't null!");
	if(state != OK) {
		return;
	}
	if(count != 0) {
		printf("\n\tWrong count returned for path without root! Expected: %d, actual: %d.\n", 0, count);
		return;
	}

	printf("OK.\n");

	return;
}

void test_split_dir_path_bad() {
	char path[255];
	char **result = NULL;
	int count = 0;
	int state = NOK;

	printf("Running test_split_dir_path_bad. ");

	strcpy(path, "");
	result = split_dir_path(path, &count);
	state = assert_null(result, "\n\tEmpty path isn't null!\n");
	if(state != OK) {
		return;
	}
	if(count != 0) {
		printf("\n\tWrong count returned for empty path! Expected: %d, actual: %d.\n", 0 , count);
		return;
	}


	strcpy(path, "/ahoj.txt");
	result = split_dir_path(path, &count);
	state = assert_null(result, "\n\tBad directory isn't null!");
	if(state != OK) {
		return;
	}
	if(count != 0) {
		printf("\n\tWrong count returned for bad directory! Expected: %d, actual: %d.\n", 0, count);
		return;
	}

	strcpy(path, "dir/");
	result = split_dir_path(path, &count);
	state = assert_null(result, "\n\tPath without root isn't null!\n");
	if(state != OK) {
		return;
	}
	if(count != 0) {
		printf("\n\tWrong count returned for path without root! expected: %d, actual: %d.\n", 0, count);
		return;
	}

	printf("OK.\n");

	return;
}

void test_split_file_path1() {
	char path[255];
	char **result = NULL;
	int count = 0;
	int i = 0;
	int state = NOK;
	char * expected[] = {
			"dir-1\0", "dir-2\0", "test.txt\0"
	};

	printf("Running test_split_file_path1. ");

	strcpy(path, "/dir-1/dir-2/test.txt");
	result = split_file_path(path, &count);
	state = assert_not_null((void *) result, "\n\tPath is null!\n");
	if(state != OK) {
		return;
	}

	if(count != 3) {
		printf("\n\tWrong number of path items! Expected: %d, actual: %d.\n", 3, count);
		return;
	}

	for(i = 0; i < count; i++) {
		if(strcmp(expected[i], result[i]) != 0) {
			printf("\n\tWrong path item %d! Expected: %s, actual: %s.\n", i, expected[i], result[i]);
			return;
		}
	}

	printf("OK.\n");
}

void test_split_file_path2() {
	char path[255];
	char **result = NULL;
	int count = 0;
	int i = 0;
	int state = NOK;
	char * expected[] = {
			"cisla.txt"
	};

	printf("Running test_split_file_path2. ");

	strcpy(path, "/cisla.txt");
	result = split_file_path(path, &count);
	state = assert_not_null((void *) result, "\n\tPath is null!\n");
	if(state != OK) {
		return;
	}

	if(count != 1) {
		printf("\n\tWrong number of path items! Expected: %d, actual: %d.\n", 1, count);
		return;
	}

	for(i = 0; i < count; i++) {
		if(strcmp(expected[i], result[i]) != 0) {
			printf("\n\tWrong path item %d! Expected: %s, actual: %s.\n", i, expected[i], result[i]);
			return;
		}
	}

	printf("OK.\n");
}

void test_split_dir_path1() {
	char path[255];
	char **result = NULL;
	int count = 0;
	int state = NOK;

	printf("Running test_split_dir_path1. ");

	strcpy(path, "/");
	result = split_dir_path(path, &count);
	state = assert_not_null((void *) result, "\n\tPath is null!\n");
	if(state != OK) {
		return;
	}

	if(count != 1) {
		printf("\n\tWrong number of path items! Expected: %d, actual: %d.\n", 1, count);
		return;
	}


	printf("OK.\n");
}

void test_split_dir_path2() {
	char path[255];
	char **result = NULL;
	int count = 0;
	int i = 0;
	int state = NOK;
	char * expected[] = {
				"", "dir"
		};

	printf("Running test_split_dir_path2. ");

	strcpy(path, "/dir/");
	result = split_dir_path(path, &count);
	state = assert_not_null((void *) result, "\n\tPath is null!\n");
	if(state != OK) {
		return;
	}

	if(count != 2) {
		printf("\n\tWrong number of path items! Expected: %d, actual: %d.\n", 2, count);
		return;
	}

	for(i = 0; i < 2; i++) {
		if(strcmp(result[i], expected[i]) != 0) {
			printf("\n\tWrong path item on position %d. Expected: %s, actual: %s.\n", i, expected[i], result[i]);
		}
	}


	printf("OK.\n");
}

void test_is_cluster_bad() {
    int small_c_size = BAD_SEQ_LEN*2-1;
    int bad_c_size = BAD_SEQ_LEN*2+5;
    int good_c_size = BAD_SEQ_LEN*2+5;
    int i = 0;
    int res = 0;
    char small_cluster[BAD_SEQ_LEN*2-1];    // cluster too small
    char bad_cluster[BAD_SEQ_LEN*2+5];      // bad cluster - minimal size + some additional space
    char good_cluster[BAD_SEQ_LEN*2+5];     // good cluster - minimal size + some additional space

    printf("Running test_is_cluster_bad. ");

    // fill clusters
    for(i = 0; i < BAD_SEQ_LEN; i++) {
        good_cluster[i] = '\0';
        good_cluster[good_c_size - 1 - i] = '\0';

        bad_cluster[i] = BAD_BYTE;
        bad_cluster[bad_c_size - 1 - i] = BAD_BYTE;
    }

    res = is_cluster_bad(small_cluster, small_c_size);
    if(res != OK) {
        printf("\n\tSmall cluster should be bad!\n");
        return;
    }

    res = is_cluster_bad(bad_cluster, bad_c_size);
    if(res != OK) {
        printf("\n\tBad cluster should be bad!\n");
        return;
    }

    res = is_cluster_bad(good_cluster, good_c_size);
    if(res != NOK) {
        printf("\n\tGood cluster shouldn't be bad!\n");
        return;
    }

    printf("OK.\n");
}

void run_tests() {
	// add tests calls here
	test_split_file_path1();
	test_split_file_path2();
	test_split_dir_path1();
	test_split_dir_path2();
	test_split_file_path_bad();
	test_split_dir_path_bad();
    test_is_cluster_bad();
}

