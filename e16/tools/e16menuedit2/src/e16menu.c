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
 *  File: e16menu.h
 *  Created by: Andreas Volz <linux@brachttal.net>
 *
 */

#include <stdio.h>
#include "e16menu.h"
#include "file.h"
#include "e16menuedit2.h"
#include "treeview.h"

extern struct global_variables gv;

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

  if (gv.emenu == EMENU_AUTODETECT)
  {
    struct stat stat_buf;
    gboolean epath_missing = TRUE;

    sprintf (buf, "%s/file.menu", gv.epath);
    epath_missing = stat (buf, &stat_buf);

    if (epath_missing)
    {
      sprintf (buf, "%s/menus/file.menu", gv.epath);
    }
  }
  else if (gv.emenu == EMENU_MENUS)
  {
    sprintf (buf, "%s/menus/file.menu", gv.epath);
  }
  else
  {
    g_critical ("unknown menu definition!\n");
    exit (1);
  }

  menufile = fopen (buf, "r");
  if (!menufile)
  {
    g_critical ("hmm. looks like you have some \"issues\" as you don't have "
                "a %s file.\n", buf);
    exit(1);
  }
#ifdef DEBUG
  g_print ("Loading menu: %s\n", buf);
#endif /* DEBUG */

  while (fgets (s, 4096, menufile))
  {
    s[strlen (s) - 1] = 0;
    if ((s[0] && s[0] != '#'))
    {
      if (first)
      {
        gchar **data;

	menu_regex (s, &data);

	data[3] = to_utf8 (buf);
      
        gtk_tree_store_append (store, &iter, NULL);
        gtk_tree_store_set (store, &iter,
                            COL_DESCRIPTION, data[0],
                            COL_ICON, gdk_pixbuf_new_from_file (data[1], NULL),
                            COL_ICONNAME, data[1],
                            COL_PARAMS, data[3],
                            -1);

	g_free (data[0]);
	g_free (data[1]);
	g_free (data[2]);
	g_free (data[3]);
	g_free (data);
        first = 0;
      }
      else
      {
        gchar **data;
      
	menu_regex (s, &data);

        gtk_tree_store_append (store, &sub_iter, &iter);
        gtk_tree_store_set (store, &sub_iter,
                            COL_DESCRIPTION, data[0],
                            COL_ICON, gdk_pixbuf_new_from_file (data[1], NULL),
                            COL_ICONNAME, data[1],
                            COL_PARAMS, data[3],
                            -1);

        if (!strcasecmp (data[2], "menu"))
          load_sub_menu_from_disk (data[3], store, &sub_iter);

	g_free (data[0]);
	g_free (data[1]);
	g_free (data[2]);
	g_free (data[3]);
	g_free (data);
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
    sprintf (buf, "%s/%s", gv.epath, file_to_load);
  else
    sprintf (buf, "%s", file_to_load);

  menufile = fopen (buf, "r");

#ifdef DEBUG
  g_print ("Loading submenu: %s\n", buf);
#endif /* DEBUG */   

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
        gchar **data;
      
	menu_regex (s, &data);

        gtk_tree_store_append (store, &sub_iter, iter);
        gtk_tree_store_set (store, &sub_iter,
                            COL_DESCRIPTION, data[0],
                            COL_ICON, gdk_pixbuf_new_from_file (data[1], NULL),
                            COL_ICONNAME, data[1],
                            COL_PARAMS, data[3],
                            -1);


        if (!strcasecmp (data[2], "menu"))
          load_sub_menu_from_disk (data[3], store, &sub_iter);

	g_free (data[0]);
	g_free (data[1]);
	g_free (data[2]);
	g_free (data[3]);
	g_free (data);
      }
    }
  }

  fclose (menufile);

  return;
}

