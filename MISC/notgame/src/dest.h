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
typedef struct dest_struct {
  char *name;
} dest_t;
typedef struct dest_group_struct {
  char *name;
  GList *members;
} dest_group_t;

/************ Variables ************/
extern GList *dest_groups, *dest_group_names;
extern GtkWidget *dest_clist, *dest_groups_box;

/************ Function Prototypes ************/

extern char dest_group_cmp(const dest_group_t *g1, const dest_group_t *g2);
extern char dest_group_cmp_name(const dest_group_t *group, const char *name);
extern unsigned char dest_group_add(dest_group_t *group);
extern void dest_group_add_from_gui(void);
extern void dest_group_update_lists_from_gui(void);
extern void dest_group_update_list(dest_group_t *entry, GtkWidget *clist);
extern dest_group_t *dest_group_find_by_name(char *name);
extern char dest_cmp(const dest_t *p1, const dest_t *p2);
extern char dest_cmp_name(const dest_t *p1, const char *name);
extern dest_t *dest_find(dest_group_t *group, dest_t *dest);
extern dest_t *dest_find_by_name(dest_group_t *group, char *name);
extern unsigned char dest_group_add_dest(dest_group_t *group, dest_t *dest);
extern void dest_group_delete_dest(dest_group_t *group, dest_t *dest);
extern void dest_group_delete_dest_by_name(dest_group_t *group, char *name);
extern gint dest_find_in_clist(GtkWidget *clist, char *name);
extern void dest_add_to_clist(dest_t *dest, GtkWidget *list);
extern void dest_group_make_clist(GtkWidget *list, dest_group_t *group);
extern dest_group_t *dest_group_get_current(void);
extern void dest_group_edit_dialog(void);

#endif	/* _DEST_H_ */
