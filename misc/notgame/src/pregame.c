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
#include "help.h"
#include "notgame.h"
#include "parse.h"
#include "play.h"
#include "players.h"
#include "pregame.h"

static void pregame_menu_init(GtkWidget *vbox);
static void pregame_player_frame_init(GtkWidget *vbox);
static void pregame_dest_frame_init(GtkWidget *vbox);
static void button_cb(GtkWidget *w, gpointer item);
static void gamemenu_cb(gpointer item);
static void helpmenu_cb(gpointer item);

GtkWidget *pregame_win = NULL;

void
pregame_init(void) {

  GtkWidget *vbox, *buttonbox, *button;

  /* Create main pre-game window */
  pregame_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(pregame_win), PACKAGE " " VERSION " -- Pre-Game Setup");
  gtk_signal_connect(GTK_OBJECT(pregame_win), "destroy", GTK_SIGNAL_FUNC(clean_exit), NULL);
  gtk_signal_connect(GTK_OBJECT(pregame_win), "delete_event", GTK_SIGNAL_FUNC(clean_exit), NULL);
  gtk_container_set_border_width(GTK_CONTAINER(pregame_win), 1);

  /* This is the vbox into which all window elements are packed */
  vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(pregame_win), vbox);

  /* Create menubar and pack it into the vbox */
  pregame_menu_init(vbox);

  /* Create the player area and pack it */
  pregame_player_frame_init(vbox);

  /* Create the destination area and pack it */
  pregame_dest_frame_init(vbox);

  /* The buttons at the bottom of the pre-game screen */
  buttonbox = gtk_hbutton_box_new();
  gtk_container_set_border_width(GTK_CONTAINER(buttonbox), 5);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(buttonbox), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing(GTK_BUTTON_BOX(buttonbox), 3);
  button = gtk_button_new_with_label("Play Game");
  gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(button_cb), (gpointer) 1);
  gtk_container_add(GTK_CONTAINER(buttonbox), button);
  gtk_widget_show(button);
  button = gtk_button_new_with_label("Add/Delete Players");
  gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(button_cb), (gpointer) 2);
  gtk_container_add(GTK_CONTAINER(buttonbox), button);
  gtk_widget_show(button);
  button = gtk_button_new_with_label("Add/Delete Destinations");
  gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(button_cb), (gpointer) 3);
  gtk_container_add(GTK_CONTAINER(buttonbox), button);
  gtk_widget_show(button);
  gtk_box_pack_start(GTK_BOX(vbox), buttonbox, FALSE, FALSE, 0);

  /* Cleanup and finalize everything */
  gtk_widget_realize(pregame_win);
  gtk_widget_realize(vbox);
  gtk_widget_realize(buttonbox);
  gtk_widget_show(buttonbox);
  gtk_widget_show(vbox);
  gtk_window_set_policy(GTK_WINDOW(pregame_win), TRUE, TRUE, FALSE);
  gtk_widget_show(pregame_win);
}

static void
pregame_menu_init(GtkWidget *vbox) {

  GtkWidget *menubar, *menu, *menuitem;

  menubar = gtk_menu_bar_new();

  /* Game menu */
  menu = gtk_menu_new();
  menuitem = gtk_tearoff_menu_item_new();
  gtk_menu_append(GTK_MENU(menu), menuitem);
  gtk_widget_show(menuitem);
  menuitem = gtk_menu_item_new_with_label("Play Game");
  gtk_menu_append(GTK_MENU(menu), menuitem);
  gtk_signal_connect_object(GTK_OBJECT(menuitem), "activate", GTK_SIGNAL_FUNC(gamemenu_cb), (gpointer) 1);
  gtk_widget_show(menuitem);
  menuitem = gtk_menu_item_new_with_label("Save Configuration");
  gtk_menu_append(GTK_MENU(menu), menuitem);
  gtk_signal_connect_object(GTK_OBJECT(menuitem), "activate", GTK_SIGNAL_FUNC(gamemenu_cb), (gpointer) 2);
  gtk_widget_show(menuitem);
  menuitem = gtk_menu_item_new();
  gtk_menu_append(GTK_MENU(menu), menuitem);
  gtk_widget_show(menuitem);
  menuitem = gtk_menu_item_new_with_label("Exit");
  gtk_menu_append(GTK_MENU(menu), menuitem);
  gtk_signal_connect_object(GTK_OBJECT(menuitem), "activate", GTK_SIGNAL_FUNC(clean_exit), (gpointer) NULL);
  gtk_widget_show(menuitem);
  menuitem = gtk_menu_item_new_with_label("Game");
  gtk_widget_show(menuitem);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), menu);
  gtk_menu_bar_append(GTK_MENU_BAR(menubar), menuitem);

  /* Help menu */
  menu = gtk_menu_new();
  menuitem = gtk_tearoff_menu_item_new();
  gtk_menu_append(GTK_MENU(menu), menuitem);
  gtk_widget_show(menuitem);
  menuitem = gtk_menu_item_new_with_label("How to Play...");
  gtk_menu_append(GTK_MENU(menu), menuitem);
  gtk_signal_connect_object(GTK_OBJECT(menuitem), "activate", GTK_SIGNAL_FUNC(helpmenu_cb), (gpointer) 1);
  gtk_widget_show(menuitem);
  menuitem = gtk_menu_item_new_with_label("About the Not Game...");
  gtk_menu_append(GTK_MENU(menu), menuitem);
  gtk_signal_connect_object(GTK_OBJECT(menuitem), "activate", GTK_SIGNAL_FUNC(helpmenu_cb), (gpointer) 2);
  gtk_widget_show(menuitem);
  menuitem = gtk_menu_item_new_with_label("Help");
  gtk_widget_show(menuitem);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), menu);
  gtk_menu_bar_append(GTK_MENU_BAR(menubar), menuitem);

  gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
  gtk_widget_show(menubar);
}

