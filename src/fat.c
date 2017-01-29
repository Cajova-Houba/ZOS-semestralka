
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

/*
 * Loads a fat table from file. File table is expected to contain
 * boot_record->usable_cluster_count records.
 */
int load_fat_table(FILE* file, Boot_record* boot_record, int32_t* dest) {
    int status = 0;
    int size = 0;

    // seek to the start of fat table
    size = sizeof(Boot_record);
    errno = 0;
    status = fseek(file, size, SEEK_SET);
    if(status != 0) {
        return ERR_READING_FILE;
    }

    // load the fat table
    size = sizeof(int32_t)*boot_record->usable_cluster_count;
    status = (int)fread(dest, (size_t)size, 1, file);
    if(status != 1) {
        return ERR_READING_FILE;
    }

    return OK;
}

/*
 * Loads contents of root directory from file and stores them to dest.
 * Dest is expected to be an array.
 */
int load_root_dir(FILE* file, Boot_record* boot_record, Directory* dest) {
    int status = 0;
    int size = 0;
    int i = 0;
    int dir_struct_size = 0;
    char log_msg[255];

    if(file == NULL || boot_record == NULL) {
        return ERR_READING_FILE;
    }

    // seek to the start of root dir
    size = sizeof(Boot_record)+ sizeof(int32_t)*boot_record->usable_cluster_count*boot_record->fat_copies;
    errno = 0;
    status = fseek(file, size, SEEK_SET);
    if(status != 0) {
        sprintf(log_msg, "Error while seeking to the root dir: %s.\n",strerror(errno));
        serror(NO_NAME, log_msg);
        return ERR_READING_FILE;
    }

    // start loading the contents
    // stopping conditions are:
    //      read bytes >= cluster size
    //      name starts with '\0'
    i = -1;
    size = 0;
    dir_struct_size = sizeof(Directory);
    do {
        i++;

        errno = 0;
        status = (int)fread(&dest[i], (size_t)dir_struct_size, 1, file);
        if(status != 1) {
            sprintf(log_msg, "Error while loading root item %d.\n", i);
            serror(NO_NAME, log_msg);
        }

        size += dir_struct_size;

    } while (size <= boot_record->cluster_size && dest[i].name[0] != '\0');

    return i;
}

/*
 * Prints directory structure to the buffer.
 */
void print_dir(char* buffer, Directory* directory, int level) {
    int i = 0;

    if(directory == NULL) {
        return;
    }

    for(i = 0; i < level; i++) {
        buffer[i] = '\t';
    }

    if(directory->isFile == true) {
        sprintf(&buffer[i], FILE_PRINT_FORMAT, directory->name, directory->start_cluster, directory->size);
    } else {
        sprintf(&buffer[i], DIR_PRINT_FORMAT, directory->name);
    }
}
