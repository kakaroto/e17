#include <gdk_imlib.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "viewer.h"
#include "menus.h"


extern GtkTooltips *tooltips;
extern GtkAccelGroup *accel_group;

GtkWidget *ctree;

GtkWidget *
create_main_window(void)
{
	GtkWidget *win;
	GtkWidget *bigvbox;
	GtkWidget *menubar;
	GtkWidget *panes;
	GtkWidget *scrollybit;
	GtkWidget *vbox;
	GtkWidget *vbox2;
	GtkWidget *table;
	GtkWidget *frames;
	GtkWidget *button;
	GtkWidget *entry;
	GtkWidget *label;
	GtkWidget *alignment;
	GtkWidget *hbox;

	win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_object_set_data(GTK_OBJECT(win),"menu_editor",win);
	GTK_WIDGET_SET_FLAGS(win,GTK_CAN_FOCUS);
	GTK_WIDGET_SET_FLAGS(win,GTK_CAN_DEFAULT);
	gtk_window_set_title(GTK_WINDOW(win),"E Menu Editor");

	bigvbox = gtk_vbox_new(FALSE,0);
	gtk_widget_show(bigvbox);
	gtk_container_add(GTK_CONTAINER(win), bigvbox);

	menubar = gtk_menu_bar_new();
	gtk_widget_show(menubar);
	gtk_box_pack_start(GTK_BOX(bigvbox), menubar, FALSE, FALSE, 0);

	{
		GtkWidget *menu;
		GtkWidget *menuitem;

		menu = CreateBarSubMenu(menubar,"File");
		menuitem = CreateMenuItem(menu,"Save","","Save Current Data",NULL,
				"save data");
		menuitem = CreateMenuItem(menu,"Save & Quit","",
				"Save Current Data & Quit Application",NULL, "save quit");
		menuitem = CreateMenuItem(menu,"Quit","","Quit Without Saving",NULL,
				"quit program");

	}

	{
		GtkWidget *menu;
		GtkWidget *menuitem;

		menu = CreateRightAlignBarSubMenu(menubar,"Help");
		menuitem = CreateMenuItem(menu,"About","","About E Menu Editor",
				NULL, "about");
		menuitem = CreateMenuItem(menu,"Documentation","",
				"Read the Menu Editor Documentation",NULL, "read docs");

	}

	panes = gtk_hpaned_new();
	gtk_widget_show(panes);
	gtk_paned_set_gutter_size(GTK_PANED(panes), 10);
	gtk_box_pack_start(GTK_BOX(bigvbox), panes, TRUE, TRUE, 0);

	scrollybit = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrollybit);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollybit),
			GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_paned_pack1(GTK_PANED(panes), scrollybit, TRUE, FALSE);

	ctree = gtk_ctree_new(5,0);
	gtk_widget_show(ctree);
	gtk_container_add(GTK_CONTAINER(scrollybit),ctree);

	vbox = gtk_vbox_new(FALSE,3);
	gtk_widget_show(vbox);
	gtk_paned_pack2(GTK_PANED(panes), vbox, FALSE, TRUE);

	frames = gtk_frame_new("Edit Menu Item Properties");
	gtk_container_set_border_width(GTK_CONTAINER(frames),2);
	gtk_widget_show(frames);
	gtk_box_pack_start(GTK_BOX(vbox),frames,TRUE,TRUE,0);

	vbox2 = gtk_vbox_new(FALSE,3);
	gtk_widget_show(vbox2);
	gtk_container_add(GTK_CONTAINER(frames),vbox2);

	table = gtk_table_new(4,3,FALSE);
	gtk_widget_show(table);
	gtk_table_set_row_spacings(GTK_TABLE(table),3);
	gtk_table_set_col_spacings(GTK_TABLE(table),3);
	gtk_box_pack_start(GTK_BOX(vbox2),table,FALSE,FALSE,2);

	hbox = gtk_hbox_new(FALSE,3);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,2);

	button = gtk_button_new_with_label(" Insert Menu Entry ");
	gtk_widget_show(button);
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,FALSE,2);

	button = gtk_button_new_with_label(" Insert New SubMenu ");
	gtk_widget_show(button);
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,FALSE,2);

	hbox = gtk_hbox_new(FALSE,3);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,2);

	button = gtk_button_new_with_label(" Delete Menu Entry ");
	gtk_widget_show(button);
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,FALSE,2);

	button = gtk_button_new_with_label(" Save ");
	gtk_widget_show(button);
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,FALSE,2);

	button = gtk_button_new_with_label(" Quit ");
	gtk_widget_show(button);
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,FALSE,2);


	return win;
}

void
on_exit_application(GtkWidget * widget, gpointer user_data)
{

	if (user_data) {
		widget = NULL;
	}
	gtk_exit(0);

}


int
main(int argc, char *argv[])
{
	GtkWidget *main_win;

	gtk_set_locale();
	gtk_init(&argc,&argv);
	gdk_imlib_init();

	tooltips = gtk_tooltips_new();
	accel_group = gtk_accel_group_new();

	gtk_widget_push_visual(gdk_imlib_get_visual());
	gtk_widget_push_colormap(gdk_imlib_get_colormap());

	main_win = create_main_window();
	gtk_widget_show(main_win);
	gtk_signal_connect(GTK_OBJECT(main_win), "destroy",
			GTK_SIGNAL_FUNC(on_exit_application), NULL);
	gtk_signal_connect(GTK_OBJECT(main_win), "delete_event",
			GTK_SIGNAL_FUNC(on_exit_application), NULL);

	gtk_main();

	return 0;
}
