#include "commands.h"
#include "fat.h"

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
	if(dir_name[0] != '/' || dir_name[len-1] != '/') {
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
    int found = NOK;
    int ret = ERR_PATH_NOT_FOUND;
    int tmp = 0;
    int file_path_count = 0;
    char** filepath = NULL;
    Directory found_file;


    // split filename to filepath
    filepath = split_file_path(filename, &file_path_count);
    if(file_path_count == 0) {
    	if(filepath != NULL) {
    		free(filepath);
    	}
		serror(COMMANDS_NAME, "Error while parsing the file path\n");
		printf("%s\n",PATH_NOT_FOUND_MSG);
		return ret;
	}

    // find the file
    tmp = find_file(file, boot_record, filepath, file_path_count, &found_file, NULL);
    if(tmp >= 0) {
    	found = OK;
    }

    if(found == OK) {
        // print file
        ret = OK;
        printf("%s %d",filename, found_file.start_cluster);
        tmp = fat[found_file.start_cluster];
        while(tmp != FAT_FILE_END) {
            printf(":%d",tmp);
            tmp = fat[tmp];
        }
        printf("\n");
    } else {
        printf("%s\n",PATH_NOT_FOUND_MSG);
    }

    free(filepath);
    return ret;
}

int print_file_content(FILE* file, Boot_record* boot_record, int32_t* fat, char* filename) {
    int found = NOK;
    int ret = ERR_PATH_NOT_FOUND;
    int tmp = 0;
    int file_path_count = 0;
    int position = 0;
    int offset = 0;
    char* buffer = NULL;
    int buffer_size = 0;
    char** filepath = NULL;
    Directory found_file;

    // split the filename to file path
    filepath = split_file_path(filename, &file_path_count);
    if(file_path_count == 0) {
    	if(filepath != NULL) {
    		free(filepath);
    	}
        serror(COMMANDS_NAME, "Error while parsing the file path\n");
        printf("%s\n",PATH_NOT_FOUND_MSG);
        return ret;
    }

    tmp = find_file(file, boot_record, filepath, file_path_count, &found_file, NULL);
    if( tmp >= 0) {
    	found = OK;
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
        tmp = found_file.start_cluster;

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
    return ret;
}

int add_directory(FILE* file, Boot_record* boot_record, int32_t* fat, char* newdir_name, char* target) {
	int i = 0;
	int found = NOK;
	int ret = ERR_PATH_NOT_FOUND;
	int file_path_count = 0;
	int position = 0;
	int offset = 0;
	Directory target_dir;
	char **filepath = NULL;
	Directory tmp_dir;
	int dir_cluster = FAT_DIRECTORY;
	int free_cluster = NO_CLUSTER;
	int tmp = 0;
	char log_msg[255];

	// split the filename to path items
	filepath = split_dir_path(target, &file_path_count);
	if(file_path_count == 0) {
		if(filepath != NULL) {
			free(filepath);
		}
		serror(COMMANDS_NAME, "Error while parsing the file path\n");
		return ret;
	}

	// found the target dir
	tmp = find_directory(file, boot_record, filepath, file_path_count, &target_dir, NULL);
	if(tmp >= 0) {
		found = OK;
	}

	if(found == OK) {
		// add new directory
		// position of the cluster containing target dir
		position = get_data_position(boot_record) +
				   boot_record->cluster_size * target_dir.start_cluster;

		// find free directory in the target dir
		offset = get_free_directory_in_cluster(file, boot_record, fat, target_dir.start_cluster);

		// check, if there's a room for a new directory in this cluster
		if(offset < 0) {
			// no room
			sprintf(log_msg, "No free room in the cluster %d.\n", target_dir.start_cluster);
			serror(COMMANDS_NAME, log_msg);
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
	return ret;
}

int delete_dir(FILE *file, Boot_record *boot_record, int32_t *fat, char *dir_name) {
	int item_count = 0;
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

	// split the filename to path items
	filepath = split_dir_path(dir_name, &file_path_count);
	if(file_path_count == 0) {
		if(filepath != NULL) {
			free(filepath);
		}
		serror(COMMANDS_NAME, "Error while parsing the file path\n");
		return ret;
	}

	dir_position = find_directory(file, boot_record, filepath, file_path_count, &target_dir, &parent_dir);
	if(dir_position >= 0) {
		found = OK;
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
	return ret;
}


int delete_file(FILE *file, Boot_record *boot_record, int32_t* fat, char *filename) {
	int i = 0;
	int found = NOK;
	int ret = ERR_PATH_NOT_FOUND;
	int file_path_count = 0;
	int position = 0;
	int offset = 0;
	int fat_offset = 0;
	char **filepath = NULL;
	Directory empty_dir;
	Directory target_dir;
	Directory parent_dir;
	int parent_cluster = 0;
	int file_position = 0;
	int32_t unused_cluster = FAT_UNUSED;
	int tmp_clstr = 0;


	// split the filename to path items
	filepath = split_file_path(filename, &file_path_count);
	if(file_path_count == 0) {
		if(filepath != NULL) {
			free(filepath);
		}
		serror(COMMANDS_NAME, "Error while parsing the file path\n");
		return ret;
	}

	// find the file
	file_position = find_file(file, boot_record, filepath, file_path_count, &target_dir, &parent_dir);
	if(file_position >= 0) {
		found = OK;
	}

	if(found == OK) {
		// delete the file
		position = get_data_position(boot_record);

		// rewrite the Directory entry with empty Directory
		// to rewrite the directory entry, parent cluster must be found
		memset(&empty_dir, '\0', sizeof(Directory));
		offset = parent_cluster * boot_record->cluster_size + sizeof(Directory)*file_position;
		fseek(file, position + offset, SEEK_SET);
		fwrite(&empty_dir, sizeof(Directory), 1, file);

		// mark all the file clusters in fat (and all it's copies) as UNUSED
		position = sizeof(Boot_record);
		offset = 0;
		for(i = 0; i < boot_record->fat_copies; i++) {
			// for every copy of fat, go through the file clusters and mark them as UNUSED
			tmp_clstr = target_dir.start_cluster;
			while(tmp_clstr != FAT_FILE_END) {
				// current cluster position
				fat_offset = sizeof(int32_t) * tmp_clstr;

				// next cluster
				tmp_clstr = fat[tmp_clstr];

				// delete current cluster
				fseek(file, position + offset + fat_offset, SEEK_SET);
				fwrite(&unused_cluster, sizeof(int32_t), 1, file);
			}

			offset += sizeof(int32_t)*boot_record->usable_cluster_count;	// start of the next copy of fat.
		}
		ret = OK;
	}

	// clean up
	free(filepath);
	return ret;
}

int add_file(FILE *file, Boot_record *boot_record, int32_t *fat, char *source_filename, char *dest_filename) {
	int ret = ERR_PATH_NOT_FOUND;
	char **destination = NULL;
	char dest_fname_buffer[255];
	char fname_buffer[20];
	int i = 0;
	int tmp = 0;
	int dest_exists = NOK;
	int file_size;
	FILE *source = NULL;
	Directory dest_dir;
	Directory new_file;
	int file_path_count = 0;
	int dir_position = 0;
	char *buffer = NULL;
	int dir_offset = 0;
	int offset = 0;
	char log_msg[255];
	int bytes_read = 0;
	int tmp_cluster = 0;
	int next_cluster = 0;
	int buffer_size = 0;
	int position = 0;

	// locate the destination (dir)
	// separate the filename from the destination path
	tmp = strlen(dest_filename);
	if(tmp < 2 || dest_filename[tmp -1] == '/') {
		// not a valid filename
		return ret;
	}

	strcpy(dest_fname_buffer, dest_filename);
	fname_buffer[0] = '\0';
	for(i = tmp - 1; i >= 0; i--) {
		if(dest_fname_buffer[i] == '/') {
			strcpy(fname_buffer, &dest_fname_buffer[i+1]);
			dest_fname_buffer[i+1] = '\0';
			break;
		}
	}
	if(fname_buffer[0] == '\0') {
		// not a valid filename
		return ret;
	}

	sprintf(log_msg, "Name of the new file: %s.\n", fname_buffer);
	sdebug(COMMANDS_NAME, log_msg);

	// find the destination dir
	destination = split_dir_path(dest_fname_buffer, &file_path_count);
	if(file_path_count <= 0) {
		if(destination != NULL) {
			free(destination);
		}
		serror(COMMANDS_NAME, "Error while parsing the destination path.\n");
		return ret;
	}

	dir_position = find_directory(file, boot_record, destination, file_path_count, &dest_dir, NULL);
	if(dir_position >= 0) {
		dest_exists = OK;
	}

	// locate the source file
	source = fopen(source_filename , "rb");
	if(source == NULL) {
		sprintf(log_msg, "Error while opening source file: %s.\n", source_filename);
		serror(COMMANDS_NAME, log_msg);
		dest_exists = NOK;
	}

	// write the file to the fat
	if(dest_exists == OK) {
		strcpy(new_file.name, fname_buffer);
		new_file.start_cluster = get_free_cluster(fat, boot_record->usable_cluster_count);
		new_file.isFile = true;
		file_size = 0;

		// check if there's a room in the cluster for a new file
		dir_offset = get_free_directory_in_cluster(file, boot_record, fat, dest_dir.start_cluster);
		if(dir_offset < 0) {
			// no room
			sprintf(log_msg, "Error: no room for a new file in the %s directory.\n", dest_fname_buffer);
			serror(COMMANDS_NAME, log_msg);
			free(destination);
			fclose(source);
			return ret;
		}

		// read file by cluster
		bytes_read = 0;
		buffer_size = sizeof(char) * boot_record->cluster_size;
		buffer = malloc(buffer_size);
		if(buffer == NULL) {
			serror(COMMANDS_NAME, "Error while allocating memory for buffer.\n");
			free(destination);
			fclose(source);
			return ret;
		}

		// fill the buffer with zeroes, so that it's correctly stored in fat
		memset(buffer, '\0', buffer_size);

		// read from source file to buffer, the size of buffer is actually
        // a cluster size.
		position = get_data_position(boot_record);
		next_cluster = new_file.start_cluster;
		tmp_cluster = new_file.start_cluster;
		while((bytes_read = fread(buffer, 1, buffer_size, source)) > 0) {
			// save from buffer to fat
			file_size += bytes_read;

			offset = next_cluster * boot_record->cluster_size;
			fseek(file, position+offset, SEEK_SET);
			fwrite(buffer, buffer_size, 1, file);

            // buffer full, get next cluster
            if(file_size % buffer_size == 0) {
                // mark it as FAT_FILE_END so that get_free_cluster() function will not return it
                tmp_cluster = next_cluster;
                fat[tmp_cluster] = FAT_FILE_END;
                next_cluster = get_free_cluster(fat, boot_record->usable_cluster_count);
                fat[tmp_cluster] = next_cluster;

                // clear the buffer
                memset(buffer, '\0', buffer_size);
            } else {
                tmp_cluster = next_cluster;
            }
		}
		fclose(source);
		fat[tmp_cluster] = FAT_FILE_END;

		// update fat table
		position = sizeof(Boot_record);
		fseek(file, position, SEEK_SET);
		for(i = 0; i < boot_record->fat_copies; i++) {
			fwrite(fat, sizeof(int32_t)*boot_record->usable_cluster_count, 1, file);
		}

		// save the new file entry
		new_file.size = file_size;

		position = get_data_position(boot_record);
		offset = dest_dir.start_cluster*boot_record->cluster_size;
		fseek(file, position+offset+dir_offset, SEEK_SET);
		fwrite(&new_file, sizeof(new_file), 1, file);
	}


	// cleanup
	free(destination);
	return ret;
}
