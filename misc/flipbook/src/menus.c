/*****************************************************************************/
/* VA Linux Systems Flipbook demo                                            */
/*****************************************************************************/
/*
 * Copyright (C) 2000 Brad Grantham, Geoff Harrison, and VA Linux Systems
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
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "menus.h"


GtkWidget *
CreateBarSubMenu(GtkWidget *menu, char *szName) {
	GtkWidget *menuitem;
	GtkWidget *submenu;

	menuitem = gtk_menu_item_new_with_label(szName);
	gtk_menu_bar_append(GTK_MENU_BAR(menu), menuitem);
	gtk_widget_show(menuitem);
	submenu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),submenu);

	return submenu;

}

GtkWidget *
CreateRightAlignBarSubMenu(GtkWidget *menu, char *szName) {
	GtkWidget *menuitem;
	GtkWidget *submenu;

	menuitem = gtk_menu_item_new_with_label(szName);
	gtk_menu_item_right_justify (GTK_MENU_ITEM (menuitem));
	gtk_menu_bar_append(GTK_MENU_BAR(menu), menuitem);
	gtk_widget_show(menuitem);
	submenu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),submenu);

	return submenu;

}

GtkWidget *
CreateMenuItem(GtkWidget *menu, char *szName, char *szAccel, char *szTip,
	   	GtkSignalFunc func, gpointer data)
{
	GtkWidget *menuitem;

	if(szName && strlen (szName)) {
		menuitem = gtk_menu_item_new_with_label(szName);
		gtk_signal_connect(GTK_OBJECT(menuitem),"activate",
				GTK_SIGNAL_FUNC(func),data);
	} else {
		menuitem = gtk_menu_item_new();
	}

	gtk_menu_append(GTK_MENU(menu),menuitem);
	gtk_widget_show(menuitem);
	
	if(szAccel && szAccel[0] == '^') {
		gtk_widget_add_accelerator(menuitem,"activate",accel_group,szAccel[1],
				GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	}

	if(szTip && strlen(szTip)) {
		gtk_tooltips_set_tip(tooltips,menuitem,szTip,NULL);
	}

	return menuitem;

}
