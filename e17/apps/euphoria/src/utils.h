#ifndef __UTILS_H
#define __UTILS_H

/*
 * $Id$
 * vim:noexpandtab:sw=4:sts=4:ts=4
 */

typedef enum {
	DEBUG_LEVEL_CRITICAL,
	DEBUG_LEVEL_WARNING,
	DEBUG_LEVEL_INFO,
	DEBUG_LEVEL_NUM
} DebugLevel;

char *find_theme(const char *name);
bool is_dir(const char *dir);
char *strstrip(char *str);
const char *get_login();

void debug(DebugLevel level, const char *fmt, ...);

#endif

