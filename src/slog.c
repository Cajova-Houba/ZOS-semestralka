
#include "slog.h"

#define LOG_FORMAT	"%s [%2d.%2d. %4d %2d:%2d] %s: %s"

void sinfo(char *name, char *msg) {
	if(LOGGING_ON != 1) {
		return;
	}
	time_t rawtime = time(NULL);
	struct tm *inf = gmtime(&rawtime);
	printf(LOG_FORMAT,"INFO",
	inf->tm_mday,inf->tm_mon+1,inf->tm_year+1900,inf->tm_hour,inf->tm_min,
	name,msg);
}

void sdebug(char *name, char *msg) {
	if(LOGGING_ON != 1) {
		return;
	}
	time_t rawtime = time(NULL);
	struct tm *inf = gmtime(&rawtime);
	printf(LOG_FORMAT,"DEBUG",
	inf->tm_mday,inf->tm_mon+1,inf->tm_year+1900,inf->tm_hour,inf->tm_min,
	name,msg);
}

void serror(char *name, char *msg) {
	if(LOGGING_ON != 1) {
		return;
	}
	time_t rawtime = time(NULL);
	struct tm *inf = gmtime(&rawtime);
	printf(LOG_FORMAT,"DEBUG",
	inf->tm_mday,inf->tm_mon+1,inf->tm_year+1900,inf->tm_hour,inf->tm_min,	
	name,msg);
}

