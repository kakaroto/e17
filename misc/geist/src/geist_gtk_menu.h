#ifndef GEIST_GTK_MENU_H
#define GEIST_GTK_MENU_H
extern GtkAccelGroup *accel_group;
extern GtkTooltips *tooltips;

GtkWidget *geist_gtk_create_submenu(GtkWidget * menu, char *szName);
GtkWidget *geist_gtk_create_menuitem(GtkWidget * menu, char *szName,
                                     char *szAccel, char *szTip,
                                     GtkFunction func, gpointer data);
#endif
