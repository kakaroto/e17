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

#include "conf.h"
#include "notgame.h"
#include "play.h"
#include "pregame.h"

static gint about_cb(GtkWidget *widget, GdkEvent *event);
static gint help_cb(GtkWidget *widget, GdkEvent *event);
static GtkWidget *about = NULL, *help = NULL;

void
about_box_display(void) {

  GtkWidget *button, *label, *vbox;

  if (about != ((GtkWidget *) NULL)) {
    return;
  }

  about = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width(GTK_CONTAINER(about), 10);
  gtk_window_set_title(GTK_WINDOW(about), "About " PACKAGE " " VERSION);
  gtk_signal_connect(GTK_OBJECT(about), "delete_event", GTK_SIGNAL_FUNC(about_cb), NULL);
  gtk_signal_connect(GTK_OBJECT(about), "destroy", GTK_SIGNAL_FUNC(about_cb), NULL);
  gtk_window_set_transient_for(GTK_WINDOW(about), GTK_WINDOW(pregame_win));
  gtk_window_set_position(GTK_WINDOW(about), GTK_WIN_POS_CENTER);

  vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(about), vbox);

  label = gtk_label_new(PACKAGE " " VERSION " -- The Not Game for Gtk+\n"
                        "by Michael Jennings\n"
                        "Copyright 1999, Michael Jennings\n\n"
                        "Original concept and Java implementation by Rob Coffey.  Concept\n"
                        "and name used with permission.\n\n"
                        "This program is free software and is distributed under the terms of\n"
                        "the Artistic License.  Please see the \"Artistic\" file supplied with\n"
                        "this program for license terms.");
  gtk_label_set_line_wrap(GTK_LABEL(label), FALSE);
  gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 10);

  button = gtk_button_new_with_label("OK");
  gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(about_cb), NULL);
  gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 10);

  gtk_widget_show(label);
  gtk_widget_show(button);
  gtk_widget_show(vbox);
  gtk_widget_show(about);

}

static gint
about_cb(GtkWidget *widget, GdkEvent *event) {

  if (about == NULL) {
    return FALSE;
  }
  gtk_widget_destroy(about);
  about = NULL;
  return (TRUE);
}

void
help_display(void) {

  GtkWidget *button, *label, *vbox;

  if (help != ((GtkWidget *) NULL)) {
    return;
  }

  help = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width(GTK_CONTAINER(help), 10);
  gtk_window_set_title(GTK_WINDOW(help), "How to Play the Not Game");
  gtk_signal_connect(GTK_OBJECT(help), "delete_event", GTK_SIGNAL_FUNC(help_cb), NULL);
  gtk_signal_connect(GTK_OBJECT(help), "destroy", GTK_SIGNAL_FUNC(help_cb), NULL);
  gtk_window_set_transient_for(GTK_WINDOW(help), GTK_WINDOW(pregame_win));
  gtk_window_set_position(GTK_WINDOW(help), GTK_WIN_POS_CENTER);

  vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(help), vbox);

  label = gtk_label_new("Instructions");
  gtk_label_set_line_wrap(GTK_LABEL(label), FALSE);
  gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 10);

  button = gtk_button_new_with_label("OK");
  gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(help_cb), NULL);
  gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 10);

  gtk_widget_show(label);
  gtk_widget_show(button);
  gtk_widget_show(vbox);
  gtk_widget_show(help);

}

static gint
help_cb(GtkWidget *widget, GdkEvent *event) {

  if (help == NULL) {
    return FALSE;
  }
  gtk_widget_destroy(help);
  help = NULL;
  return (TRUE);
}
