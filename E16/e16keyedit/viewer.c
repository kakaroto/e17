#include "config.h"

#include <gtk/gtk.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <X11/Xlib.h>
#include <gdk/gdkx.h>
#include <unistd.h>

#include "viewer.h"
#include "menus.h"
#include "ipc.h"

#if __GNUC__			/* FIXME - Use autofoo */
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif

#define DEBUG 0

extern GtkTooltips *tooltips;
extern GtkAccelGroup *accel_group;

static GtkWidget   *clist;
static GtkWidget   *act_key;
static GtkWidget   *act_params;
static GtkWidget   *act_mod;
static GtkWidget   *act_clist;

#define VER_E16_OLD 0
#define VER_E16_8   1
static int          e16_ver = VER_E16_OLD;

static gchar       *e_ipc_msg = NULL;
static char         dont_update = 0;
static int          last_row = 0;
static int          real_rows = 0;

/* Various UI text strings */
#define TXT_SELECT(old, new) ((e16_ver==VER_E16_OLD) ? old : new)
#define TXT_PARAMETERS_USED TXT_SELECT("Parameters Used", "Command")
#define TXT_PARAMETERS      TXT_SELECT("Parameters:", "Command:")

typedef struct
{
   const char         *text;
   gint                id;
   gchar               param_tpe;
   const char         *params;
   const char         *command;
} ActionOpt;

static const char  *mod_str[] = {
   "",
   "CTRL",
   "ALT",
   "SHIFT",
   "CTRL+ALT",
   "CTRL+SHIFT",
   "ALT+SHIFT",
   "CTRL+ALT+SHIFT",
   "WIN",
   "MOD3",
   "MOD4",
   "MOD5",
   "WIN+SHIFT",
   "WIN+CTRL",
   "WIN+ALT",
   "MOD4+SHIFT",
   "MOD4+CTRL",
   "MOD4+CTRL+SHIFT",
   "MOD5+SHIFT",
   "MOD5+CTRL",
   "MOD5+CTRL+SHIFT",
};
#define N_MODIFIERS (sizeof(mod_str)/sizeof(char*))

