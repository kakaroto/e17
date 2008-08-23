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
 *  File: treeview.c
 *  Created by: Andreas Volz <linux@brachttal.net>
 *
 */

#include <string.h>
#include "treeview.h"
#include "e16menu.h"
#include "callbacks.h"
#include "nls.h"
#include "e16menuedit2.h"

extern struct global_variables gv;

static GtkTargetEntry row_targets[] =
  {
    { "GTK_TREE_MODEL_ROW", GTK_TARGET_SAME_APP,
      TARGET_GTK_TREE_MODEL_ROW }
  };

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
    _("Description"),
    renderer_desc,
    "text", COL_DESCRIPTION,
    NULL);

  renderer_icon = gtk_cell_renderer_pixbuf_new ();
  icon_column = gtk_tree_view_column_new_with_attributes (
                  _("Icon"),
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
      _("Parameters"),
      renderer_params,
      "text", COL_PARAMS,
      NULL);

  model = load_menus_from_disk ();

  /* connect signals to edit cell */
  g_signal_connect (renderer_desc, "edited",
                    G_CALLBACK (on_descrenderer_edited), model);
  g_signal_connect (renderer_params, "edited",
                    G_CALLBACK (on_paramsrenderer_edited), model);
  //g_signal_connect (icon_column, "clicked",
  //                  G_CALLBACK (on_iconcolumn_clicked), model);

  gtk_tree_view_set_model (GTK_TREE_VIEW (treeview_menu), model);

  gtk_tree_view_expand_to_path (GTK_TREE_VIEW (treeview_menu),
                                 gtk_tree_path_new_from_string  ("0"));

  activate_dragndrop (GTK_TREE_VIEW (treeview_menu));

  g_object_unref (model); /* destroy model automatically with view */

  return;
}

void deactivate_dragndrop (GtkTreeView *treeview_menu)
{
  gtk_tree_view_unset_rows_drag_source (GTK_TREE_VIEW (treeview_menu));
  gtk_tree_view_unset_rows_drag_dest (GTK_TREE_VIEW (treeview_menu));
}

void activate_dragndrop (GtkTreeView *treeview_menu)
{
  gtk_tree_view_enable_model_drag_source (GTK_TREE_VIEW (treeview_menu),
                                          GDK_BUTTON1_MASK,
                                          row_targets,
                                          G_N_ELEMENTS (row_targets),
                                          GDK_ACTION_MOVE | GDK_ACTION_COPY);

  gtk_tree_view_enable_model_drag_dest (GTK_TREE_VIEW (treeview_menu),
                                        row_targets,
                                        G_N_ELEMENTS (row_targets),
                                        GDK_ACTION_MOVE | GDK_ACTION_COPY);
}

void new_table_row (GtkWidget *treeview_menu)
{
  GtkTreeModel *model;
  GtkTreeSelection *select;
  GtkTreeIter sibling;
  GtkTreeIter new_iter;
  GtkTreeIter root_iter;
  gchar *empty_desc, *empty_icon;
  gchar *tree_string;

  model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview_menu));

  select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview_menu));

  if (gtk_tree_selection_get_selected (select, &model, &sibling))
  {
    empty_desc = g_strdup (_("New Entry"));
    empty_icon = g_strdup ("");

    tree_string = gtk_tree_model_get_string_from_iter (
                    GTK_TREE_MODEL(model), &sibling);

    if (!strcmp (tree_string, "0"))
    {
      gtk_tree_model_get_iter_first (GTK_TREE_MODEL(model), &root_iter);
      gtk_tree_store_append (GTK_TREE_STORE (model), &new_iter, &root_iter);
    }
    else
    {
      gtk_tree_store_insert_after (GTK_TREE_STORE (model),
                                   &new_iter,
                                   NULL,
                                   &sibling);
    }

    gtk_tree_store_set (GTK_TREE_STORE (model), &new_iter,
                        COL_DESCRIPTION, empty_desc,
                        COL_ICONNAME, empty_icon,
                        -1);
    g_free (empty_desc);
    g_free (empty_icon);
  }
}

void delete_table_row (GtkWidget *treeview_menu)
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  GtkTreeSelection *select;

  model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview_menu));
  select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview_menu));

  if (gtk_tree_selection_get_selected (select, &model, &iter))
  {
    gtk_tree_store_remove (GTK_TREE_STORE (model), &iter);
  }
}

void save_table_to_menu (GtkWidget *treeview_menu)
{
  GtkTreeModel *model;
  int i = 0;

  model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview_menu));
  gtk_tree_model_foreach (GTK_TREE_MODEL(model), table_check_func, NULL);

  if (gv.app_errno == AE_NO_ERROR)
  {
    gtk_tree_model_foreach (GTK_TREE_MODEL(model), table_save_func, NULL);
    print_statusbar (_("Menu saved!"));

    /* free allocated menu files */
    while (menu_file[i] != NULL)
    {
      g_free (menu_file[i]);
      i++;
    }
  }
  else
  {
    if (gv.app_errno == AE_EMPTY_SUBMENU)
    {
      gtk_tree_view_set_cursor (GTK_TREE_VIEW (treeview_menu),
                                gtk_tree_path_new_from_string  (gv.app_errno_str),
                                gtk_tree_view_get_column (
                                  GTK_TREE_VIEW (treeview_menu),
                                  COL_PARAMS),
                                TRUE);
      print_statusbar (_("Submenu must have a name! -> menu not saved!"));
    }
    else
    {
      print_statusbar (_("Unknown error occurred while checking menu -> menu not saved!"));
    }
  }
}
