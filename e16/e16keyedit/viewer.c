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
static gchar *wait_for_ipc_msg(void);

gchar *e_ipc_msg = NULL;
GList *keys = NULL;


typedef struct _keybind {
	char *key;
	gint modifier;
	gint id;
	char *params;
	gint action_id;
} Keybind;

typedef struct _actionopt {
	gchar *text;
	gint id;
	gchar param_tpe;
	gchar *params;
} ActionOpt;

static ActionOpt actions[] = {
	{"Run command", 1, 1, NULL},

	{"Restart Enlightenment", 7, 0, "restart"},
	{"Exit Enlightenment", 7, 0, NULL},

	{"Goto Next Desktop", 15, 0, NULL},
	{"Goto Previous Deskop", 16, 0, NULL},
	{"Goto Desktop", 42, 2, NULL},
	{"Raise Desktop", 17, 0, NULL},
	{"Lower Desktop", 18, 0, NULL},
	{"Reset Desktop In Place", 21, 0, NULL},

	{"Toggle Deskrays", 43, 0, NULL},

	{"Cleanup Windows", 8, 0, NULL},
	{"Scroll Windows to left", 48, 0, "-16 0"},
	{"Scroll Windows to right", 48, 0, "16 0"},
	{"Scroll Windows up", 48, 0, "0 -16"},
	{"Scroll Windows down", 48, 0, "0 16"},
	{"Scroll Windows by [X Y] pixels", 48, 3, NULL},

	{"Move mouse pointer to left", 66, 0, "-1 0"},
	{"Move mouse pointer to right", 66, 0, "1 0"},
	{"Move mouse pointer up", 66, 0, "0 -1"},
	{"Move mouse pointer down", 66, 0, "0 1"},
	{"Move mouse pointer by [X Y]", 66, 3, NULL},

	{"Goto Desktop area [X Y]", 62, 3, NULL},
	{"Move to Desktop area on the left", 63, 0, "-1 0"},
	{"Move to Desktop area on the right", 63, 0, "1 0"},
	{"Move to Desktop area above", 63, 0, "0 -1"},
	{"Move to Desktop area below", 63, 0, "0 1"},

	{"Raise Window", 5, 0, NULL},
	{"Lower Window", 6, 0, NULL},
	{"Close Window", 13, 0, NULL},
	{"Annihilate Window", 14, 0, NULL},
	{"Stick / Unstick Window", 20, 0, NULL},
	{"Iconify Window", 46, 0, NULL},
	{"Shade / Unshade Window", 49, 0, NULL},
	{"Maximise Height of Window", 50, 0, "conservative"},
	{"Maximise Height of Window to whole screen", 50, 0, NULL},
	{"Maximise Height of Window toavailable space", 50, 0, "available"},
	{"Maximise Width of Window", 51, 0, "conservative"},
	{"Maximise Width of Window to whole screen", 51, 0, NULL},
	{"Maximise Width of Window toavailable space", 51, 0, "available"},
	{"Maximise Size of Window", 52, 0, "conservative"},
	{"Maximise Size of Window to whole screen", 52, 0, NULL},
	{"Maximise Size of Window toavailable space", 52, 0, "available"},
	{"Send window to next desktop", 53, 0, NULL},
	{"Send window to previous desktop", 54, 0, NULL},
	{"Switch focus to next window", 58, 0, NULL},
	{"Switch focus to previous window", 59, 0, NULL},
	{"Glue / Unglue Window to Desktop screen", 64, 0, NULL},
	{"Set Window layer to On Top", 65, 0, "20"},
	{"Set Window layer to Above", 65, 0, "6"},
	{"Set Window layer to Normal", 65, 0, "4"},
	{"Set Window layer to Below", 65, 0, "2"},
	{"Set Window layer", 65, 2, NULL},
	{"Move Window to area on left", 0, 0, "-1 0"},
	{"Move Window to area on right", 0, 0, "1 0"},
	{"Move Window to area above", 0, 0, "0 -1"},
	{"Move Window to area below", 0, 0, "0 1"},
	{"Move Window by area [X Y]", 0, 3, NULL},

	{"Set Window border style to the Default", 69, 0, "DEFAULT"},
	{"Set Window border style to the Borderless", 69, 0, "BORDERLESS"},

	{"Forget everything about Window", 55, 0, "none"},
	{"Remember all Window settings", 55, 0, NULL},
	{"Remember Window Border", 55, 0, "border"},
	{"Remember Window Desktop", 55, 0, "desktop"},
	{"Remember Window Desktop Area", 55, 0, "area"},
	{"Remember Window Size", 55, 0, "size"},
	{"Remember Window Location", 55, 0, "location"},
	{"Remember Window Layer", 55, 0, "layer"},
	{"Remember Window Stickyness", 55, 0, "sticky"},
	{"Remember Window Shadedness", 55, 0, "shade"},

	{"Show Root Menu", 9, 0, "ROOT_2"},
	{"Show Winops Menu", 9, 0, "WINOPS_MENU"},
	{"Show Named Menu", 9, 1, NULL},

	{"Goto Linear Area", 70, 2, NULL},
	{"Previous Linear Area", 71, 0, "-1"},
	{"Next Linear Area", 71, 0, "1"},
	{NULL, 0, 0, NULL}
};



