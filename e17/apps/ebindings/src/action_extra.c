/**************************************************************************
 * action_extra.c
 * Project: Ebindings
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * October 13, 2001
 * Notebook tab, and callback function definitions for the focus interface
 * in ebindings
 *************************************************************************/
#include "action_extra.h"

static struct
{
   GtkWidget *pos;              /* position option menu */
   GtkWidget *ev;               /* event type option menu */
   GtkWidget *ac;               /* action type option menu */
   GtkWidget *mod;              /* modifier key option menu */
}
f_menus;
static GtkWidget *button_e;     /* entry widget for the button number */
static GtkWidget *clist;        /* columned list of mouse events */
static int row_count;           /* the number of rows in clist */
static int current_row;         /* the currently selected row in the clist */

/* global variables to the focus interface */

static void focus_clist_select_row(GtkWidget * w, gint row, gint column,
                                   GdkEventButton * ev, gpointer data);

static void window_pos_option_menu_modified(GtkWidget *, gpointer);
static void event_option_menu_modified(GtkWidget *, gpointer);
static void action_option_menu_modified(GtkWidget *, gpointer);
static void mod_key_option_menu_modified(GtkWidget *, gpointer);

static void event_change_button_cb(GtkWidget *, gpointer);
static void event_save_button_cb(GtkWidget *, gpointer);
static void event_add_button_cb(GtkWidget *, gpointer);
static void event_del_button_cb(GtkWidget *, gpointer);

/* callbacks(commented below) */

/* option menu data */
static gchar *mod_str[] = {
   "NONE",
   "SHIFT",
   "CTRL",
   "SHIFT+CTRL",
   "ALT",
   "SHIFT+ALT",
   "CTRL+ALT",
   "CTRL+SHIFT+ALT",
   "WIN",
   "WIN+SHIFT",
   "WIN+CTRL",
   "WIN+SHIFT+CTRL",
   "WIN+ALT",
   "WIN+SHIFT+ALT",
   "WIN+CTRL",
   "WIN+CTRL+SHIFT",
   "WIN+CTRL+SHIFT+ALT"
};

/* options based on enum _ev_modifiers in Ecore.h */
static gchar *e_action_str[] = {
   "Window_Move",
   "Window_Resize",
   "Window_Resize_Horizontal",
   "Window_Resize_Vertical",
   "Window_Close",
   "Window_Kill",
   "Window_Shade",
   "Window_Raise",
   "Window_Lower",
   "Window_Raise_Lower",
   "Menu",
   "Window_Stick",
   "Sound",
   "Window_Iconify",
   "Window_Max_Size",
   "Window_Snap",
   "Window_Zoom",
   "Desktop",
   "Desktop_Relative",
   "Window_Next",
   /* count is 20 currently */
   /* based on options in e_action_init in actions.c in e17/e/src */
};
static gchar *ebits_hot_spots[] = {
   "Decoration",
   "Title_Bar",
   "Close",
   "Iconify",
   "Raise",
   "Raise_Lower",
   "Max_Size",
   "Full_Screen",
   "Shade",
   "Menu",
   "Resize",
   "Resize_Vertical",
   "Resize_Horizontal",
   "Icon",
   "State",
   "Load",
   "Geometry",
   "Scrollbar_Trough",
   "Scrollbar_Bar",
   "Scrollbar_Arrow1",
   "Scrollber_Arrow2",
   "Window_Grab",
   "Window_Place"
      /* parts of this came from etcher class types on the properties tag */
};
static gchar *e_action_type[] = {
   "Mouse In On Window Position",
   "Mouse Out On Window Position",
   "Mouse Button Down On Window Position",
   "Mouse Button Double Click On Window Position",
   "Mouse Button Triple Click On Window Position",
   "Mouse Button Up On Window Position",
   "Mouse Click On Window Position",
   "Mouse Move On Window Position"
};

/* see actions.h in the e17 source tree enum _e_action_type */

/* the world can only ask for this to be added to an existing notebook, all
 * the callbacks and variable names are static for cleanliness 
 * @param w: widget pointing to the window
 * @param note: widget pointing to the notebook to attach to
 * @param sheet: what tab to insert as
 */