/* *INDENT-OFF* */
static const ActionOpt actions_default[] = {
    {"Run command", 1, 1, NULL, "exec "},

    {"Restart Enlightenment", 7, 0, "restart", "restart"},
    {"Exit Enlightenment", 7, 0, NULL, "exit"},

    {"Goto Next Desktop", 15, 0, NULL, "desk next"},
    {"Goto Previous Deskop", 16, 0, NULL, "desk prev"},
    {"Goto Desktop", 42, 2, NULL, "desk goto "},
    {"Raise Desktop", 17, 0, NULL, "desk raise"},
    {"Lower Desktop", 18, 0, NULL, "desk lower"},
    {"Reset Desktop In Place", 21, 0, NULL, "desk this"},

    {"Toggle Deskrays", 43, 0, NULL, NULL},

    {"Cleanup Windows", 8, 0, NULL, "misc arrange size"},

    {"Scroll Windows to left", 48, 0, "-16 0", NULL},
    {"Scroll Windows to right", 48, 0, "16 0", NULL},
    {"Scroll Windows up", 48, 0, "0 -16", NULL},
    {"Scroll Windows down", 48, 0, "0 16", NULL},
    {"Scroll Windows by [X Y] pixels", 48, 3, NULL, NULL},

    {"Move mouse pointer to next screen", -1, 0, NULL, "warp screen"},

    {"Move mouse pointer to left", 66, 0, "-1 0", "warp rel -1 0"},
    {"Move mouse pointer to right", 66, 0, "1 0", "warp rel 1 0"},
    {"Move mouse pointer up", 66, 0, "0 -1", "warp rel 0 -1"},
    {"Move mouse pointer down", 66, 0, "0 1", "warp rel 0 1"},
    {"Move mouse pointer by [X Y]", 66, 3, NULL, "warp rel "},

    {"Goto Desktop area [X Y]", 62, 3, NULL, "area goto"},
    {"Move to Desktop area on the left", 63, 0, "-1 0", "area move -1 0"},
    {"Move to Desktop area on the right", 63, 0, "1 0", "area move 1 0"},
    {"Move to Desktop area above", 63, 0, "0 -1", "area move 0 -1"},
    {"Move to Desktop area below", 63, 0, "0 1", "area move 0 1"},

    {"Raise Window", 5, 0, NULL, "wop * raise"},
    {"Lower Window", 6, 0, NULL, "wop * lower"},
    {"Close Window", 13, 0, NULL, "wop * close"},
    {"Annihilate Window", 14, 0, NULL, "wop * kill"},
    {"Stick / Unstick Window", 20, 0, NULL, "wop * stick"},
    {"Iconify Window", 46, 0, NULL, "wop * iconify"},
    {"Shade / Unshade Window", 49, 0, NULL, "wop * shade"},
    {"Maximise Height of Window", 50, 0, "conservative", "wop * th conservative"},
    {"Maximise Height of Window to available space", 50, 0, "available", "wop * th available"},
    {"Maximise Height of Window to whole screen", 50, 0, NULL, "wop * th"},
    {"Maximise Width of Window", 51, 0, "conservative", "wop * tw conservative"},
    {"Maximise Width of Window to available space", 51, 0, "available", "wop * tw available"},
    {"Maximise Width of Window to whole screen", 51, 0, NULL, "wop * tw"},
    {"Maximise Size of Window", 52, 0, "conservative", "wop * ts conservative"},
    {"Maximise Size of Window to available space", 52, 0, "available", "wop * ts available"},
    {"Maximise Size of Window to whole screen", 52, 0, NULL, "wop * ts"},
    {"Toggle Window fullscreen state", -1, 0, NULL, "wop * fullscreen"},
    {"Toggle Window zoom state", -1, 0, NULL, "wop * zoom"},
    {"Send window to next desktop", 53, 0, NULL, "wop * desk next"},
    {"Send window to previous desktop", 54, 0, NULL, "wop * desk prev"},
    {"Switch focus to next window", 58, 0, NULL, "focus next"},
    {"Switch focus to previous window", 59, 0, NULL, "focus prev"},
    {"Glue / Unglue Window to Desktop screen", 64, 0, NULL, "wop * no_user_move"},
    {"Set Window layer to On Top", 65, 0, "20", "wop * layer 20"},
    {"Set Window layer to Above", 65, 0, "6", "wop * layer 6"},
    {"Set Window layer to Normal", 65, 0, "4", "wop * layer 4"},
    {"Set Window layer to Below", 65, 0, "2", "wop * layer 2"},
    {"Set Window layer", 65, 2, NULL, "wop * layer "},
    {"Move Window to area on left", 0, 0, "-1 0", "wop * area move -1 0"},
    {"Move Window to area on right", 0, 0, "1 0", "wop * area move 1 0"},
    {"Move Window to area above", 0, 0, "0 -1", "wop * area move 0 -1"},
    {"Move Window to area below", 0, 0, "0 1", "wop * area move 0 1"},
    {"Move Window by area [X Y]", 0, 3, NULL, "wop * area "},

    {"Set Window border style to the Default", 69, 0, "DEFAULT", "wop * border DEFAULT"},
    {"Set Window border style to the Borderless", 69, 0, "BORDERLESS", "wop * border BORDERLESS"},

    {"Forget everything about Window", 55, 0, "none", "wop * snap none"},
    {"Remember all Window settings", 55, 0, NULL, "wop * snap all"},
    {"Remember Window Border", 55, 0, "border", "wop * snap border"},
    {"Remember Window Desktop", 55, 0, "desktop", "wop * snap desktop"},
    {"Remember Window Desktop Area", 55, 0, "area", "wop * snap area"},
    {"Remember Window Size", 55, 0, "size", "wop * snap size"},
    {"Remember Window Location", 55, 0, "location", "wop * snap location"},
    {"Remember Window Layer", 55, 0, "layer", "wop * snap layer"},
    {"Remember Window Stickyness", 55, 0, "sticky", "wop * snap sticky"},
    {"Remember Window Shadedness", 55, 0, "shade", "wop * snap shade"},

    {"Show Root Menu", 9, 0, "ROOT_2", "menus show ROOT_2"},
    {"Show Winops Menu", 9, 0, "WINOPS_MENU", "menus show WINOPS_MENU"},
    {"Show Named Menu", 9, 1, NULL, "menus show "},

    {"Goto Linear Area", 70, 2, NULL, NULL},
    {"Previous Linear Area", 71, 0, "-1", NULL},
    {"Next Linear Area", 71, 0, "1", NULL},

    {NULL, 0, 0, NULL, NULL}
};
/* *INDENT-ON* */

static const ActionOpt *actions = NULL;
static unsigned int action_count = sizeof(actions_default) / sizeof(ActionOpt);
static unsigned int *action_index_to_row = NULL;
static unsigned int *action_row_to_index = NULL;

static int
match_action_by_binding(int opcode, const char *params)
{
   int                 k, len;

   for (k = 0; actions[k].text; k++)
     {
	if (e16_ver > VER_E16_OLD)
	  {
	     if (!actions[k].command)	/* Not avaliable in 16.8 */
		continue;

	     len = strlen(actions[k].command);
	     if (strncmp(actions[k].command, params, len))
		continue;

	     return k;
	  }
	else
	  {
	     if (opcode != actions[k].id)
		continue;

	     if (*params == '\0' && !actions[k].params)
		return k;	/* No parameters */

	     if ((actions[k].param_tpe == 0) && (actions[k].params))
	       {
		  if (!strcmp(params, actions[k].params))
		     return k;
	       }
	     else
	       {
		  return k;
	       }
	  }
     }

   return -1;
}

static int
match_action_by_selection(const char *text)
{
   int                 k;

   for (k = 0; actions[k].text; k++)
     {
	if (strcmp(text, actions[k].text))
	   continue;

	return k;
     }

   return -1;
}

