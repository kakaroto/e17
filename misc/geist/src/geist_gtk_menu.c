
#include "geist.h"
#include "geist_gtk_menu.h"

GtkAccelGroup *accel_group;
GtkTooltips *tooltips;

GtkWidget *
geist_gtk_create_submenu(GtkWidget * menu, char *szName)
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
geist_gtk_create_menuitem(GtkWidget * menu, char *szName, char *szAccel,
                          char *szTip, GtkFunction func, gpointer data)
{
   GtkWidget *menuitem;

   if (szName && strlen(szName))
   {
      menuitem = gtk_menu_item_new_with_label(szName);
      gtk_signal_connect(GTK_OBJECT(menuitem), "activate",
                         GTK_SIGNAL_FUNC(func), data);
   }
   else
   {
      menuitem = gtk_menu_item_new();
   }

   gtk_menu_append(GTK_MENU(menu), menuitem);
   gtk_widget_show(menuitem);

   if (szAccel && szAccel[0] == '^')
   {
      gtk_widget_add_accelerator(menuitem, "activate", accel_group,
                                 szAccel[1], GDK_CONTROL_MASK,
                                 GTK_ACCEL_VISIBLE);
   }

   if (szTip && strlen(szTip))
   {
      gtk_tooltips_set_tip(tooltips, menuitem, szTip, NULL);
   }

   return menuitem;

}