void
add_actions_extra_notebook(GtkWidget * w, GtkWidget * note, int sheet)
{

   GtkWidget *tab_label;
   GtkWidget *hpaned;
   GtkWidget *hbox;
   GtkWidget *vbox;
   GtkWidget *scroller1, *scroller2;
   GtkWidget *frame;
   GtkWidget *table;
   int i;
   eaction_item *e;

   GtkWidget *newb, *delb, *saveb, *button_b;
   GtkWidget *o_menu, *menu, *option;
   GtkWidget *win_pos_l, *event_l, *mod_l, *action_l, *button_l;
   GtkWidget *disclaimer;
   char buf[64];

   gchar *fields[] = {
      "Window Position", "Event", "Action", "Mouse Button", "Modifier Key"
   };
   gchar *key_fields[5];

   current_row = row_count = 0;
   /* setup the static globals =) */

   tab_label = gtk_label_new("Extras");

   hpaned = gtk_hpaned_new();
   gtk_notebook_insert_page(GTK_NOTEBOOK(note), hpaned, tab_label, sheet);

   scroller1 = gtk_scrolled_window_new(NULL, NULL);
   gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller1),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

   clist = gtk_clist_new_with_titles(5, fields);
   gtk_widget_ref(clist);
   gtk_widget_set_name(GTK_WIDGET(clist), "focus_list");

   /* gtk_clist_set_column_visibility(GTK_CLIST(clist), 0, FALSE); */
   gtk_clist_set_column_visibility(GTK_CLIST(clist), 1, FALSE);
   gtk_clist_set_column_auto_resize(GTK_CLIST(clist), 2, TRUE);
   gtk_clist_set_column_auto_resize(GTK_CLIST(clist), 3, TRUE);
   gtk_clist_set_column_auto_resize(GTK_CLIST(clist), 4, TRUE);
   gtk_signal_connect(GTK_OBJECT(clist), "select-row",
                      GTK_SIGNAL_FUNC(focus_clist_select_row), NULL);

   gtk_container_add(GTK_CONTAINER(scroller1), clist);
   gtk_paned_pack1(GTK_PANED(hpaned), scroller1, TRUE, FALSE);

   vbox = gtk_vbox_new(FALSE, 0);

   frame = gtk_frame_new("Edit Focus Settings");
   gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 1);

   /* Start Keybind table */
   table = gtk_table_new(5, 3, FALSE);

   /* Row 1 */
   win_pos_l = gtk_label_new("Window Position:");
   gtk_table_attach(GTK_TABLE(table), win_pos_l, 0, 1, 0, 1, GTK_FILL, 0, 2,
                    2);

   o_menu = gtk_option_menu_new();
   gtk_widget_ref(o_menu);
   gtk_widget_set_name(o_menu, "win_pos_menu");
   gtk_table_attach(GTK_TABLE(table), o_menu, 1, 3, 0, 1, GTK_FILL, 0, 2, 2);

   f_menus.pos = o_menu;
   menu = gtk_menu_new();

   for (i = 0; i < EBITS_SPOT_MAX; i++)
   {
      option = gtk_menu_item_new_with_label(ebits_hot_spots[i]);
      gtk_widget_show(option);
      gtk_signal_connect(GTK_OBJECT(option), "activate",
                         GTK_SIGNAL_FUNC(window_pos_option_menu_modified),
                         (gpointer) i);
      gtk_menu_append(GTK_MENU(menu), option);
   }
   gtk_option_menu_set_menu(GTK_OPTION_MENU(o_menu), menu);
   gtk_option_menu_set_history(GTK_OPTION_MENU(o_menu), 0);
   gtk_widget_show(menu);
   gtk_widget_show(o_menu);

   /* Row 2 */
   event_l = gtk_label_new("Event Type: ");
   gtk_table_attach(GTK_TABLE(table), event_l, 0, 1, 1, 2, GTK_FILL, 0, 2, 2);

   o_menu = gtk_option_menu_new();
   gtk_widget_ref(o_menu);
   gtk_widget_set_name(o_menu, "event_menu");
   gtk_table_attach(GTK_TABLE(table), o_menu, 1, 3, 1, 2, GTK_FILL, 0, 2, 2);

   f_menus.ev = o_menu;
   menu = gtk_menu_new();

   for (i = 0; i < E_ACTION_TYPE_MAX; i++)
   {
      option = gtk_menu_item_new_with_label(e_action_type[i]);
      gtk_widget_show(option);
      gtk_signal_connect(GTK_OBJECT(option), "activate",
                         GTK_SIGNAL_FUNC(event_option_menu_modified),
                         (gpointer) i);
      gtk_menu_append(GTK_MENU(menu), option);
   }
   gtk_option_menu_set_menu(GTK_OPTION_MENU(o_menu), menu);
   gtk_option_menu_set_history(GTK_OPTION_MENU(o_menu), 0);
   gtk_widget_show(menu);
   gtk_widget_show(o_menu);

   /* Row 3 */
   action_l = gtk_label_new("Action Request: ");
   gtk_table_attach(GTK_TABLE(table), action_l, 0, 1, 2, 3, GTK_FILL, 0, 2,
                    2);

   o_menu = gtk_option_menu_new();
   gtk_widget_ref(o_menu);
   gtk_widget_set_name(o_menu, "mod_menu");
   gtk_table_attach(GTK_TABLE(table), o_menu, 1, 3, 2, 3, GTK_FILL, 0, 2, 2);

   f_menus.ac = o_menu;

   menu = gtk_menu_new();

   for (i = 0; i < FOCUS_ACTION_STR_MAX; i++)
   {
      option = gtk_menu_item_new_with_label(e_action_str[i]);
      gtk_widget_show(option);
      gtk_signal_connect(GTK_OBJECT(option), "activate",
                         GTK_SIGNAL_FUNC(action_option_menu_modified),
                         (gpointer) i);
      gtk_menu_append(GTK_MENU(menu), option);
   }
   gtk_option_menu_set_menu(GTK_OPTION_MENU(o_menu), menu);
   gtk_option_menu_set_history(GTK_OPTION_MENU(o_menu), 0);
   gtk_widget_show(menu);
   gtk_widget_show(o_menu);

   /* Row 4 */
   button_l = gtk_label_new("Button Pressed: ");
   gtk_table_attach(GTK_TABLE(table), button_l, 0, 1, 3, 4, GTK_FILL, 0, 2,
                    2);

   button_e = gtk_entry_new();
   gtk_entry_set_editable(GTK_ENTRY(button_e), FALSE);
   gtk_table_attach(GTK_TABLE(table), button_e, 1, 2, 3, 4, GTK_FILL, 0, 2,
                    2);

   button_b = gtk_button_new_with_label("Change");
   gtk_table_attach(GTK_TABLE(table), button_b, 2, 3, 3, 4, GTK_FILL, 0, 2,
                    2);
   gtk_signal_connect(GTK_OBJECT(button_b), "clicked",
                      GTK_SIGNAL_FUNC(event_change_button_cb), clist);

   /* Row 5 */
   mod_l = gtk_label_new("Modifier Key: ");
   gtk_table_attach(GTK_TABLE(table), mod_l, 0, 1, 4, 5, GTK_FILL, 0, 2, 2);

   o_menu = gtk_option_menu_new();
   gtk_widget_ref(o_menu);
   gtk_widget_set_name(o_menu, "mod_menu");
   gtk_table_attach(GTK_TABLE(table), o_menu, 1, 3, 4, 5, GTK_FILL, 0, 2, 2);

   f_menus.mod = o_menu;

   menu = gtk_menu_new();

   for (i = 0; i < MOD_STR_MAX; i++)
   {
      option = gtk_menu_item_new_with_label(mod_str[i]);
      gtk_widget_show(option);
      gtk_signal_connect(GTK_OBJECT(option), "activate",
                         GTK_SIGNAL_FUNC(mod_key_option_menu_modified),
                         (gpointer) i);
      gtk_menu_append(GTK_MENU(menu), option);
   }
   gtk_option_menu_set_menu(GTK_OPTION_MENU(o_menu), menu);
   gtk_option_menu_set_history(GTK_OPTION_MENU(o_menu), 0);
   gtk_widget_show(menu);
   gtk_widget_show(o_menu);
   /* Row 6 */

   scroller2 = gtk_scrolled_window_new(NULL, NULL);
   gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller2),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

   disclaimer = gtk_text_new(NULL, NULL);
   gtk_text_set_editable(GTK_TEXT(disclaimer), FALSE);
   gtk_text_insert(GTK_TEXT(disclaimer), NULL, NULL, NULL,
                   "This is only a test, you can change values here, but \n"
                   "this is not a good interface for handling this kind of\n"
                   "configuration.  It does allow you to modify how the mouse\n"
                   "interacts with windows, but some ebits knowledge is needed \n"
                   "to change certain features.", 254);

   gtk_container_add(GTK_CONTAINER(scroller2), disclaimer);
   gtk_table_attach(GTK_TABLE(table), scroller2, 0, 3, 5, 6,
                    GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 2, 2);
   /* end Rows */

   /* add table to the frame */
   gtk_container_add(GTK_CONTAINER(frame), table);

   /* vbox for save button across the botom */
   hbox = gtk_hbox_new(FALSE, 0);

   newb = gtk_button_new_with_label("New Entry");
   gtk_widget_ref(newb);
   gtk_widget_set_name(newb, "new_focus_button");
   gtk_signal_connect(GTK_OBJECT(newb), "clicked",
                      GTK_SIGNAL_FUNC(event_add_button_cb), clist);

   gtk_box_pack_start(GTK_BOX(hbox), newb, TRUE, TRUE, 2);

   delb = gtk_button_new_with_label("Delete Selected");
   gtk_widget_ref(delb);
   gtk_widget_set_name(delb, "del_focus_button");
   gtk_signal_connect(GTK_OBJECT(delb), "clicked",
                      GTK_SIGNAL_FUNC(event_del_button_cb), clist);
   gtk_box_pack_start(GTK_BOX(hbox), delb, TRUE, TRUE, 2);

   saveb = gtk_button_new_with_label("Save");
   gtk_widget_ref(saveb);
   gtk_widget_set_name(saveb, "save_focus_button");
   gtk_signal_connect(GTK_OBJECT(saveb), "clicked",
                      GTK_SIGNAL_FUNC(event_save_button_cb), clist);
   gtk_box_pack_start(GTK_BOX(hbox), saveb, TRUE, TRUE, 2);

   gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);
   /* vbox is setup and added to the hbox, which is going to the right pane */

   gtk_paned_pack2(GTK_PANED(hpaned), vbox, FALSE, FALSE);

   /* lower right hbox */
   gtk_widget_show(newb);
   gtk_widget_show(delb);
   gtk_widget_show(saveb);
   gtk_widget_show(hbox);

   /* upper right frame and table */
   gtk_widget_show(disclaimer);
   gtk_widget_show(scroller2);
   gtk_widget_show(button_b);
   gtk_widget_show(button_l);
   gtk_widget_show(button_e);
   gtk_widget_show(action_l);
   gtk_widget_show(event_l);
   gtk_widget_show(mod_l);
   gtk_widget_show(win_pos_l);
   gtk_widget_show(table);
   gtk_widget_show(frame);

   /* show right pane(consists of vbox */
   gtk_widget_show(vbox);

   /* show left side scroller and clist */
   gtk_widget_show(scroller1);
   gtk_widget_show(clist);

   /* show hpaned for the notebook */
   gtk_widget_show(hpaned);
   /* show notebook label */
   gtk_widget_show(tab_label);

   ewd_list_goto_first(action_container.focus);
   while ((e = (eaction_item *) ewd_list_next(action_container.focus)))
   {
      snprintf(buf, 64, "Button %d", e->button);
      key_fields[0] = g_strdup(e->name);
      key_fields[1] = g_strdup(e_action_type[e->event]);
      key_fields[2] = g_strdup(e->action);
      key_fields[3] = g_strdup(buf);
      key_fields[4] = g_strdup(mod_str[e->modifiers]);

      gtk_clist_append(GTK_CLIST(clist), key_fields);
      row_count++;

      IF_FREE(key_fields[0]);
      IF_FREE(key_fields[1]);
      IF_FREE(key_fields[2]);
      IF_FREE(key_fields[3]);
      IF_FREE(key_fields[4]);
   }
   if (row_count)
      gtk_clist_select_row(GTK_CLIST(clist), 0, 0);

   return;
   UN(w);
}

