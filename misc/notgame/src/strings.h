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

#ifndef _STRINGS_H_

#define _STRINGS_H_

#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#ifndef TRUE
#  define TRUE    ((unsigned char)(1))
#  define FALSE   ((unsigned char)(0))
#endif

#ifndef swap
#  define swap(a, b) (((int)(b)) ^= ((int)(a)) ^= ((int)(b)) ^= ((int)(a)))
#endif

#ifndef cswap
#  define cswap(a, b) ((b) ^= (a) ^= (b) ^= (a))
#endif

#define DEFAULT_DELIM " \r\n\f\t\v"

#define CONST_STRLEN(x)            (sizeof(x) - 1)
#define BEG_STRCASECMP(s, constr)  (strncasecmp(s, constr, CONST_STRLEN(constr)))

extern char *Word(unsigned long, const char *);
extern char *PWord(unsigned long, char *);
extern unsigned long NumWords(const char *);
extern char *StripWhitespace(char *);
extern char *CondenseWhitespace(char *);

#endif /* _STRINGS_H_ */
