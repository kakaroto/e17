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
