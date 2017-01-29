#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "fat.h"
#include "commands.h"

/*
 * Tries to load the fat file name and command name.
 * Returns the number of parameters read.
 */
int load_params(int argc, char** argv, char* fat_filename, char* command_name) {
    if(argc <= 1) {
        // todo: print help
        return 0;
    }

    if(argc <= 2) {
        // load file name
        strcpy(fat_filename, argv[1]);
        return 1;
    }

    if(argc <= 3) {
        // load command name
        strcpy(command_name, argv[2]);
        return 2;
    }

    return 0;
}

int main(int argc, char** argv) {
    char fat_filename[255];
    char command_name[3];
    int params_loaded = 0;
    int tmp = 0;
    int i = 0;
    int32_t fat_table[500];
    Boot_record fat_record;
    FILE* file = NULL;

    // load parameters
    params_loaded = load_params(argc, argv, fat_filename, command_name);
    if(params_loaded < 1) {
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
    print_boot_record(&fat_record);

    // load fat table
    tmp = load_fat_table(file, &fat_record, fat_table);
    if(tmp != OK) {
        printf("Error while loading fat table from file %d.\n", tmp);
        return 0;
    }

    // print fat table
    printf("\nFAT:\n");
    for(i = 0; i < fat_record.usable_cluster_count; i++) {
        printf("fat[%d] = %d\n", i, fat_table[i]);
    }

    return 0;
}