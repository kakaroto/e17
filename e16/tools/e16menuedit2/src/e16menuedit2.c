/* Created by Anjuta version 1.2.2 */
/*	This file will not be overwritten */

#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include "e16menuedit2.h"
#include "file.h"
#include "callbacks.h"

int main (int argc, char *argv[])
{
  GtkWidget *main_window;
  GtkWidget *treeview_menu;
  GladeXML *main_xml;
  GtkWidget *toolbar1;
  GtkToolItem *toolitem1;
  int i;
  char app_dir[PATH_MAX];

#ifdef ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
  textdomain (PACKAGE);
#endif

  gtk_init (&argc, &argv);

  for (i = 0; i < MAX_RECURSION; i++)
  {
    menu_file[i] = NULL;
  }

  main_xml = glade_xml_new (PACKAGE_SOURCE_DIR"/e16menuedit2.glade",
                            "main_window", NULL);

  /* This is important */
  glade_xml_signal_autoconnect (main_xml);
  main_window = glade_xml_get_widget (main_xml, "main_window");

  gtk_window_set_icon_from_file (GTK_WINDOW (main_window),
                                 PACKAGE_PIXMAPS_DIR"/e16menuedit2-icon.png",
                                 NULL);

  g_object_set_data (G_OBJECT (main_window),
                     "main_xml", main_xml);

  treeview_menu = glade_xml_get_widget (main_xml, "treeview_menu");
  create_tree_model (treeview_menu);

  /* new entry toolbar button */
  toolbar1 = glade_xml_get_widget (main_xml, "toolbar1");
  toolitem1 = gtk_tool_button_new_from_stock (GTK_STOCK_NEW);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar1), toolitem1, TB_NEW);
  gtk_widget_show (GTK_WIDGET (toolitem1));
  gtk_tool_item_set_tooltip (toolitem1, GTK_TOOLBAR (toolbar1)->tooltips,
                             to_utf8(_("New Entry")), "");
  g_signal_connect (toolitem1, "clicked",
                    G_CALLBACK (on_new_button), treeview_menu);

  /* new submenu toolbar button */
  /*toolbar1 = glade_xml_get_widget (main_xml, "toolbar1");
  toolitem1 = gtk_tool_button_new_from_stock (GTK_STOCK_INDEX);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar1), toolitem1, TB_SUBMENU);
  gtk_widget_show (GTK_WIDGET (toolitem1));
  gtk_tool_item_set_tooltip (toolitem1, GTK_TOOLBAR (toolbar1)->tooltips,
                             _("New Submenu"), "");*/
  //g_signal_connect (toolitem1, "clicked",
  //                  G_CALLBACK (on_new_button), treeview_menu);

  /* change icon toolbar button */
  toolbar1 = glade_xml_get_widget (main_xml, "toolbar1");
  toolitem1 = gtk_tool_button_new_from_stock (GTK_STOCK_SELECT_COLOR);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar1), toolitem1, TB_CHANGE_ICON);
  gtk_widget_show (GTK_WIDGET (toolitem1));
  gtk_tool_item_set_tooltip (toolitem1, GTK_TOOLBAR (toolbar1)->tooltips,
                             to_utf8(_("Change Icon")), "");
  g_signal_connect (toolitem1, "clicked",
                    G_CALLBACK (on_change_icon_button), treeview_menu);

  /* save toolbar button */
  toolbar1 = glade_xml_get_widget (main_xml, "toolbar1");
  toolitem1 = gtk_tool_button_new_from_stock (GTK_STOCK_SAVE);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar1), toolitem1, TB_SAVE);
  gtk_widget_show (GTK_WIDGET (toolitem1));
  gtk_tool_item_set_tooltip (toolitem1, GTK_TOOLBAR (toolbar1)->tooltips,
                             to_utf8(_("Save Menu")), "");
  g_signal_connect (toolitem1, "clicked",
                    G_CALLBACK (on_save_button), treeview_menu);

  /* delete toolbar button */
  toolitem1 = gtk_tool_button_new_from_stock (GTK_STOCK_DELETE);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar1), toolitem1, TB_DELETE);
  gtk_widget_show (GTK_WIDGET (toolitem1));
  gtk_tool_item_set_tooltip (toolitem1, GTK_TOOLBAR (toolbar1)->tooltips,
                             to_utf8(_("Delete Entry or Submenu")), "");
  g_signal_connect (toolitem1, "clicked",
                    G_CALLBACK (on_delete_button), treeview_menu);

  /* quit toolbar button */
  toolitem1 = gtk_tool_button_new_from_stock (GTK_STOCK_QUIT);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar1), toolitem1, TB_QUIT);
  gtk_widget_show (GTK_WIDGET (toolitem1));
  gtk_tool_item_set_tooltip (toolitem1, GTK_TOOLBAR (toolbar1)->tooltips,
                             to_utf8(_("Quit Menu Editor")), "");
  g_signal_connect (toolitem1, "clicked",
                    G_CALLBACK (gtk_main_quit), NULL);


  /* create initial directories */

  sprintf (app_dir, "%s/%s/%s", homedir (getuid ()), APP_HOME, ICON_DIR);

  mkdir_with_parent (app_dir, 0755);

  gtk_widget_show (main_window);

  gtk_main ();
  return 0;
}

