/**************************************************************************
 * keybind_interface.c
 * Project: Ebindings
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * October 12, 2001
 * Notebook tab, and callback function definitions for the keybind interface
 * in ebindings
 *************************************************************************/
#include "keybind_interface.h"

static struct
{
   GtkWidget *o, *p, *k, *a, *list;
   /* o = option menu p = parameter entry k = key entry a = action used */
   int count, current_row;
}
k_entry;

static void keybind_option_menu_modified(GtkWidget *, gpointer);
static void keybind_clist_row_select(GtkWidget *, gint, gint, GdkEventButton *,
                                     gpointer);
static void keybind_parameter_entry_changed(GtkWidget *, gpointer);
static void keybind_capture_key_cb(GtkWidget *, gpointer);
static void keybind_save_button_clicked(GtkWidget *, gpointer);
static void keybind_add_button_clicked(GtkWidget *, gpointer);
static void keybind_del_button_clicked(GtkWidget *, gpointer);

/* stuff */
gchar *mod_str[] = {
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
gchar *action_str[] = {
   "Execute",
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
   "Exit",
   "Restart",
   "Window_Stick",
   "Sound",
   "Window_Iconify",
   "Window_Max_Size",
   "Window_Snap",
   "Window_Zoom",
   "Desktop",
   "Window_Next"
      /* count is 22 currently */
      /* based on options in e_action_init in actions.c in e17/e/src */
};

void
add_keybinds_notebook(GtkWidget * w, GtkWidget * note, int sheet)
{
   GtkWidget *tab_label;
   GtkWidget *hpaned;
   GtkWidget *hbox;
   GtkWidget *vbox;
   GtkWidget *clist, *clist2;
   GtkWidget *scroller, *scroller1;
   GtkWidget *frame;
   GtkWidget *table;
   int i;
   eaction_item *e;

   GtkWidget *newb, *delb, *saveb, *changeb;
   GtkWidget *keyl, *modl, *paraml;
   GtkWidget *e_key, *e_param;
   GtkWidget *o_menu, *menu, *option;

   gchar *fields[] = {
      "Modifier", "Key Used", "Action to Perform", "Parameters"
   };
   gchar *action_title[] = {
      "Actions Used"
   };
   gchar *key_fields[4];

   tab_label = gtk_label_new("Keys");

   hpaned = gtk_hpaned_new();
   gtk_notebook_insert_page(GTK_NOTEBOOK(note), hpaned, tab_label, sheet);

   scroller1 = gtk_scrolled_window_new(NULL, NULL);
   gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller1),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

   clist = gtk_clist_new_with_titles(4, fields);
   gtk_widget_ref(clist);
   gtk_widget_set_name(GTK_WIDGET(clist), "keybinds_clist");

   gtk_clist_set_column_auto_resize(GTK_CLIST(clist), 0, TRUE);
   gtk_clist_set_column_auto_resize(GTK_CLIST(clist), 1, TRUE);
   gtk_clist_set_column_auto_resize(GTK_CLIST(clist), 2, TRUE);
   gtk_signal_connect(GTK_OBJECT(clist), "select-row",
                      GTK_SIGNAL_FUNC(keybind_clist_row_select), NULL);

   gtk_container_add(GTK_CONTAINER(scroller1), clist);
   gtk_paned_pack1(GTK_PANED(hpaned), scroller1, TRUE, FALSE);

   vbox = gtk_vbox_new(FALSE, 0);

   frame = gtk_frame_new("Edit Keybindings");
   gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 1);

   /* Start Keybind table */
   table = gtk_table_new(4, 3, FALSE);

   /* Row 1 */
   keyl = gtk_label_new("Key:");
   gtk_table_attach(GTK_TABLE(table), keyl, 0, 1, 0, 1, GTK_FILL, 0, 2, 2);

   e_key = gtk_entry_new();
   gtk_table_attach(GTK_TABLE(table), e_key, 1, 2, 0, 1, GTK_FILL, 0, 2, 2);
   gtk_entry_set_editable(GTK_ENTRY(e_key), FALSE);

   changeb = gtk_button_new_with_label("Change");
   gtk_table_attach(GTK_TABLE(table), changeb, 2, 3, 0, 1, GTK_FILL, 0, 2, 2);
   gtk_signal_connect(GTK_OBJECT(changeb), "clicked",
                      GTK_SIGNAL_FUNC(keybind_capture_key_cb), clist);

   /* Row 2 */
   modl = gtk_label_new("Modifiers");
   gtk_table_attach(GTK_TABLE(table), modl, 0, 1, 1, 2, GTK_FILL, 0, 2, 2);

   o_menu = gtk_option_menu_new();
   gtk_table_attach(GTK_TABLE(table), o_menu, 1, 3, 1, 2, GTK_FILL, 0, 2, 2);

   menu = gtk_menu_new();

   for (i = 0; i < MOD_STR_MAX; i++)
   {
      option = gtk_menu_item_new_with_label(mod_str[i]);
      gtk_widget_show(option);
      gtk_signal_connect(GTK_OBJECT(option), "activate",
                         GTK_SIGNAL_FUNC(keybind_option_menu_modified),
                         (gpointer) i);
      gtk_menu_append(GTK_MENU(menu), option);
   }
   gtk_option_menu_set_menu(GTK_OPTION_MENU(o_menu), menu);
   gtk_option_menu_set_history(GTK_OPTION_MENU(o_menu), 0);

   /* Row 3 */
   paraml = gtk_label_new("Parameters: ");
   gtk_table_attach(GTK_TABLE(table), paraml, 0, 1, 2, 3, GTK_FILL, 0, 2, 2);

   e_param = gtk_entry_new();
   gtk_table_attach(GTK_TABLE(table), e_param, 1, 3, 2, 3, GTK_FILL, 0, 2, 2);

   gtk_signal_connect(GTK_OBJECT(e_param), "changed",
                      GTK_SIGNAL_FUNC(keybind_parameter_entry_changed), clist);
   /* Row 4 */
   scroller = gtk_scrolled_window_new(NULL, NULL);
   gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

   clist2 = gtk_clist_new_with_titles(1, action_title);
   gtk_widget_ref(clist2);
   gtk_widget_set_name(clist2, "action_clist");
   gtk_signal_connect(GTK_OBJECT(clist2), "select-row",
                      GTK_SIGNAL_FUNC(keybind_clist_row_select), NULL);

   for (i = 0; i < ACTION_STR_MAX; i++)
   {
      action_title[0] = g_strdup(action_str[i]);
      gtk_clist_append(GTK_CLIST(clist2), action_title);
      g_free(action_title[0]);
   }
   gtk_container_add(GTK_CONTAINER(scroller), clist2);
   gtk_table_attach(GTK_TABLE(table), scroller, 0, 3, 3, 4, GTK_FILL,
                    GTK_FILL | GTK_EXPAND, 2, 2);

   /* add table to the frame */
   gtk_container_add(GTK_CONTAINER(frame), table);

   /* Setup keyboard container for objects with a large scope */
   k_entry.o = o_menu;
   k_entry.p = e_param;
   k_entry.k = e_key;
   k_entry.list = clist;
   k_entry.a = clist2;
   k_entry.count = k_entry.current_row = 0;
   /* k_entry setup done */

   /* vbox for save button across the botom */
   hbox = gtk_hbox_new(FALSE, 0);

   newb = gtk_button_new_with_label("New Keybind");
   gtk_widget_ref(newb);
   gtk_widget_set_name(newb, "new_keybind_button");
   gtk_signal_connect(GTK_OBJECT(newb), "clicked",
                      GTK_SIGNAL_FUNC(keybind_add_button_clicked), clist);

   gtk_box_pack_start(GTK_BOX(hbox), newb, TRUE, TRUE, 2);

   delb = gtk_button_new_with_label("Delete Selected Keybind");
   gtk_widget_ref(delb);
   gtk_widget_set_name(delb, "del_keybind_button");
   gtk_signal_connect(GTK_OBJECT(delb), "clicked",
                      GTK_SIGNAL_FUNC(keybind_del_button_clicked), clist);
   gtk_box_pack_start(GTK_BOX(hbox), delb, TRUE, TRUE, 2);

   saveb = gtk_button_new_with_label("Save Keybinds");
   gtk_widget_ref(saveb);
   gtk_widget_set_name(saveb, "save_keybind_button");
   gtk_signal_connect(GTK_OBJECT(saveb), "clicked",
                      GTK_SIGNAL_FUNC(keybind_save_button_clicked), clist);
   gtk_box_pack_start(GTK_BOX(hbox), saveb, TRUE, TRUE, 2);

   gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);
   /* vbox is setup and added to the hbox, which is going to the right pane 
    */

   gtk_paned_pack2(GTK_PANED(hpaned), vbox, FALSE, FALSE);

   /* lower right hbox */
   gtk_widget_show(newb);
   gtk_widget_show(delb);
   gtk_widget_show(saveb);
   gtk_widget_show(hbox);

   /* upper right frame and table */
   gtk_widget_show(menu);
   gtk_widget_show(o_menu);
   gtk_widget_show(clist2);
   gtk_widget_show(scroller);
   gtk_widget_show(changeb);
   gtk_widget_show(e_key);
   gtk_widget_show(e_param);
   gtk_widget_show(keyl);
   gtk_widget_show(modl);
   gtk_widget_show(paraml);
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

   parse_user_actions_db();
   ewd_list_goto_first(action_container.keys);
   while ((e = (eaction_item *) ewd_list_next(action_container.keys)))
   {
      key_fields[0] = g_strdup(mod_str[e->modifiers]);
      key_fields[1] = g_strdup(e->key);
      key_fields[2] = g_strdup(e->action);
      key_fields[3] = g_strdup(e->params);

      gtk_clist_append(GTK_CLIST(clist), key_fields);
      k_entry.count++;

      FREE(key_fields[0]);
      FREE(key_fields[1]);
      FREE(key_fields[2]);
      FREE(key_fields[3]);
      if (!e->name)
         e->name = strdup("Key_Binding");
      /* this is needed later */
   }
   gtk_clist_select_row(GTK_CLIST(clist), 0, 0);

   return;
   UN(w);
}

