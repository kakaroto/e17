#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "viewer.h"
#include "menus.h"
#include "ipc.h"

extern GtkTooltips *tooltips;
extern GtkAccelGroup *accel_group;

GtkWidget *clist;

static void receive_ipc_msg(gchar * msg);
gchar *e_ipc_msg = NULL;


void
selection_made(GtkWidget *clist, gint row, gint column, GdkEventButton *event,
		                    gpointer data)
{

	return;
}

void
on_resort_columns(GtkWidget *widget, gint column, gpointer user_data)
{
	static int order=0;
	static int last_col=0;

	if(user_data) {
		widget = NULL;
	}
	gtk_clist_set_sort_column(GTK_CLIST(clist),column);
	if(last_col == column) {
		if(order) {
			order=0;
			gtk_clist_set_sort_type(GTK_CLIST(clist),GTK_SORT_DESCENDING);
		} else {
			order=1;
			gtk_clist_set_sort_type(GTK_CLIST(clist),GTK_SORT_ASCENDING);
		}
	} else {
		order=1;
		gtk_clist_set_sort_type(GTK_CLIST(clist),GTK_SORT_ASCENDING);
		last_col = column;
	}

	gtk_clist_sort(GTK_CLIST(clist));

	return;

}


void
on_exit_application(GtkWidget * widget, gpointer user_data)
{

	if (user_data) {
		widget = NULL;
	}
	gtk_exit(0);

}

GtkWidget *
create_list_window(void)
{

	GtkWidget *list_window;
	GtkWidget *bigvbox;
	GtkWidget *menubar;
	GtkWidget *panes;
	GtkWidget *scrollybit;
	GtkWidget *vbox;
	GtkWidget *frames;
	GtkWidget *alignment;
	GtkWidget *frame_vbox;


	list_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_object_set_data(GTK_OBJECT(list_window), "key_editor", list_window);
	GTK_WIDGET_SET_FLAGS(list_window, GTK_CAN_FOCUS);
	GTK_WIDGET_SET_FLAGS(list_window, GTK_CAN_DEFAULT);
	gtk_window_set_title(GTK_WINDOW(list_window), "E Keys Editor");

	bigvbox = gtk_vbox_new(FALSE,0);
	gtk_widget_show(bigvbox);
	gtk_container_add(GTK_CONTAINER(list_window), bigvbox);

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
		menuitem = CreateMenuItem(menu,"About","","About E Keybinding Editor",
				NULL, "about");
		menuitem = CreateMenuItem(menu,"Documentation","",
				"Read the Keybinding Editor Documentation",NULL, "read docs");

	}

	panes = gtk_hpaned_new();
	gtk_widget_show(panes);
	gtk_paned_set_gutter_size(GTK_PANED(panes), 10);
	gtk_box_pack_start(GTK_BOX(bigvbox), panes, TRUE, TRUE, 0);

	scrollybit = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrollybit);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollybit),
			GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
	gtk_paned_pack1(GTK_PANED(panes), scrollybit, TRUE, FALSE);

	clist = gtk_clist_new(4);
	gtk_widget_show(clist);
	gtk_container_add(GTK_CONTAINER(scrollybit), clist);

	gtk_clist_set_column_title(GTK_CLIST(clist), 0, "Modifier");
	gtk_clist_set_column_title(GTK_CLIST(clist), 1, "Key");
	gtk_clist_set_column_title(GTK_CLIST(clist), 2, "Action to perform");
	gtk_clist_set_column_title(GTK_CLIST(clist), 3, "Optional Parameters");
	gtk_clist_column_titles_show(GTK_CLIST(clist));
	gtk_signal_connect(GTK_OBJECT(clist), "select_row",
			GTK_SIGNAL_FUNC(selection_made), NULL);
	gtk_signal_connect(GTK_OBJECT(clist), "click_column",
			GTK_SIGNAL_FUNC(on_resort_columns), NULL);

	vbox = gtk_vbox_new(FALSE,0);
	gtk_widget_show(vbox);
	frames = gtk_frame_new("Edit Keybinding Properties");
	gtk_container_set_border_width(GTK_CONTAINER(frames),2);
	gtk_widget_show(frames);
	gtk_paned_pack2(GTK_PANED(panes),vbox,FALSE,TRUE);
	gtk_box_pack_start(GTK_BOX(vbox), frames, TRUE, TRUE, 0);


	frame_vbox = gtk_vbox_new(FALSE,3);
	gtk_widget_show(frame_vbox);

	gtk_container_set_border_width(GTK_CONTAINER(frame_vbox),4);
	gtk_container_add(GTK_CONTAINER(frames), frame_vbox);
			

	return list_window;

}

static void receive_ipc_msg(gchar * msg)
{
	gdk_flush();
	e_ipc_msg = g_strdup(msg);
	/* */
	gtk_main_quit();
	/*  got_e_ipc_msg = 1;*/
}

int main(int argc, char *argv[])
{
	GtkWidget *lister;

	gtk_set_locale();
	gtk_init(&argc, &argv);

	tooltips = gtk_tooltips_new();
	accel_group = gtk_accel_group_new();

	if(!CommsInit(receive_ipc_msg)) {
		GtkWidget *win, *label, *align, *frame, *button, *vbox;

		win = gtk_window_new(GTK_WINDOW_POPUP);
		gtk_window_set_policy(GTK_WINDOW(win), 0, 0, 1);
		gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);
		frame = gtk_frame_new(NULL);
		gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_OUT);
		align = gtk_alignment_new(0.0, 0.0, 0.0, 0.0);
		gtk_container_set_border_width(GTK_CONTAINER(align), 32);
		vbox = gtk_vbox_new(FALSE, 5);
		button = gtk_button_new_with_label("Quit");
		gtk_signal_connect(GTK_OBJECT(button), "clicked",
				GTK_SIGNAL_FUNC(on_exit_application), NULL);
		label = gtk_label_new("You are not running Enlightenment\n"
					"\n"
					"This window manager has to be running in order\n"
					"to configure it.\n" "\n");
		gtk_container_add(GTK_CONTAINER(win), frame);
		gtk_container_add(GTK_CONTAINER(frame), align);
		gtk_container_add(GTK_CONTAINER(align), vbox);
		gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
		gtk_widget_show_all(win);
		gtk_main();
		exit(1);
	}
	CommsSend("set clientname Enlightenment Configuration Utility");
	CommsSend("set version 0.1.0");
	CommsSend("set author Mandrake (Geoff Harrison)");
	CommsSend("set email mandrake@mandrake.net");
	CommsSend("set web http://mandrake.net/");
	CommsSend("set address C/O VA Linux Systems, USA");
	CommsSend("set info "
			"This is the Enlightenemnt KeyBindings Configuration Utility\n"
			"that uses Enlightenment's IPC mechanism to configure\n"
			"it remotely.");

	lister = create_list_window();

	gtk_widget_show(lister);
	gtk_signal_connect(GTK_OBJECT(lister), "destroy",
			GTK_SIGNAL_FUNC(on_exit_application), NULL);
	gtk_signal_connect(GTK_OBJECT(lister), "delete_event",
			GTK_SIGNAL_FUNC(on_exit_application), NULL);

	gtk_main();

	return 0;
}
