/* Copyright (C) 2004 Andreas Volz and various contributors
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *  File: e16menuedit2.c
 *  Created by: Andreas Volz <linux@brachttal.net>
 *
 */

#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdarg.h>
#include "e16menuedit2.h"
#include "file.h"
#include "callbacks.h"
#include "e16menu.h"
#include "treeview.h"

int librsvg_cmp;

int main (int argc, char *argv[])
{
  GladeXML *main_xml;
  GtkWidget *main_window;
  GtkWidget *treeview_menu;
  GtkWidget *main_statusbar;
  char app_dir[PATH_MAX];
  char package[] = "librsvg-2.0";
  char good_version[] = "2.7.1";
  char *version;
  int i;  

#ifdef ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (PACKAGE);
#endif

  gtk_init (&argc, &argv);

  for (i = 0; i < MAX_RECURSION; i++)
  {
    menu_file[i] = NULL;
  }

  main_xml = glade_xml_new (PACKAGE_DATA_DIR"/"PACKAGE"/glade/e16menuedit2.glade",
                            "main_window", NULL);

  register_libglade_parent (main_xml, "main_window");


  glade_xml_signal_autoconnect (main_xml);


  main_window = lookup_libglade_widget ("main_window", "main_window");


  gtk_window_set_icon_from_file (GTK_WINDOW (main_window),
                                 PACKAGE_PIXMAPS_DIR"/e16menuedit2-icon.png",
                                 NULL);

  treeview_menu = lookup_libglade_widget ("main_window", "treeview_menu");
  create_tree_model (treeview_menu);

  /* bind callbacks manual. Is Glade too stupid for it? */
  bind_toolbar_callbacks (treeview_menu);
  bind_menubar_callbacks (treeview_menu);

  /* create initial directories */
  sprintf (app_dir, "%s/%s/%s", homedir (getuid ()), APP_HOME, ICON_DIR);
  mkdir_with_parent (app_dir, 0755);

  /* get librsvg version and check if good enough */
  version = pkg_config_version (package);
  librsvg_cmp = version_cmp (version, good_version);
  g_free (version);

  print_statusbar (_("Menu successfully loaded!"));

  gtk_main ();
  return 0;
}

void print_statusbar (const gchar *format, ...)
{
  const int statusbar_len = 100;
  va_list ap;
  gchar str[statusbar_len];
  GtkWidget *main_statusbar;

  va_start (ap, format);

  g_vsnprintf (str, statusbar_len, format, ap);

  main_statusbar = lookup_libglade_widget ("main_window", "main_statusbar");

  gtk_statusbar_push (GTK_STATUSBAR (main_statusbar),
                      0,
                      str);
}
