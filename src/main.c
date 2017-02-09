#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "fat.h"
#include "commands.h"
#include "tests.h"

/*
 * 1 = execute the tests.
 * 0 = execute the app.
 */
#define RUN_TESTS	0

/*
 * Tries to load the fat file name and command name.
 * Returns the number of parameters read.
 */
int load_params(int argc, char** argv, char* fat_filename, char* command_name) {
    int params_loaded = 0;
    if(argc <= 1) {
        // todo: print help
        return 0;
    }

    if(argc >= 2) {
        // load file name
        strcpy(fat_filename, argv[1]);
        params_loaded++;
    }

    if(argc >= 3) {
        // load command name
        strcpy(command_name, argv[2]);
        params_loaded++;
    }

    return params_loaded;
}

void print_fat(Boot_record *boot_record, int32_t* fat) {
	int i = 0;
	for(i = 0; i < /*boot_record->usable_cluster_count*/50; i++) {
		switch (fat[i]) {
			case FAT_UNUSED:
				printf("fat[%d] = FAT_UNUSED\n", i);
				break;
			case FAT_FILE_END:
				printf("fat[%d] = FAT_FILE_END\n", i);
				break;
			case FAT_BAD_CLUSTERS:
				printf("fat[%d] = FAT_BAD_CLUSTERS\n", i);
				break;
			case FAT_DIRECTORY:
				printf("fat[%d] = FAT_DIRECTORY\n", i);
				break;
			default:
				printf("fat[%d] = %d\n", i, fat[i]);
				break;
		}
	}
}

void print_result(int state) {
    switch (state) {
        case OK:
            printf(OK_MSG);
            break;
        case ERR_PATH_NOT_FOUND:
            printf(PATH_NOT_FOUND_MSG);
            break;
        case ERR_FILE_TOO_BIG:
            printf(FILE_TOO_BIG_MSG);
            break;
        case ERR_PATH_NOT_EMPTY:
            printf(PATH_NOT_EMPTY_MSG);
            break;
        case ERR_NO_FREE_ROOM:
            printf(NO_FREE_ROOM_MSG);
            break;
        case ERR_ALREADY_EXISTS:
            printf(ALREADY_EXISTS_MSG);
            break;
        default:
            printf(ERR_MSG);
    }
}

int main(int argc, char** argv) {
    char fat_filename[255];
    char command_name[3];
    int params_loaded = 0;
    int tmp = 0;
    int32_t fat_table[500];
    Boot_record fat_record;
    FILE* file = NULL;
    char buffer1[255];
    char buffer2[255];
    int state = 0;
    char log_msg[255];

    if(RUN_TESTS == 1) {
    	run_tests();
    	return 0;
    }

    // load parameters
    params_loaded = load_params(argc, argv, fat_filename, command_name);
    if(params_loaded < 2) {
        return 0;
    }

    // load boot record from file
    file = fopen(fat_filename, "r+");
    if(file == NULL) {
        printf("Error while opening file %s: %d - %s.\n", fat_filename, errno, strerror(errno));
        return 0;
    }
    tmp = load_boot_record(file, &fat_record);
    if(tmp != OK) {
        printf("Error while loading boot record from file %d.\n", tmp);
        return 0;
    }

    // load fat table
    tmp = load_fat_table(file, &fat_record, fat_table);
    if(tmp != OK) {
        printf("Error while loading fat table from file %s.\n", fat_filename);
        return 0;
    }

    // command
    if(strcmp(command_name, ADD_FILE_CMD) == 0) {
        // add file - two more arguments expected
        if(argc >= 5) {
            // source
            strcpy(buffer1, argv[3]);

            // dest
            strcpy(buffer2, argv[4]);

            state = add_file(file, &fat_record, fat_table, buffer1, buffer2);
            print_result(state);
        }
    } else if(strcmp(command_name, DELETE_FILE_CMD) == 0) {
        // delete file - one more arg expected
        if(argc >= 4) {
            strcpy(buffer1, argv[3]);

            state = delete_file(file, &fat_record, fat_table, buffer1);
            print_result(state);
        }

    } else if(strcmp(command_name, FILE_CLUSTERS_CMD) == 0) {
        // print file clusters - one more arg expected
        if(argc >= 4) {
            strcpy(buffer1, argv[3]);

            state = print_clusters(file, &fat_record, fat_table, buffer1);
            if(state != OK) {
                printf(PATH_NOT_FOUND_MSG);
            }
        }
    } else if(strcmp(command_name, NEW_DIR_CMD) == 0) {
        // new dir - two more arg expected
        if(argc >= 5) {
            strcpy(buffer1, argv[3]);
            strcpy(buffer2, argv[4]);

            state = add_directory(file, &fat_record, fat_table, buffer1, buffer2);
            print_result(state);
        }
    } else if(strcmp(command_name, DELETE_DIR_CMD) == 0) {
        // delete dir - one more arg expected
        if(argc >= 4) {
            strcpy(buffer1, argv[3]);

            state = delete_dir(file, &fat_record, fat_table, buffer1);
            print_result(state);
        }
    } else if(strcmp(command_name, PRINT_FILE_CMD) == 0) {
        // delete dir - one more arg expected
        if(argc >= 4) {
            strcpy(buffer1, argv[3]);

            state = print_file_content(file, &fat_record, fat_table, buffer1);
            if(state != OK) {
                printf(PATH_NOT_FOUND_MSG);
            }
        }
    } else if(strcmp(command_name, PRINT_ALL_CMD) == 0) {
        // print the file tree, no more args expected
        print_file_tree(file, &fat_record);
    } else if(strcmp(command_name, BAD_BLOCKS_CMD) == 0) {
        state = fix_bad_blocks(file, &fat_record, fat_table);
        if(state != NOK) {
            printf(OK_MSG);
            sprintf(log_msg, "%d bad clusters detected and moved.\n", state);
            sdebug(NO_NAME, log_msg);
        }
    }

	fclose(file);
    return 0;
}
