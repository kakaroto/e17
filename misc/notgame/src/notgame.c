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

#include "notgame.h"

static gint about_cb(GtkWidget *widget, GdkEvent *event);
static GtkWidget *about = NULL;

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
  /*gtk_window_set_transient_for(GTK_WINDOW(about), GTK_WINDOW(desktop->handle));*/
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
ng_init(void) {

}

int
main(int argc, char **argv) {

  signal(SIGINT, fatal_handler);
  signal(SIGPIPE, fatal_handler);
  gtk_init(&argc, &argv);
  gdk_init(&argc, &argv);
  gdk_imlib_init();
  ng_init();
  gtk_main();
  exit(0);
}

void
clean_exit(const char *msg, ...) {

  va_list args;

  if (msg != NULL) {
    va_start(args, msg);
    fprintf(stderr, PACKAGE ":  ");
    vfprintf(stderr, msg, args);
    va_end(args);
    fflush(stderr);
  }
  gtk_exit(0);
}

void fatal_handler(int sig) {

  clean_exit("Got fatal signal %d\n", sig);
}
