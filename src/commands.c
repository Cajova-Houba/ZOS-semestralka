#include "commands.h"

char** split_dir_path(char *dir_name, int *count) {
	int cntr = 0;
	int i = 0;
	int len = 0;
	char** result = NULL;
	char* tmp = NULL;

	// get the filename length
	len = strlen(dir_name);
	if(len == 0) {
		*count = 0;
		return result;
	}

	// check that the '/' is present
	if(dir_name[0] != '/') {
		*count = 0;
		return result;
	}

	// count the number of path items
	cntr = 0;
	for(i = 0; i < len; i++) {
		if (dir_name[i] == PATH_DELIMITER[0]) {
			cntr++;
		}
	}
	*count = cntr;

	// allocate memory for result
	result = malloc(sizeof(char*) * cntr);

	// split the filename
	tmp = strdup(dir_name);
	for(i = 0; i < cntr; i++) {
		result[i] = strsep(&tmp, PATH_DELIMITER);
	}

	return result;
}

char** split_file_path(char* filename, int* count) {
    int cntr = 0;
    int i = 0;
    int len = 0;
    char** result = NULL;
    char* tmp = NULL;

    // get the filename length
    len = strlen(filename);
    if(len == 0) {
        *count = 0;
        return result;
    }

    // check that the '/' is present
    if(filename[0] != '/') {
    	*count = 0;
    	return result;
    }

    // count the number of path items
    cntr = 0;
    for(i = 0; i < len; i++) {
        if (filename[i] == PATH_DELIMITER[0]) {
            cntr++;
        }
    }
    *count = cntr;

    // allocate memory for result
    result = malloc(sizeof(char*) * cntr);

    // split the filename - skip the first '/'
    tmp = strdup(&filename[1]);
    for(i = 0; i < cntr; i++) {
    	result[i] = strsep(&tmp, PATH_DELIMITER);
    }

    return result;
}