/* focus_clist_select_row: When the focus clist has a row selected do this
 * @param w: pointer to the clist containing the focus settings
 * @param row: the row that is selected
 * @param column: the colum in the clist selected
 * @param ev: I haven't a clue 
 * @param data: always be NULL
 */
static void
focus_clist_select_row(GtkWidget * w, gint row, gint column,
                       GdkEventButton * ev, gpointer data)
{
   gchar *fields[5] = {
      "", "", "", "", ""
   };
   int i;

   if (!w)
      return;
   current_row = row;

   gtk_clist_get_text(GTK_CLIST(w), row, 0, &fields[0]);
   gtk_clist_get_text(GTK_CLIST(w), row, 1, &fields[1]);
   gtk_clist_get_text(GTK_CLIST(w), row, 2, &fields[2]);
   gtk_clist_get_text(GTK_CLIST(w), row, 3, &fields[3]);
   gtk_clist_get_text(GTK_CLIST(w), row, 4, &fields[4]);
   /* get text from the selected widget */

   /* then update the option menus and button number accordingly */
   for (i = 0; i < EBITS_SPOT_MAX; i++)
   {
      if (!strcmp(fields[0], ebits_hot_spots[i]))
      {
         gtk_option_menu_set_history(GTK_OPTION_MENU(f_menus.pos), i);
         break;
      }
   }
   for (i = 0; i < E_ACTION_TYPE_MAX; i++)
   {
      if (!strcmp(fields[1], e_action_type[i]))
      {
         gtk_option_menu_set_history(GTK_OPTION_MENU(f_menus.ev), i);
         break;
      }
   }
   for (i = 0; i < FOCUS_ACTION_STR_MAX; i++)
   {
      if (!strcmp(fields[2], e_action_str[i]))
      {
         gtk_option_menu_set_history(GTK_OPTION_MENU(f_menus.ac), i);
         break;
      }
   }
   for (i = 0; i < MOD_STR_MAX; i++)
   {
      if (!strcmp(fields[4], mod_str[i]))
      {
         gtk_option_menu_set_history(GTK_OPTION_MENU(f_menus.mod), i);
         break;
      }
   }
   if ((fields[3]) && (strlen(fields[3]) > 0))
   {
      gchar *s;
      char buf[64];

      s = fields[3];
      s += 7;
      snprintf(buf, 64, "%s", s);
      /* chop the word Button off the beginning */
      gtk_entry_set_text(GTK_ENTRY(button_e), buf);
   }

   return;
   UN(w);
   UN(ev);
   UN(data);
   UN(column);
}

