#ifndef SEMESTRALKA_COMMANDS_H
#define SEMESTRALKA_COMMANDS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fat.h"
#include "slog.h"

#define ADD_FILE_CMD        "-a"
#define DELETE_FILE_CMD     "-f"
#define FILE_CLUSTERS_CMD   "-c"
#define NEW_DIR_CMD         "-m"
#define DELETE_DIR_CMD      "-r"
#define PRINT_FILE_CMD      "-l"
#define PRINT_ALL_CMD       "-p"

/*
 * Prints the cluster of a filename.
 *
 * Returns:
 * OK: everything is OK.
 * PATH_NOT_FOUND: filename not found in fat.
 */
int print_clusters(FILE* file, char* filename, Boot_record* boot_record, int32_t* fat);

/*
 * Splits the filename in format dir/dir/dir/fname to an array
 * [dir],[dir],[dir],[filename].
 * The array is returned (must be freed) and the number of items is stored into count.
 *
 * If an error occurs, NULL is returned.
 */
char** split_file_path(char* filename, int* count);

/*
 * Prints the contents of the file (filename).
 *
 * Returns:
 * OK: everything is OK.
 * PATH_NOT_FOUND: filename not found in fat.
 */
int print_file_content(FILE* file, char* filename, Boot_record* boot_record, int32_t* fat);



#endif //SEMESTRALKA_COMMANDS_H
