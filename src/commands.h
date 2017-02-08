#ifndef SEMESTRALKA_COMMANDS_H
#define SEMESTRALKA_COMMANDS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "fat.h"
#include "slog.h"

#define ADD_FILE_CMD        "-a"
#define DELETE_FILE_CMD     "-f"
#define FILE_CLUSTERS_CMD   "-c"
#define NEW_DIR_CMD         "-m"
#define DELETE_DIR_CMD      "-r"
#define PRINT_FILE_CMD      "-l"
#define PRINT_ALL_CMD       "-p"
#define BAD_BLOCKS_CMD       "-v"

/*
 * Size of a content buffer for producer-consumer like approach.
 */
#define CONTENT_BUFFER_SIZE     10
/*
 * Structure will contain cluster and its position in file to which
 * it should be saved.
 *
 * Cluster will be saved into file only if the write=OK. After that, write will be set to NOK,
 * indicating that the item can be used again.
 */
typedef struct {
    int write;
    int position;
    int cluster_size;
    char *cluster;
} Writable;

/*
 * Arguments passed to consumer(writer) thread.
 */
typedef struct {
    FILE *file;
    Boot_record *boot_record;
    int32_t *fat;
    Writable *content_buffer;
    int *stop_condition;


    pthread_mutex_t mutex;
    sem_t full;
    sem_t empty;
} Consumer_args;

/*
 * A consumer thread which will write items from content_buffer to file.
 * The consumer will keep writing items while the stop_condition is NOK.
 * Mutexes and semaphores are expected to be already initialized.
 */
void *writer_thread(void *thread_args);

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

/*
 * Deletes the file from fat.
 *
 * Returns:
 * OK: everything is OK.
 * PATH_NOT_FOUND: file not found.
 */
int delete_file(FILE *file, Boot_record *boot_record, int32_t* fat, char *filename);

/*
 * Adds a file source_filename to the path of fat. The path is specified by dest_filename. If the path doens't exist
 * (in the source or dest), PATH_NOT_FOUND will be returned. Note that the only the dest path must exist in the fat, not the
 * file (it will be created by this method).
 *
 * Returns:
 * OK: file added
 * PATH_NOT_FOUND: path (source or dest) not found.
 * ERR_READING_FILE: error while reading file (with fat or source file).
 * ERR_FILE_TOO_BIG: the file won't fit into the fat.
 */
int add_file(FILE *file, Boot_record *boot_record, int32_t *fat, char *source_filename, char *dest_filename);

/*
 * Prints the array of Directory items.
 * Each item will be prefixed by \t * level.
 * If the item is directory, uses recursion to print its contents.
 */
void print_items(FILE *file, Boot_record *boot_record, Directory *items, int item_count, int level);

/*
 * Prints a whole file tree.
 */
void print_file_tree(FILE *file, Boot_record *boot_record);

/*
 * Goes through the fat table and if the bad cluster is detected, its contents will be moved to UNUSED cluster.
 *
 * Returns:
 * number of bad blocks moved: everything is ok.
 * ERR_READING_FILE: error while reading the file with fat table.
 */
int fix_bad_blocks(FILE *file, Boot_record *boot_record, int32_t *fat);

/*
 * Recursively checks the items in dir and its subdirs. The dir is defined by the cluster.
 *
 * Returns:
 * Number of found and fixed bad clusters: everything was ok.
 * NOK: error occurred.
 */
int check_directory_items_bad_blocks(FILE *file, Boot_record *boot_record, int32_t *fat, Directory *items, int item_count, int parent_cluster);

/*
 * Checks the whole file tree for bad blocks and if the bad block is detected, its content will be moved to another,
 * unused cluster.
 *
 * Returns:
 * Number of found and fixed bad blocks: everything was ok.
 * NOK: error while fixing bad blocks.
 */
int check_file_tree(FILE *file, Boot_record *boot_record, int32_t *fat);

/*
 * Checks the cluster and if it's bad, moves it to first free cluster found.
 * Also, if the cluster is bad, it will be marked in a fat, so if the fat[cluster]
 * is pointing to the next cluster, save the value before calling this function.
 *
 * Note that the fat in file is not updated.
 *
 * If the new cluster number is returned, fat[new_cluster] is still UNUSED.
 *
 * Returns:
 * OK:  cluster is ok.
 * Number of the new cluster: cluster was bad and was moved to the new cluster.
 * NOK: error occurred.
 */
int check_cluster(FILE *file, Boot_record *boot_record, int32_t *fat, int cluster);

/*
 * Checks if the file contains any bad blocks. If the file contains bad blocks, those will
 * be moved to new unused blocks and references in fat will be updated.
 *
 * For every bad cluster fixed, the bad_cluster_cntr will be incremented.
 *
 * Returns:
 * OK: file checked.
 * FILE_CHANGED: file had bad clusters, which were fixed but the first cluster remained unchanged
 * Number of the new cluster: new first cluster.
 * NOK: error occurred.
 */
int check_file(FILE *file, Boot_record *boot_record, int32_t *fat, int cluster, int *bad_cluster_cntr);
#endif //SEMESTRALKA_COMMANDS_H
