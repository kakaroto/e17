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
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <gtk/gtk.h>

#include "debug.h"
#include "conf.h"
#include "dest.h"
#include "notgame.h"
#include "parse.h"
#include "players.h"
#include "pregame.h"
#include "strings.h"

static void save_dialog(char *msg);
static gint close_cb(GtkWidget *button, gpointer data);
static void save_player(player_t *player, FILE *fp);
static void save_player_group(player_group_t *group, FILE *fp);
static void save_dest(dest_t *dest, FILE *fp);
static void save_dest_group(dest_group_t *group, FILE *fp);

void *
parse_player_group(char *buff, void *state) {

  player_group_t *group;
  player_t *player;

  if (*buff == CONF_BEGIN_CHAR) {
    group = (player_group_t *) malloc(sizeof(player_group_t));
    MEMSET(group, 0, sizeof(player_group_t));
    return ((void *) group);
  } else if (*buff == CONF_END_CHAR) {
    ASSERT(state != NULL);
    group = (player_group_t *) state;
    if (group->name == NULL || !player_group_add(group)) {
      free(group);  /* Duplicate/invalid group, so delete the structure */
    }
    return (NULL);
  } else {
    ASSERT(state != NULL);
    group = (player_group_t *) state;
    if (!BEG_STRCASECMP(buff, "name")) {
      group->name = Word(2, buff);
    } else if (!BEG_STRCASECMP(buff, "player")) {
      player = (player_t *) malloc(sizeof(player_t));
      player->name = Word(2, buff);
      player->type = Word(3, buff);
      if (player->name == NULL || player->type == NULL || !player_group_add_player(group, player)) {
        free(player);
      }
    } else {
      print_error("Parse error in file %s, line %lu:  Attribute \"%s\" is not valid in the current context.", file_peek_path(), file_peek_line(), buff);
    }
    return (state);
  }
}

void *
parse_dest_group(char *buff, void *state) {

  dest_group_t *group;
  dest_t *dest;

  if (*buff == CONF_BEGIN_CHAR) {
    group = (dest_group_t *) malloc(sizeof(dest_group_t));
    MEMSET(group, 0, sizeof(dest_group_t));
    return ((void *) group);
  } else if (*buff == CONF_END_CHAR) {
    ASSERT(state != NULL);
    group = (dest_group_t *) state;
    if (group->name == NULL || !dest_group_add(group)) {
      free(group);  /* Duplicate/invalid group, so delete the structure */
    }
    return (NULL);
  } else {
    ASSERT(state != NULL);
    group = (dest_group_t *) state;
    if (!BEG_STRCASECMP(buff, "name")) {
      group->name = Word(2, buff);
    } else if (!BEG_STRCASECMP(buff, "dest")) {
      dest = (dest_t *) malloc(sizeof(dest_t));
      dest->name = Word(2, buff);
      if (dest->name == NULL || !dest_group_add_dest(group, dest)) {
        free(dest);
      }
    } else {
      print_error("Parse error in file %s, line %lu:  Attribute \"%s\" is not valid in the current context.", file_peek_path(), file_peek_line(), buff);
    }
    return (state);
  }
}

static void
save_dialog(char *msg) {

  GtkWidget *dialog, *align, *vbox, *button, *label;

  dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(dialog), "Save Status");
  gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(pregame_win));
  gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_MOUSE);
  gtk_signal_connect(GTK_OBJECT(dialog), "destroy", GTK_SIGNAL_FUNC(close_cb), (void *) dialog);
  gtk_signal_connect(GTK_OBJECT(dialog), "delete_event", GTK_SIGNAL_FUNC(close_cb), (void *) dialog);
  gtk_container_set_border_width(GTK_CONTAINER(dialog), 5);

  vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(dialog), vbox);

  label = gtk_label_new(msg);
  gtk_misc_set_alignment(GTK_MISC(label), 1.0, 0.5);
  gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, FALSE, 20);
  gtk_widget_show(label);

  button = gtk_button_new_with_label("OK");
  align = gtk_alignment_new(0.5, 0.5, 0.1, 0.0);
  gtk_container_add(GTK_CONTAINER(align), button);
  gtk_widget_show(button);
  gtk_box_pack_start(GTK_BOX(vbox), align, TRUE, FALSE, 0);
  gtk_widget_show(align);
  gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(close_cb), (void *) dialog);

  gtk_widget_show(vbox);
  gtk_widget_show(dialog);

}

