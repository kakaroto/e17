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
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <gtk/gtk.h>
#include <gdk_imlib.h>

#include "debug.h"
#include "conf.h"
#include "notgame.h"
#include "players.h"

GList *player_groups = NULL, *player_group_names = NULL;
GtkWidget *player_clist, *player_groups_box;

char
player_group_cmp(const player_group_t *g1, const player_group_t *g2) {
  return (strcasecmp(g1->name, g2->name));
}

char
player_group_cmp_name(const player_group_t *group, const char *name) {
  return (strcasecmp(group->name, name));
}

/* Create new group from the program */
unsigned char
player_group_add(player_group_t *group) {

  if (g_list_find_custom(player_group_names, (gpointer) group->name, (GCompareFunc) strcasecmp) != NULL) {
    print_warning("Duplicate player group \"%s\"", group->name);
    return (0);
  }
  player_groups = g_list_insert_sorted(player_groups, (gpointer) group, (GCompareFunc) player_group_cmp);
  player_group_names = g_list_insert_sorted(player_group_names, (gpointer) group->name, (GCompareFunc) strcasecmp);
  return (1);
}

/* Creating from the GUI */
void
player_group_add_from_gui(void) {

  char *name;
  player_group_t *entry, *group;

  name = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(player_groups_box)->entry));
  REQUIRE(name != NULL);
  D(("name == %s (0x%08x)\n", name, name));

  entry = player_group_find_by_name(name);
  D(("entry == 0x%08x\n", entry));

  if (entry) {
    print_warning("Duplicate player group \"%s\"", name);
    return;
  }    
  group = (player_group_t *) malloc(sizeof(player_group_t));
  group->name = strdup(name);
  group->members = NULL;
  player_groups = g_list_insert_sorted(player_groups, (gpointer) group, (GCompareFunc) player_group_cmp);
  player_group_names = g_list_insert_sorted(player_group_names, (gpointer) group->name, (GCompareFunc) strcasecmp);
  gtk_combo_set_popdown_strings(GTK_COMBO(player_groups_box), player_group_names);
  player_group_update_lists(entry);
}

/* Updating from the GUI */
void
player_group_update_lists_from_gui(void) {

  char *name;
  player_group_t *entry;

  name = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(player_groups_box)->entry));
  REQUIRE(name != NULL);
  D(("name == %s (0x%08x)\n", name, name));

  entry = player_group_find_by_name(name);
  player_group_update_lists(entry);
}

void
player_group_update_lists(player_group_t *entry) {

  REQUIRE(entry != NULL);
  D(("entry == 0x%08x\n", entry));

  gtk_clist_freeze(GTK_CLIST(player_clist));
  gtk_clist_clear(GTK_CLIST(player_clist));
  player_group_make_clist(player_clist, entry);
  gtk_clist_thaw(GTK_CLIST(player_clist));
}

player_group_t *
player_group_find_by_name(char *name) {

  GList *entry;

  entry = g_list_find_custom(player_groups, name, (GCompareFunc) player_group_cmp_name);
  if (entry) {
    return (entry->data);
  } else {
    return (NULL);
  }
}

char
player_cmp(const player_t *p1, const player_t *p2) {
  return (strcasecmp(p1->name, p2->name));
}

char
player_cmp_name(const player_t *p1, const char *name) {
  return (strcasecmp(p1->name, name));
}

unsigned char
player_group_add_player(player_group_t *group, player_t *player) {

  ASSERT(group != NULL);
  ASSERT(player != NULL);

  if (g_list_find_custom(group->members, (gpointer) player, (GCompareFunc) player_cmp) != NULL) {
    print_warning("Duplicate player \"%s\"", player->name);
    return (0);
  }
  group->members = g_list_insert_sorted(group->members, (gpointer) player, (GCompareFunc) player_cmp);
  return (1);
}

void
player_add_to_clist(player_t *player, GtkWidget *list) {
  gtk_clist_append(GTK_CLIST(list), (gchar **) player);
}

void
player_group_make_clist(GtkWidget *list, player_group_t *group) {

  ASSERT(group != NULL);
  ASSERT(list != NULL);

  g_list_foreach(group->members, (GFunc) player_add_to_clist, (gpointer) list);
}

player_group_t *
player_group_get_current(void) {

  char *name;
  GList *entry;

  name = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(player_groups_box)->entry));
  REQUIRE_RVAL(name != NULL, NULL);
  D(("name == %s (0x%08x)\n", name, name));

  return (player_group_find_by_name(name));
}
