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
 *  File: callbacks.c
 *  Created by: Andreas Volz <linux@brachttal.net>
 *
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "callbacks.h"
#include "e16menuedit2.h"
#include "file.h"
#include "icon_chooser.h"
#include "e16menu.h"
#include "treeview.h"

void bind_toolbar_callbacks (GladeXML *main_xml, GtkWidget *treeview_menu)
{
  GtkWidget *toolbar1;
  GtkWidget *toolitem1;

  toolbar1 = glade_xml_get_widget (main_xml, "toolbar1");

  /* bind callbacks */
  toolitem1 = glade_xml_get_widget (main_xml, "toolbutton_save");
  gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (toolitem1),
                             GTK_TOOLBAR (toolbar1)->tooltips,
                             _("Save"), "");
  g_signal_connect (toolitem1, "clicked",
                    G_CALLBACK (on_toolbutton_save_clicked), treeview_menu);

  toolitem1 = glade_xml_get_widget (main_xml, "toolbutton_new");
  gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (toolitem1),
                             GTK_TOOLBAR (toolbar1)->tooltips,
                             _("New Entry"), "");
  g_signal_connect (toolitem1, "clicked",
                    G_CALLBACK (on_toolbutton_new_clicked), treeview_menu);

  toolitem1 = glade_xml_get_widget (main_xml, "toolbutton_icon");
  gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (toolitem1),
                             GTK_TOOLBAR (toolbar1)->tooltips,
                             _("Choose Icon"), "");
  g_signal_connect (toolitem1, "clicked",
                    G_CALLBACK (on_toolbutton_icon_clicked), treeview_menu);

  toolitem1 = glade_xml_get_widget (main_xml, "toolbutton_del");
  gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (toolitem1),
                             GTK_TOOLBAR (toolbar1)->tooltips,
                             _("Delete Entry"), "");
  g_signal_connect (toolitem1, "clicked",
                    G_CALLBACK (on_toolbutton_del_clicked), treeview_menu);

  toolitem1 = glade_xml_get_widget (main_xml, "toolbutton_quit");
  gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (toolitem1),
                             GTK_TOOLBAR (toolbar1)->tooltips,
                             _("Quit"), "");
}

void bind_menubar_callbacks (GladeXML *main_xml, GtkWidget *treeview_menu)
{
  GtkWidget *menuitem;

  menuitem = glade_xml_get_widget (main_xml, "menu_save");
  g_signal_connect (menuitem, "activate",
                    G_CALLBACK (on_menu_save_activate), treeview_menu);

  menuitem = glade_xml_get_widget (main_xml, "menu_new");
  g_signal_connect (menuitem, "activate",
                    G_CALLBACK (on_menu_new_activate), treeview_menu);

  menuitem = glade_xml_get_widget (main_xml, "menu_icon");
  g_signal_connect (menuitem, "activate",
                    G_CALLBACK (on_menu_icon_activate), treeview_menu);

  menuitem = glade_xml_get_widget (main_xml, "menu_delete");
  g_signal_connect (menuitem, "activate",
                    G_CALLBACK (on_menu_delete_activate), treeview_menu);

}

void on_descrenderer_edited (GtkCellRendererText *celltext,
                             const gchar *string_path,
                             const gchar *new_text,
                             gpointer data)
{
  GtkTreeModel *model = GTK_TREE_MODEL(data);
  GtkTreeIter iter;
  gchar *desc = NULL;

  gtk_tree_model_get_iter_from_string (model, &iter, string_path);
  gtk_tree_model_get (model, &iter, COL_DESCRIPTION, &desc, -1);
  gtk_tree_store_set (GTK_TREE_STORE (model), &iter,
                      COL_DESCRIPTION, new_text,
                      -1);

  g_free (desc);
}

void on_paramsrenderer_edited (GtkCellRendererText *celltext,
                               const gchar *string_path,
                               const gchar *new_text,
                               gpointer data)
{
  GtkTreeModel *model = GTK_TREE_MODEL(data);
  GtkTreeIter iter;
  gchar *params = NULL;

  gtk_tree_model_get_iter_from_string (model, &iter, string_path);
  gtk_tree_model_get (model, &iter, COL_PARAMS, &params, -1);
  gtk_tree_store_set (GTK_TREE_STORE (model), &iter,
                      COL_PARAMS, new_text,
                      -1);

  g_free (params);
}

void on_iconcolumn_clicked (GtkTreeViewColumn *treeviewcolumn,
                            gpointer user_data)

{
}


void on_menu_save_activate (GtkMenuItem *menuitem,
                            gpointer user_data)
{
  GtkWidget *treeview_menu;

  treeview_menu = (GtkWidget*) user_data;
  save_table_to_menu (treeview_menu);
}

void on_menu_info_activate (GtkMenuItem *menuitem,
                            gpointer user_data)
{
  GladeXML *info_xml;
  GtkWidget *info_window;
  GtkWidget *logo_image;

  info_xml = glade_xml_new (PACKAGE_SOURCE_DIR"/e16menuedit2.glade",
                            "info_window", NULL);
  glade_xml_signal_autoconnect (info_xml);

  info_window = glade_xml_get_widget (info_xml, "info_window");

  logo_image = glade_xml_get_widget (info_xml, "logo_image");


  gtk_image_set_from_file (GTK_IMAGE (logo_image),
                           PACKAGE_PIXMAPS_DIR"/e16menuedit2-icon.png");


  gtk_widget_show (info_window);
}

void on_toolbutton_save_clicked (GtkToolButton *toolbutton,
                                 gpointer user_data)
{
  GtkWidget *treeview_menu;

  treeview_menu = (GtkWidget*) user_data;
  save_table_to_menu (treeview_menu);
}


void on_toolbutton_new_clicked (GtkToolButton *toolbutton,
                                gpointer user_data)
{
  GtkWidget *treeview_menu;

  treeview_menu = (GtkWidget*) user_data;
  new_table_row (treeview_menu);
}


void on_toolbutton_icon_clicked (GtkToolButton *toolbutton,
                                 gpointer user_data)
{
  open_icon_chooser ((GtkWidget *) user_data);
}


void on_toolbutton_del_clicked (GtkToolButton *toolbutton,
                                gpointer user_data)
{
  GtkWidget *treeview_menu;

  treeview_menu = (GtkWidget*) user_data;
  delete_table_row (treeview_menu);
}

void on_menu_new_activate (GtkMenuItem *menuitem,
                           gpointer user_data)
{
  GtkWidget *treeview_menu;

  treeview_menu = (GtkWidget*) user_data;
  new_table_row (treeview_menu);
}

void on_menu_icon_activate (GtkMenuItem *menuitem,
                            gpointer user_data)
{
  open_icon_chooser ((GtkWidget *) user_data);
}

void on_menu_delete_activate (GtkMenuItem *menuitem,
                              gpointer user_data)
{
  GtkWidget *treeview_menu;

  treeview_menu = (GtkWidget*) user_data;
  delete_table_row (treeview_menu);
}