/* 
 * window_pos_option_menu_modified: when the option menu is modified, update
 * the clist to reflect the latest changes 
 * @param w: is actually a pointer to the menu item option int the menu, so it's
 * 	useless, access the pos option menu with the global f_menus->pos
 * @param data: is the integer value in the option menu of the item selected
 */
static void
window_pos_option_menu_modified(GtkWidget * w, gpointer data)
{
   int i;
   gchar *new_pos = NULL;

   if (!w)
      return;

   i = (int) data;

   gtk_option_menu_set_history(GTK_OPTION_MENU(f_menus.pos), i);
   /* set the option upto date */

   new_pos = g_strdup(ebits_hot_spots[i]);
   gtk_clist_set_text(GTK_CLIST(clist), current_row, 0, new_pos);
   /* update the clist */

   return;
   UN(w);
   UN(data);
}

/* 
 * event_option_menu_modified: when the option menu is modified, update
 * the clist to reflect the latest changes 
 * @param w: is actually a pointer to the menu item option in the menu, so it's
 * 	useless, access the pos option menu with the global f_menus->pos
 * @param data: is the integer value in the option menu of the item selected
 */
static void
event_option_menu_modified(GtkWidget * w, gpointer data)
{
   int i;
   gchar *new_ev = NULL;

   if (!w)
      return;

   i = (int) data;
   gtk_option_menu_set_history(GTK_OPTION_MENU(f_menus.ev), i);
   /* update the history */

   new_ev = g_strdup(e_action_type[i]);
   gtk_clist_set_text(GTK_CLIST(clist), current_row, 1, new_ev);
   /* update clist */

   return;
   UN(w);
   UN(data);
}

