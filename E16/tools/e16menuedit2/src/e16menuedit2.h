/* Copyright (C) 2004 Andreas Volz and various contributors
 * 
 * Permission  is  hereby granted, free of charge, to any person ob-
 * taining a copy of  this  software  and  associated  documentation
 * files  (the "Software"), to deal in the Software without restric-
 * tion, including without limitation the rights to use, copy, modi-
 * fy, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is  fur-
 * nished to do so, subject to the following conditions:
 * 
 * The  above  copyright  notice and this permission notice shall be
 * included in all copies of the  Software,  its  documentation  and
 * marketing & publicity materials, and acknowledgment shall be giv-
 * en in the documentation, materials  and  software  packages  that
 * this Software was used.
 * 
 * THE  SOFTWARE  IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO  THE  WARRANTIES
 * OF  MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONIN-
 * FRINGEMENT. IN NO EVENT SHALL  THE  AUTHORS  BE  LIABLE  FOR  ANY
 * CLAIM,  DAMAGES  OR OTHER LIABILITY, WHETHER IN AN ACTION OF CON-
 * TRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR  IN  CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *  File: e16menuedit2.h
 *  Created by: Andreas Volz <linux@brachttal.net>
 *
 */

#ifndef _E16MENUEDIT_H
#define _E16MENUEDIT_H

#include <gtk/gtk.h>
#include <glade/glade.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <popt.h>
#include "nls.h"
#include "libglade_support.h"
#include "regex_func.h"

#define to_utf8(String) g_locale_to_utf8(String,-1,0,0,0)
#define from_utf8(String) g_locale_from_utf8(String,-1,0,0,0)

#define APP_ERRNO_STR_LEN 1024
#define APP_HOME ".e16menuedit2"
#define ICON_DIR "icons"
#define DEBUG_OUTPUT printf ("(%s, %d): ", __FILE__, __LINE__);
#define ENLIGHTENMENT_PATH ".enlightenment"
#define E16_PATH ".e16"
#define NOT_INSTALLED -1

void print_statusbar (const gchar *format, ...);
gboolean browser_func (GtkTreeModel *model, GtkTreePath *path,
                       GtkTreeIter *iter, gpointer user_data);
void parse_options (int argc, char **argv);
void show_version ();

struct global_variables
{
  int app_errno;
  char app_errno_str[APP_ERRNO_STR_LEN];
  int librsvg_cmp;
  char *browser;
  char *glade_file;
  char *epath;
  int emenu;
  regex_t re_mark;
  regex_t re_char;
  regex_t re_space;
  char *pattern_mark;
  char *pattern_char;
  char *pattern_space;
};

/* app_errno codes */
enum
{
  AE_NO_ERROR = 0,
  AE_EMPTY_SUBMENU
};

enum
{
  ARG_MENUS = 1,
  ARG_VERSION
};

enum
{
  EMENU_AUTODETECT,
  EMENU_MENUS
};

#endif /* _E16MENUEDIT_H */
