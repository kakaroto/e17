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
#include "play.h"
#include "pregame.h"

GList *dest_group_list = NULL;

void
dest_group_add(GtkWidget *w, gpointer pbox) {

  char *name;
  GtkWidget *dest_groups;
  GList *entry;

  dest_groups = (GtkWidget *) pbox;
  name = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(dest_groups)->entry));
  REQUIRE(name != NULL);
  D(("name == %s (0x%08x)\n", name, name));

  entry = g_list_find_custom(dest_group_list, name, (GCompareFunc) strcasecmp);
  D(("entry == 0x%08x\n", entry));

  if (!entry) {
    dest_group_list = g_list_append(dest_group_list, strdup(name));
    gtk_combo_set_popdown_strings(GTK_COMBO(dest_groups), dest_group_list);
  }
  
}

void
dest_list_update(GList *group) {

}