/* 
 * mod_key_option_menu_modified: when the option menu is modified, update
 * the clist to reflect the latest changes 
 * @param w: is actually a pointer to the menu item option in the menu, so it's
 * 	useless, access the pos option menu with the global f_menus->pos
 * @param data: is the integer value in the option menu of the item selected
 */
static void
mod_key_option_menu_modified(GtkWidget * w, gpointer data)
{
   int i;
   gchar *new_mod = NULL;

   if (!w)
      return;

   i = (int) data;
   gtk_option_menu_set_history(GTK_OPTION_MENU(f_menus.mod), i);
   /* update history */

   new_mod = g_strdup(mod_str[i]);
   gtk_clist_set_text(GTK_CLIST(clist), current_row, 4, new_mod);
   /* update clist */
   return;
   UN(w);
   UN(data);
}

/* 
 * action_option_menu_modified: when the option menu is modified, update
 * the clist to reflect the latest changes 
 * @param w: is actually a pointer to the menu item option in the menu, so it's
 * 	useless, access the pos option menu with the global f_menus->pos
 * @param data: is the integer value in the option menu of the item selected
 */
static void
action_option_menu_modified(GtkWidget * w, gpointer data)
{
   int i;
   gchar *new_ac = NULL;

   if (!w)
      return;

   i = (int) data;
   gtk_option_menu_set_history(GTK_OPTION_MENU(f_menus.ac), i);
   /* update history */

   new_ac = g_strdup(e_action_str[i]);
   gtk_clist_set_text(GTK_CLIST(clist), current_row, 2, new_ac);
   /* update clist */

   return;
   UN(w);
   UN(data);
}