static void
e_cb_key_change(GtkWidget * widget __UNUSED__, gpointer data __UNUSED__)
{
   GtkWidget          *win, *label, *frame, *align;

   win = gtk_window_new(GTK_WINDOW_POPUP);
   gtk_window_set_policy(GTK_WINDOW(win), 0, 0, 1);
   gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_MOUSE);
   frame = gtk_frame_new(NULL);
   gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_OUT);
   align = gtk_alignment_new(0.0, 0.0, 0.0, 0.0);
   gtk_container_set_border_width(GTK_CONTAINER(align), 32);
   label = gtk_label_new("Please press the key on the keyboard\n"
			 "you wish to modify this keyboard-shortcut\n"
			 "to use from now on.");
   gtk_container_add(GTK_CONTAINER(win), frame);
   gtk_container_add(GTK_CONTAINER(frame), align);
   gtk_container_add(GTK_CONTAINER(align), label);
   gtk_widget_show_all(win);
   while (gtk_events_pending())
      gtk_main_iteration();
   gdk_flush();
   while (gtk_events_pending())
      gtk_main_iteration();

   {
      char               *key;
      XEvent              ev;

      gdk_window_set_events(win->window, GDK_KEY_PRESS_MASK);
      XSetInputFocus(GDK_DISPLAY(), GDK_WINDOW_XWINDOW(win->window),
		     RevertToPointerRoot, CurrentTime);
      gdk_keyboard_grab(win->window, TRUE, CurrentTime);
      XWindowEvent(GDK_DISPLAY(), GDK_WINDOW_XWINDOW(win->window),
		   KeyPressMask, &ev);
#ifdef ENABLE_GTK2
      gdk_keyboard_ungrab(GDK_CURRENT_TIME);
#else
      gdk_keyboard_ungrab(gdk_time_get());
#endif
      key = XKeysymToString(XKeycodeToKeysym(GDK_DISPLAY(),
					     ev.xkey.keycode, 0));
      gtk_entry_set_text(GTK_ENTRY(act_key), key);
      gtk_clist_set_text(GTK_CLIST(clist), last_row, 1, key);
   }

   gtk_widget_destroy(win);
}

static void
e_cb_modifier(GtkWidget * widget __UNUSED__, gpointer data)
{
   gint                value;

   value = (gint) data;
   gtk_clist_set_text(GTK_CLIST(clist), last_row, 0, mod_str[value]);
}

static gchar       *
wait_for_ipc_msg(void)
{
   gtk_main();
   return e_ipc_msg;
}

static void
change_action(GtkWidget * my_clist __UNUSED__, gint row, gint column __UNUSED__,
	      GdkEventButton * event __UNUSED__, gpointer data __UNUSED__)
{
   int                 k;

   if (dont_update)
      return;

   k = action_row_to_index[row];

   if (actions[k].param_tpe != 0)
     {
	gtk_entry_set_editable(GTK_ENTRY(act_params), TRUE);
	gtk_widget_set_sensitive(act_params, TRUE);
     }
   else
     {
	gtk_entry_set_editable(GTK_ENTRY(act_params), FALSE);
	gtk_widget_set_sensitive(act_params, FALSE);
	gtk_entry_set_text(GTK_ENTRY(act_params), "");
     }

   if (e16_ver != VER_E16_OLD)
     {
	if (actions[k].command)
	   gtk_entry_set_text(GTK_ENTRY(act_params), actions[k].command);
	else
	   gtk_entry_set_text(GTK_ENTRY(act_params), "* Not available *");
     }
   else
     {
	if (actions[k].params)
	   gtk_entry_set_text(GTK_ENTRY(act_params), actions[k].params);
     }
   gtk_clist_set_text(GTK_CLIST(clist), last_row, 2, actions[k].text);
   gtk_clist_set_text(GTK_CLIST(clist), last_row, 3,
		      gtk_entry_get_text(GTK_ENTRY(act_params)));
}

static char        *
dupcat(char *dst, const char *src)
{
   char               *s;
   int                 len1, len2;

   if (!dst)
      return strdup(src);

   len1 = strlen(dst);
   len2 = strlen(src);

   s = realloc(dst, len1 + len2 + 1);
   strcpy(s + len1, src);

   return s;
}

