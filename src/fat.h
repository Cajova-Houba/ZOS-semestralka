

#ifndef SEMESTRALKA_FAT_H
#define SEMESTRALKA_FAT_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "errors.h"


#define DESCRIPTION_LEN         250
#define FAT_TYPE_LEN            1
#define FAT_COPIES_LEN          1
#define CLUSTER_SIZE_LEN        2
#define USABLE_CLUSTER_LEN      4
#define SIGNATURE_LEN           9

typedef enum {
    FAT_UNUSED = INT32_MAX - 1,
    FAT_FILE_END = INT32_MAX - 2,
    FAT_BAD_CLUSTERS = INT32_MAX -3,
    FAT_DIRECTORY = INT32_MAX -4
} Fat_special;

/*
 * Definition of boot record.
 */
typedef struct{
    char volume_descriptor[DESCRIPTION_LEN];    // FS description,                  250B
    int8_t fat_type;                            // FAT type (FAT12, FAT16...),      1B
    int8_t fat_copies;                          // number of FAT copies,            1B
    int16_t cluster_size;                       // cluster size                     2B
    int32_t usable_cluster_count;               // max number of cluster for data   4B
    char signature[SIGNATURE_LEN];              // orion login                      9B
} Boot_record;//                                                                    267B

/*
 * Definition of directory record.
 */
typedef struct {
    char name[13];                              // File name, 8+3+'\0'
    bool isFile;                                // Is file flag
    int32_t size;                               // Size of the file, 0 for directory
    int32_t start_cluster;                      // Start cluster
} Directory;

/*
 * Prints boot record.
 */
void print_boot_record(Boot_record* boot_record);


/*
 * Loads boot record from the file.
 * Boot record is expected to be at the beginning of the file.
 *
 * Returns:
 * OK: boot record loaded.
 * ERR_READING_FILE: error while reading file.
 */
int load_boot_record(FILE* file, Boot_record* boot_record);

/*
 * Loads a fat table from file. File table is expected to contain
 * boot_record->usable_cluster_count records.
 *
 * Returns:
 * OK: fat table loaded.
 */
int load_fat_table(FILE* file, Boot_record* boot_record, int32_t* dest);

#endif //SEMESTRALKA_FAT_H