void create_tree_model (GtkWidget *treeview_menu)
{
  GtkCellRenderer *renderer_desc, *renderer_icon;
  GtkCellRenderer *renderer_iconname, *renderer_params;
  GtkTreeViewColumn *iconname_column, *icon_column;
  GtkTreeModel *model;

  renderer_desc = gtk_cell_renderer_text_new ();
  g_object_set (renderer_desc, "editable", TRUE, NULL);
  gtk_tree_view_insert_column_with_attributes (
    GTK_TREE_VIEW (treeview_menu),
    -1,
    "Description",
    renderer_desc,
    "text", COL_DESCRIPTION,
    NULL);

  renderer_icon = gtk_cell_renderer_pixbuf_new ();
  icon_column = gtk_tree_view_column_new_with_attributes (
                  "Icon",
                  renderer_icon,
                  "pixbuf", COL_ICON,
                  NULL);
  gtk_tree_view_insert_column (GTK_TREE_VIEW (treeview_menu),
                               icon_column,  -1);

  renderer_iconname = gtk_cell_renderer_text_new ();
  iconname_column = gtk_tree_view_column_new_with_attributes (
                      "Icon Name",
                      renderer_iconname,
                      "text", COL_ICONNAME,
                      NULL);
  g_object_set (iconname_column, "visible", FALSE, NULL);
  gtk_tree_view_insert_column (GTK_TREE_VIEW (treeview_menu),
                               iconname_column, -1);


  renderer_params = gtk_cell_renderer_text_new ();
  g_object_set (renderer_params, "editable", TRUE, NULL);
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview_menu),
      -1,
      "Parameters",
      renderer_params,
      "text", COL_PARAMS,
      NULL);

  model = load_menus_from_disk ();

  /* connect signals
  to edit cell */
  g_signal_connect (renderer_desc, "edited",
                    G_CALLBACK (on_descrenderer_edited), model);
  g_signal_connect (renderer_params, "edited",
                    G_CALLBACK (on_paramsrenderer_edited), model);
  //g_signal_connect (icon_column, "clicked",
  //                  G_CALLBACK (on_iconcolumn_clicked), model);

  gtk_tree_view_set_model (GTK_TREE_VIEW (treeview_menu), model);

  g_object_unref (model); /* destroy model automatically with view */

  return;
}