static void
on_save_data(GtkWidget * widget __UNUSED__, gpointer data __UNUSED__)
{
   char               *buf = NULL;
   int                 i;

   buf = dupcat(buf, "set_keybindings ");
   for (i = 0; i < real_rows; i++)
     {
	char                tmp[1024];
	char               *params;
	char                params_tmp[1024];
	char               *action;
	char               *key;
	char               *mod;
	int                 modifier = 0;
	int                 action_id = 0;
	int                 j;

	gtk_clist_get_text(GTK_CLIST(clist), i, 0, &mod);
	for (j = 0; j < 21; j++)
	  {
	     if (!strcmp(mod_str[j], mod))
	       {
		  modifier = j;
	       }
	  }
	gtk_clist_get_text(GTK_CLIST(clist), i, 1, &key);
	gtk_clist_get_text(GTK_CLIST(clist), i, 2, &action);
	j = match_action_by_selection(action);
	action_id = (j >= 0) ? actions[j].id : -1;
	gtk_clist_get_text(GTK_CLIST(clist), i, 3, &params);

	if (e16_ver > VER_E16_OLD)
	  {
	     if (*params == '*')
		continue;
	     snprintf(tmp, sizeof(tmp), "%s %i %i %s\n", key, modifier, 0,
		      params);
	  }
	else if (*params != '\0')
	  {
	     if (action_id < 0)
		continue;

	     if (action_id == 9)
	       {
		  snprintf(params_tmp, sizeof(params_tmp), "%s %s", "named",
			   params);
		  params = (char *)params_tmp;
	       }
	     snprintf(tmp, sizeof(tmp), "%s %i %i %s\n", key, modifier,
		      action_id, params);
	  }
	else
	  {
	     snprintf(tmp, sizeof(tmp), "%s %i %i\n", key, modifier, action_id);
	  }
	buf = dupcat(buf, tmp);
     }

#if DEBUG > 0
   printf("%s", buf);
#else
   CommsSend(buf);
   CommsSend("save_config");
#endif
   free(buf);
}

static void
selection_made(GtkWidget * my_clist __UNUSED__, gint row,
	       gint column __UNUSED__, GdkEventButton * event __UNUSED__,
	       gpointer data __UNUSED__)
{
   gchar              *modstring;
   gchar              *keyused;
   gchar              *actperform;
   gchar              *paramsused;
   int                 i;

   dont_update = 1;

   gtk_clist_get_text(GTK_CLIST(clist), row, 0, &modstring);
   gtk_option_menu_set_history(GTK_OPTION_MENU(act_mod), 0);
   for (i = 1; i < 21; i++)
     {
	if (!strcmp(mod_str[i], modstring))
	  {
	     gtk_option_menu_set_history(GTK_OPTION_MENU(act_mod), i);
	  }
     }

   gtk_clist_get_text(GTK_CLIST(clist), row, 1, &keyused);
   gtk_entry_set_text(GTK_ENTRY(act_key), keyused);

   gtk_clist_get_text(GTK_CLIST(clist), row, 2, &actperform);

   gtk_clist_get_text(GTK_CLIST(clist), row, 3, &paramsused);
   gtk_entry_set_text(GTK_ENTRY(act_params), paramsused);

   i = match_action_by_selection(actperform);
   if (i < 0 || actions[i].param_tpe == 0)
     {
	gtk_entry_set_editable(GTK_ENTRY(act_params), FALSE);
	gtk_widget_set_sensitive(act_params, FALSE);
     }
   else
     {
	gtk_entry_set_editable(GTK_ENTRY(act_params), TRUE);
	gtk_widget_set_sensitive(act_params, TRUE);
     }

   if (i >= 0)
     {
	i = action_index_to_row[i];
	gtk_clist_select_row(GTK_CLIST(act_clist), i, 0);
	gtk_clist_moveto(GTK_CLIST(act_clist), i, 0, 0.5, 0.5);
     }

   /* printf("%s\n%s\n%s\n%s\n",modstring,keyused,actperform,paramsused); */

   last_row = row;

   dont_update = 0;
}

static gchar       *get_line(gchar * str, int num);

