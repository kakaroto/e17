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
#include <gdk_imlib.h>

#include "debug.h"
#include "conf.h"
#include "parse.h"
#include "players.h"
#include "strings.h"

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
    if (!player_group_add(group)) {
      free(group);  /* Duplicate group, so delete the structure */
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
      if (!player_group_add_player(group, player)) {
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

  if (*buff == CONF_BEGIN_CHAR) {
    return (NULL);
  } else if (*buff == CONF_END_CHAR) {
    return (NULL);
  } else {
    if (!BEG_STRCASECMP(buff, "name")) {
    } else if (!BEG_STRCASECMP(buff, "dest")) {
    } else {
    }
    return (state);
  }
}

