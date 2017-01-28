
#include <string.h>
#include "fat.h"

/*
 * Reads n bytes from file and stores them to dest.
 * Returns OK if the n bytes are read, otherwise returns ERR_READING_FILE.
 */
int read_bytes_from_file(FILE* file, void* dest, int n) {
    int bytes_read = 0;

    bytes_read = (int)fread(dest, (size_t)1, (size_t)n, file);
    if(bytes_read != n) {
        return ERR_READING_FILE;
    }

    return OK;
}


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
    char buffer[SIGNATURE_LEN];
    int i = 0;

    if(file == NULL || boot_record == NULL) {
        return ERR_READING_FILE;
    }

    // read from file to the structure
    // description
    bytes_read = read_bytes_from_file(file, boot_record->volume_descriptor, DESCRIPTION_LEN);
    if(bytes_read != OK) {
        return ERR_READING_FILE;
    }
    boot_record->volume_descriptor[DESCRIPTION_LEN] = '\0';

    // fat type
    bytes_read = read_bytes_from_file(file, &boot_record->fat_type, FAT_TYPE_LEN);
    if(bytes_read != OK) {
        return ERR_READING_FILE;
    }

    // fat copies
    bytes_read = read_bytes_from_file(file, &boot_record->fat_copies, FAT_COPIES_LEN);
    if(bytes_read != OK) {
        return ERR_READING_FILE;
    }

    // cluster size
    bytes_read = read_bytes_from_file(file, &boot_record->cluster_size, CLUSTER_SIZE_LEN);
    if(bytes_read != OK) {
        return ERR_READING_FILE;
    }

    // usable cluster count
    bytes_read = read_bytes_from_file(file, &boot_record->usable_cluster_count, USABLE_CLUSTER_LEN);
    if(bytes_read != OK) {
        return ERR_READING_FILE;
    }

    // signature
    bytes_read = read_bytes_from_file(file, buffer, SIGNATURE_LEN);
    if(bytes_read != OK) {
        return ERR_READING_FILE;
    }

    // trim \0 at the beginning of signature
    while(buffer[i] == '\0' && i < SIGNATURE_LEN) {
        i++;
    }
    if(i < SIGNATURE_LEN) {
        strcpy(boot_record->signature, &buffer[i]);
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

