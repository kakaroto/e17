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
#include "file.h"


extern GtkTooltips *tooltips;
extern GtkAccelGroup *accel_group;
GtkWidget *descriptionfield;
GtkWidget *iconfield;
GtkWidget *execfield;
int execedit=1;

GtkWidget *ctree;
GtkCTreeNode *parent;

void on_select_submenu_box(GtkWidget *widget, gpointer user_data) {

	if(user_data) {
		widget = NULL;
	}
	if(execedit) {
		gtk_entry_set_editable(GTK_ENTRY(execfield),FALSE);
		gtk_widget_set_sensitive(execfield,FALSE);
		execedit = 0;
	} else {
		gtk_entry_set_editable(GTK_ENTRY(execfield),TRUE);
		gtk_widget_set_sensitive(execfield,TRUE);
		execedit = 1;
	}

	return;
}

void load_new_menu_from_disk(char *file_to_load, GtkCTreeNode *my_parent) {

	FILE *menufile;
	char buf[1024];
	char first=1;
	char s[4096];

	sprintf(buf,"%s/.enlightenment/%s",homedir(getuid()),file_to_load);
	menufile=fopen(buf,"r");
	if(!menufile) {
		return;
	}

	while(fgets(s,4096,menufile)) {
		s[strlen(s) - 1] = 0;
		if((s[0] && s[0] !=  '#')) {
			if(first) {
				first = 0;
			} else {
				char *txt = NULL, *icon = NULL, *act = NULL, *params = NULL;
				gchar *text[3];


				txt = field(s, 0);
				icon = field(s, 1);
				act = field(s, 2);
				params = field(s, 3);

				text[0] = txt;
				text[1] = icon;
				text[2] = params;

				/* printf("subitem: %s, %s, %s, %s\n",txt,icon,act,params); */
				gtk_ctree_insert_node (GTK_CTREE(ctree), my_parent, NULL,
					   	text, 5, NULL,NULL,NULL,NULL, FALSE, FALSE);

				if(txt)
					free(txt);
				if(icon)
					free(icon);
				if(act)
					free(act);
				if(params)
					free(params);

			}
		}
	}

	fclose(menufile);

	return;
}

