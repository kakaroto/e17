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

#ifndef _NOTGAME_H_
#define _NOTGAME_H_

/************ Macros and Definitions ************/

/************ Structures ************/

/************ Variables ************/

/************ Function Prototypes ************/
extern void clean_exit(const char *msg, ...);
extern void fatal_handler(int sig);
extern void ng_init(void);
extern void print_error(const char *msg, ...);
extern void print_warning(const char *msg, ...);

#endif	/* _NOTGAME_H_ */
