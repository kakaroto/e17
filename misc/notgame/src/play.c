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
#include "players.h"
#include "pregame.h"

static gint close_cb(void);
static gint click_cb(GtkWidget *button, gpointer data);

static GtkWidget **dest_buttons, *game_win = NULL, *label, *current_player, *quit_button, *quit_label;
static unsigned char dest_cnt, dest_total;

void
play_game(void) {

  unsigned short i, rows, cols, r, c;
  GtkWidget *big_vbox, *hbox, *dest_frame, *dest_table, *game_frame, *game_table, *align;
  GList *dest_current;
  dest_group_t *dest_group;

  /* Create game window */
  if (game_win != NULL) {
    return;
  }
  game_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(game_win), "Game in Progress");
  gtk_window_set_transient_for(GTK_WINDOW(game_win), GTK_WINDOW(pregame_win));
  gtk_window_set_position(GTK_WINDOW(game_win), GTK_WIN_POS_CENTER);
  gtk_signal_connect(GTK_OBJECT(game_win), "destroy", GTK_SIGNAL_FUNC(close_cb), NULL);
  gtk_signal_connect(GTK_OBJECT(game_win), "delete_event", GTK_SIGNAL_FUNC(close_cb), NULL);
  gtk_container_set_border_width(GTK_CONTAINER(game_win), 5);

  /* This is the vbox into which all window elements are packed */
  big_vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(game_win), big_vbox);

  dest_frame = gtk_frame_new(NULL);
  gtk_container_set_border_width(GTK_CONTAINER(dest_frame), 5);
  gtk_frame_set_label(GTK_FRAME(dest_frame), "Destinations");
  gtk_frame_set_label_align(GTK_FRAME(dest_frame), 0.0, 0.0);
  gtk_frame_set_shadow_type(GTK_FRAME(dest_frame), GTK_SHADOW_ETCHED_IN);
  gtk_box_pack_start(GTK_BOX(big_vbox), dest_frame, TRUE, FALSE, 0);

  dest_group = dest_group_get_current();
  dest_current = dest_group->members;
  dest_total = (unsigned char) g_list_length(dest_current);
  rows = dest_total / 3;
  if (rows == 0) rows = 1;
  cols = dest_total / 3 + ((dest_total % 3) ? 1 : 0);
  dest_table = gtk_table_new(rows, cols, FALSE);
  gtk_table_set_row_spacings(GTK_TABLE(dest_table), 3);
  gtk_table_set_col_spacings(GTK_TABLE(dest_table), 3);
  gtk_container_set_border_width(GTK_CONTAINER(dest_table), 5);
  gtk_container_add(GTK_CONTAINER(dest_frame), dest_table);

  dest_buttons = (GtkWidget **) malloc(sizeof(GtkWidget *) * dest_total);
  for (r=0, c=0, dest_cnt = 0; (dest_current != NULL); dest_current = g_list_next(dest_current), dest_cnt++) {
    GtkWidget *button;
    char *name;

    name = ((player_t *) (dest_current->data))->name;
    button = gtk_button_new_with_label(name);
    gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(click_cb), NULL);
    gtk_object_set_data(GTK_OBJECT(button), "label", (void *) name);
    gtk_table_attach_defaults(GTK_TABLE(dest_table), GTK_WIDGET(button), c, c+1, r, r+1);
    gtk_widget_show(button);
    if (++c == cols) {
      c = 0;
      if (++r == rows) {
        r = 0;
      }
    }
  }

  gtk_widget_show(dest_table);
  gtk_widget_show(dest_frame);

  game_frame = gtk_frame_new(NULL);
  gtk_container_set_border_width(GTK_CONTAINER(game_frame), 5);
  gtk_frame_set_label(GTK_FRAME(game_frame), "Game Status");
  gtk_frame_set_label_align(GTK_FRAME(game_frame), 0.0, 0.0);
  gtk_frame_set_shadow_type(GTK_FRAME(game_frame), GTK_SHADOW_ETCHED_IN);
  gtk_box_pack_start(GTK_BOX(big_vbox), game_frame, TRUE, FALSE, 0);

  game_table = gtk_table_new(2, 2, FALSE);
  gtk_table_set_row_spacings(GTK_TABLE(game_table), 3);
  gtk_table_set_col_spacings(GTK_TABLE(game_table), 3);
  gtk_container_set_border_width(GTK_CONTAINER(game_table), 3);
  gtk_container_add(GTK_CONTAINER(game_frame), game_table);

  label = gtk_label_new("Current Player:  ");
  gtk_misc_set_alignment(GTK_MISC(label), 1.0, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(game_table), GTK_WIDGET(label), 0, 1, 0, 1);
  gtk_widget_show(label);
  current_player = gtk_label_new("Moo");
  gtk_misc_set_alignment(GTK_MISC(current_player), 0.0, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(game_table), GTK_WIDGET(current_player), 1, 2, 0, 1);
  gtk_widget_show(current_player);

  quit_label = gtk_label_new("Quit Game");
  quit_button = gtk_button_new();
  gtk_container_add(GTK_CONTAINER(quit_button), quit_label);
  gtk_widget_show(quit_label);
  align = gtk_alignment_new(0.5, 0.5, 0.1, 0.0);
  gtk_container_add(GTK_CONTAINER(align), quit_button);
  gtk_widget_show(quit_button);
  gtk_table_attach_defaults(GTK_TABLE(game_table), GTK_WIDGET(align), 0, 2, 1, 2);
  gtk_widget_show(align);
  gtk_signal_connect(GTK_OBJECT(quit_button), "clicked", GTK_SIGNAL_FUNC(close_cb), NULL);

  gtk_widget_show(game_table);
  gtk_widget_show(game_frame);

  gtk_widget_show(big_vbox);
  gtk_widget_show(game_win);

}

static gint
close_cb(void) {
  gtk_widget_destroy(game_win);
  return ((gint) 1);
}

static gint
click_cb(GtkWidget *button, gpointer data) {

  unsigned short i;
  char *label_text;

  if (GTK_IS_BUTTON(button)) {
    gtk_widget_set_sensitive(button, FALSE);
    if (--dest_cnt == 1) {
      for (i=0; i < dest_total; i++) {
        if (GTK_WIDGET_SENSITIVE(dest_buttons[i])) {
          label_text = (char *) gtk_object_get_data(GTK_OBJECT(dest_buttons[i]), "label");
          gtk_label_set_text(GTK_LABEL(label), "Winner:  ");
          gtk_label_set_text(GTK_LABEL(current_player), (label_text ? label_text : ""));
          gtk_signal_disconnect_by_func(GTK_OBJECT(dest_buttons[i]), GTK_SIGNAL_FUNC(click_cb), NULL);
          gtk_window_set_title(GTK_WINDOW(game_win), "Game Over");
          gtk_label_set_text(GTK_LABEL(quit_label), "Close");
          break;
        }
      }
    }
  }
  return ((gint) 1);
}

