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
char *browser;
char *glade_file;
char *epath;
int emenu;

int main (int argc, char *argv[])
{
  GladeXML *main_xml;
  GtkWidget *main_window;
  GtkWidget *treeview_menu;
  GtkWidget *main_statusbar;
  char app_dir[PATH_MAX];
  char package[] = "librsvg-2.0";
  char librsvg_version[] = "2.7.1";
  char *version;
  int i;
  char *pixmap_file;
  FILE *fz_properties;
  gchar *filename_properties;
  char key[KEY_LENGTH];
  char value[VALUE_LENGTH];

#ifdef ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (PACKAGE);
#endif

  /* set log level for app exit */
  g_log_set_always_fatal (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL);

  parse_options (argc, argv);

  gtk_init (&argc, &argv);

  for (i = 0; i < MAX_RECURSION; i++)
  {
    menu_file[i] = NULL;
  }

  glade_file = searchGladeFile ("e16menuedit2.glade");
  main_xml = glade_xml_new (glade_file, "main_window", NULL);

  register_libglade_parent (main_xml, "main_window");

  glade_xml_signal_autoconnect (main_xml);

  main_window = lookup_libglade_widget ("main_window", "main_window");

  pixmap_file = searchPixmapFile ("e16menuedit2-icon.png");
  gtk_window_set_icon_from_file (GTK_WINDOW (main_window),
                                 pixmap_file, NULL);
  g_free (pixmap_file);

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
  librsvg_cmp = version_cmp (version, librsvg_version);
  g_free (version);

  print_statusbar (_("Menu successfully loaded!"));

  /* read some properties */
  filename_properties = g_strdup_printf ("%s/%s/properties",
                                         homedir (getuid ()), APP_HOME);

  fz_properties = fopen (filename_properties, "r");
  if (fz_properties != NULL)
  {
    fscanf (fz_properties, "%s = %s", key, value);

    g_free (browser);
    browser = g_malloc (strlen (value)+1);
    strncpy (browser, value, strlen (value)+1);

    fclose (fz_properties);
  }

  g_free (filename_properties);

  gtk_main ();

  free (epath);
  g_free (glade_file);

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

void dbg_printf (const gchar *format, ...)
{
  va_list ap;

  va_start (ap, format);

  printf ("(%s, %d): ", __FILE__, __LINE__);
  g_vprintf(format, ap);
}

gboolean browser_func (GtkTreeModel *model, GtkTreePath *path,
                       GtkTreeIter *iter, gpointer user_data)
{
  gchar *value;
  gchar *tree_path_str;
  GtkWidget *comboboxentry1;

  comboboxentry1 = lookup_libglade_widget ("properties_window", "comboboxentry1");

  gtk_tree_model_get (model, iter, 0, &value, -1);

  tree_path_str = gtk_tree_path_to_string(path);

  if (!strcmp (value, (char*) user_data))
  {
    gtk_combo_box_set_active (GTK_COMBO_BOX (comboboxentry1),
                              atoi (tree_path_str));

    g_free (browser);
    browser = g_malloc (strlen (value)+1);
    strncpy (browser, value, strlen (value)+1);

    g_free(tree_path_str);
    return TRUE;
  }

  g_free(tree_path_str);

  return FALSE;
}

void parse_options (int argc, char **argv)
{
  poptContext context;
  int option;
  epath = NULL;
  struct stat buf;
  gboolean epath_missing = TRUE;
  char *tmp;
  
  emenu = EMENU_AUTODETECT;

  struct poptOption options[] =
    {
      {"menus", 'm', POPT_ARG_NONE, NULL, ARG_MENUS,
       "Force search of file.menu in 'menus' subdirecory of enlightenment"
       "configuration direcory. If not given e16menuedit2 try to auto detect it.",
       NULL},
      {"version", 'v', POPT_ARG_NONE, NULL, ARG_VERSION, "show version", NULL},
      POPT_AUTOHELP {NULL, '\0', 0, NULL, 0}
    };

  /* get E path */
  tmp = getenv ("ECONFDIR");
  epath = g_strdup_printf (tmp);
  
  if (!epath)
  {
    epath = g_strdup_printf ("%s/%s", homedir (getuid ()),
                                  ENLIGHTENMENT_PATH);
  }
  epath_missing = stat (epath, &buf);
  if (epath_missing)
  {
    g_critical ("The direcory %s seems not to be a E16 conf path!\n", epath);
  }

  context = poptGetContext ("popt1", argc, (const char **) argv, options, 0);
  
  /* start option handling */
  while ((option = poptGetNextOpt (context)) > 0)
  {  
    switch (option)
    {
    case ARG_MENUS:
      emenu = EMENU_MENUS;
      break;
    case ARG_VERSION:
      show_version ();
      exit (0);
      break;
    }
  }

}

void show_version ()
{
  g_print ("Package name: ");
  g_print ("%s %s\n", PACKAGE, VERSION);
  g_print ("Build information: ");
  g_print ("%s %s\n", __DATE__, __TIME__);
}
