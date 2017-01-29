

#ifndef SEMESTRALKA_ERRORS_H
#define SEMESTRALKA_ERRORS_H

#include "errno.h"

typedef enum {

    OK = -100,
    ERR_READING_FILE,
    ERR_FAT_NOT_FOUND,
    ERR_PATH_NOT_FOUND,
    ERR_PATH_NOT_EMPTY

} Errors;

#endif //SEMESTRALKA_ERRORS_H