static gchar       *
get_line(gchar * str, int num)
{
   gchar              *s1, *s2, *s;
   gint                i, count, l;

   i = 0;
   count = 0;
   s1 = str;
   if (*str == '\n')
      i = 1;
   s2 = NULL;
   for (i = 0;; i++)
     {
	if ((str[i] == '\n') || (str[i] == 0))
	  {
	     s2 = &(str[i]);
	     if ((count == num) && (s2 > s1))
	       {
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

static void
on_resort_columns(GtkWidget * widget __UNUSED__, gint column,
		  gpointer data __UNUSED__)
{
   static int          order = 0;
   static int          last_col = 0;

   gtk_clist_set_sort_column(GTK_CLIST(clist), column);
   if (last_col == column)
     {
	if (order)
	  {
	     order = 0;
	     gtk_clist_set_sort_type(GTK_CLIST(clist), GTK_SORT_DESCENDING);
	  }
	else
	  {
	     order = 1;
	     gtk_clist_set_sort_type(GTK_CLIST(clist), GTK_SORT_ASCENDING);
	  }
     }
   else
     {
	order = 1;
	gtk_clist_set_sort_type(GTK_CLIST(clist), GTK_SORT_ASCENDING);
	last_col = column;
     }

   gtk_clist_sort(GTK_CLIST(clist));
}

static void
on_delete_row(GtkWidget * widget __UNUSED__, gpointer data __UNUSED__)
{
   gtk_clist_remove(GTK_CLIST(clist), last_row);

   real_rows--;
   if (last_row >= real_rows)
      last_row--;
   gtk_clist_select_row(GTK_CLIST(clist), last_row, 0);
   gtk_clist_moveto(GTK_CLIST(clist), last_row, 0, 0.5, 0.5);
}

static void
on_create_row(GtkWidget * widget __UNUSED__, gpointer data __UNUSED__)
{
   char               *stuff[4];

   stuff[0] = malloc(2);
   strcpy(stuff[0], "");
   stuff[1] = malloc(2);
   strcpy(stuff[1], "");
   stuff[2] = malloc(2);
   strcpy(stuff[2], "");
   stuff[3] = malloc(2);
   strcpy(stuff[3], "");
   gtk_clist_select_row(GTK_CLIST(clist),
			gtk_clist_append(GTK_CLIST(clist), stuff), 0);

   if (stuff[0])
      free(stuff[0]);
   if (stuff[1])
      free(stuff[1]);
   if (stuff[2])
      free(stuff[2]);
   if (stuff[3])
      free(stuff[3]);

   last_row = real_rows++;
   gtk_clist_select_row(GTK_CLIST(clist), last_row, 0);
   gtk_clist_moveto(GTK_CLIST(clist), last_row, 0, 0.5, 0.5);
}

static void
on_change_params(GtkWidget * widget __UNUSED__, gpointer data __UNUSED__)
{
   if (!dont_update)
     {
	gtk_clist_set_text(GTK_CLIST(clist), last_row, 3,
			   gtk_entry_get_text(GTK_ENTRY(act_params)));
     }
}

void
on_exit_application(GtkWidget * widget __UNUSED__, gpointer data __UNUSED__)
{
   gtk_exit(0);
}

static void
on_save_and_exit_application(GtkWidget * widget, gpointer data)
{
   on_save_data(widget, data);
   on_exit_application(widget, data);
}

static GtkWidget   *
create_list_window(void)
{
   GtkWidget          *list_window;
   GtkWidget          *bigvbox;
   GtkWidget          *menubar;
   GtkWidget          *panes;
   GtkWidget          *scrollybit;
   GtkWidget          *vbox;
   GtkWidget          *frames;
   GtkWidget          *alignment;
   GtkWidget          *frame_vbox;
   GtkWidget          *table;
   GtkWidget          *label;
   GtkWidget          *entry;
   GtkWidget          *button;
   GtkWidget          *hbox;
   GtkWidget          *m, *mi, *om;
   GtkWidget          *menu, *menuitem;

   list_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   gtk_window_set_default_size(GTK_WINDOW(list_window), 400, 400);
   gtk_object_set_data(GTK_OBJECT(list_window), "key_editor", list_window);
   GTK_WIDGET_SET_FLAGS(list_window, GTK_CAN_FOCUS);
   GTK_WIDGET_SET_FLAGS(list_window, GTK_CAN_DEFAULT);
   gtk_window_set_title(GTK_WINDOW(list_window), "E Keys Editor");

   bigvbox = gtk_vbox_new(FALSE, 0);
   gtk_widget_show(bigvbox);
   gtk_container_add(GTK_CONTAINER(list_window), bigvbox);

   menubar = gtk_menu_bar_new();
   gtk_widget_show(menubar);
   gtk_box_pack_start(GTK_BOX(bigvbox), menubar, FALSE, FALSE, 0);

   menu = CreateBarSubMenu(menubar, "File");
   menuitem = CreateMenuItem(menu, "Save", "", "Save Current Data", NULL,
			     "save data");
   gtk_signal_connect(GTK_OBJECT(menuitem), "activate",
		      GTK_SIGNAL_FUNC(on_save_data), NULL);
   menuitem = CreateMenuItem(menu, "Save & Quit", "",
			     "Save Current Data & Quit Application", NULL,
			     "save quit");
   gtk_signal_connect(GTK_OBJECT(menuitem), "activate",
		      GTK_SIGNAL_FUNC(on_save_and_exit_application), NULL);
   menuitem =
      CreateMenuItem(menu, "Quit", "", "Quit Without Saving", NULL,
		     "quit program");
   gtk_signal_connect(GTK_OBJECT(menuitem), "activate",
		      GTK_SIGNAL_FUNC(on_exit_application), NULL);

   menu = CreateRightAlignBarSubMenu(menubar, "Help");
   menuitem = CreateMenuItem(menu, "About", "", "About E Keybinding Editor",
			     NULL, "about");
   menuitem = CreateMenuItem(menu, "Documentation", "",
			     "Read the Keybinding Editor Documentation", NULL,
			     "read docs");

   panes = gtk_hpaned_new();
   gtk_widget_show(panes);
   gtk_paned_set_gutter_size(GTK_PANED(panes), 10);
   gtk_box_pack_start(GTK_BOX(bigvbox), panes, TRUE, TRUE, 0);

   scrollybit = gtk_scrolled_window_new(NULL, NULL);
   gtk_widget_show(scrollybit);
   gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollybit),
				  GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
   gtk_paned_pack1(GTK_PANED(panes), scrollybit, TRUE, FALSE);

   clist = gtk_clist_new(4);
   gtk_widget_show(clist);
   gtk_container_add(GTK_CONTAINER(scrollybit), clist);

   gtk_clist_set_column_title(GTK_CLIST(clist), 0, "Modifier");
   gtk_clist_set_column_title(GTK_CLIST(clist), 1, "Key");
   gtk_clist_set_column_title(GTK_CLIST(clist), 2, "Action to Perform");
   gtk_clist_set_column_title(GTK_CLIST(clist), 3, TXT_PARAMETERS_USED);
   gtk_clist_column_titles_show(GTK_CLIST(clist));
   gtk_signal_connect(GTK_OBJECT(clist), "select_row",
		      GTK_SIGNAL_FUNC(selection_made), NULL);
   gtk_signal_connect(GTK_OBJECT(clist), "click_column",
		      GTK_SIGNAL_FUNC(on_resort_columns), NULL);

   {
      char               *msg, *buf;
      int                 i, j, k, modifier, opcode;

      CommsSend("get_keybindings");
      msg = wait_for_ipc_msg();
      i = 0;
      while ((buf = get_line(msg, i++)))
	{
	   /* stuff[0] = modifier */
	   /* stuff[1] = key */
	   /* stuff[2] = action */
	   /* stuff[3] = params */
	   char                key[128], *params;
	   const char         *stuff[4];
	   int                 len;

	   if (strlen(buf) < 1)
	      break;

	   opcode = modifier = -1;
	   j = sscanf(buf, "%127s %d %d %n", key, &modifier, &opcode, &len);
#if DEBUG > 0
	   printf("buf(%d): %s\n", j, buf);
#endif
	   if (j < 3 || opcode < 0)
	      continue;

	   if (modifier < 0 || modifier >= (int)N_MODIFIERS)
	      continue;

	   params = buf + len;
	   if (opcode == 9 && !strncmp(params, "named ", 6))	/* Hack for e16 < 0.16.8 */
	      params += 6;

#if DEBUG > 0
	   printf("key: %s, mod: %s, opc=%d, params: %s\n", key,
		  mod_str[modifier], opcode, params);
#endif

	   k = match_action_by_binding(opcode, params);

#if DEBUG > 1
	   printf("key: %s, mod: %s, act=%d, params: %s\n", key,
		  mod_str[modifier], k, params);
#endif

	   stuff[0] = mod_str[modifier];
	   stuff[1] = key;
	   stuff[2] = (k >= 0) ? actions[k].text : "* Not recognised *";
	   stuff[3] = params;
	   gtk_clist_append(GTK_CLIST(clist), (char **)stuff);
	   real_rows++;

	   g_free(buf);
	}
      g_free(msg);
   }
   gtk_clist_columns_autosize(GTK_CLIST(clist));

   vbox = gtk_vbox_new(FALSE, 0);
   gtk_widget_show(vbox);
   frames = gtk_frame_new("Edit Keybinding Properties");
   gtk_container_set_border_width(GTK_CONTAINER(frames), 2);
   gtk_widget_show(frames);
   gtk_paned_pack2(GTK_PANED(panes), vbox, FALSE, TRUE);
   gtk_box_pack_start(GTK_BOX(vbox), frames, TRUE, TRUE, 0);

   frame_vbox = gtk_vbox_new(FALSE, 3);
   gtk_widget_show(frame_vbox);

   gtk_container_set_border_width(GTK_CONTAINER(frame_vbox), 4);
   gtk_container_add(GTK_CONTAINER(frames), frame_vbox);

   table = gtk_table_new(3, 3, FALSE);
   gtk_widget_show(table);
   gtk_table_set_row_spacings(GTK_TABLE(table), 3);
   gtk_table_set_col_spacings(GTK_TABLE(table), 3);
   gtk_box_pack_start(GTK_BOX(frame_vbox), table, FALSE, FALSE, 2);

   alignment = gtk_alignment_new(1.0, 0.5, 0, 0);
   label = gtk_label_new("Key:");
   gtk_container_add(GTK_CONTAINER(alignment), label);
   gtk_widget_show(alignment);
   gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_RIGHT);
   gtk_widget_show(label);
   gtk_table_attach(GTK_TABLE(table), alignment, 0, 1, 0, 1,
		    GTK_FILL, (GtkAttachOptions) (0), 0, 0);

   alignment = gtk_alignment_new(1.0, 0.5, 0, 0);
   label = gtk_label_new("Modifier:");
   gtk_container_add(GTK_CONTAINER(alignment), label);
   gtk_widget_show(alignment);
   gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_RIGHT);
   gtk_widget_show(label);
   gtk_table_attach(GTK_TABLE(table), alignment, 0, 1, 1, 2,
		    GTK_FILL, (GtkAttachOptions) (0), 0, 0);

   alignment = gtk_alignment_new(1.0, 0.5, 0, 0);
   label = gtk_label_new(TXT_PARAMETERS);
   gtk_container_add(GTK_CONTAINER(alignment), label);
   gtk_widget_show(alignment);
   gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_RIGHT);
   gtk_widget_show(label);
   gtk_table_attach(GTK_TABLE(table), alignment, 0, 1, 2, 3,
		    GTK_FILL, (GtkAttachOptions) (0), 0, 0);

   act_key = entry = gtk_entry_new_with_max_length(4096);
   gtk_widget_show(entry);
   gtk_widget_set_sensitive(entry, FALSE);
   /* gtk_widget_set_usize(entry, 24, -1); */
   gtk_table_attach(GTK_TABLE(table), entry, 1, 2, 0, 1,
		    GTK_EXPAND | GTK_FILL, (GtkAttachOptions) (0), 0, 0);

   button = gtk_button_new_with_label("Change");
   gtk_widget_show(button);
   gtk_table_attach(GTK_TABLE(table), button, 2, 3, 0, 1,
		    GTK_EXPAND | GTK_FILL, (GtkAttachOptions) (0), 0, 0);
   gtk_signal_connect(GTK_OBJECT(button), "clicked",
		      GTK_SIGNAL_FUNC(e_cb_key_change), NULL);

   m = gtk_menu_new();
   gtk_widget_show(m);

   mi = gtk_menu_item_new_with_label("NONE");
   gtk_widget_show(mi);
   gtk_signal_connect(GTK_OBJECT(mi), "activate",
		      GTK_SIGNAL_FUNC(e_cb_modifier), (gpointer) 0);
   gtk_menu_append(GTK_MENU(m), mi);
   {
      gint                i;

      for (i = 1; i < 21; i++)
	{
	   mi = gtk_menu_item_new_with_label(mod_str[i]);
	   gtk_widget_show(mi);
	   gtk_signal_connect(GTK_OBJECT(mi), "activate",
			      GTK_SIGNAL_FUNC(e_cb_modifier), (gpointer) i);
	   gtk_menu_append(GTK_MENU(m), mi);
	}
   }

   act_mod = om = gtk_option_menu_new();
   gtk_widget_show(om);
   gtk_option_menu_set_menu(GTK_OPTION_MENU(om), m);
   gtk_option_menu_set_history(GTK_OPTION_MENU(om), 0);
   gtk_table_attach(GTK_TABLE(table), om, 1, 3, 1, 2,
		    GTK_EXPAND | GTK_FILL, (GtkAttachOptions) (0), 0, 0);

   act_params = entry = gtk_entry_new_with_max_length(4096);
   gtk_widget_show(entry);
   gtk_widget_set_sensitive(entry, FALSE);
   gtk_table_attach(GTK_TABLE(table), entry, 1, 3, 2, 3,
		    GTK_EXPAND | GTK_FILL, (GtkAttachOptions) (0), 0, 0);
   gtk_signal_connect(GTK_OBJECT(entry), "changed",
		      GTK_SIGNAL_FUNC(on_change_params), NULL);

   scrollybit = gtk_scrolled_window_new(NULL, NULL);
   gtk_widget_show(scrollybit);
   gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollybit),
				  GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

   act_clist = gtk_clist_new(1);
   gtk_widget_show(act_clist);
   gtk_box_pack_start(GTK_BOX(frame_vbox), scrollybit, TRUE, TRUE, 0);
   gtk_clist_set_column_title(GTK_CLIST(act_clist), 0, "Action Used:");
   gtk_clist_column_titles_show(GTK_CLIST(act_clist));
   gtk_signal_connect(GTK_OBJECT(act_clist), "select_row",
		      GTK_SIGNAL_FUNC(change_action), NULL);
   gtk_container_add(GTK_CONTAINER(scrollybit), act_clist);

   {
      const char         *stuff[1];
      int                 k, row;

      action_index_to_row = calloc(action_count, sizeof(int));
      action_row_to_index = calloc(action_count, sizeof(int));

      for (k = row = 0; (actions[k].text); k++)
	{
	   if (e16_ver == VER_E16_OLD && actions[k].id < 0)
	      continue;
	   stuff[0] = actions[k].text;
	   gtk_clist_append(GTK_CLIST(act_clist), (char **)stuff);
	   action_index_to_row[k] = row;
	   action_row_to_index[row++] = k;
	}
   }

   hbox = gtk_hbox_new(FALSE, 0);
   gtk_widget_show(hbox);
   gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);

   button = gtk_button_new_with_label(" New Keybinding ");
   gtk_widget_show(button);
   gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, FALSE, 5);
   gtk_signal_connect(GTK_OBJECT(button), "clicked",
		      GTK_SIGNAL_FUNC(on_create_row), NULL);

   button = gtk_button_new_with_label(" Delete Current Row ");
   gtk_widget_show(button);
   gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, FALSE, 5);
   gtk_signal_connect(GTK_OBJECT(button), "clicked",
		      GTK_SIGNAL_FUNC(on_delete_row), NULL);

   button = gtk_button_new_with_label(" Save ");
   gtk_widget_show(button);
   gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, FALSE, 5);
   gtk_signal_connect(GTK_OBJECT(button), "clicked",
		      GTK_SIGNAL_FUNC(on_save_data), NULL);

   button = gtk_button_new_with_label(" Quit ");
   gtk_widget_show(button);
   gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, FALSE, 5);
   gtk_signal_connect(GTK_OBJECT(button), "clicked",
		      GTK_SIGNAL_FUNC(on_exit_application), NULL);

   gtk_clist_select_row(GTK_CLIST(clist), 0, 0);

   return list_window;
}

