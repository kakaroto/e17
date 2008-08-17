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
#include "help.h"
#include "notgame.h"
#include "play.h"
#include "pregame.h"

static gint about_cb(void);
static gint help_cb(void);
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
about_cb(void) {

  if (about == NULL) {
    return FALSE;
  }
  gtk_widget_destroy(about);
  about = NULL;
  return (TRUE);
}

void
help_display(void) {

  GtkWidget *button, *label, *vbox, *hbox, *text, *vscrollbar, *align;
  const char *instructions =
    "The Not Game is a mechanism for getting a diverse group of people to agree on a single "
    "choice by allowing each person to eliminate one alternative of their choice per turn "
    "until only one remains.  The single remaining choice wins.\n\n"
    "Before beginning the game, use the Pre-Game screen to add, modify, and choose your "
    "players and destination choices.  You can create new groups, add players or destinations "
    "to a group, and so on from this screen.\n\n"
    "Once you are finished, select \"Play Game\" from the menu (or click the button) to begin "
    "playing the Not Game.\n\n"
    "During the game, each player will take turns eliminating one destination at a time by "
    "clicking on the button for that choice.  The button will then deactivate.  Continue "
    "until only one button remains active; that is your winner!  The text near the bottom "
    "of the game window will tell you whose turn it is.  You can also quit the game early "
    "by pressing the \"Quit Game\" button.\n";

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
  gtk_widget_show(label);

  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  text = gtk_text_new(NULL, NULL);
  gtk_widget_set_usize(text, 300, 100);
  gtk_box_pack_start(GTK_BOX(hbox), text, FALSE, FALSE, 0);
  gtk_text_set_editable(GTK_TEXT(text), FALSE);
  gtk_text_set_word_wrap(GTK_TEXT(text), TRUE);
  vscrollbar = gtk_vscrollbar_new(GTK_TEXT(text)->vadj);
  gtk_box_pack_start(GTK_BOX(hbox), vscrollbar, FALSE, FALSE, 0);
  gtk_widget_show(vscrollbar);
  gtk_text_insert(GTK_TEXT(text), NULL, NULL, NULL, instructions, -1);
  gtk_widget_show(text);
  gtk_widget_show(hbox);

  button = gtk_button_new_with_label("OK");
  align = gtk_alignment_new(0.5, 0.5, 0.2, 0.0);
  gtk_box_pack_start(GTK_BOX(vbox), align, FALSE, FALSE, 10);
  gtk_container_add(GTK_CONTAINER(align), button);
  gtk_widget_show(align);
  gtk_widget_show(button);
  
  gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(help_cb), NULL);

  gtk_widget_show(vbox);
  gtk_widget_show(help);

}

static gint
help_cb(void) {

  if (help == NULL) {
    return FALSE;
  }
  gtk_widget_destroy(help);
  help = NULL;
  return (TRUE);
}