static void
keybind_option_menu_modified(GtkWidget * w, gpointer data)
{
   int i;
   gchar *new_mod = NULL;

   if (!w)
      return;

   i = (int) data;
   gtk_option_menu_set_history(GTK_OPTION_MENU(k_entry.o), i);

   new_mod = g_strdup(mod_str[i]);
   gtk_clist_set_text(GTK_CLIST(k_entry.list), k_entry.current_row, 0,
                      new_mod);
}
static void
keybind_capture_key_cb(GtkWidget * w, gpointer data)
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
      gtk_label_new("Please press the key on the keyboard\n"
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
      char *key;
      XEvent ev;

      gdk_window_set_events(win->window, GDK_KEY_PRESS_MASK);
      XSetInputFocus(GDK_DISPLAY(), GDK_WINDOW_XWINDOW(win->window),
                     RevertToPointerRoot, CurrentTime);
      gdk_keyboard_grab(win->window, TRUE, CurrentTime);
      XWindowEvent(GDK_DISPLAY(), GDK_WINDOW_XWINDOW(win->window),
                   KeyPressMask, &ev);
      gdk_keyboard_ungrab(gdk_time_get());
      key =
         XKeysymToString(XKeycodeToKeysym(GDK_DISPLAY(), ev.xkey.keycode, 0));
      gtk_entry_set_text(GTK_ENTRY(k_entry.k), key);
      gtk_clist_set_text(GTK_CLIST(data), k_entry.current_row, 1, key);
   }
   gtk_widget_destroy(win);

}
static void
keybind_clist_row_select(GtkWidget * w, gint row, gint column,
                         GdkEventButton * ev, gpointer data)
{
   gchar *lname = NULL;

   lname = gtk_widget_get_name(GTK_WIDGET(w));

   if (!strcmp(lname, "keybinds_clist"))
   {
      gchar *temp, *temp_desc, *params;
      int i;

      k_entry.current_row = row;
      /* keep track of which row is currently selected */
      gtk_clist_get_text(GTK_CLIST(w), row, 0, &temp);
      gtk_option_menu_set_history(GTK_OPTION_MENU(k_entry.o), 0);
      for (i = 1; i < 16; i++)
      {
         if (!strcmp(mod_str[i], temp))
            gtk_option_menu_set_history(GTK_OPTION_MENU(k_entry.o), i);
      }
      /* setup the drop down menu first */

      gtk_clist_get_text(GTK_CLIST(w), row, 1, &temp);
      gtk_entry_set_text(GTK_ENTRY(k_entry.k), temp);
      /* setup the specified key */

      gtk_clist_get_text(GTK_CLIST(w), row, 2, &temp);
      if ((!strcmp("Execute", temp)) || (!strcmp("Desktop", temp)))
      {
         gtk_entry_set_editable(GTK_ENTRY(k_entry.p), TRUE);
         gtk_clist_get_text(GTK_CLIST(w), row, 3, &params);
         temp_desc = g_strdup(params);
         gtk_entry_set_text(GTK_ENTRY(k_entry.p), temp_desc);
      }
      else
      {
         gtk_entry_set_editable(GTK_ENTRY(k_entry.p), FALSE);
         gtk_entry_set_text(GTK_ENTRY(k_entry.p), "");
      }
      for (i = 0; i < 22; i++)
      {
         if (!strcmp(action_str[i], temp))
         {
            gtk_clist_select_row(GTK_CLIST(k_entry.a), i, 0);
            break;
         }
      }
      /* then setup the parameters when needed */
   }
   else if (!strcmp(lname, "action_clist"))
   {
      gchar *temp, *change_temp;

      gtk_clist_get_text(GTK_CLIST(w), row, 0, &temp);

      if ((!strcmp("Execute", temp)) || (!strcmp("Desktop", temp)))
      {
         gtk_entry_set_editable(GTK_ENTRY(k_entry.p), TRUE);
      }
      else
      {
         gtk_entry_set_editable(GTK_ENTRY(k_entry.p), FALSE);
         gtk_entry_set_text(GTK_ENTRY(k_entry.p), "");
      }

      change_temp = g_strdup(temp);
      gtk_clist_set_text(GTK_CLIST(k_entry.list), k_entry.current_row, 2,
                         change_temp);
   }
   else
   {
      fprintf(stderr, "Unknown list selected\n");
   }
   return;
   UN(ev);
   UN(data);
   UN(column);
}

