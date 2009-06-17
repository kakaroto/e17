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

#ifndef _PARSE_H_
#define _PARSE_H_

/************ Macros and Definitions ************/

/************ Structures ************/

/************ Variables ************/

/************ Function Prototypes ************/
extern void *parse_player_group(char *buff, void *state);
extern void *parse_dest_group(char *buff, void *state);
extern unsigned char save_config(char *filename);

#endif	/* _PARSE_H_ */
