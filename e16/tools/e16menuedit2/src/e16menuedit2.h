/***************************************************************************
 *            e16menuedit.h
 *
 *  Tue Jun  1 13:51:01 2004
 *  Copyright  2004  Andreas Volz
 *  linux@brachttal.net
 ****************************************************************************/

#ifndef _E16MENUEDIT_H
#define _E16MENUEDIT_H

#include <gtk/gtk.h>
#include <glade/glade.h>
#include <stdio.h>
#include <config.h>
#include "nls.h"

#define to_utf8(String) g_locale_to_utf8(String,-1,0,0,0)
#define from_utf8(String) g_locale_from_utf8(String,-1,0,0,0)

#define MAX_RECURSION 128

void create_tree_model (GtkWidget *treeview_menu);
GtkTreeModel *create_and_fill_model (void);
GtkTreeModel *load_menus_from_disk (void);
void load_sub_menu_from_disk (char *file_to_load, GtkTreeStore *store,
                              GtkTreeIter *iter);
gboolean table_save_func (GtkTreeModel *model, GtkTreePath *path,
                          GtkTreeIter *iter, gpointer user_data);
		  
#define APP_HOME ".e16menuedit2"
#define ICON_DIR "icons"

enum tree_columns
{
  COL_DESCRIPTION,
  COL_ICON,
  COL_ICONNAME,
  COL_PARAMS,
  TREE_COLUMNS_SIZE
};

enum toolbar_buttons
{
  TB_NEW,
  TB_CHANGE_ICON,
  TB_SAVE,
  TB_DELETE,
  TB_QUIT
};

enum
{
  TARGET_GTK_TREE_MODEL_ROW
};

gchar *menu_file[MAX_RECURSION];

#endif /* _E16MENUEDIT_H */
