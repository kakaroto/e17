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

#ifndef _PLAYERS_H_
#define _PLAYERS_H_

/************ Macros and Definitions ************/

/************ Structures ************/
typedef struct player_struct {
  char *name;
  char *type;
} player_t;
typedef struct player_group_struct {
  char *name;
  GList *members;
} player_group_t;

/************ Variables ************/
extern GList *player_groups, *player_group_names;
extern GtkWidget *player_clist, *player_groups_box;

/************ Function Prototypes ************/

extern char player_group_cmp(const player_group_t *g1, const player_group_t *g2);
extern char player_group_cmp_name(const player_group_t *group, const char *name);
extern unsigned char player_group_add(player_group_t *group);
extern void player_group_add_from_gui(void);
extern void player_group_update_lists_from_gui(void);
extern void player_group_update_list(player_group_t *entry, GtkWidget *clist);
extern player_group_t *player_group_find_by_name(char *name);
extern char player_cmp(const player_t *p1, const player_t *p2);
extern char player_cmp_name(const player_t *p1, const char *name);
extern player_t *player_find(player_group_t *group, player_t *player);
extern player_t *player_find_by_name(player_group_t *group, char *name);
extern unsigned char player_group_add_player(player_group_t *group, player_t *player);
extern void player_group_delete_player(player_group_t *group, player_t *player);
extern void player_group_delete_player_by_name(player_group_t *group, char *name);
extern gint player_find_in_clist(GtkWidget *clist, char *name);
extern void player_add_to_clist(player_t *player, GtkWidget *list);
extern void player_group_make_clist(GtkWidget *list, player_group_t *group);
extern player_group_t *player_group_get_current(void);
extern void player_group_edit_dialog(void);

#endif	/* _PLAYERS_H_ */
