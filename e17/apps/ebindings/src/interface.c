/**************************************************************************
 * Name: interface.c 
 * Project: Ebindings
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * Date: October 10, 2001
 * Description: Gtk Gui used is built here, tabs can easily be added with
 * function calls, see add_menu_notebook for an example. 
 *************************************************************************/
#include"interface.h"

/* app_exit:
 * 	Signal can be attached to widgets to quit the gtk display 
 */
static gint
app_exit(GtkWidget * w, gpointer data)
{
   gtk_main_quit();
   return FALSE;
   UN(w);
   UN(data);
}

/* create_main_ebindings_window
 * returns a pointer to the GtkWidget that is the created.  Creates a menu
 * and a notebook.  functions to add features to the notebook can be added
 * at the bottom.  NOTHING else needs to go in here but notebook page
 * addition function calls
 */
GtkWidget *
create_main_ebindings_window(void)
{
   GtkWidget *win;
   GtkWidget *vbox;
   GtkWidget *notebook;
   GtkWidget *menu_bar;
   GtkWidget *menu1;
   GtkWidget *f_menu;
   GtkWidget *menu_quit;
   GtkWidget *status;

   win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   gtk_window_set_title(GTK_WINDOW(win), "Ebindings");
   gtk_window_set_wmclass(GTK_WINDOW(win), "Ebindings", "Main");
   gtk_widget_set_usize(GTK_WIDGET(win), 600, 300);
   gtk_signal_connect(GTK_OBJECT(win), "delete_event",
                      GTK_SIGNAL_FUNC(app_exit), NULL);
   gtk_widget_ref(win);
   gtk_object_set_data_full(GTK_OBJECT(win), "win", win,
                            (GtkDestroyNotify) gtk_widget_unref);

   vbox = gtk_vbox_new(FALSE, 1);

   notebook = gtk_notebook_new();
   gtk_object_set_data_full(GTK_OBJECT(win), "ebindings_notebook", notebook,
                            (GtkDestroyNotify) gtk_widget_unref);
   gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_LEFT);

   /* setup the status bar too */
   status = gtk_statusbar_new();

   /* setup the menu because menus are in everything */
   menu_bar = gtk_menu_bar_new();
   gtk_widget_show(menu_bar);

   f_menu = gtk_menu_item_new_with_label("File");
   gtk_menu_bar_append(GTK_MENU_BAR(menu_bar), f_menu);
   gtk_widget_show(f_menu);

   menu1 = gtk_menu_new();
   gtk_menu_item_set_submenu(GTK_MENU_ITEM(f_menu), menu1);
   gtk_widget_show(menu1);

   menu_quit = gtk_menu_item_new_with_label("Quit");
   gtk_menu_append(GTK_MENU(menu1), menu_quit);
   gtk_signal_connect(GTK_OBJECT(menu_quit), "activate",
                      GTK_SIGNAL_FUNC(app_exit), NULL);
   gtk_widget_show(menu_quit);
   /* done with boring menus blah blah */

   add_menu_notebook(win, notebook, 0);
   add_keybinds_notebook(win, notebook, 1);
   add_focus_notebook(win, notebook, 2);

   gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 1);
   gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 1);
   gtk_box_pack_start(GTK_BOX(vbox), status, FALSE, FALSE, 1);
   gtk_container_add(GTK_CONTAINER(win), vbox);

   gtk_widget_show(status);
   gtk_widget_show(notebook);
   gtk_widget_show(vbox);
   gtk_widget_show(win);

   ebindings_statusbar = status;

   return win;
}
