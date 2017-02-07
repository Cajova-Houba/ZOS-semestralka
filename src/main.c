#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "fat.h"
#include "commands.h"
#include "tests.h"

/*
 * 1 = execute the tests.
 * 0 = execute the app.
 */
#define RUN_TESTS	0

/*
 * Tries to load the fat file name and command name.
 * Returns the number of parameters read.
 */
int load_params(int argc, char** argv, char* fat_filename, char* command_name) {
    if(argc <= 1) {
        // todo: print help
        return 0;
    }

    if(argc <= 2) {
        // load file name
        strcpy(fat_filename, argv[1]);
        return 1;
    }

    if(argc <= 3) {
        // load command name
        strcpy(command_name, argv[2]);
        return 2;
    }

    return 0;
}

void print_fat(Boot_record *boot_record, int32_t* fat) {
	int i = 0;
	for(i = 0; i < /*boot_record->usable_cluster_count*/50; i++) {
		switch (fat[i]) {
			case FAT_UNUSED:
				printf("fat[%d] = FAT_UNUSED\n", i);
				break;
			case FAT_FILE_END:
				printf("fat[%d] = FAT_FILE_END\n", i);
				break;
			case FAT_BAD_CLUSTERS:
				printf("fat[%d] = FAT_BAD_CLUSTERS\n", i);
				break;
			case FAT_DIRECTORY:
				printf("fat[%d] = FAT_DIRECTORY\n", i);
				break;
			default:
				printf("fat[%d] = %d\n", i, fat[i]);
				break;
		}
	}
}

