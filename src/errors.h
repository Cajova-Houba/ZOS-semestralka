

#ifndef SEMESTRALKA_ERRORS_H
#define SEMESTRALKA_ERRORS_H

#include "errno.h"

#define OK_MSG						"OK\n"
#define PATH_NOT_FOUND_MSG          "PATH NOT FOUND\n"
#define PATH_NOT_EMPTY_MSG			"PATH NOT EMPTY\n"
#define FILE_TOO_BIG_MSG            "FILE TOO BIG\n"
#define ERR_MSG                     "ERROR\n"
#define ERR_READING_FILE_MSG        "ERROR WHILE READING FILE\n"
#define NO_FREE_ROOM_MSG            "NO ROOM\n"
#define ALREADY_EXISTS_MSG          "FILE ALREADY EXISTS\n"

typedef enum {

    OK = -100,
    NOK,
    NO_THREAD,
    FILE_CHANGED,
    ERR_READING_FILE,
    ERR_FAT_NOT_FOUND,
    ERR_FILE_TOO_BIG,
    ERR_PATH_NOT_FOUND,
    ERR_PATH_NOT_EMPTY,
    ERR_NO_FREE_ROOM,
    ERR_ALREADY_EXISTS

} Errors;

#endif //SEMESTRALKA_ERRORS_H
