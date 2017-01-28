
#include <string.h>
#include "fat.h"


/*
 * Loads boot record from the file.
 * Boot record is expected to be at the beginning of the file.
 * The file is assumed to be already opened.
 *
 *
 * Returns:
 * OK: boot record loaded.
 * ERR_READING_FILE: error while reading file.
 */
int load_boot_record(FILE* file, Boot_record* boot_record) {
    int bytes_read = 0;

    if(file == NULL || boot_record == NULL) {
        return ERR_READING_FILE;
    }

    bytes_read = (int)fread(boot_record, sizeof(Boot_record), 1, file);
    if(bytes_read != 1) {
        return ERR_READING_FILE;
    }
    return OK;
}


/*
 * Prints boot record.
 */
void print_boot_record(Boot_record* boot_record) {
    if(boot_record == NULL) {
        return;
    }

    printf("Volume description: %s\nFat type: %hhd\nFat copies: %hhd\nCluster size: %hdB\nUsable clusters: %d\nSignature: %s\n",
            boot_record->volume_descriptor, boot_record->fat_type, boot_record->fat_copies, boot_record->cluster_size, boot_record->usable_cluster_count, boot_record->signature);
}

