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
char *emenu_path;

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
  free (emenu_path);

  gtk_main ();

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
  char *emenu = NULL;
  char *e_version_current;
  int e_version_cmp;
  char e_version[] = "0.16.8";

  struct poptOption options[] =
    {
      {"emenu", 'e', POPT_ARG_STRING,
        &emenu, ARG_EMENU,
        "Enlightenment menu dir (enlightenment or e16).",
        NULL},
      {"version", 'v', POPT_ARG_NONE, NULL, ARG_VERSION, "show version", NULL},
      POPT_AUTOHELP {NULL, '\0', 0, NULL, 0}
    };

  context = poptGetContext ("popt1", argc, (const char **) argv, options, 0);

  /* start option handling */
  while ((option = poptGetNextOpt (context)) > 0)
  {
    switch (option)
    {
    case ARG_EMENU:
      g_print ("emenu case\n");
      break;
    case ARG_VERSION:
      show_version ();
      exit (0);
      break;
    }
  }

  if (emenu == NULL)
  {
    e_version_current = e16_version ();
    e_version_cmp = version_cmp (e_version_current, e_version);
    free (e_version_current);

    if (e_version_cmp >= 0)
    {
      emenu_path = malloc (strlen (E16_MENU) + 1);
      strcpy (emenu_path, E16_MENU);
    }
    else
    {
      emenu_path = malloc (strlen (ENLIGHTENMENT_MENU) + 1);
      strcpy (emenu_path, ENLIGHTENMENT_MENU);
    }
  }
  else
  {
    if (!strcmp (emenu, "enlightenment"))
    {
      emenu_path = malloc (strlen (ENLIGHTENMENT_MENU) + 1);
      strcpy (emenu_path, ENLIGHTENMENT_MENU);
    }
    else if (!strcmp (emenu, "e16"))
    {
      emenu_path = malloc (strlen (E16_MENU) + 1);
      strcpy (emenu_path, E16_MENU);
    }
    else
    {
      g_print ("Sorry, the parameter 'emenu' has only state 'enlightenment'\n"
               "for old menu structure and 'e16' for new direcory structure\n"
	       "in '.e16/menus' for E16 > 0.16.8.\n");
      exit (0);
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
