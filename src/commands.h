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
 * Creates a new directory 'dirname' in existing directory 'target'.
 *
 * Returns:
 * OK: everything is OK.
 * PATH_NOT_FOUND: 'target' not found in fat.
 *
 */
int add_directory(FILE *file, Boot_record *boot_record, int32_t *fat, char *newdir_name, char *target);

/*
 * Prints the cluster of a filename.
 *
 * Returns:
 * OK: everything is OK.
 * PATH_NOT_FOUND: filename not found in fat.
 */
int print_clusters(FILE *file,Boot_record *boot_record, int32_t *fat, char *filename);

/*
 * Splits the dirname in formar /dir1/dir2/dir3/ to an array
 * [],[dir1],[dir2],[dir2].
 *
 * If an error occurs, count is set to 0.
 */
char** split_dir_path(char *dir_name, int *count);

/*
 * Splits the filename in format /dir/dir/dir/fname to an array
 * [dir],[dir],[dir],[fname].
 * So the /filename would result into [filename] array.
 * The array is returned (must be freed) and the number of items is stored into count.
 *
 * If an error occurs, count is set to 0.
 */
char** split_file_path(char *filename, int *count);

/*
 * Prints the contents of the file (filename).
 *
 * Returns:
 * OK: everything is OK.
 * PATH_NOT_FOUND: filename not found in fat.
 */
int print_file_content(FILE *file, Boot_record *boot_record, int32_t *fat, char *filename);

/*
 * Deletes the directory from fat.
 *
 * Returns:
 * OK: everything is OK.
 * PATH_NOT_FOUND: directory not found.
 * PATH_NOT_EMPTY: directory contains items.
 */
int delete_dir(FILE *file, Boot_record *boot_record, int32_t *fat, char *dir_name);



#endif //SEMESTRALKA_COMMANDS_H
