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
 *  File: e16menu.h
 *  Created by: Andreas Volz <linux@brachttal.net>
 *
 */

#include <stdio.h>
#include "e16menu.h"
#include "file.h"
#include "e16menuedit2.h"
#include "treeview.h"

int app_errno;
char app_errno_str[APP_ERRNO_STR_LEN];
extern char *emenu_path;

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

  if (!strcmp (emenu_path, ENLIGHTENMENT_MENU))
    sprintf (buf, "%s/%s/file.menu", homedir (getuid ()), emenu_path);
  else if (!strcmp (emenu_path, E16_MENU))
    sprintf (buf, "%s/%s/%s/file.menu", homedir (getuid ()), emenu_path, "menus");
  else
  {
    printf ("unknown menu definition!\n");
    exit (1);
  }

  menufile = fopen (buf, "r");
  if (!menufile)
  {
    printf ("hmm. looks like you have some \"issues\" as you don't have\n"
            "a %s file.\n", buf);
    gtk_exit (1);
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
    sprintf (buf, "%s/%s/%s", homedir (getuid ()), emenu_path, file_to_load);
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
    realfile = g_strjoin ("/", homedir (getuid ()),
                          emenu_path, params, NULL);
  }
  else
  {
    realfile = params;
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
      fprintf (menu_ptr2, "%s", g_locale_from_utf8 (buffer,
               -1, NULL, NULL, NULL));
      fclose (menu_ptr2);
#else
      g_print ("write header to: \"%s\"\n", menu_file[depth]);
#endif /* !WRITE_FILE */    
      
      sprintf (buffer, "\"%s\"\t%s\tmenu\t\"%s\"\n",
               description[0] == '\0' ? "NULL" : description,
               icon[0] == '\0' ? "NULL" : icon,
               params[0] == '\0' ? "" : params);
#ifdef WRITE_FILE      
      fprintf (menu_ptr, "%s", g_locale_from_utf8 (buffer,
               -1, NULL, NULL, NULL));
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
      fprintf (menu_ptr, "%s", g_locale_from_utf8 (buffer,
               -1, NULL, NULL, NULL));
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
    fprintf (menu_ptr2, "%s", g_locale_from_utf8 (buffer,
             -1, NULL, NULL, NULL));
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

  g_free(tree_path_str);

  g_free(description);
  g_free(icon);
  g_free(params);

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

  app_errno = 0;
  strcpy (app_errno_str, "");

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
        app_errno = AE_EMPTY_SUBMENU;
        strncpy (app_errno_str, tree_path_str, APP_ERRNO_STR_LEN);

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