#define WRITE_FILE /* undef this for debugging without writing menus */
gboolean table_save_func (GtkTreeModel *model, GtkTreePath *path,
                          GtkTreeIter *iter, gpointer user_data)
{
  gchar *description, *icon, *params;
  gchar *tree_path_str;
  gboolean has_child;
  gint depth;
  gchar *realfile;
  gchar *dirname;
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

  if (depth + 1 >= MAX_RECURSION)
  {
    g_print ("maximum menu recursion reached! -> %d\n", MAX_RECURSION);
    return TRUE;
  }

  if (!g_path_is_absolute (params))
  {
    /* Tarnation! A relative path */    
    //realfile = g_strjoin ("/", gv.epath, params, NULL);
    realfile = g_strdup_printf ("%s/%s", gv.epath, params);
    //g_print ("FILE: %s,%s,%s\n", gv.epath, params, realfile);
  }
  else
  {
    realfile = g_strdup (params);
  }

  menu_file[depth] = g_strdup (realfile);

#ifdef WRITE_FILE
  /* create recursiv new menu subdirectory */
  dirname = g_path_get_dirname (realfile);
  mkdir_with_parent (dirname, 0755);
  g_free (dirname);
#endif /* WRITE_FILE */

  if (depth > 0)
  {
#ifdef WRITE_FILE  
    menu_ptr = fopen (menu_file[depth-1], "a+");
#endif /* WRITE_FILE */  
    if (menu_ptr == NULL)
    {
      printf ("Couldn't save menu to: %s\n", menu_file[depth-1]);
    }

    if (has_child)
    {
#ifdef WRITE_FILE      
      menu_ptr2 = fopen (menu_file[depth], "w");
      if (menu_ptr2 == NULL)
      {
        printf ("Couldn't save menu to: %s\n", menu_file[depth]);
      }
#endif /* WRITE_FILE */     

      sprintf (buffer, "\"%s\"\n", description);
#ifdef WRITE_FILE    
      fprintf (menu_ptr2, "%s", from_utf8 (buffer));
      fclose (menu_ptr2);
#else
      g_print ("write header to: \"%s\"\n", menu_file[depth]);
#endif /* !WRITE_FILE */    

      sprintf (buffer, "\"%s\"\t%s\tmenu\t\"%s\"\n",
               description[0] == '\0' ? "NULL" : description,
               icon[0] == '\0' ? "NULL" : icon,
               params[0] == '\0' ? "" : params);
#ifdef WRITE_FILE      
      fprintf (menu_ptr, "%s", from_utf8 (buffer));
#else
      g_print ("write menu to: \"%s\"\n", menu_file[depth-1]);
#endif /* WRITE_FILE */     
    }
    else
    {
      sprintf (buffer, "\"%s\"\t%s\texec\t\"%s\"\n",
               description[0] == '\0' ? "NULL" : description,
               icon[0] == '\0' ? "NULL" : icon,
               params[0] == '\0' ? "" : params);
#ifdef WRITE_FILE
      fprintf (menu_ptr, "%s", from_utf8 (buffer));
#else
      g_print ("write exec to: \"%s\"\n", menu_file[depth-1]);
#endif /*WRITE_FILE */    
    }
#ifdef WRITE_FILE  
    fclose (menu_ptr);
#endif /*WRITE_FILE */   
  }
  else
  {
#ifdef WRITE_FILE    
    menu_ptr2 = fopen (menu_file[depth], "w");
    if (menu_ptr2 == NULL)
    {
      printf ("Couldn't save menu to: %s\n", menu_file[depth]);
    }
#endif /*WRITE_FILE */

    sprintf (buffer, "\"%s\"\n", description);
#ifdef WRITE_FILE  
    fprintf (menu_ptr2, "%s", from_utf8 (buffer));
    fclose (menu_ptr2);
#else
    g_print ("write first header to: \"%s\"\n", menu_file[depth]);
#endif /* WRITE_FILE */      
  }

#ifndef WRITE_FILE
  g_print ("%s | ", tree_path_str);
  g_print ("%s %s %s\n", description, icon, params);
#endif /* !WRITE_FILE */

  /* end of filelist to free it later */
  menu_file[depth+1] = NULL;

  g_free (tree_path_str);
  g_free (realfile);
  g_free (description);
  g_free (icon);
  g_free (params);

  return FALSE;
}

gboolean table_check_func (GtkTreeModel *model, GtkTreePath *path,
                           GtkTreeIter *iter, gpointer user_data)
{
  gchar *description, *icon, *params;
  gchar *tree_path_str;
  gboolean has_child;
  gint depth;
  gchar buffer[128];

  gtk_tree_model_get (model, iter,
                      COL_DESCRIPTION, &description,
                      COL_ICONNAME, &icon,
                      COL_PARAMS, &params,
                      -1);

  tree_path_str = gtk_tree_path_to_string(path);

  has_child = gtk_tree_model_iter_has_child (model, iter);
  depth = gtk_tree_path_get_depth (path) - 1;

  gv.app_errno = 0;
  strcpy (gv.app_errno_str, "");

  if (depth + 1 >= MAX_RECURSION)
  {
    g_print ("maximum menu recursion reached! -> %d\n", MAX_RECURSION);
    return TRUE;
  }

  if (depth > 0)
  {
    if (has_child)
    {
      /* some checks for submenus */
      if (!strcmp (params, ""))
      {
        gv.app_errno = AE_EMPTY_SUBMENU;
        strncpy (gv.app_errno_str, tree_path_str, APP_ERRNO_STR_LEN);

        return TRUE;
      }
    }
    else
    {
      /* some checks for entries */
    }
  }
  else
  {
    /* check for root node */
  }

  g_free(tree_path_str);
  g_free(description);
  g_free(icon);
  g_free(params);

  return FALSE;
}
