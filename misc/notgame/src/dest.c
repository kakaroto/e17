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

#include "debug.h"
#include "conf.h"
#include "dest.h"
#include "notgame.h"
#include "play.h"
#include "pregame.h"

GList *dest_groups = NULL, *dest_group_names = NULL;
GtkWidget *dest_clist, *dest_groups_box;
static GtkWidget *edit_win, *edit_clist, *edit_name_entry, *add_name_entry;
static dest_group_t *edit_dest_group;
static gint current_row;

static gint close_cb(void);
static void add_cb(void);
static void delete_cb(void);
static void update_name_cb(void);
static void select_row_cb(GtkWidget *w, gint row, gint col);

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
  dest_group_update_list(entry, dest_clist);
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
  dest_group_update_list(entry, dest_clist);
}

void
dest_group_update_list(dest_group_t *entry, GtkWidget *clist) {

  REQUIRE(entry != NULL);
  REQUIRE(clist != NULL);
  D(("entry == 0x%08x\n", entry));

  gtk_clist_freeze(GTK_CLIST(clist));
  gtk_clist_clear(GTK_CLIST(clist));
  dest_group_make_clist(clist, entry);
  gtk_clist_thaw(GTK_CLIST(clist));
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

dest_t *
dest_find(dest_group_t *group, dest_t *dest) {

  GList *entry;

  ASSERT(group != NULL);
  ASSERT(dest != NULL);

  entry = g_list_find_custom(group->members, dest, (GCompareFunc) dest_cmp);
  if (entry) {
    return ((dest_t *) (entry->data));
  } else {
    return (NULL);
  }
}

dest_t *
dest_find_by_name(dest_group_t *group, char *name) {

  GList *entry;

  ASSERT(group != NULL);
  ASSERT(name != NULL);

  entry = g_list_find_custom(group->members, name, (GCompareFunc) dest_cmp_name);
  if (entry) {
    return ((dest_t *) (entry->data));
  } else {
    return (NULL);
  }
}

unsigned char
dest_group_add_dest(dest_group_t *group, dest_t *dest) {

  ASSERT(group != NULL);
  ASSERT(dest != NULL);

  if (dest_find(group, dest) != NULL) {
    print_warning("Duplicate dest \"%s\"", dest->name);
    return (0);
  }
  group->members = g_list_insert_sorted(group->members, (gpointer) dest, (GCompareFunc) dest_cmp);
  return (1);
}

void
dest_group_delete_dest(dest_group_t *group, dest_t *dest) {

  ASSERT(group != NULL);
  ASSERT(dest != NULL);

  group->members = g_list_remove(group->members, (gpointer) dest);
}

void
dest_group_delete_dest_by_name(dest_group_t *group, char *name) {

  dest_t *dest;

  ASSERT(group != NULL);
  ASSERT(name != NULL);

  if ((dest = dest_find_by_name(group, name)) != NULL) {
    group->members = g_list_remove(group->members, (gpointer) dest);
  }
}

gint
dest_find_in_clist(GtkWidget *clist, char *name) {

  register gint i;
  char *clist_text;

  ASSERT(clist != NULL);
  ASSERT(name != NULL);

  for (i = 0; ; i++) {
    if ((gtk_clist_get_text(GTK_CLIST(clist), i, 0, (gchar **) (&clist_text))) == 0) {
      /* We've gone too far. */
      return -1;
    }
    if (!strcasecmp(name, clist_text)) {
      return i;
    }
  }
  return -1;
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

  name = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(dest_groups_box)->entry));
  REQUIRE_RVAL(name != NULL, NULL);
  D(("name == %s (0x%08x)\n", name, name));

  return (dest_group_find_by_name(name));
}

