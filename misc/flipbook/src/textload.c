/*****************************************************************************/
/* VA Linux Systems Flipbook demo                                            */
/*****************************************************************************/
/*
 * Copyright (C) 2000 Brad Grantham, Geoff Harrison, and VA Linux Systems
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "textload.h"

char *GetLine(char *s, int size, FILE * f)
{

	/* This function will get a single line from the file */

	char *ret, *ss, inquote;
	int i, j, k;
	static int line_stack_size = 0;
	static char **line_stack = NULL;

	s[0] = 0;
	if (line_stack_size > 0) {
		strncpy(s, line_stack[0], size);
		free(line_stack[0]);
		for (i = 0; i < line_stack_size - 1; i++)
			line_stack[i] = line_stack[i + 1];
		line_stack_size--;
		if (line_stack_size > 0) {
			line_stack = realloc(line_stack, line_stack_size * sizeof(char *));
		} else {
			free(line_stack);
			line_stack = NULL;
		}
		return s;
	}
	ret = fgets(s, size, f);

	if (strlen(s) > 0)
		s[strlen(s) - 1] = 0;

	while (IsWhitespace(s)) {
		s[0] = 0;
		ret = fgets(s, size, f);
		if (!ret)
			return NULL;
		if (strlen(s) > 0)
			s[strlen(s) - 1] = 0;
	}

	i = 0;
	inquote = 0;
	while (s[i]) {
		if (!inquote) {
			if (s[i] == '"') {
				j = i;
				while (s[j]) {
					s[j] = s[j + 1];
					j++;
				}
				inquote = 1;
				i--;
			}
		} else {
			if (s[i] == '"') {
				j = i + 1;
				while (s[j]) {
					if (s[j] == ';')
						break;
					if ((s[j] == '"') && (j == (i + 1)))
						break;
					if (!isspace(s[j])) {
						j--;
						break;
					}
					j++;
				}
				k = j - i;
				j = i;
				while (s[j]) {
					s[j] = s[j + k];
					j++;
				}
				inquote = 0;
				i--;
			}
		}
		i++;
	}

	j = strlen(s);
	if (j > 0) {
		if (strchr(s, ';')) {
			s[j] = ';';
			s[j + 1] = 0;
		}
	}
	i = 0;
	ss = s;
	while (s[i]) {
		if (s[i] == ';') {
			j = (&(s[i]) - ss);
			if (j > 0) {
				line_stack_size++;
				if (!line_stack)
					line_stack = malloc(line_stack_size * sizeof(char *));

				else
					line_stack = realloc(line_stack,
										  line_stack_size * sizeof(char *));

				line_stack[line_stack_size - 1] = malloc(j + 1);
				strncpy(line_stack[line_stack_size - 1], ss, j);
				line_stack[line_stack_size - 1][j] = 0;
				ss = &(s[i + 1]);
			}
		}
		i++;
	}

	if (line_stack_size > 0) {
		strncpy(s, line_stack[0], size);
		free(line_stack[0]);
		for (i = 0; i < line_stack_size - 1; i++)
			line_stack[i] = line_stack[i + 1];
		line_stack_size--;
		if (line_stack_size > 0) {
			line_stack = realloc(line_stack, line_stack_size * sizeof(char *));

		} else {
			free(line_stack);
			line_stack = NULL;
		}
		return s;
	}
	return ret;
}

int IsWhitespace(const char *s)
{
	int i = 0;

	while (s[i]) {
		if ((s[i] != ' ') && (s[i] != '\n') && (s[i] != '\t'))
			return 0;
		i++;
	}
	return 1;
}

