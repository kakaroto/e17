/* The Not Game
 *
 * Original concept and Java implementation by Rob Coffey.  Concept
 * and name used with permission.
 *
 * The Not Game for Gtk+, Copyright 1999, Michael Jennings
 *
 * This program is free software and is distributed under the terms of
 * the Artistic License.  Please see the file "Artistic" supplied with
 * this program for license terms.
 */

static const char cvs_ident[] = "$Id$";

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include "debug.h"
#include "strings.h"

/* Return malloc'd pointer to index-th word in str.  "..." counts as 1 word. */
char *
Word(unsigned long index, const char *str)
{

  char *tmpstr;
  char *delim = DEFAULT_DELIM;
  register unsigned long i, j, k;

  k = strlen(str) + 1;
  if ((tmpstr = (char *) malloc(k)) == NULL) {
    fprintf(stderr, "Word(%lu, %s):  Unable to allocate memory -- %s.\n",
	    index, str, strerror(errno));
    return ((char *) NULL);
  }
  *tmpstr = 0;
  for (i = 0, j = 0; j < index && str[i]; j++) {
    for (; isspace(str[i]); i++);
    switch (str[i]) {
      case '\"':
	delim = "\"";
	i++;
	break;
      case '\'':
	delim = "\'";
	i++;
	break;
      default:
	delim = DEFAULT_DELIM;
    }
    for (k = 0; str[i] && !strchr(delim, str[i]);) {
      if (str[i] == '\\') {
	if (str[i + 1] == '\'' || str[i + 1] == '\"') {
	  i++;
	}
      }
      tmpstr[k++] = str[i++];
    }
    switch (str[i]) {
      case '\"':
      case '\'':
	i++;
	break;
    }
    tmpstr[k] = 0;
  }

  if (j != index) {
    free(tmpstr);
    D(("Word(%lu, %s) returning NULL.\n", index, str));
    return ((char *) NULL);
  } else {
    tmpstr = (char *) realloc(tmpstr, strlen(tmpstr) + 1);
    D(("Word(%lu, %s) returning \"%s\".\n", index, str, tmpstr));
    return (tmpstr);
  }
}

/* Return pointer into str to index-th word in str.  "..." counts as 1 word. */
char *
PWord(unsigned long index, char *str)
{

  register char *tmpstr = str;
  register unsigned long j;

  if (!str)
    return ((char *) NULL);
  for (; isspace(*tmpstr) && *tmpstr; tmpstr++);
  for (j = 1; j < index && *tmpstr; j++) {
    for (; !isspace(*tmpstr) && *tmpstr; tmpstr++);
    for (; isspace(*tmpstr) && *tmpstr; tmpstr++);
  }

  if (*tmpstr == '\"' || *tmpstr == '\'') {
    tmpstr++;
  }
  if (*tmpstr == '\0') {
    D(("PWord(%lu, %s) returning NULL.\n", index, str));
    return ((char *) NULL);
  } else {
    D(("PWord(%lu, %s) returning \"%s\"\n", index, str, tmpstr));
    return tmpstr;
  }
}

/* Returns the number of words in str, for use with Word() and PWord().  "..." counts as 1 word. */
unsigned long
NumWords(const char *str)
{

  register unsigned long cnt = 0;
  char *delim = DEFAULT_DELIM;
  register unsigned long i;

  for (i = 0; str[i] && strchr(delim, str[i]); i++);
  for (; str[i]; cnt++) {
    switch (str[i]) {
      case '\"':
	delim = "\"";
	i++;
	break;
      case '\'':
	delim = "\'";
	i++;
	break;
      default:
	delim = DEFAULT_DELIM;
    }
    for (; str[i] && !strchr(delim, str[i]); i++);
    switch (str[i]) {
      case '\"':
      case '\'':
	i++;
	break;
    }
    for (; str[i] && isspace(str[i]); i++);
  }

  D(("NumWords() returning %lu\n", cnt));
  return (cnt);
}

char *
StripWhitespace(register char *str)
{

  register unsigned long i, j;

  if ((j = strlen(str))) {
    for (i = j - 1; isspace(*(str + i)); i--);
    str[j = i + 1] = 0;
    for (i = 0; isspace(*(str + i)); i++);
    j -= i;
    memmove(str, str + i, j + 1);
  }
  return (str);
}

char *
CondenseWhitespace(char *s)
{

  register unsigned char gotspc = 0;
  register char *pbuff = s, *pbuff2 = s;

  for (; *pbuff2; pbuff2++) {
    if (isspace(*pbuff2)) {
      if (!gotspc) {
	*pbuff = ' ';
	gotspc = 1;
	pbuff++;
      }
    } else {
      *pbuff = *pbuff2;
      gotspc = 0;
      pbuff++;
    }
  }
  if ((pbuff >= s) && (isspace(*(pbuff - 1))))
    pbuff--;
  *pbuff = 0;
  return (realloc(s, strlen(s) + 1));
}