void
dest_group_edit_dialog(void) {

  GtkWidget *edit_vbox, *edit_frame, *edit_table;
  GtkWidget *buttonbox, *button, *align, *scroller, *label;
  char *name, *label_text, *clist_text;
  const char *cols[] = { "Destination" }, label_prefix[] = "Editing Destination Group:  ";

  if (edit_win != NULL) {
    return;
  }
  edit_dest_group = dest_group_get_current();
  REQUIRE(edit_dest_group != NULL);
  name = edit_dest_group->name;

  /* Add the frame around the dest section.  This will contain everything we create in this function */
  edit_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(edit_win), "Edit Destination Group");
  gtk_window_set_transient_for(GTK_WINDOW(edit_win), GTK_WINDOW(pregame_win));
  gtk_window_set_position(GTK_WINDOW(edit_win), GTK_WIN_POS_MOUSE);
  gtk_signal_connect(GTK_OBJECT(edit_win), "destroy", GTK_SIGNAL_FUNC(close_cb), NULL);
  gtk_signal_connect(GTK_OBJECT(edit_win), "delete_event", GTK_SIGNAL_FUNC(close_cb), NULL);
  gtk_container_set_border_width(GTK_CONTAINER(edit_win), 1);

  edit_vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(edit_win), edit_vbox);

  edit_frame = gtk_frame_new(NULL);
  gtk_container_set_border_width(GTK_CONTAINER(edit_frame), 5);
  gtk_frame_set_label(GTK_FRAME(edit_frame), "Modify Group Members");
  gtk_frame_set_label_align(GTK_FRAME(edit_frame), 0.0, 0.0);
  gtk_frame_set_shadow_type(GTK_FRAME(edit_frame), GTK_SHADOW_ETCHED_IN);
  gtk_box_pack_start(GTK_BOX(edit_vbox), edit_frame, TRUE, TRUE, 0);

  edit_table = gtk_table_new(5, 2, FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(edit_table), 5);
  gtk_container_add(GTK_CONTAINER(edit_frame), edit_table);

  /* The label for the groups combo box, right justified */
  label_text = (char *) malloc(strlen(name) + sizeof(label_prefix) + 1);
  strcpy(label_text, label_prefix);
  strcat(label_text, name);
  label = gtk_label_new(label_text);
  free(label_text);
  align = gtk_alignment_new(0.5, 0.5, 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(edit_table), GTK_WIDGET(align), 0, 2, 0, 1, (GTK_FILL | GTK_SHRINK), 0, 10, 5);
  gtk_container_add(GTK_CONTAINER(align), label);
  gtk_widget_show(align);
  gtk_widget_show(label);

  /* The clist for the dests in the current group */
  scroller = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scroller), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_table_attach_defaults(GTK_TABLE(edit_table), GTK_WIDGET(scroller), 0, 2, 1, 2);
  edit_clist = gtk_clist_new_with_titles(1, (gchar **) cols);
  gtk_widget_set_usize(edit_clist, 1, 80);
  gtk_container_add(GTK_CONTAINER(scroller), edit_clist);
  gtk_clist_column_titles_passive(GTK_CLIST(edit_clist));
  gtk_clist_set_column_justification(GTK_CLIST(edit_clist), 1, GTK_JUSTIFY_LEFT);
  gtk_clist_set_column_justification(GTK_CLIST(edit_clist), 2, GTK_JUSTIFY_LEFT);

  dest_group_make_clist(edit_clist, edit_dest_group);
  if (GTK_CLIST(edit_clist)->rows) {
    gtk_clist_select_row(GTK_CLIST(edit_clist), 0, 0);
  }
  gtk_signal_connect(GTK_OBJECT(edit_clist), "select_row", GTK_SIGNAL_FUNC(select_row_cb), NULL);
  gtk_widget_show(edit_clist);
  gtk_widget_show(scroller);

  label = gtk_label_new("Edit Destination:");
  align = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(edit_table), GTK_WIDGET(align), 0, 1, 2, 3, (GTK_FILL | GTK_SHRINK), 0, 5, 10);
  gtk_container_add(GTK_CONTAINER(align), label);
  gtk_widget_show(align);
  gtk_widget_show(label);

  edit_name_entry = gtk_entry_new();
  if (edit_dest_group->members) {
    gtk_clist_get_text(GTK_CLIST(edit_clist), 0, 0, (gchar **) (&clist_text));
    gtk_entry_set_text(GTK_ENTRY(edit_name_entry), clist_text);
  } else {
    gtk_entry_set_text(GTK_ENTRY(edit_name_entry), "");
  }
  gtk_signal_connect(GTK_OBJECT(edit_name_entry), "activate", GTK_SIGNAL_FUNC(update_name_cb), (gpointer) edit_name_entry);
  gtk_table_attach(GTK_TABLE(edit_table), GTK_WIDGET(edit_name_entry), 1, 2, 2, 3, (GTK_FILL | GTK_SHRINK | GTK_EXPAND), 0, 0, 0);
  gtk_widget_show(edit_name_entry);

  buttonbox = gtk_hbutton_box_new();
  gtk_container_set_border_width(GTK_CONTAINER(buttonbox), 5);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(buttonbox), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing(GTK_BUTTON_BOX(buttonbox), 3);
  button = gtk_button_new_with_label("Update");
  gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(update_name_cb), (gpointer) edit_name_entry);
  gtk_container_add(GTK_CONTAINER(buttonbox), button);
  gtk_widget_show(button);
  button = gtk_button_new_with_label("Delete");
  gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(delete_cb), (gpointer) edit_clist);
  gtk_container_add(GTK_CONTAINER(buttonbox), button);
  gtk_widget_show(button);
  gtk_table_attach(GTK_TABLE(edit_table), GTK_WIDGET(buttonbox), 0, 2, 4, 5, (GTK_FILL | GTK_SHRINK), 0, 0, 2);
  gtk_widget_show(buttonbox);

  gtk_widget_show(edit_table);
  gtk_widget_show(edit_frame);

  edit_frame = gtk_frame_new(NULL);
  gtk_container_set_border_width(GTK_CONTAINER(edit_frame), 5);
  gtk_frame_set_label(GTK_FRAME(edit_frame), "Add New Group Members");
  gtk_frame_set_label_align(GTK_FRAME(edit_frame), 0.0, 0.0);
  gtk_frame_set_shadow_type(GTK_FRAME(edit_frame), GTK_SHADOW_ETCHED_IN);
  gtk_box_pack_start(GTK_BOX(edit_vbox), edit_frame, FALSE, FALSE, 0);

  edit_table = gtk_table_new(3, 2, FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(edit_table), 5);
  gtk_container_add(GTK_CONTAINER(edit_frame), edit_table);

  label = gtk_label_new("New Destination:");
  align = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(edit_table), GTK_WIDGET(align), 0, 1, 0, 1, (GTK_FILL | GTK_SHRINK), 0, 5, 10);
  gtk_container_add(GTK_CONTAINER(align), label);
  gtk_widget_show(align);
  gtk_widget_show(label);

  add_name_entry = gtk_entry_new();
  gtk_signal_connect(GTK_OBJECT(add_name_entry), "activate", GTK_SIGNAL_FUNC(add_cb), (gpointer) add_name_entry);
  gtk_table_attach(GTK_TABLE(edit_table), GTK_WIDGET(add_name_entry), 1, 2, 0, 1, (GTK_FILL | GTK_SHRINK | GTK_EXPAND), 0, 0, 0);
  gtk_widget_show(add_name_entry);

  buttonbox = gtk_hbutton_box_new();
  gtk_container_set_border_width(GTK_CONTAINER(buttonbox), 5);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(buttonbox), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing(GTK_BUTTON_BOX(buttonbox), 3);
  button = gtk_button_new_with_label("Add");
  gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(add_cb), (gpointer) NULL);
  gtk_container_add(GTK_CONTAINER(buttonbox), button);
  gtk_widget_show(button);
  gtk_table_attach(GTK_TABLE(edit_table), GTK_WIDGET(buttonbox), 0, 2, 2, 3, (GTK_FILL | GTK_SHRINK), 0, 0, 0);
  gtk_widget_show(buttonbox);

  gtk_widget_show(edit_table);
  gtk_widget_show(edit_frame);

  button = gtk_button_new_with_label("Done");
  align = gtk_alignment_new(0.5, 0.5, 0.1, 0.0);
  gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(close_cb), (gpointer) NULL);
  gtk_container_add(GTK_CONTAINER(align), button);
  gtk_box_pack_start(GTK_BOX(edit_vbox), align, FALSE, FALSE, 5);
  gtk_widget_show(button);
  gtk_widget_show(align);

  gtk_widget_show(edit_vbox);
  gtk_widget_show(edit_win);
}