GtkTreeModel *load_menus_from_disk (void)
{

  FILE *menufile;
  char buf[1024];
  char first = 1;
  char s[4096];
  GtkTreeStore *store;
  GtkTreeIter iter, sub_iter;

  store = gtk_tree_store_new (TREE_COLUMNS_SIZE,
                              G_TYPE_STRING,
                              GDK_TYPE_PIXBUF,
                              G_TYPE_STRING,
                              G_TYPE_STRING);

  /* currently hardcoded, but not a big issue to change later */
  sprintf (buf, "%s/.enlightenment/file.menu", homedir (getuid ()));
  menufile = fopen (buf, "r");
  if (!menufile)
  {
    printf ("hmm. looks like you have some \"issues\" as you don't have\n"
            "a %s file.\n", buf);
    gtk_exit (1);
  }

  while (fgets (s, 4096, menufile))
  {
    s[strlen (s) - 1] = 0;
    if ((s[0] && s[0] != '#'))
    {
      if (first)
      {
        gchar *text[3];
        char *txt = NULL;
        char *txt2 = NULL;
        char *txt3 = NULL;

        txt = field (s, 0);
        text[0] = g_locale_to_utf8 (txt, -1, NULL, NULL, NULL);
        txt2 = g_strdup ("");
        text[1] = g_locale_to_utf8 (txt2, -1, NULL, NULL, NULL);
        txt3 = g_strdup (buf);
        text[2] = g_locale_to_utf8 (txt3, -1, NULL, NULL, NULL);

        gtk_tree_store_append (store, &iter, NULL);
        gtk_tree_store_set (store, &iter,
                            COL_DESCRIPTION, text[0],
                            COL_ICON, gdk_pixbuf_new_from_file (text[1], NULL),
                            COL_ICONNAME, text[1],
                            COL_PARAMS, text[2],
                            -1);

        //printf("mainitem: %s, %s, %s\n",txt,txt2,txt3);

        if (txt)
          g_free (txt);
        if (txt2)
          g_free (txt2);
        if (txt3)
          g_free (txt3);

        first = 0;
      }
      else
      {
        char *txt = NULL, *icon = NULL, *act = NULL, *params = NULL;
        gchar *text[3];

        txt = field (s, 0);
        icon = field (s, 1);
        act = field (s, 2);
        params = field (s, 3);

        text[0] = g_locale_to_utf8 (txt, -1, NULL, NULL, NULL);
        if (!icon)
          icon = g_strdup ("");
        text[1] = g_locale_to_utf8 (icon, -1, NULL, NULL, NULL);
        if (!params)
          params = g_strdup ("");
        text[2] = g_locale_to_utf8 (params, -1, NULL, NULL, NULL);

        //printf("subitem: %s, %s, %s, %s\n",txt,icon,act,params);

        gtk_tree_store_append (store, &sub_iter, &iter);
        gtk_tree_store_set (store, &sub_iter,
                            COL_DESCRIPTION, text[0],
                            COL_ICON, gdk_pixbuf_new_from_file (icon, NULL),
                            COL_ICONNAME, text[1],
                            COL_PARAMS, text[2],
                            -1);

        if (!strcasecmp (act, "menu"))
          load_sub_menu_from_disk (params, store, &sub_iter);

        if (txt)
          g_free (txt);
        if (icon)
          g_free (icon);
        if (act)
          g_free (act);
        if (params)
          g_free (params);

      }
    }
  }

  fclose (menufile);

  return GTK_TREE_MODEL (store);
}

/* load submenu from disk */
void load_sub_menu_from_disk (char *file_to_load, GtkTreeStore *store,
                              GtkTreeIter *iter)
{

  FILE *menufile;
  char buf[1024];
  char first = 1;
  char s[4096];
  GtkTreeIter sub_iter;

  if (!file_to_load)
    return;
  if (file_to_load[0] != '/')
    sprintf (buf, "%s/.enlightenment/%s", homedir (getuid ()), file_to_load);
  else
    sprintf (buf, "%s", file_to_load);

  menufile = fopen (buf, "r");

  if (!menufile)
    return;

  while (fgets (s, 4096, menufile))
  {
    s[strlen (s) - 1] = 0;
    if ((s[0] && s[0] != '#'))
    {
      if (first)
        first = 0;
      else
      {
        char *txt = NULL, *icon = NULL, *act = NULL, *params = NULL;
        gchar *text[3];

        txt = field (s, 0);
        icon = field (s, 1);
        act = field (s, 2);
        params = field (s, 3);

        text[0] = g_locale_to_utf8 (txt, -1, NULL, NULL, NULL);
        if (!icon)
          icon = g_strdup ("");
        text[1] = g_locale_to_utf8 (icon, -1, NULL, NULL, NULL);
        if (!params)
          params = g_strdup ("");
        text[2] = g_locale_to_utf8 (params, -1, NULL, NULL, NULL);

        /* printf("subitem: %s, %s, %s, %s\n",txt,icon,act,params); */

        gtk_tree_store_append (store, &sub_iter, iter);
        gtk_tree_store_set (store, &sub_iter,
                            COL_DESCRIPTION, text[0],
                            COL_ICON, gdk_pixbuf_new_from_file (icon, NULL),
                            COL_ICONNAME, text[1],
                            COL_PARAMS, text[2],
                            -1);


        if (!strcasecmp (act, "menu"))
          load_sub_menu_from_disk (params, store, &sub_iter);

        if (txt)
          g_free (txt);
        if (icon)
          g_free (icon);
        if (act)
          g_free (act);
        if (params)
          g_free (params);
      }
    }
  }

  fclose (menufile);

  return;
}