static void
keybind_save_button_clicked(GtkWidget * w, gpointer data)
{
   int i, j, written;
   eaction_item *e;
   gchar *tmp;

   action_container_keys_reinit();
   /* clear out the old keys list so we can repopulate it, non key actions
      are never touched */
   for (i = 0; i < k_entry.count; i++)
   {
      e = eaction_item_new();

      e->name = strdup("Key_Binding");

      tmp = NULL;
      gtk_clist_get_text(GTK_CLIST(k_entry.list), i, 2, &tmp);
      if (!tmp)
      {
         eaction_item_free(e);
         printf("Actions need parameters\n");
         continue;
      }
      else
         e->action = strdup(tmp);
      tmp = NULL;

      gtk_clist_get_text(GTK_CLIST(k_entry.list), i, 3, &tmp);
      if ((!strcmp("Execute", e->action)) || (!strcmp("Desktop", e->action)))
      {
         if (tmp)
            e->params = strdup(tmp);
         else
            fprintf(stderr, "Actions need parameters\n");
      }
      else
         e->params = strdup("");
      tmp = NULL;

      gtk_clist_get_text(GTK_CLIST(k_entry.list), i, 1, &tmp);
      if (tmp)
         e->key = strdup(tmp);
      tmp = NULL;

      gtk_clist_get_text(GTK_CLIST(k_entry.list), i, 0, &tmp);
      for (j = 0; j < 22; j++)
      {
         if (!strcmp(mod_str[j], tmp))
         {
            e->modifiers = j;
            break;
         }
      }
      e->event = 8;
      /* keybinds are event 8 */

      ewd_list_append(action_container.keys, (void *) e);
   }
   written = write_user_actions_db();
   /* write the changes now =) */
   if (written)
      status_bar_message("Error Saving your Keybinds", 3000);
   else
      status_bar_message("Keybinds Saved Successfully ...", 3000);
   return;
   UN(w);
   UN(data);
}

