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
#include "dest.h"
#include "notgame.h"
#include "play.h"
#include "pregame.h"

GList *dest_groups = NULL, *dest_group_names = NULL;
GtkWidget *dest_clist, *dest_groups_box;

char
dest_group_cmp(const dest_group_t *g1, const dest_group_t *g2) {
  return (strcasecmp(g1->name, g2->name));
}

char
dest_group_cmp_name(const dest_group_t *group, const char *name) {
  return (strcasecmp(group->name, name));
}

/* Create new group from the program */
unsigned char
dest_group_add(dest_group_t *group) {

  if (g_list_find_custom(dest_group_names, (gpointer) group->name, (GCompareFunc) strcasecmp) != NULL) {
    print_warning("Duplicate dest group \"%s\"", group->name);
    return (0);
  }
  dest_groups = g_list_insert_sorted(dest_groups, (gpointer) group, (GCompareFunc) dest_group_cmp);
  dest_group_names = g_list_insert_sorted(dest_group_names, (gpointer) group->name, (GCompareFunc) strcasecmp);
  return (1);
}

/* Creating from the GUI */
void
dest_group_add_from_gui(void) {

  char *name;
  dest_group_t *entry, *group;

  name = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(dest_groups_box)->entry));
  REQUIRE(name != NULL);
  D(("name == %s (0x%08x)\n", name, name));

  entry = dest_group_find_by_name(name);
  D(("entry == 0x%08x\n", entry));

  if (entry) {
    print_warning("Duplicate dest group \"%s\"", name);
    return;
  }    
  group = (dest_group_t *) malloc(sizeof(dest_group_t));
  group->name = strdup(name);
  group->members = NULL;
  dest_groups = g_list_insert_sorted(dest_groups, (gpointer) group, (GCompareFunc) dest_group_cmp);
  dest_group_names = g_list_insert_sorted(dest_group_names, (gpointer) group->name, (GCompareFunc) strcasecmp);
  gtk_combo_set_popdown_strings(GTK_COMBO(dest_groups_box), dest_group_names);
  dest_group_update_lists(entry);
}

/* Updating from the GUI */
void
dest_group_update_lists_from_gui(void) {

  char *name;
  dest_group_t *entry;

  name = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(dest_groups_box)->entry));
  REQUIRE(name != NULL);
  D(("name == %s (0x%08x)\n", name, name));

  entry = dest_group_find_by_name(name);
  dest_group_update_lists(entry);
}

void
dest_group_update_lists(dest_group_t *entry) {

  REQUIRE(entry != NULL);
  D(("entry == 0x%08x\n", entry));

  gtk_clist_freeze(GTK_CLIST(dest_clist));
  gtk_clist_clear(GTK_CLIST(dest_clist));
  dest_group_make_clist(dest_clist, entry);
  gtk_clist_thaw(GTK_CLIST(dest_clist));
}

dest_group_t *
dest_group_find_by_name(char *name) {

  GList *entry;

  entry = g_list_find_custom(dest_groups, name, (GCompareFunc) dest_group_cmp_name);
  if (entry) {
    return (entry->data);
  } else {
    return (NULL);
  }
}

char
dest_cmp(const dest_t *p1, const dest_t *p2) {
  return (strcasecmp(p1->name, p2->name));
}

char
dest_cmp_name(const dest_t *p1, const char *name) {
  return (strcasecmp(p1->name, name));
}

unsigned char
dest_group_add_dest(dest_group_t *group, dest_t *dest) {

  ASSERT(group != NULL);
  ASSERT(dest != NULL);

  if (g_list_find_custom(group->members, (gpointer) dest, (GCompareFunc) dest_cmp) != NULL) {
    print_warning("Duplicate dest \"%s\"", dest->name);
    return (0);
  }
  group->members = g_list_insert_sorted(group->members, (gpointer) dest, (GCompareFunc) dest_cmp);
  return (1);
}

void
dest_add_to_clist(dest_t *dest, GtkWidget *list) {
  gtk_clist_append(GTK_CLIST(list), (gchar **) dest);
}

void
dest_group_make_clist(GtkWidget *list, dest_group_t *group) {

  ASSERT(group != NULL);
  ASSERT(list != NULL);

  g_list_foreach(group->members, (GFunc) dest_add_to_clist, (gpointer) list);
}

dest_group_t *
dest_group_get_current(void) {

  char *name;
  GList *entry;

  name = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(dest_groups_box)->entry));
  REQUIRE_RVAL(name != NULL, NULL);
  D(("name == %s (0x%08x)\n", name, name));

  return (dest_group_find_by_name(name));
}