static void
pregame_player_frame_init(GtkWidget *vbox) {

  GtkWidget *player_frame, *player_table, *align, *scroller;
  GtkWidget *label;
  const char *cols[] = { "Player Name", "Player Type" };

  /* Add the frame around the player section.  This will contain everything we create in this function */
  player_frame = gtk_frame_new(NULL);
  gtk_container_set_border_width(GTK_CONTAINER(player_frame), 5);
  gtk_frame_set_label(GTK_FRAME(player_frame), "Not Game Players");
  gtk_frame_set_label_align(GTK_FRAME(player_frame), 0.0, 0.0);
  gtk_frame_set_shadow_type(GTK_FRAME(player_frame), GTK_SHADOW_ETCHED_IN);
  gtk_box_pack_start(GTK_BOX(vbox), player_frame, TRUE, TRUE, 0);

  /* A 2-by-2 table will contain the two labels, the combo box, and the CList */
  player_table = gtk_table_new(2, 2, FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(player_table), 5);
  gtk_container_add(GTK_CONTAINER(player_frame), player_table);

  /* The label for the groups combo box, right justified */
  label = gtk_label_new("Group:");
  align = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(player_table), GTK_WIDGET(align), 0, 1, 0, 1, (GTK_FILL | GTK_SHRINK), 0, 5, 0);
  gtk_container_add(GTK_CONTAINER(align), label);
  gtk_widget_show(align);
  gtk_widget_show(label);

  /* The combo box containing the player groups */
  player_groups_box = gtk_combo_new();
  gtk_combo_set_use_arrows_always(GTK_COMBO(player_groups_box), TRUE);
  gtk_combo_set_case_sensitive(GTK_COMBO(player_groups_box), TRUE);

  if (player_group_names) {
    gtk_combo_set_popdown_strings(GTK_COMBO(player_groups_box), player_group_names);
  }
  gtk_signal_connect(GTK_OBJECT(GTK_COMBO(player_groups_box)->entry), "activate", GTK_SIGNAL_FUNC(player_group_add_from_gui), (gpointer) NULL);
  gtk_signal_connect(GTK_OBJECT(GTK_COMBO(player_groups_box)->list), "selection_changed", GTK_SIGNAL_FUNC(player_group_update_lists_from_gui), (gpointer) NULL);
  gtk_table_attach(GTK_TABLE(player_table), GTK_WIDGET(player_groups_box), 1, 2, 0, 1, (GTK_FILL | GTK_SHRINK), 0, 0, 0);
  gtk_widget_show(player_groups_box);

  /* The clist for the players in the current group */
  scroller = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scroller), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_table_attach_defaults(GTK_TABLE(player_table), GTK_WIDGET(scroller), 1, 2, 1, 2);
  player_clist = gtk_clist_new_with_titles(2, (gchar **) cols);
  gtk_widget_set_usize(player_clist, 1, 80);
  gtk_container_add(GTK_CONTAINER(scroller), player_clist);
  gtk_clist_column_titles_passive(GTK_CLIST(player_clist));
  gtk_clist_set_column_justification(GTK_CLIST(player_clist), 1, GTK_JUSTIFY_LEFT);
  gtk_clist_set_column_justification(GTK_CLIST(player_clist), 2, GTK_JUSTIFY_LEFT);

  if (player_groups) {
    player_group_make_clist(player_clist, player_groups->data);
  }
  gtk_widget_show(player_clist);
  gtk_widget_show(scroller);

  gtk_widget_show(player_table);
  gtk_widget_show(player_frame);

}

