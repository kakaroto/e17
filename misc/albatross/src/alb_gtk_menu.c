/* alb_gtk_menu.c

Copyright (C) 1999,2000 Tom Gilbert.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/


#include "alb.h"
#include "alb_gtk_menu.h"

GtkAccelGroup *accel_group;
GtkTooltips *tooltips;

GtkWidget *
alb_gtk_create_submenu(GtkWidget * menu,
                       char *szName)
{
  GtkWidget *menuitem;
  GtkWidget *submenu;

  menuitem = gtk_menu_item_new_with_label(szName);
  gtk_menu_bar_append(GTK_MENU_BAR(menu), menuitem);
  gtk_widget_show(menuitem);
  submenu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), submenu);

  return submenu;

}

GtkWidget *
alb_gtk_create_menuitem(GtkWidget * menu,
                        char *szName,
                        char *szAccel,
                        char *szTip,
                        GtkFunction func,
                        gpointer data)
{
  GtkWidget *menuitem;

  if (szName && strlen(szName)) {
    menuitem = gtk_menu_item_new_with_label(szName);
    gtk_signal_connect(GTK_OBJECT(menuitem), "activate",
                       GTK_SIGNAL_FUNC(func), data);
  } else {
    menuitem = gtk_menu_item_new();
  }

  gtk_menu_append(GTK_MENU(menu), menuitem);
  gtk_widget_show(menuitem);

  if (szAccel && szAccel[0] == '^') {
    gtk_widget_add_accelerator(menuitem, "activate", accel_group, szAccel[1],
                               GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  }

  if (szTip && strlen(szTip)) {
    gtk_tooltips_set_tip(tooltips, menuitem, szTip, NULL);
  }

  return menuitem;

}