int main(int argc, char** argv) {
    char fat_filename[255];
    char command_name[3];
    int params_loaded = 0;
    int tmp = 0;
    int i = 0;
    int32_t fat_table[500];
    Boot_record fat_record;
    Directory root_dir[10];
    FILE* file = NULL;
    char buffer[250];

    if(RUN_TESTS == 1) {
    	run_tests();
    	return 0;
    }

    // load parameters
    params_loaded = load_params(argc, argv, fat_filename, command_name);
    if(params_loaded < 1) {
        return 0;
    }

    // load boot record from file
    file = fopen(fat_filename, "r+");
    if(file == NULL) {
        printf("Error while opening file %s: %d - %s.\n", fat_filename, errno, strerror(errno));
        return 0;
    }
    tmp = load_boot_record(file, &fat_record);
    if(tmp != OK) {
        printf("Error while loading boot record from file %d.\n", tmp);
        return 0;
    }
    print_boot_record(&fat_record);

    // load fat table
    tmp = load_fat_table(file, &fat_record, fat_table);
    if(tmp != OK) {
        printf("Error while loading fat table from file %s.\n", fat_filename);
        return 0;
    }

    // print fat table
    printf("Printing FAT table...\n");
    print_fat(&fat_record, fat_table);
    printf("OK.\n\n");


    // load root directory contents
    printf("Loading directory tree...\n");
    tmp = load_dir(file, &fat_record, 0, root_dir);
    if(tmp < 0) {
        printf("Error while loading root dir from file %s.\n", fat_filename);
    }
    printf("+ROOT\n");
    for(i = 0; i < tmp; i++) {
        print_dir(buffer, &root_dir[i], 1);
        printf(buffer);
    }
    printf("--\n");
    printf("OK.\n\n");

    // try to print clusters
    printf("Printing clusters...\n");
    print_clusters(file, &fat_record, fat_table, "/cisla.txt\0");
    print_clusters(file, &fat_record, fat_table, "/pohadka.txt\0");
    print_clusters(file, &fat_record, fat_table, "/pohadka\0");
    printf("OK\n\n");

    // try to print content
    printf("Printing contents...\n");
    print_file_content(file, &fat_record, fat_table, "/cisla.txt\0");
    print_file_content(file, &fat_record, fat_table, "/pohadka.txt\0");
    print_file_content(file, &fat_record, fat_table, "/msg.txt\0");
    print_file_content(file, &fat_record, fat_table, "/asdads.txt\0");
    print_file_content(file, &fat_record, fat_table, "/direct-1/asdads.txt\0");
    printf("OK\n\n");

    // add a new dir
    printf("Adding new directory to root...\n");
    add_directory(file, &fat_record, fat_table, "new_dir", "/");
    printf("OK\n\n");

    printf("Loading directory tree...\n");
	tmp = load_dir(file, &fat_record, 0, root_dir);
	if(tmp < 0) {
		printf("Error while loading root dir from file %s.\n", fat_filename);
	}
	printf("+ROOT\n");
	for(i = 0; i < tmp; i++) {
		print_dir(buffer, &root_dir[i], 1);
		printf(buffer);
	}
	printf("--\n");
	printf("OK.\n\n");

	tmp = load_fat_table(file, &fat_record, fat_table);
	if(tmp != OK) {
		printf("Error while loading fat table from file %s.\n", fat_filename);
		return 0;
	}
	printf("Printing FAT table...\n");
	print_fat(&fat_record, fat_table);
	printf("OK.\n\n");

	printf("Deleting directory... \n");
	delete_dir(file, &fat_record, fat_table, "/new_dir/");
	printf("OK.\n\n");

	printf("Loading directory tree...\n");
	tmp = load_dir(file, &fat_record, 0, root_dir);
	if(tmp < 0) {
		printf("Error while loading root dir from file %s.\n", fat_filename);
	}
	printf("+ROOT\n");
	for(i = 0; i < tmp; i++) {
		print_dir(buffer, &root_dir[i], 1);
		printf(buffer);
	}
	printf("--\n");
	printf("OK.\n\n");

	tmp = load_fat_table(file, &fat_record, fat_table);
	if(tmp != OK) {
		printf("Error while loading fat table from file %s.\n", fat_filename);
		return 0;
	}
	printf("Printing FAT table...\n");
	print_fat(&fat_record, fat_table);
	printf("OK.\n\n");






	// add dir1 and dir2
	// delete dir 1
	// add dir 3
	// dir 3 should be in dir 1 place
	printf("Adding new directory to root...\n");
	add_directory(file, &fat_record, fat_table, "new_dir1", "/");
	printf("OK\n\n");

	printf("Adding new directory to root...\n");
	add_directory(file, &fat_record, fat_table, "new_dir2", "/");
	printf("OK\n\n");

	printf("+ROOT\n");
	tmp = load_dir(file, &fat_record, 0, root_dir);
	if(tmp < 0) {
		printf("Error while loading root dir from file %s.\n", fat_filename);
	}
	for(i = 0; i < tmp; i++) {
		print_dir(buffer, &root_dir[i], 1);
		printf(buffer);
	}
	printf("--\n");
	printf("OK.\n\n");

	printf("Deleting directory... \n");
	delete_dir(file, &fat_record, fat_table, "/new_dir1/");
	printf("OK.\n\n");

	printf("+ROOT\n");
	tmp = load_dir(file, &fat_record, 0, root_dir);
	if(tmp < 0) {
		printf("Error while loading root dir from file %s.\n", fat_filename);
	}
	for(i = 0; i < tmp; i++) {
		print_dir(buffer, &root_dir[i], 1);
		printf(buffer);
	}
	printf("--\n");
	printf("OK.\n\n");

	printf("Adding new directory to root...\n");
	add_directory(file, &fat_record, fat_table, "new_dir3", "/");
	printf("OK\n\n");

	printf("+ROOT\n");
	tmp = load_dir(file, &fat_record, 0, root_dir);
	if(tmp < 0) {
		printf("Error while loading root dir from file %s.\n", fat_filename);
	}
	for(i = 0; i < tmp; i++) {
		print_dir(buffer, &root_dir[i], 1);
		printf(buffer);
	}
	printf("--\n");
	printf("OK.\n\n");




	// delete 1-cluster file
	printf("Deleting a file...\n");
	delete_file(file, &fat_record, fat_table, "/cisla.txt");
	printf("OK.\n\n");

	printf("+ROOT\n");
	tmp = load_dir(file, &fat_record, 0, root_dir);
	if(tmp < 0) {
		printf("Error while loading root dir from file %s.\n", fat_filename);
	}
	for(i = 0; i < tmp; i++) {
		print_dir(buffer, &root_dir[i], 1);
		printf(buffer);
	}
	printf("--\n");
	printf("OK.\n\n");
	tmp = load_fat_table(file, &fat_record, fat_table);
	if(tmp != OK) {
		printf("Error while loading fat table from file %s.\n", fat_filename);
		return 0;
	}
	printf("Printing FAT table...\n");
	print_fat(&fat_record, fat_table);
	printf("OK.\n\n");



	// add new file
	printf("Adding new file...\n");
	tmp = add_file(file, &fat_record, fat_table, "/home/zdenda/tmp/test_file.txt", "/test_f.txt");
	printf("OK.\n\n");
	printf("+ROOT\n");
	tmp = load_dir(file, &fat_record, 0, root_dir);
	if(tmp < 0) {
		printf("Error while loading root dir from file %s.\n", fat_filename);
	}
	for(i = 0; i < tmp; i++) {
		print_dir(buffer, &root_dir[i], 1);
		printf(buffer);
	}
	printf("--\n");
	printf("OK.\n\n");
	tmp = load_fat_table(file, &fat_record, fat_table);
	if(tmp != OK) {
		printf("Error while loading fat table from file %s.\n", fat_filename);
		return 0;
	}
	printf("Printing FAT table...\n");
	print_fat(&fat_record, fat_table);
	printf("OK.\n\n");

    printf("Printing contents...\n");
    print_file_content(file, &fat_record, fat_table, "/test_f.txt");
    printf("OK.\n\n");

    printf("Adding multi-cluster file...\n");
    add_file(file, &fat_record, fat_table, "/home/zdenda/tmp/multi_clust_file.txt", "/direct-1/multi-c.txt");
    printf("OK.\n\n");

    print_clusters(file, &fat_record, fat_table, "/direct-1/multi-c.txt");
    print_file_content(file, &fat_record, fat_table, "/direct-1/multi-c.txt");

    printf("Printing directory tree...\n");
    print_file_tree(file, &fat_record);
    printf("OK.\n\n");

	fclose(file);
    return 0;
}
