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
 *  File: toolbar.c
 *  Created by: Andreas Volz <linux@brachttal.net>
 *
 */

#include "toolbar.h"
#include "callbacks.h"
#include "nls.h"

void create_toolbar (GtkWidget *toolbar1, GtkWidget *treeview_menu)
{
  GtkToolItem *toolitem1;

  /* new entry toolbar button */
  toolitem1 = gtk_tool_button_new_from_stock (GTK_STOCK_NEW);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar1), toolitem1, TB_NEW);
  gtk_widget_show (GTK_WIDGET (toolitem1));
  gtk_tool_item_set_tooltip (toolitem1, GTK_TOOLBAR (toolbar1)->tooltips,
                             _("New Entry"), "");
  g_signal_connect (toolitem1, "clicked",
                    G_CALLBACK (on_new_button), treeview_menu);

  /* change icon toolbar button */
  toolitem1 = gtk_tool_button_new_from_stock (GTK_STOCK_SELECT_COLOR);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar1), toolitem1, TB_CHANGE_ICON);
  gtk_widget_show (GTK_WIDGET (toolitem1));
  gtk_tool_item_set_tooltip (toolitem1, GTK_TOOLBAR (toolbar1)->tooltips,
                             _("Change Icon"), "");
  g_signal_connect (toolitem1, "clicked",
                    G_CALLBACK (on_change_icon_button), treeview_menu);

  /* save toolbar button */
  toolitem1 = gtk_tool_button_new_from_stock (GTK_STOCK_SAVE);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar1), toolitem1, TB_SAVE);
  gtk_widget_show (GTK_WIDGET (toolitem1));
  gtk_tool_item_set_tooltip (toolitem1, GTK_TOOLBAR (toolbar1)->tooltips,
                             _("Save Menu"), "");
  g_signal_connect (toolitem1, "clicked",
                    G_CALLBACK (on_save_button), treeview_menu);

  /* delete toolbar button */
  toolitem1 = gtk_tool_button_new_from_stock (GTK_STOCK_DELETE);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar1), toolitem1, TB_DELETE);
  gtk_widget_show (GTK_WIDGET (toolitem1));
  gtk_tool_item_set_tooltip (toolitem1, GTK_TOOLBAR (toolbar1)->tooltips,
                             _("Delete Entry or Submenu"), "");
  g_signal_connect (toolitem1, "clicked",
                    G_CALLBACK (on_delete_button), treeview_menu);

  /* quit toolbar button */
  toolitem1 = gtk_tool_button_new_from_stock (GTK_STOCK_QUIT);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar1), toolitem1, TB_QUIT);
  gtk_widget_show (GTK_WIDGET (toolitem1));
  gtk_tool_item_set_tooltip (toolitem1, GTK_TOOLBAR (toolbar1)->tooltips,
                             _("Quit Menu Editor"), "");
  g_signal_connect (toolitem1, "clicked",
                    G_CALLBACK (gtk_main_quit), NULL);
}
