/*
 * $Id$
 */

#include <config.h>
#include <Evas.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "utils.h"

/**
 * Finds the filename for the theme @name.
 * Looks in: ~/.e/apps/eplayer/themes
 *           $prefix/share/eplayer/themes
 */
char *find_theme(const char *name) {
	static char eet[PATH_MAX + 1];
	struct stat st;

	snprintf(eet, sizeof(eet),
	         "%s/.e/apps/" PACKAGE "/" "themes/%s.eet",
	         getenv("HOME"), name);

	if (!stat(eet, &st))
		return eet;

	snprintf(eet, sizeof(eet), DATA_DIR "/themes/%s.eet", name);

	return stat(eet, &st) ? NULL : eet;
}

bool is_dir(const char *dir) {
	struct stat st;

	if (stat(dir, &st))
		return false;

	return (S_ISDIR(st.st_mode));
}

/**
 * Removes leading and trailing whitespace from a string.
 *
 * @param str String to strip
 * @return Stripped string
 */
char *strstrip(char *str) {
	char *start, *ptr = str;

	/* step over leading whitespace */
	for (start = str; isspace(*start); start++);

	if (str != start) {
		while ((*ptr++ = *start++));
		*ptr = 0;
	}

	if (!*str)
		return str;

	/* remove trailing whitespace */
	ptr = &str[strlen(str) - 1];

	if (!isspace(*ptr))
		return str;

	while (isspace(*ptr) && ptr >= str)
		ptr--;

	ptr[1] = 0;

	return str;
}

void debug(DebugLevel level, const char *fmt, ...) {
	va_list list;

	if (level > DEBUG_LEVEL || !fmt || !fmt[0])
		return;

	va_start(list, fmt);
	vprintf(fmt, list);
	va_end(list);
}