static void
receive_ipc_msg(gchar * msg)
{
   gdk_flush();
   e_ipc_msg = g_strdup(msg);

   gtk_main_quit();
}

static int
get_e16_version(void)
{
   char               *msg;
   const char         *s;
   int                 ver, minor;

   ver = VER_E16_OLD;

   CommsSend("ver");
   msg = wait_for_ipc_msg();
   if (!msg)
      goto done;

   s = strstr(msg, "0.16.");
   if (!s)
      goto done;

   sscanf(s, "0.16.%d", &minor);
   if (minor >= 8)
      ver = VER_E16_8;

 done:
   if (msg)
      free(msg);
   return ver;
}

static void
load_actions(void)
{
   char                kbdb[1024], buf[1024], text[1024], command[1024];
   char               *s;
   FILE               *f;
   int                 n, opt;
   unsigned int        nao;
   ActionOpt          *pao;

   actions = actions_default;

   if (e16_ver == VER_E16_OLD)
      return;

   /* FIXME - Should be fetched via IPC. */

   s = getenv("EROOT");
   if (!s)
      return;

   snprintf(kbdb, sizeof(kbdb), "%s/config/e16keyedit.db", s);

   f = fopen(kbdb, "r");
   if (!f)
      return;

   nao = 0;
   pao = NULL;
   for (;;)
     {
	s = fgets(buf, sizeof(buf), f);
	if (!s)
	   break;
	while (isspace(*s))
	   s++;
	if (*s == '\0' || *s == '#')
	   continue;
	n = strlen(s);
	while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r'))
	   n--;
	if (n <= 0)
	   continue;
	s[n] = '\0';

#if DEBUG > 0
	printf("Got: %s\n", s);
#endif
	text[0] = command[0] = '\0';
	opt = -1;
	n = sscanf(s, "\"%1023[^\"]\", %d, \"%1023[^\"]\"", text, &opt,
		   command);
	if (n < 2)
	  {
	     printf("*** ERROR: %s\n", buf);
	     printf
		("*** ERROR: Keybindings database (%s) corrupt, using defaults.\n",
		 kbdb);
	     if (pao)
		free(pao);
	     fclose(f);
	     return;
	  }
#if DEBUG > 0
	printf("n=%d t=%s o=%d c=%s\n", n, text, opt, command);
#endif

	pao = realloc(pao, (nao + 1) * sizeof(ActionOpt));
	memset(pao + nao, 0, sizeof(ActionOpt));
	pao[nao].text = strdup(text);
	pao[nao].param_tpe = opt;
	if (command[0])
	   pao[nao].command = strdup(command);
	nao++;
     }
   fclose(f);

   if (nao == 0)
      return;			/* No entries ??? */

   /* Add terminator record */
   pao = realloc(pao, (nao + 1) * sizeof(ActionOpt));
   memset(pao + nao, 0, sizeof(ActionOpt));

   actions = pao;
   action_count = nao;
}

