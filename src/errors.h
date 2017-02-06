

#ifndef SEMESTRALKA_ERRORS_H
#define SEMESTRALKA_ERRORS_H

#include "errno.h"

#define OK_MSG						"OK"
#define PATH_NOT_FOUND_MSG          "PATH NOT FOUND"
#define PATH_NOT_EMPTY_MSG			"PATH NOT EMPTY"

typedef enum {

    OK = -100,
    NOK,
    ERR_READING_FILE,
    ERR_FAT_NOT_FOUND,
    ERR_PATH_NOT_FOUND,
    ERR_PATH_NOT_EMPTY

} Errors;

#endif //SEMESTRALKA_ERRORS_H
