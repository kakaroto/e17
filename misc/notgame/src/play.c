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

static gint close_cb(void);
static gint click_cb(GtkWidget *button, gpointer data);

static GtkWidget **dest_buttons, *game_win, *current_player;
static char *dests[] = { "Burger King", "Denny's", "Gumba's", "Hobee's", "Java Street Cafe", "Kal's BBQ", "Mandarin", "McDonalds", "Sneha" };

void
play_game(void) {

  unsigned short i;
  GtkWidget *vbox, *hbox, *label, *button;

  /* Create game window */
  game_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(game_win), PACKAGE " " VERSION " -- Game Play");
  gtk_signal_connect(GTK_OBJECT(game_win), "destroy", GTK_SIGNAL_FUNC(close_cb), NULL);
  gtk_signal_connect(GTK_OBJECT(game_win), "delete_event", GTK_SIGNAL_FUNC(close_cb), NULL);
  gtk_container_set_border_width(GTK_CONTAINER(game_win), 5);

  /* This is the vbox into which all window elements are packed */
  vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(game_win), vbox);

  dest_buttons = (GtkWidget **) malloc(sizeof(GtkWidget *) * 9);
  for (i=0; i < 9; i++) {
    dest_buttons[i] = gtk_button_new_with_label(dests[i]);
    gtk_signal_connect(GTK_OBJECT(dest_buttons[i]), "clicked", GTK_SIGNAL_FUNC(click_cb), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), dest_buttons[i], FALSE, FALSE, 3);
    gtk_widget_show(dest_buttons[i]);
  }

  hbox = gtk_hbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(hbox), 10);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 3);
  label = gtk_label_new("Current Player:  ");
  gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 3);
  gtk_widget_show(label);
  current_player = gtk_label_new("Moo");
  gtk_box_pack_start(GTK_BOX(hbox), current_player, FALSE, FALSE, 3);
  gtk_widget_show(current_player);
  gtk_widget_show(hbox);

  button = gtk_button_new_with_label("Quit Game");
  gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(close_cb), NULL);
  gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 3);
  gtk_widget_show(button);

  gtk_widget_show(vbox);
  gtk_widget_show(game_win);

}

static gint
close_cb(void) {
  gtk_widget_destroy(game_win);
  return ((gint) 1);
}

static gint
click_cb(GtkWidget *button, gpointer data) {

  if (GTK_IS_BUTTON(button)) {
    gtk_widget_set_sensitive(button, FALSE);
  }
  return ((gint) 1);
}

