#include <config.h>
#include <Evas.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdarg.h>
#include <stdio.h>
#include "utils.h"

int is_dir(const char *dir) {
	struct stat st;

	if (stat(dir, &st))
		return 0;

	return (S_ISDIR(st.st_mode));
}

Evas_List *dir_get_files(const char *directory) {
	Evas_List *list = NULL;
	DIR *dir;
	struct dirent *entry;

	if (!(dir = opendir(directory)))
		return NULL;

	while ((entry = readdir(dir))) {
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
			continue;

		if (!is_dir(entry->d_name))
			list = evas_list_prepend(list, strdup(entry->d_name));
	}

	closedir(dir);

	if (list)
		list = evas_list_reverse(list);

	return list;
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