static gint
close_cb(void) {
  gtk_widget_destroy(edit_win);
  edit_win = NULL;
  return ((gint) 1);
}

static void
add_cb(void) {

  char *name;
  dest_t *new;
  dest_group_t *current;

  current = dest_group_get_current();
  name = (char *) gtk_entry_get_text(GTK_ENTRY(add_name_entry));
  REQUIRE(name && strlen(name) > 0);
  new = (dest_t *) malloc(sizeof(dest_t));
  new->name = strdup(name);
  if (!dest_group_add_dest(current, new)) {
    free(new->name);
    free(new);
  }
  gtk_entry_set_text(GTK_ENTRY(add_name_entry), "");
  dest_group_update_list(current, dest_clist);
  dest_group_update_list(current, edit_clist);

  current_row = dest_find_in_clist(edit_clist, name);
  gtk_clist_select_row(GTK_CLIST(edit_clist), current_row, 0);
  gtk_entry_set_text(GTK_ENTRY(edit_name_entry), name);
}

static void
delete_cb(void) {

  char *clist_text, *name;
  dest_group_t *current;
  dest_t *dest;

  REQUIRE(current_row >= 0);
  current = dest_group_get_current();
  gtk_clist_get_text(GTK_CLIST(edit_clist), current_row, 0, (gchar **) (&name));
  REQUIRE(name && strlen(name) > 0);
  dest = dest_find_by_name(current, name);
  REQUIRE(dest != NULL);
  dest_group_delete_dest(current, dest);

  dest_group_update_list(current, dest_clist);
  dest_group_update_list(current, edit_clist);

  for (; current_row >= GTK_CLIST(edit_clist)->rows; current_row--);
  if (current_row >= 0) {
    gtk_clist_select_row(GTK_CLIST(edit_clist), current_row, 0);
    gtk_clist_get_text(GTK_CLIST(edit_clist), current_row, 0, (gchar **) (&clist_text));
    gtk_entry_set_text(GTK_ENTRY(edit_name_entry), clist_text);
  } else {
    gtk_entry_set_text(GTK_ENTRY(edit_name_entry), "");
  }
}