void load_menus_from_disk(void) {

	FILE *menufile;
	char buf[1024];
	char first=1;
	char s[4096];

	sprintf(buf,"%s/.enlightenment/file.menu",homedir(getuid()));
	menufile=fopen(buf,"r");
	if(!menufile) {
		printf("hmm. looks like you have some \"issues\" as you don't have\n"
				"a %s file.  Sucks to be you\n",buf);
		exit(1);
	}

	while(fgets(s,4096,menufile)) {
		s[strlen(s) - 1] = 0;
		if((s[0] && s[0] !=  '#')) {
			if(first) {
				gchar *text[3];
				char *txt = NULL;
				char *txt2 = NULL;
				char *txt3 = NULL;
				char *txt4 = NULL;

				txt = field(s, 0);
				text[0] = txt;
				text[1] = txt2;
				text[2] = txt4;

				parent = gtk_ctree_insert_node (GTK_CTREE(ctree), NULL, NULL,
					   	text, 5, NULL,NULL,NULL,NULL, FALSE, TRUE);
				/* printf("mainitem: %s, %s, %s, %s\n",txt,txt2,txt3,txt4); */

				if(txt)
					free(txt);

				first =0;

			} else {
				char *txt = NULL, *icon = NULL, *act = NULL, *params = NULL;
				gchar *text[3];
				GtkCTreeNode *current;


				txt = field(s, 0);
				icon = field(s, 1);
				act = field(s, 2);
				params = field(s, 3);

				text[0] = txt;
				text[1] = icon;
				text[2] = params;

				/* printf("subitem: %s, %s, %s, %s\n",txt,icon,act,params); */
				current = gtk_ctree_insert_node (GTK_CTREE(ctree), parent, NULL,
					   	text, 5, NULL,NULL,NULL,NULL, FALSE, FALSE);

				if(!strcmp(act,"menu")) {
					load_new_menu_from_disk(params,current);
				}

				if(txt)
					free(txt);
				if(icon)
					free(icon);
				if(act)
					free(act);
				if(params)
					free(params);

			}
		}
	}

	fclose(menufile);

	return;
}

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
	GtkWidget *checkbox;

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

	ctree = gtk_ctree_new(3,0);
	gtk_widget_show(ctree);
	gtk_ctree_set_line_style (GTK_CTREE(ctree), GTK_CTREE_LINES_DOTTED);
	gtk_clist_set_column_auto_resize (GTK_CLIST (ctree), 0, TRUE);
	gtk_clist_set_column_title(GTK_CLIST(ctree), 0, "Description");
	gtk_clist_set_column_title(GTK_CLIST(ctree), 1, "Icon");
	gtk_clist_set_column_title(GTK_CLIST(ctree), 2, "Params");
	gtk_clist_column_titles_show(GTK_CLIST(ctree));
	gtk_container_add(GTK_CONTAINER(scrollybit),ctree);

	vbox = gtk_vbox_new(FALSE,3);
	gtk_widget_show(vbox);
	gtk_paned_pack2(GTK_PANED(panes), vbox, FALSE, TRUE);
	gtk_container_set_border_width(GTK_CONTAINER(vbox),2);

	frames = gtk_frame_new("Edit Menu Item Properties");
	gtk_container_set_border_width(GTK_CONTAINER(frames),2);
	gtk_widget_show(frames);
	gtk_box_pack_start(GTK_BOX(vbox),frames,TRUE,TRUE,0);

	vbox2 = gtk_vbox_new(FALSE,3);
	gtk_widget_show(vbox2);
	gtk_container_add(GTK_CONTAINER(frames),vbox2);
	gtk_container_set_border_width(GTK_CONTAINER(vbox2),4);

	hbox = gtk_hbox_new(FALSE,3);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox2),hbox,FALSE,FALSE,2);

	checkbox = gtk_check_button_new_with_label("Is Active");
	gtk_widget_show(checkbox);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox),TRUE);
	gtk_box_pack_start(GTK_BOX(hbox),checkbox,TRUE,FALSE,2);

	checkbox = gtk_check_button_new_with_label("Is SubMenu");
	gtk_widget_show(checkbox);
	gtk_box_pack_start(GTK_BOX(hbox),checkbox,TRUE,FALSE,2);
	gtk_signal_connect(GTK_OBJECT(checkbox),"toggled",
			GTK_SIGNAL_FUNC(on_select_submenu_box),NULL);

	table = gtk_table_new(3,3,FALSE);
	gtk_widget_show(table);
	gtk_table_set_row_spacings(GTK_TABLE(table),3);
	gtk_table_set_col_spacings(GTK_TABLE(table),3);
	gtk_box_pack_start(GTK_BOX(vbox2),table,FALSE,FALSE,2);

	alignment = gtk_alignment_new(1.0,0.5,0,0);
	gtk_widget_show(alignment);
	label = gtk_label_new("Description:");
	gtk_widget_show(label);
	gtk_label_set_justify(GTK_LABEL(label),GTK_JUSTIFY_RIGHT);
	gtk_container_add(GTK_CONTAINER(alignment),label);
	gtk_table_attach(GTK_TABLE(table),alignment,0,1,0,1,
			GTK_FILL, (GtkAttachOptions) (0),0,0);

	alignment = gtk_alignment_new(1.0,0.5,0,0);
	gtk_widget_show(alignment);
	label = gtk_label_new("Icon:");
	gtk_widget_show(label);
	gtk_label_set_justify(GTK_LABEL(label),GTK_JUSTIFY_RIGHT);
	gtk_container_add(GTK_CONTAINER(alignment),label);
	gtk_table_attach(GTK_TABLE(table),alignment,0,1,1,2,
			GTK_FILL, (GtkAttachOptions) (0),0,0);

	alignment = gtk_alignment_new(1.0,0.5,0,0);
	gtk_widget_show(alignment);
	label = gtk_label_new("Executes:");
	gtk_widget_show(label);
	gtk_label_set_justify(GTK_LABEL(label),GTK_JUSTIFY_RIGHT);
	gtk_container_add(GTK_CONTAINER(alignment),label);
	gtk_table_attach(GTK_TABLE(table),alignment,0,1,2,3,
			GTK_FILL, (GtkAttachOptions) (0),0,0);

	descriptionfield = entry = gtk_entry_new_with_max_length(200);
	gtk_widget_show(entry);
	gtk_table_attach(GTK_TABLE(table), entry, 1, 3, 0, 1,
			GTK_EXPAND | GTK_FILL, (GtkAttachOptions) (0), 0, 0);

	iconfield = entry = gtk_entry_new_with_max_length(200);
	gtk_widget_show(entry);
	gtk_table_attach(GTK_TABLE(table), entry, 1, 2, 1, 2,
			GTK_EXPAND | GTK_FILL, (GtkAttachOptions) (0), 0, 0);

	button = gtk_button_new_with_label("Browse");
	gtk_widget_show(button);
	gtk_table_attach(GTK_TABLE(table), button, 2, 3, 1, 2,
			(GtkAttachOptions) 0, (GtkAttachOptions) (0), 0, 0);

	execfield = entry = gtk_entry_new_with_max_length(200);
	gtk_widget_show(entry);
	gtk_table_attach(GTK_TABLE(table), entry, 1, 3, 2, 3,
			GTK_EXPAND | GTK_FILL, (GtkAttachOptions) (0), 0, 0);
	execedit = 1;

	hbox = gtk_hbox_new(FALSE,3);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,2);

	button = gtk_button_new_with_label(" Insert Menu Entry ");
	gtk_widget_show(button);
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,FALSE,2);

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
	load_menus_from_disk();
	gtk_signal_connect(GTK_OBJECT(main_win), "destroy",
			GTK_SIGNAL_FUNC(on_exit_application), NULL);
	gtk_signal_connect(GTK_OBJECT(main_win), "delete_event",
			GTK_SIGNAL_FUNC(on_exit_application), NULL);

	gtk_main();

	return 0;
}