/* @param data is the clist on the left pane, modify it's 4th column */
/* @param w: is the widget we can read from  */
static void
keybind_parameter_entry_changed(GtkWidget * w, gpointer data)
{
   gchar *new_val = NULL;
   GtkCTreeNode *node = NULL, *node_data = NULL;

   new_val = gtk_entry_get_text(GTK_ENTRY(w));
   node = GTK_CTREE_NODE(GTK_CLIST(data)->selection);
   /* 
    * Paranoid Checking, make sure something in the clist is the selected
    * node 
    */
   if (node)
   {
      node_data = GTK_CTREE_NODE((GTK_CLIST(data)->selection)->data);
      /* 
       * - Get the address of the container that holds the selected row
       * - then match up the widget name with the specified column =) 
       * FIXME: g_strdup leak ?
       */
      gtk_clist_set_text(GTK_CLIST(data), k_entry.current_row, 3, new_val);

   }
   else
   {
      fprintf(stderr, "ERROR: No ctree node currently selected.\n");
   }
}
static void
keybind_del_button_clicked(GtkWidget * w, gpointer data)
{
   gtk_clist_remove(GTK_CLIST(data), k_entry.current_row);
   k_entry.current_row = 0;
   gtk_clist_select_row(GTK_CLIST(data), 0, 0);
   /* FIXME I should check to make sure the list has something in it */
   k_entry.count--;
   return;
   UN(w);
   UN(data);
}
static void
keybind_add_button_clicked(GtkWidget * w, gpointer data)
{
   gchar *fields[4];
   gint retval;

   fields[0] = g_strdup(mod_str[0]);
   fields[1] = g_strdup("");
   fields[2] = g_strdup(action_str[0]);
   fields[3] = g_strdup("");

   retval =
      gtk_clist_insert(GTK_CLIST(data), (k_entry.current_row + 1), fields);

   gtk_clist_select_row(GTK_CLIST(data), retval, 0);
   k_entry.count++;
   return;
   UN(w);
   UN(data);
}
