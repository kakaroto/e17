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
#include <math.h>
#include <gtk/gtk.h>

#include "debug.h"
#include "conf.h"
#include "dest.h"
#include "notgame.h"
#include "play.h"
#include "players.h"
#include "pregame.h"

static gint close_cb(void);
static gint click_cb(GtkWidget *button);
static void error_dialog(const char *msg);

static GtkWidget **dest_buttons, *game_win = NULL, *label, *current_player, *quit_button, *quit_label;
static unsigned char dest_cnt, dest_total;
static GList *player_current;
static player_t *player;
static player_group_t *player_group;

static void
error_dialog(const char *msg) {

  char message[255];
  GtkWidget *align, *big_vbox;

  game_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(game_win), "Can't Play! =(");
  gtk_window_set_transient_for(GTK_WINDOW(game_win), GTK_WINDOW(pregame_win));
  gtk_window_set_position(GTK_WINDOW(game_win), GTK_WIN_POS_MOUSE);
  gtk_signal_connect(GTK_OBJECT(game_win), "destroy", GTK_SIGNAL_FUNC(close_cb), NULL);
  gtk_signal_connect(GTK_OBJECT(game_win), "delete_event", GTK_SIGNAL_FUNC(close_cb), NULL);
  gtk_container_set_border_width(GTK_CONTAINER(game_win), 5);

  /* This is the vbox into which all window elements are packed */
  big_vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(game_win), big_vbox);

  sprintf(message, "Error:  %s", msg);
  label = gtk_label_new(message);
  gtk_misc_set_alignment(GTK_MISC(label), 1.0, 0.5);
  gtk_box_pack_start(GTK_BOX(big_vbox), label, TRUE, FALSE, 20);
  gtk_widget_show(label);

  quit_button = gtk_button_new_with_label("Oops!");
  align = gtk_alignment_new(0.5, 0.5, 0.1, 0.0);
  gtk_container_add(GTK_CONTAINER(align), quit_button);
  gtk_widget_show(quit_button);
  gtk_box_pack_start(GTK_BOX(big_vbox), align, TRUE, FALSE, 0);
  gtk_widget_show(align);
  gtk_signal_connect(GTK_OBJECT(quit_button), "clicked", GTK_SIGNAL_FUNC(close_cb), NULL);

  gtk_widget_show(big_vbox);
  gtk_widget_show(game_win);

}

static int
computer_player(void) {

  static unsigned char first_time = 1;
  unsigned char i;

  if (first_time) {
    srand(time(NULL) + getpid());
    first_time = 0;
  }
  do {
    i = (unsigned char) (((float) dest_total) * rand() / (RAND_MAX + 1.0));
  } while (!GTK_WIDGET_SENSITIVE(dest_buttons[i]));
  sleep(1);
  click_cb(dest_buttons[i]);
  return (0);
}

void
play_game(void) {

  unsigned short rows, cols, r, c;
  GtkWidget *big_vbox, *dest_frame, *dest_table, *game_frame, *game_table, *align;
  GList *dest_current;
  dest_group_t *dest_group;

  /* Create game window */
  if (game_win != NULL) {
    return;
  }

  if ((player_group = player_group_get_current()) == NULL) {
    error_dialog("No player groups created");
    return;
  }
  if ((player_current = player_group->members) == NULL) {
    error_dialog("No players in group");
    return;
  }
  player = ((player_t *) (player_current->data));

  game_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(game_win), "Game in Progress");
  gtk_window_set_transient_for(GTK_WINDOW(game_win), GTK_WINDOW(pregame_win));
  gtk_window_set_position(GTK_WINDOW(game_win), GTK_WIN_POS_MOUSE);
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

  if ((dest_group = dest_group_get_current()) == NULL) {
    error_dialog("No destination groups created");
    return;
  }
  if ((dest_current = dest_group->members) == NULL) {
    error_dialog("No destinations in group");
    return;
  }
  dest_total = (unsigned char) g_list_length(dest_current);
  rows = (unsigned short) (sqrt(dest_total));
  cols = rows + (((rows * rows) < dest_total) ? 1 : 0);
  if ((rows * cols) < dest_total) rows++;
  D(("There are %lu destinations, %lu rows, and %lu columns.\n", dest_total, rows, cols));
  dest_table = gtk_table_new(rows, cols, FALSE);
  gtk_table_set_row_spacings(GTK_TABLE(dest_table), 3);
  gtk_table_set_col_spacings(GTK_TABLE(dest_table), 3);
  gtk_container_set_border_width(GTK_CONTAINER(dest_table), 5);
  gtk_container_add(GTK_CONTAINER(dest_frame), dest_table);

  dest_buttons = (GtkWidget **) malloc(sizeof(GtkWidget *) * dest_total);
  for (r=0, c=0, dest_cnt = 0; (dest_current != NULL); dest_current = g_list_next(dest_current), dest_cnt++) {
    char *name;

    name = ((player_t *) (dest_current->data))->name;
    dest_buttons[dest_cnt] = gtk_button_new_with_label(name);
    gtk_signal_connect(GTK_OBJECT(dest_buttons[dest_cnt]), "clicked", GTK_SIGNAL_FUNC(click_cb), NULL);
    gtk_object_set_data(GTK_OBJECT(dest_buttons[dest_cnt]), "label", (void *) name);
    gtk_table_attach_defaults(GTK_TABLE(dest_table), GTK_WIDGET(dest_buttons[dest_cnt]), c, c+1, r, r+1);
    gtk_widget_show(dest_buttons[dest_cnt]);
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
  current_player = gtk_label_new(player->name);
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

  if (!strcasecmp(player->type, "ai") || !strcasecmp(player->type, "computer")) {
    gtk_timeout_add(COMP_DELAY, (GtkFunction) computer_player, NULL);
  }
}

static gint
close_cb(void) {
  gtk_widget_destroy(game_win);
  game_win = NULL;
  return ((gint) 1);
}

static gint
click_cb(GtkWidget *button) {

  unsigned short i;
  char *label_text;

  D(("click_cb():  button == 0x%08x\n", (int) button));

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
    } else {
      if ((player_current = g_list_next(player_current)) == NULL) {
        player_current = player_group->members;
      }
      player = ((player_t *) (player_current->data));
      gtk_label_set_text(GTK_LABEL(current_player), player->name);
      if (!strcasecmp(player->type, "ai") || !strcasecmp(player->type, "computer")) {
        gtk_timeout_add(COMP_DELAY, (GtkFunction) computer_player, NULL);
      }
    }
  }
  return ((gint) 1);
}

