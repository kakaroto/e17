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

#ifndef _DEST_H_
#define _DEST_H_

/************ Macros and Definitions ************/

/************ Structures ************/

/************ Variables ************/
extern GList *dest_group_list;

/************ Function Prototypes ************/
extern void dest_group_add(gpointer pbox, GtkWidget *w);
extern void dest_list_update(GList *group);

#endif	/* _DEST_H_ */