int print_clusters(FILE* file, Boot_record* boot_record, int32_t* fat, char* filename) {
    int max_items_in_dir = 0;
    Directory *dir_items = NULL;
    int item_count = 0;
    int i = 0;
    int found = NOK;
    int ret = ERR_PATH_NOT_FOUND;
    int tmp = 0;
    int file_path_count = 0;
    char** filepath = NULL;


    // load root dir
    max_items_in_dir = max_items_in_directory(boot_record);
    dir_items = malloc(sizeof(Directory) * max_items_in_dir);
    item_count = load_dir(file, boot_record, ROOT_CLUSTER, dir_items);

    // split filename to filepath
    filepath = split_file_path(filename, &file_path_count);
    if(file_path_count == 0) {
		serror(COMMANDS_NAME, "Error while parsing the file path\n");
		printf("%s\n",PATH_NOT_FOUND_MSG);
		return ret;
	}

    // find the directory item
	// todo: search the whole path, not just the first path item
    for(i = 1; i < item_count; i++) {
        if(strcmp(filepath[0], dir_items[i].name) == 0 && dir_items[i].isFile) {
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

int print_file_content(FILE* file, Boot_record* boot_record, int32_t* fat, char* filename) {
	int max_items_in_dir = 0;
    Directory *dir_items = NULL;
    int item_count = 0;
    int i = 0;
    int found = NOK;
    int ret = ERR_PATH_NOT_FOUND;
    int tmp = 0;
    int file_path_count = 0;
    int position = 0;
    int offset = 0;
    char* buffer = NULL;
    int buffer_size = 0;
    char** filepath = NULL;

    // load root dir
    max_items_in_dir = max_items_in_directory(boot_record);
    dir_items = malloc(sizeof(Directory) * max_items_in_dir);
    item_count = load_dir(file, boot_record, ROOT_CLUSTER, dir_items);

    // split the filename to file path
    filepath = split_file_path(filename, &file_path_count);
    if(file_path_count == 0) {
        serror(COMMANDS_NAME, "Error while parsing the file path\n");
        printf("%s\n",PATH_NOT_FOUND_MSG);
        return ret;
    }

    // find the directory item
    // todo: search the whole path, not just the first path item
    for(i = 0; i < item_count; i++) {
        if(strcmp(filepath[0], dir_items[i].name) == 0 && dir_items[i].isFile) {
            found = OK;
            break;
        }
    }

    if(found == OK) {
		// seek to the start of the data section
		position = get_data_position(boot_record);
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

    // clean up
    free(filepath);
    free(dir_items);
    return ret;
}

int add_directory(FILE* file, Boot_record* boot_record, int32_t* fat, char* newdir_name, char* target) {
	int max_items_in_dir = 0;
	Directory *dir_items = NULL;
	Directory target_dir;
	int item_count = 0;
	int fp_item = 0;
	int i = 0;
	int found = NOK;
	int ret = ERR_PATH_NOT_FOUND;
	int file_path_count = 0;
	int position = 0;
	int offset = 0;
	char **filepath = NULL;
	Directory tmp_dir;
	int dir_cluster = FAT_DIRECTORY;
	int free_cluster = NO_CLUSTER;

	// load root dir
	max_items_in_dir = max_items_in_directory(boot_record);
	dir_items = malloc(sizeof(Directory) * max_items_in_dir);
	item_count = load_dir(file, boot_record, ROOT_CLUSTER, dir_items);

	// split the filename to path items
	filepath = split_dir_path(target, &file_path_count);
	if(file_path_count == 0) {
		serror(COMMANDS_NAME, "Error while parsing the file path\n");
		return ret;
	}

	// set the target dir to root dir
	target_dir.start_cluster = ROOT_CLUSTER;
	if(file_path_count == 1) {
		// root dir, mark as found
		found = OK;
	}

	// find the target dir
	// todo: search the whole path, not just the first path item
	for(fp_item = 1; fp_item < file_path_count; fp_item++) {
		// for every item in the path, search for dir in the current directory,
		// then go inside until the last path item is found.
		for(i = 0; i < item_count; i++) {
			if(strcmp(filepath[0], dir_items[i].name) == 0 && !dir_items[i].isFile) {
				found = OK;
				target_dir = dir_items[i];
				break;
			}
		}
	}

	if(found == OK) {
		// add new directory
		// position of the cluster containing target dir
		position = get_data_position(boot_record) +
				   boot_record->cluster_size * target_dir.start_cluster;

		// find the last Directory entry in the cluster
		item_count = count_items_in_dir(file, boot_record, &target_dir);
		offset = sizeof(Directory) * item_count;

		// check, if there's a room for a new directory in this cluster
		if(item_count >= max_items_in_dir) {
			// no room
			serror(COMMANDS_NAME, "No free room in the cluster.");
		} else {
			// find a free cluster for the dir
			free_cluster = get_free_cluster(fat, boot_record->usable_cluster_count);
			if(free_cluster == NO_CLUSTER) {
				serror(COMMANDS_NAME, "No free cluster found.");
			} else {

				// fill new directory
				tmp_dir.isFile = false;
				strcpy(tmp_dir.name, newdir_name);
				tmp_dir.size = 0;
				tmp_dir.start_cluster = free_cluster;

				// todo: parallel??
				// save the new directory
				fseek(file, position+offset, SEEK_SET);
				fwrite(&tmp_dir,sizeof(Directory), 1, file);

				// update fat table and all copies
				position = sizeof(Boot_record);
				offset = sizeof(int32_t) * free_cluster;
				for(i = 0; i < boot_record->fat_copies; i++) {
					// todo: better seek?
					fseek(file, position+offset, SEEK_SET);
					fwrite(&dir_cluster, sizeof(int32_t), 1, file);

					// i*fat_size + cluster
					offset += sizeof(int32_t) * boot_record->usable_cluster_count;
				}
				ret = OK;
			}

		}

	}

	// clean up
	free(filepath);
	free(dir_items);
	return ret;
}

int delete_dir(FILE *file, Boot_record *boot_record, int32_t *fat, char *dir_name) {
	int max_items_in_dir = 0;
	Directory *dir_items = NULL;
	Directory *tmp_dir = NULL;
	int item_count = 0;
	int fp_item = 0;
	int i = 0;
	int found = NOK;
	int ret = ERR_PATH_NOT_FOUND;
	int file_path_count = 0;
	int position = 0;
	int offset = 0;
	char **filepath = NULL;
	Directory empty_dir;
	Directory target_dir;
	Directory parent_dir;
	int parent_cluster = 0;
	int dir_position = 0;
	int32_t unused_cluster = FAT_UNUSED;

	// load root dir
	max_items_in_dir = max_items_in_directory(boot_record);
	dir_items = malloc(sizeof(Directory) * max_items_in_dir);
	item_count = load_dir(file, boot_record, ROOT_CLUSTER, dir_items);

	// split the filename to path items
	filepath = split_dir_path(dir_name, &file_path_count);
	if(file_path_count == 0) {
		serror(COMMANDS_NAME, "Error while parsing the file path\n");
		return ret;
	}

	// set the target and parent dirs to root dir
	target_dir.start_cluster = ROOT_CLUSTER;
	parent_dir.start_cluster = ROOT_CLUSTER;
	parent_cluster = ROOT_CLUSTER;
	if(file_path_count == 1) {
		// root dir, mark as found
		found = OK;
	}

	// find the target dir
	// todo: search the whole path, not just the first path item
	for(fp_item = 1; fp_item < file_path_count; fp_item++) {
		// for every item in the path, search for dir in the current directory,
		// then go inside until the last path item is found.
		for(i = 0; i < item_count; i++) {
			if(strcmp(filepath[fp_item], dir_items[i].name) == 0 && !dir_items[i].isFile) {
				found = OK;
				target_dir = dir_items[i];
				dir_position = i;
				break;
			}
		}
	}

	if(found == OK) {
		// check contents of the directory
		item_count = count_items_in_dir(file, boot_record, &target_dir);
		if(item_count != 0) {
			// dir is not empty
			ret = ERR_PATH_NOT_EMPTY;
		} else {
			position = get_data_position(boot_record);

			// rewrite the Directory entry with empty Directory
			// to rewrite the directory entry, parent cluster must be found
			memset(&empty_dir, '\0', sizeof(Directory));
			offset = parent_cluster * boot_record->cluster_size + sizeof(Directory)*dir_position;
			fseek(file, position + offset, SEEK_SET);
			fwrite(&empty_dir, sizeof(Directory), 1, file);

			// mark the cluster in fat (and all it's copies) as UNUSED
			position = sizeof(Boot_record);
			offset = target_dir.start_cluster*sizeof(int32_t);
			for(i = 0; i < boot_record->fat_copies; i++) {
				fseek(file, position + offset, SEEK_SET);
				fwrite(&unused_cluster, sizeof(int32_t), 1, file);

				offset += sizeof(int32_t)*boot_record->usable_cluster_count;
			}
			ret = OK;
		}
	}

	// clean up
	free(filepath);
	free(dir_items);
	return ret;
}

