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

void
ng_init(void) {

  /* Load the config file containing the player groups and the choice groups */
  conf_init_subsystem();
  conf_load();

  /* Create the pre-game window */
  pregame_init();

  for (; gtk_events_pending(); ) {
    gtk_main_iteration();
  }
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

void
print_error(const char *msg, ...) {

  va_list args;

  if (msg != NULL) {
    va_start(args, msg);
    fprintf(stderr, PACKAGE ":  Error:  ");
    vfprintf(stderr, msg, args);
    va_end(args);
    fflush(stderr);
  }
}

void
print_warning(const char *msg, ...) {

  va_list args;

  if (msg != NULL) {
    va_start(args, msg);
    fprintf(stderr, PACKAGE ":  Warning:  ");
    vfprintf(stderr, msg, args);
    va_end(args);
    fflush(stderr);
  }
}

