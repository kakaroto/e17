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

static void pregame_menu_init(GtkWidget *window, GtkWidget *vbox);
static void gamemenu_cb(gpointer item, GtkWidget *w);
static void helpmenu_cb(gpointer item, GtkWidget *w);

GtkWidget *pregame_win = NULL;

void
pregame_init(void) {

  GtkWidget *vbox;
  GtkWidget *scrolled_window;
  GtkWidget *drawing_area;

  pregame_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(pregame_win), PACKAGE " " VERSION " -- Pre-Game Setup");
  gtk_signal_connect(GTK_OBJECT(pregame_win), "destroy", GTK_SIGNAL_FUNC(clean_exit), NULL);
  gtk_signal_connect(GTK_OBJECT(pregame_win), "delete_event", GTK_SIGNAL_FUNC(clean_exit), NULL);
  gtk_container_set_border_width(GTK_CONTAINER(pregame_win), 1);
  
  vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(pregame_win), vbox);

  pregame_menu_init(pregame_win, vbox);

  gtk_widget_realize(pregame_win);
  gtk_widget_realize(vbox);
  gtk_widget_show(vbox);
  gtk_window_set_policy(GTK_WINDOW(pregame_win), 1, 1, 0);
  gtk_widget_show(pregame_win);
}

static void
pregame_menu_init(GtkWidget *window, GtkWidget *vbox) {

  GtkWidget *menubar, *menu, *menuitem;

  menubar = gtk_menu_bar_new();

  /* File menu */
  menu = gtk_menu_new();
  menuitem = gtk_tearoff_menu_item_new();
  gtk_menu_append(GTK_MENU(menu), menuitem);
  gtk_widget_show(menuitem);
  menuitem = gtk_menu_item_new_with_label("Play Game");
  gtk_menu_append(GTK_MENU(menu), menuitem);
  gtk_signal_connect_object(GTK_OBJECT(menuitem), "activate", GTK_SIGNAL_FUNC(gamemenu_cb), (gpointer) 1);
  gtk_widget_show(menuitem);
  menuitem = gtk_menu_item_new_with_label("Preferences");
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
  menuitem = gtk_menu_item_new_with_label("File");
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

  gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, TRUE, 0);
  gtk_widget_show(menubar);
}

static void
gamemenu_cb(gpointer item, GtkWidget *w) {

  if (((int) item) == 1) {
  } else {
  }
}

static void
helpmenu_cb(gpointer item, GtkWidget *w) {

  if (((int) item) == 1) {
    help_display();
  } else if (((int) item) == 2) {
    about_box_display();
  } else {
  }
}