static void
update_name_cb(void) {

  char *clist_text, *old_name;
  dest_group_t *current;
  dest_t *dest;

  REQUIRE(current_row >= 0);
  clist_text = gtk_entry_get_text(GTK_ENTRY(edit_name_entry));
  REQUIRE(clist_text && strlen(clist_text) > 0);

  current = dest_group_get_current();
  gtk_clist_get_text(GTK_CLIST(edit_clist), current_row, 0, (gchar **) (&old_name));
  REQUIRE(old_name && strlen(old_name) > 0);
  dest = dest_find_by_name(current, old_name);
  REQUIRE(dest != NULL);
  dest_group_delete_dest(current, dest);
  dest->name = strdup(clist_text);
  dest_group_add_dest(current, dest);

  dest_group_update_list(current, dest_clist);
  dest_group_update_list(current, edit_clist);

  current_row = dest_find_in_clist(edit_clist, clist_text);
  gtk_clist_select_row(GTK_CLIST(edit_clist), current_row, 0);
  gtk_clist_get_text(GTK_CLIST(edit_clist), current_row, 0, (gchar **) (&clist_text));
  gtk_entry_set_text(GTK_ENTRY(edit_name_entry), clist_text);
}

static void
select_row_cb(GtkWidget *w, gint row, gint col) {

  char *clist_text;

  current_row = row;
  gtk_clist_get_text(GTK_CLIST(edit_clist), current_row, 0, (gchar **) (&clist_text));
  gtk_entry_set_text(GTK_ENTRY(edit_name_entry), clist_text);

}
