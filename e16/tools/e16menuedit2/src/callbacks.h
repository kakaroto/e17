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
 *  File: callbacks.h
 *  Created by: Andreas Volz <linux@brachttal.net>
 *
 */

#ifndef _CALLBACKS_H
#define _CALLBACKS_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gtk/gtk.h>
#include <glade/glade.h>
#include "libglade_support.h"

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

/* defines for parser */
#define KEY_LENGTH 50
#define VALUE_LENGTH 100

void bind_toolbar_callbacks (GtkWidget *treeview_menu);
void bind_menubar_callbacks (GtkWidget *treeview_menu);
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

gboolean
on_treeview_menu_button_press_event (GtkWidget *widget,
                                     GdkEventButton *event,
                                     gpointer user_data);

gboolean
on_pop_menu1_destroy_event (GtkWidget *widget,
                            GdkEvent *event,
                            gpointer user_data);

void on_menu_contents_activate (GtkMenuItem *menuitem,
                                gpointer user_data);
void
on_menu_properties_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
					
void
on_properties_close_clicked            (GtkButton       *button,
                                        gpointer         user_data);

#endif /* _CALLBACKS_H */
