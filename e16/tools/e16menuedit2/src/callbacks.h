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
 *  File: callbacks.h
 *  Created by: Andreas Volz <linux@brachttal.net>
 *
 */

#ifndef _CALLBACKS_H
#define _CALLBACKS_H

#include <gtk/gtk.h>
#include <glade/glade.h>

#define ICON_SIZE_AUTO_STRING "auto (64x64 limit)"
#define ICON_SIZE1_STRING "64x64"
#define ICON_SIZE2_STRING "48x48"
#define ICON_SIZE3_STRING "36x36"
#define ICON_SIZE4_STRING "32x32"
#define ICON_SIZE5_STRING "24x24"
#define ICON_SIZE6_STRING "22x22"
#define ICON_SIZE7_STRING "16x16"

#define ICON_SIZE_AUTO 64
#define ICON_SIZE1 64
#define ICON_SIZE2 48
#define ICON_SIZE3 36
#define ICON_SIZE4 32
#define ICON_SIZE5 24
#define ICON_SIZE6 22
#define ICON_SIZE7 16

void bind_toolbar_callbacks (GladeXML *main_xml, GtkWidget *treeview_menu);
void bind_menubar_callbacks (GladeXML *main_xml, GtkWidget *treeview_menu);
void on_menu_save_activate (GtkMenuItem *menuitem,
                            gpointer user_data);
void on_descrenderer_edited (GtkCellRendererText *celltext,
                             const gchar *string_path,
                             const gchar *new_text,
                             gpointer data);
void on_paramsrenderer_edited (GtkCellRendererText *celltext,
                               const gchar *string_path,
                               const gchar *new_text,
                               gpointer data);
void on_iconcolumn_clicked (GtkTreeViewColumn *treeviewcolumn,
                            gpointer user_data);
void on_menu_info_activate (GtkMenuItem *menuitem,
                            gpointer user_data);
void on_toolbutton_save_clicked (GtkToolButton *toolbutton,
                                 gpointer user_data);

void on_toolbutton_new_clicked (GtkToolButton *toolbutton,
                                gpointer         user_data);

void on_toolbutton_icon_clicked (GtkToolButton *toolbutton,
                                 gpointer user_data);

void on_toolbutton_del_clicked (GtkToolButton *toolbutton,
                                gpointer user_data);
void on_menu_new_activate (GtkMenuItem *menuitem,
                      gpointer user_data);
void on_menu_icon_activate (GtkMenuItem *menuitem,
                       gpointer user_data);
void on_menu_delete_activate (GtkMenuItem *menuitem,
                         gpointer user_data);
			 
#endif /* _CALLBACKS_H */
