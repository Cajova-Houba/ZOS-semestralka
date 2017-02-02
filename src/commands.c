#include "commands.h"

int print_clusters(FILE* file, char* filename, Boot_record* boot_record, int32_t* fat) {
    int max_items_in_dir = 0;
    Directory *dir_items = NULL;
    int item_count = 0;
    int i = 0;
    int found = NOK;
    int ret = ERR_PATH_NOT_FOUND;
    int tmp = 0;

    // load root dir
    max_items_in_dir = max_items_in_directory(boot_record);
    dir_items = malloc(sizeof(Directory) * max_items_in_dir);
    item_count = load_dir(file, boot_record, ROOT_CLUSTER, dir_items);

    // search the whole tree until the Directory with filename is found
    // todo: better search -> filename will contain the whole path, so it's not necessary to search the whole tree
    for(i = 0; i < item_count; i++) {
        if(strcmp(filename, dir_items[i].name) == 0 && dir_items[i].isFile) {
            found = OK;
            break;
        }
    }

    if(found == OK) {
        // print file
        ret = OK;
        printf("%s %d",filename, dir_items[i].start_cluster);
        tmp = fat[dir_items[i].start_cluster];
        while(tmp != FAT_FILE_END) {
            printf(":%d",tmp);
            tmp = fat[tmp];
        }
        printf("\n");
    } else {
        printf("%s\n",PATH_NOT_FOUND_MSG);
    }

    free(dir_items);
    return ret;
}

int print_file_content(FILE* file, char* filename, Boot_record* boot_record, int32_t* fat) {
	int max_items_in_dir = 0;
    Directory *dir_items = NULL;
    int item_count = 0;
    int i = 0;
    int found = NOK;
    int ret = ERR_PATH_NOT_FOUND;
    int tmp = 0;
    int position = 0;
    int offset = 0;
    char* buffer = NULL;
    int buffer_size = 0;
    
    // load root dir
    max_items_in_dir = max_items_in_directory(boot_record);
    dir_items = malloc(sizeof(Directory) * max_items_in_dir);
    item_count = load_dir(file, boot_record, ROOT_CLUSTER, dir_items);

    // search the whole tree until the Directory with filename is found
    // todo: better search -> filename will contain the whole path, so it's not necessary to search the whole tree
    for(i = 0; i < item_count; i++) {
        if(strcmp(filename, dir_items[i].name) == 0 && dir_items[i].isFile) {
            found = OK;
            break;
        }
    }

    if(found == OK) {
		// seek to the start of the data section
		position = sizeof(Boot_record)+ sizeof(int32_t)*boot_record->usable_cluster_count*boot_record->fat_copies;
		fseek(file, position, SEEK_SET);
		buffer_size = boot_record->cluster_size + 1;
		buffer = malloc(sizeof(char) * buffer_size);
		
        // print file content
        ret = OK;
        printf("%s: ",filename);
        tmp = dir_items[i].start_cluster;
        
        while(tmp != FAT_FILE_END){
			
			// read cluster
			// printf("\n>>>>Reading from cluster %d\n", tmp);
			offset = boot_record->cluster_size*tmp;
			fseek(file, position + offset, SEEK_SET);
			fread(buffer, boot_record->cluster_size, 1, file);
			buffer[buffer_size] = '\0';
			
            printf("%s",buffer);
            tmp = fat[tmp];
        }
        printf("\n");
        
        free(buffer);
        
    } else {
        printf("%s\n",PATH_NOT_FOUND_MSG);
    }

    free(dir_items);
    return ret;
}