/*
 * event_save_button_cb: when we want to save the current focus settings.
 * @param w: pointer to the widget "Save" button
 * @param data: pointer to the global clist, could be NULL
 *
 * First we clear out what is currently in the focus list in
 * 	eaction_container(see eaction_item.h).  Those are stale either from the
 * 	last time the clist was dumped or from parse time.  Clear the focus
 * 	list, populate a new focus list with the contents of the focus clist.
 * 	Write the db. NOTE: WE DO NOT TOUCH THE KEYBINDS LIST, it's current
 * 	state is the last it was left in and has not been freed.  When focus
 * 	settings are called we accept that the keys list fully populated with
 * 	valid data.
 */
static void
event_save_button_cb(GtkWidget * w, gpointer data)
{
   int i, j, written;
   eaction_item *e = NULL;
   Ewd_List *l;
   gchar *tmp;

   action_container_focus_reinit();
   l = action_container.focus;
   /* clear out the focus list, give it a local pointer val that's easier to
      read */

   for (i = 0; i < row_count; i++)
   {
      e = eaction_item_new();

      gtk_clist_get_text(GTK_CLIST(data), i, 0, &tmp);
      for (j = 0; j < EBITS_SPOT_MAX; j++)
      {
         if (!strcmp(tmp, ebits_hot_spots[j]))
         {
            e->name = strdup(tmp);
            break;
         }
      }
      if (!e->name)
      {
         /* no name == fatal */
         fprintf(stderr,
                 "Unable to match your selection to any "
                 "available window positions\nI'm omitting this entry %s\n",
                 tmp);
         eaction_item_free(e);
         continue;
      }

      gtk_clist_get_text(GTK_CLIST(data), i, 1, &tmp);
      for (j = 0; j < E_ACTION_TYPE_MAX; j++)
      {
         if (!strcmp(tmp, e_action_type[j]))
         {
            e->event = j;
            break;
         }
      }
      tmp = NULL;

      gtk_clist_get_text(GTK_CLIST(data), i, 2, &tmp);
      for (j = 0; j < FOCUS_ACTION_STR_MAX; j++)
      {
         if (!strcmp(tmp, e_action_str[j]))
         {
            e->action = strdup(tmp);
            break;
         }
      }
      tmp = NULL;

      gtk_clist_get_text(GTK_CLIST(data), i, 4, &tmp);
      for (j = 0; j < MOD_STR_MAX; j++)
      {
         if (!strcmp(tmp, mod_str[j]))
         {
            e->modifiers = j;
            break;
         }
      }
      tmp = NULL;

      gtk_clist_get_text(GTK_CLIST(data), i, 3, &tmp);
      if ((tmp) && (strlen(tmp) > 0))
      {
         gchar *s;
         char buf[64];

         s = tmp;
         s += 7;
         snprintf(buf, 64, "%s", s);
         /* chop the word Button off the beginning */
         e->button = atoi(buf);
      }
      tmp = NULL;

      e->key = strdup("");
      e->params = strdup("");

      ewd_list_append(l, e);
   }
   written = write_user_actions_db();
   /* write, if successful pass the message */

   if (written)
      status_bar_message("Error Saving Your Focus Settings", 3000);
   else
      status_bar_message("Successfully Saved Your Focus Settings", 3000);

   return;
   UN(w);
   UN(data);
}