gboolean table_save_func (GtkTreeModel *model, GtkTreePath *path,
                          GtkTreeIter *iter, gpointer user_data)
{
  gchar *description, *icon, *params;
  gchar *tree_path_str;
  gboolean has_child;
  gint depth;
  gchar *realfile;
  FILE *menu_ptr, *menu_ptr2;
  gchar buffer[128];

  gtk_tree_model_get (model, iter,
                      COL_DESCRIPTION, &description,
                      COL_ICONNAME, &icon,
                      COL_PARAMS, &params,
                      -1);

  tree_path_str = gtk_tree_path_to_string(path);

  has_child = gtk_tree_model_iter_has_child (model, iter);
  depth = gtk_tree_path_get_depth (path) - 1;

#define WRITE_FILE /* undef this for debugging */
#ifdef WRITE_FILE
  if (depth + 1 >= MAX_RECURSION)
  {
    g_print ("maximum menu recursion reached! -> %d\n", MAX_RECURSION);
    return TRUE;
  }

  if (params[0] != '/')
  {
    /* Tarnation! A relative path */
    realfile = g_strjoin ("/", homedir (getuid ()),
                          ".enlightenment", params, NULL);
  }
  else
  {
    realfile = params;
  }
  menu_file[depth] = g_strdup (realfile);

  if (depth > 0)
  {
    menu_ptr = fopen (menu_file[depth-1], "a+");
    if (menu_ptr == NULL)
    {
      printf ("Couldn't save menu to: %s\n", menu_file[depth-1]);
    }

    if (has_child)
    {
      menu_ptr2 = fopen (menu_file[depth], "w");
      if (menu_ptr2 == NULL)
      {
        printf ("Couldn't save menu to: %s\n", menu_file[depth]);
      }
      //g_print ("write header to \"%s\":\n", menu_file[depth]);
      sprintf (buffer, "\"%s\"\n", description);
      fprintf (menu_ptr2, "%s", g_locale_from_utf8 (buffer,
               -1, NULL, NULL, NULL));
      fclose (menu_ptr2);

      //g_print ("write menu to \"%s\":\n", menu_file[depth-1]);
      sprintf (buffer, "\"%s\"\t%s\tmenu\t\"%s\"\n",
               description[0] == '\0' ? "NULL" : description,
               icon[0] == '\0' ? "NULL" : icon,
               params[0] == '\0' ? "" : params);
      fprintf (menu_ptr, "%s", g_locale_from_utf8 (buffer,
               -1, NULL, NULL, NULL));
    }
    else
    {
      //g_print ("write exec to \"%s\":\n", menu_file[depth-1]);
      sprintf (buffer, "\"%s\"\t%s\texec\t\"%s\"\n",
               description[0] == '\0' ? "NULL" : description,
               icon[0] == '\0' ? "NULL" : icon,
               params[0] == '\0' ? "" : params);

      fprintf (menu_ptr, "%s", g_locale_from_utf8 (buffer,
               -1, NULL, NULL, NULL));
    }
    fclose (menu_ptr);
  }
  else
  {
    menu_ptr2 = fopen (menu_file[depth], "w");
    if (menu_ptr2 == NULL)
    {
      printf ("Couldn't save menu to: %s\n", menu_file[depth]);
    }
    //g_print ("write first header to \"%s\":\n", menu_file[depth]);
    sprintf (buffer, "\"%s\"\n", description);
    fprintf (menu_ptr2, "%s", g_locale_from_utf8 (buffer,
             -1, NULL, NULL, NULL));
    fclose (menu_ptr2);
  }
#endif /* WRITE_FILE */

#ifndef WRITE_FILE
  g_print ("%s | ", tree_path_str);
  g_print ("%s %s %s\n", description, icon, params);
#endif /* WRITE_FILE */

  /* end of filelist to free it later */
  menu_file[depth+1] = NULL;

  g_free(tree_path_str);

  //g_free(realfile);
  g_free(description);
  g_free(icon);
  g_free(params);

  return FALSE;
}