int
main(int argc, char *argv[])
{
   GtkWidget          *lister;

   gtk_set_locale();
   gtk_init(&argc, &argv);

   tooltips = gtk_tooltips_new();
   accel_group = gtk_accel_group_new();

   if (!CommsInit(receive_ipc_msg))
     {
	GtkWidget          *win, *label, *align, *frame, *button, *vbox;

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

   CommsSend("set clientname Enlightenment Keybinding Configuration Utility");
   CommsSend("set version " VERSION);
#if 0
   CommsSend("set author Mandrake (Geoff Harrison)");
   CommsSend("set email mandrake@mandrake.net");
   CommsSend("set web http://mandrake.net/");
   CommsSend("set address C/O VA Linux Systems, USA");
   CommsSend("set info "
	     "This is the Enlightenemnt KeyBindings Configuration Utility\n"
	     "that uses Enlightenment's IPC mechanism to configure\n"
	     "it remotely.");
#endif

   e16_ver = get_e16_version();

   load_actions();

   lister = create_list_window();

   gtk_clist_set_column_auto_resize(GTK_CLIST(clist), 0, TRUE);
   gtk_clist_set_column_auto_resize(GTK_CLIST(clist), 1, TRUE);

   gtk_widget_show(lister);
   gtk_signal_connect(GTK_OBJECT(lister), "destroy",
		      GTK_SIGNAL_FUNC(on_exit_application), NULL);
   gtk_signal_connect(GTK_OBJECT(lister), "delete_event",
		      GTK_SIGNAL_FUNC(on_exit_application), NULL);

   gtk_main();

   return 0;
}