/*
 * event_add_button_cb: 
 *	Add a new entry right below the currently selected item  
 * @param w: unused
 * @param data: pointer to the clist 
 */
static void
event_add_button_cb(GtkWidget * w, gpointer data)
{
   gchar *fields[5];
   gint retval;

   fields[0] = g_strdup(ebits_hot_spots[0]);
   fields[1] = g_strdup(e_action_type[0]);
   fields[2] = g_strdup(e_action_str[0]);
   fields[3] = g_strdup("Button 0");
   fields[4] = g_strdup("NONE");

   retval = gtk_clist_insert(GTK_CLIST(data), (current_row + 1), fields);

   gtk_clist_select_row(GTK_CLIST(data), retval, 0);
   row_count++;
   return;
   UN(w);
   UN(data);
}

/* 
 * event_del_button_cb: delete the currently selected item from the clist 
 * @param w: not used
 * @param data: pointer to the clist
 */
static void
event_del_button_cb(GtkWidget * w, gpointer data)
{
   gtk_clist_remove(GTK_CLIST(data), current_row);
   current_row = current_row == 0 ? 0 : current_row--;
   gtk_clist_select_row(GTK_CLIST(data), current_row, 0);
   row_count--;

   return;
   UN(w);
   UN(data);
}

/* 
 * event_change_button_cb: callback when the change button button is
 * clicked.  It displays a small window that accepts mouse clicks.  It's
 * easy, intuitive, and you don't even have to know the actual mouse button
 * number, just click to set what you want.
 * @param w: pointer to the button(useless)
 * @param data: the global clist
 */
static void
event_change_button_cb(GtkWidget * w, gpointer data)
{
   GtkWidget *win, *label, *frame, *align;

   if (data)
      w = NULL;

   win = gtk_window_new(GTK_WINDOW_POPUP);
   gtk_window_set_policy(GTK_WINDOW(win), 0, 0, 1);
   gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_MOUSE);
   frame = gtk_frame_new(NULL);
   gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_OUT);
   align = gtk_alignment_new(0.0, 0.0, 0.0, 0.0);
   gtk_container_set_border_width(GTK_CONTAINER(align), 32);
   label =
      gtk_label_new("Please press the mouse button \n"
                    "you wish to modify this event to use \n" "from now on.");
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
      char buf[64], buf2[64];
      XEvent ev;

      gdk_window_set_events(win->window, GDK_BUTTON_PRESS_MASK);
      XSetInputFocus(GDK_DISPLAY(), GDK_WINDOW_XWINDOW(win->window),
                     RevertToPointerRoot, CurrentTime);
      gdk_pointer_grab(win->window, TRUE, GDK_BUTTON_PRESS_MASK, NULL, NULL,
                       CurrentTime);
      XWindowEvent(GDK_DISPLAY(), GDK_WINDOW_XWINDOW(win->window),
                   ButtonPressMask, &ev);
      gdk_pointer_ungrab(gdk_time_get());

      snprintf(buf, 64, "Button %d", ev.xbutton.button);
      gtk_clist_set_text(GTK_CLIST(data), current_row, 3, buf);

      snprintf(buf2, 64, "%d", ev.xbutton.button);
      gtk_entry_set_text(GTK_ENTRY(button_e), buf2);
   }
   gtk_widget_destroy(win);
}
