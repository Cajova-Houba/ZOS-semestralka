

#ifndef SEMESTRALKA_FAT_H
#define SEMESTRALKA_FAT_H

#include <stdint.h>
#include <stdio.h>

#include "errors.h"


#define DESCRIPTION_LEN         250
#define FAT_TYPE_LEN            1
#define FAT_COPIES_LEN          1
#define CLUSTER_SIZE_LEN        2
#define USABLE_CLUSTER_LEN      4
#define SIGNATURE_LEN           9

typedef struct{
    char volume_descriptor[DESCRIPTION_LEN];    // FS description,                  250B
    int8_t fat_type;                            // FAT type (FAT12, FAT16...),      1B
    int8_t fat_copies;                          // number of FAT copies,            1B
    int16_t cluster_size;                       // cluster size                     2B
    int32_t usable_cluster_count;               // max number of cluster for data   4B
    char signature[SIGNATURE_LEN];              // orion login                      9B
} Boot_record;//                                                                    267B

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


#endif //SEMESTRALKA_FAT_H