static void
pregame_dest_frame_init(GtkWidget *vbox) {

  GtkWidget *dest_frame, *dest_table, *align, *scroller;
  GtkWidget *label;
  const char *cols[] = { "Destination" };

  dest_frame = gtk_frame_new(NULL);
  gtk_container_set_border_width(GTK_CONTAINER(dest_frame), 5);
  gtk_frame_set_label(GTK_FRAME(dest_frame), "Not Game Destinations");
  gtk_frame_set_label_align(GTK_FRAME(dest_frame), 0.0, 0.0);
  gtk_frame_set_shadow_type(GTK_FRAME(dest_frame), GTK_SHADOW_ETCHED_IN);
  gtk_box_pack_start(GTK_BOX(vbox), dest_frame, TRUE, TRUE, 0);

  dest_table = gtk_table_new(2, 2, FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(dest_table), 5);
  gtk_container_add(GTK_CONTAINER(dest_frame), dest_table);

  label = gtk_label_new("Group:");
  align = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(dest_table), GTK_WIDGET(align), 0, 1, 0, 1, (GTK_FILL | GTK_SHRINK), 0, 5, 0);
  gtk_container_add(GTK_CONTAINER(align), label);
  gtk_widget_show(align);
  gtk_widget_show(label);

  dest_groups_box = gtk_combo_new();
  gtk_combo_set_use_arrows_always(GTK_COMBO(dest_groups_box), TRUE);
  gtk_combo_set_case_sensitive(GTK_COMBO(dest_groups_box), TRUE);

  /* The combo box containing the dest groups */
  dest_groups_box = gtk_combo_new();
  gtk_combo_set_use_arrows_always(GTK_COMBO(dest_groups_box), TRUE);
  gtk_combo_set_case_sensitive(GTK_COMBO(dest_groups_box), TRUE);

  if (dest_group_names) {
    gtk_combo_set_popdown_strings(GTK_COMBO(dest_groups_box), dest_group_names);
  }
  gtk_signal_connect(GTK_OBJECT(GTK_COMBO(dest_groups_box)->entry), "activate", GTK_SIGNAL_FUNC(dest_group_add_from_gui), (gpointer) NULL);
  gtk_signal_connect(GTK_OBJECT(GTK_COMBO(dest_groups_box)->list), "selection_changed", GTK_SIGNAL_FUNC(dest_group_update_lists_from_gui), (gpointer) NULL);
  gtk_table_attach(GTK_TABLE(dest_table), GTK_WIDGET(dest_groups_box), 1, 2, 0, 1, (GTK_FILL | GTK_SHRINK), 0, 0, 0);
  gtk_widget_show(dest_groups_box);

  /* The clist for the dests in the current group */
  scroller = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scroller), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_table_attach_defaults(GTK_TABLE(dest_table), GTK_WIDGET(scroller), 1, 2, 1, 2);
  dest_clist = gtk_clist_new_with_titles(1, (gchar **) cols);
  gtk_widget_set_usize(dest_clist, 1, 80);
  gtk_clist_column_titles_passive(GTK_CLIST(dest_clist));
  gtk_clist_set_column_justification(GTK_CLIST(dest_clist), 1, GTK_JUSTIFY_LEFT);
  gtk_container_add(GTK_CONTAINER(scroller), dest_clist);

  if (dest_groups) {
    dest_group_make_clist(dest_clist, dest_groups->data);
  }
  gtk_widget_show(dest_clist);
  gtk_widget_show(scroller);

  gtk_widget_show(dest_table);
  gtk_widget_show(dest_frame);

}

static void
button_cb(GtkWidget *w, gpointer item) {

  if (((int) item) == 1) {
    play_game();
  } else if (((int) item) == 2) {
    /* Edit Player Group */
    player_group_edit_dialog();
  } else if (((int) item) == 3) {
    dest_group_edit_dialog();
  } else {
    print_warning("button_cb():  Invalid button handle 0x%08x", (int) item);
  }
}

static void
gamemenu_cb(gpointer item) {

  if (((int) item) == 1) {
    play_game();
  } else if (((int) item) == 2) {
    save_config(NULL);
  } else {
    print_warning("gamemenu_cb():  Invalid button handle 0x%08x", (int) item);
  }
}

static void
helpmenu_cb(gpointer item) {

  if (((int) item) == 1) {
    help_display();
  } else if (((int) item) == 2) {
    about_box_display();
  } else {
  }
}