static gchar *wait_for_ipc_msg(void)
{
	gtk_main();
	return e_ipc_msg;
}

char               *
atword(char *s, int num)
{
	int                 cnt, i;

	if (!s)
		return NULL;
	cnt = 0;
	i = 0;

	while (s[i])
	{
		if ((s[i] != ' ') && (s[i] != '\t'))
		{
			if (i == 0)
				cnt++;
			else if ((s[i - 1] == ' ') || (s[i - 1] == '\t'))
				cnt++;
			if (cnt == num)
				return &s[i];
		}
		i++;
	}
	return NULL;
}

void
selection_made(GtkWidget *clist, gint row, gint column, GdkEventButton *event,
		gpointer data)
{

	return;
}

static gchar *get_line(gchar * str, int num);

static gchar *get_line(gchar * str, int num)
{
	gchar *s1, *s2, *s;
	gint i, count, l;

	i = 0;
	count = 0;
	s1 = str;
	if (*str == '\n')
		i = 1;
	s2 = NULL;
	for (i = 0;; i++) {
		if ((str[i] == '\n') || (str[i] == 0)) {
			s2 = &(str[i]);
			if ((count == num) && (s2 > s1)) {
				l = s2 - s1;
				s = g_malloc(l + 1);
				strncpy(s, s1, l);
				s[l] = 0;
				return s;
			}
			count++;
			if (str[i] == 0)
				return NULL;
			s1 = s2 + 1;
		}
	}
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
	gtk_clist_set_column_title(GTK_CLIST(clist), 1, "Key Used");
	gtk_clist_set_column_title(GTK_CLIST(clist), 2, "Action to perform");
	gtk_clist_set_column_title(GTK_CLIST(clist), 3, "Optional Parameters");
	gtk_clist_column_titles_show(GTK_CLIST(clist));
	gtk_signal_connect(GTK_OBJECT(clist), "select_row",
			GTK_SIGNAL_FUNC(selection_made), NULL);
	gtk_signal_connect(GTK_OBJECT(clist), "click_column",
			GTK_SIGNAL_FUNC(on_resort_columns), NULL);

	{
		gchar *msg;
		gint i, j, k;
		gchar *buf;
		gchar cmd[4096];

		CommsSend("get_keybindings");
		msg = wait_for_ipc_msg();
		i = 0;
		while ((buf = get_line(msg, i++))) {
			/* stuff[0] = modifier */
			/* stuff[1] = key */
			/* stuff[2] = action */
			/* stuff[3] = params */

			char *stuff[4];

			stuff[0] = malloc(1024);
			stuff[1] = malloc(1024);
			stuff[2] = malloc(1024);
			stuff[3] = malloc(1024);
			if (strlen(buf) < 1)
				break;
			sscanf(buf, "%1000s", cmd);
			sprintf(stuff[1],"%s",cmd);
			sscanf(buf, "%*s %i", &j);
			sprintf(stuff[0],"%d",j);
			sscanf(buf, "%*s %*s %i", &j);
			sprintf(stuff[2],"%d",j);
			if (atword(buf, 4))
				sprintf(stuff[3],"%s",atword(buf, 4));
			else
				strcpy(stuff[3],"");
			/*
			for (k = 0; ((actions[k].text) && (kb->action_id < 0)); k++) {
				if (kb->id == actions[k].id) {
					if (kb->params) {
						if ((actions[k].param_tpe == 0) && (actions[k].params)) {
							if (!strcmp(kb->params, actions[k].params))
								kb->action_id = k;
						} else
							kb->action_id = k;
					} else if (!actions[k].params)
						kb->action_id = k;
				}
			}
			if (kb->action_id < 0) {
				if (kb->key)
					g_free(kb->key);
				if (kb->params)
					g_free(kb->params);
				g_free(kb);
			} else
			*/
			gtk_clist_append(GTK_CLIST(clist), stuff);
			free(stuff[0]);
			free(stuff[1]);
			free(stuff[2]);
			free(stuff[3]);
			g_free(buf);
		}
		g_free(msg);

	}

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

	gtk_main_quit();

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
