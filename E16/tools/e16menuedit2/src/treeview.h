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
 *  File: treeview.h
 *  Created by: Andreas Volz <linux@brachttal.net>
 *
 */
 
#ifndef _TREEVIEW_H
#define _TREEVIEW_H

#include <gtk/gtk.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

void create_tree_model (GtkWidget *treeview_menu);
void new_table_row (GtkWidget *treeview_menu);
void delete_table_row (GtkWidget *treeview_menu);
void save_table_to_menu (GtkWidget *treeview_menu);
void activate_dragndrop (GtkTreeView *treeview_menu);
void deactivate_dragndrop (GtkTreeView *treeview_menu);

enum tree_columns
{
  COL_DESCRIPTION,
  COL_ICON,
  COL_ICONNAME,
  COL_PARAMS,
  TREE_COLUMNS_SIZE
};

enum
{
  TARGET_GTK_TREE_MODEL_ROW
};

#endif /* _TREEVIEW_H */
