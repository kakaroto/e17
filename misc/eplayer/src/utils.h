#ifndef __UTILS_H
#define __UTILS_H

#include <Evas.h>

typedef enum {
	DEBUG_LEVEL_CRITICAL,
	DEBUG_LEVEL_WARNING,
	DEBUG_LEVEL_INFO,
	DEBUG_LEVEL_NUM
} DebugLevel;

int is_dir(const char *dir);
char *strstrip(char *str);
Evas_List *dir_get_files(const char *directory);

void debug(DebugLevel level, const char *fmt, ...);

#endif