static gint
close_cb(GtkWidget *button, gpointer data) {

  GtkWidget *dialog;

  dialog = (GtkWidget *) data;
  gtk_widget_destroy(dialog);
  return (TRUE);
}

static void
save_player(player_t *player, FILE *fp) {

  ASSERT(player != NULL);
  ASSERT(fp != NULL);
  REQUIRE(player->name != NULL);
  REQUIRE(player->type != NULL);

  if (strpbrk(player->name, " \t\'")) {
    fprintf(fp, "  player \"%s\" ", player->name);
  } else {                                      
    fprintf(fp, "  player %s ", player->name);
  }
  if (strpbrk(player->type, " \t\'")) {
    fprintf(fp, "\"%s\"\n", player->type);
  } else {                  
    fprintf(fp, "%s\n", player->type);
  }
}

static void
save_player_group(player_group_t *group, FILE *fp) {

  ASSERT(group != NULL);
  ASSERT(fp != NULL);
  REQUIRE(group->name != NULL);

  fprintf(fp, "begin player_group\n");
  if (strpbrk(group->name, " \t\'")) {
    fprintf(fp, "  name \"%s\"\n", group->name);
  } else {
    fprintf(fp, "  name %s\n", group->name);
  }
  g_list_foreach(group->members, (GFunc) save_player, (gpointer) fp);
  fprintf(fp, "end\n\n");

}

static void
save_dest(dest_t *dest, FILE *fp) {

  ASSERT(dest != NULL);
  ASSERT(fp != NULL);
  REQUIRE(dest->name != NULL);

  if (strpbrk(dest->name, " \t\'")) {
    fprintf(fp, "  dest \"%s\"\n", dest->name);
  } else {
    fprintf(fp, "  dest %s\n", dest->name);
  }
}

static void
save_dest_group(dest_group_t *group, FILE *fp) {

  ASSERT(group != NULL);
  ASSERT(fp != NULL);
  REQUIRE(group->name != NULL);

  fprintf(fp, "begin dest_group\n");
  if (strpbrk(group->name, " \t\'")) {
    fprintf(fp, "  name \"%s\"\n", group->name);
  } else {
    fprintf(fp, "  name %s\n", group->name);
  }
  g_list_foreach(group->members, (GFunc) save_dest, (gpointer) fp);
  fprintf(fp, "end\n\n");

}

unsigned char
save_config(char *filename) {

  register FILE *fp;
  char dt_stamp[50];
  time_t cur_time = time(NULL);
  struct tm *cur_tm;
  const char format[] = "%s/notgame.cfg";

  cur_tm = localtime(&cur_time);

  if (!filename) {
    filename = (char *) malloc(strlen(getenv("HOME")) + sizeof(format));
    sprintf(filename, format, getenv("HOME"));
  }

  if ((fp = fopen(filename, "w")) == NULL) {
    save_dialog("Unable to write configuration file.  Save failed!");
    return (0);
  }
  strftime(dt_stamp, 50, "%x at %X", cur_tm);
  fprintf(fp, "<" PACKAGE "-" VERSION ">\n");
  fprintf(fp, "# Not Game Configuration File\n");
  fprintf(fp, "# Automatically generated by " PACKAGE "-" VERSION " on %s\n\n", dt_stamp);

  g_list_foreach(player_groups, (GFunc) save_player_group, (gpointer) fp);
  g_list_foreach(dest_groups, (GFunc) save_dest_group, (gpointer) fp);

  fclose(fp);
  save_dialog("Configuration saved.");
  return (1);
}
