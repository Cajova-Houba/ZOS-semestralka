#ifndef __SENET_LOGGER_H
#define __SENET_LOGGER_H

#include <stdio.h>
#include <time.h>

// set to 0 to turn logging off
#define LOGGING_ON              0

/*
 * Possible names to use for log messages.
 */
#define NO_NAME                 ""
#define COMMANDS_NAME           "commands"
#define WRITER_THREAD           "writer"

void sinfo(char *name, char *msg);
void sdebug(char *name, char *msg);
void serror(char *name, char *msg);

#endif
