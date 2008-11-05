#include "stickies.h"
#include <fcntl.h>
#include <unistd.h>

E_Stickies *ss;
static Etk_Bool _e_sticky_is_moving = ETK_FALSE;
static int _e_sticky_mouse_x = 0;
static int _e_sticky_mouse_y = 0;

static void _e_sticky_key_down_cb(Etk_Object *object, void *event, void *data);
static void _e_sticky_mouse_down_cb(Etk_Object *object, void *event, void *data);
static void _e_sticky_mouse_up_cb(Etk_Object *object, void *event, void *data);
static void _e_sticky_mouse_move_cb(Etk_Object *object, void *event, void *data);
static void _e_sticky_move_cb(Etk_Object *object, void *data);
static void _e_sticky_resize_cb(Etk_Object *object, void *data);
static int _e_sticky_delete_event_cb(Etk_Object *object, void *data);
/*
static void _e_sticky_focus_in_cb(Etk_Object *object, void *data);
static void _e_sticky_focus_out_cb(Etk_Object *object, void *data);
*/ 
static void _e_sticky_sticky_cb(Etk_Object *object, const char *property_name, void *data);
static void _e_sticky_delete_confirm_cb(Etk_Object *obj, int response_id, void *data);  
static void _e_sticky_selection_text_request_cb(Etk_Object *object, void *event, void *data);

static void
_e_sticky_key_down_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Key_Down *ev;
   E_Sticky *s;
   
   s = data;
   ev = event;
   
   if(ev->modifiers == ETK_MODIFIER_CTRL)
     {
	if(!strcmp(ev->key, "n"))
	  {
	     E_Sticky *sn;
	     
	     sn = _e_sticky_new();
	     ss->stickies = eina_list_append(ss->stickies, sn);
	     _e_sticky_show(sn);
	  }
	else if(!strcmp(ev->key, "d"))
	  {
	     _e_sticky_delete_confirm(s);
	  }
	else if(!strcmp(ev->key, "q"))
	  {
	     etk_main_quit();
	  }
	else if(!strcmp(ev->key, "c") || !strcmp(ev->key, "x"))
	  {
	     const char *text = NULL;
	     Etk_String *string = NULL;
	     Etk_Textblock_Iter *iter1;
	     Etk_Textblock_Iter *iter2;	     
	     
	     iter1 = etk_textblock_object_cursor_get(ETK_TEXT_VIEW(s->textview)->textblock_object);
	     iter2 = etk_textblock_object_selection_bound_get(ETK_TEXT_VIEW(s->textview)->textblock_object);
	     
	     string = etk_textblock_range_text_get(
				   etk_text_view_textblock_get(ETK_TEXT_VIEW(s->textview)),
				   iter1, iter2, ETK_FALSE);
	     
	     if(string && (text = etk_string_get(string)))
	       {		  
		  etk_selection_text_set(ETK_SELECTION_CLIPBOARD, text);
		  etk_object_destroy(ETK_OBJECT(string));
	       }
	     
	     if(!strcmp(ev->key, "x"))
	       etk_textblock_delete_range(etk_text_view_textblock_get(ETK_TEXT_VIEW(s->textview)),
					  iter1, iter2);
	  }
	else if(!strcmp(ev->key, "v"))
	  {
	     etk_selection_text_request(ETK_SELECTION_CLIPBOARD, ETK_WIDGET(s->win));
	  }
	else if(!strcmp(ev->key, "a"))
	  {
	     Etk_Textblock_Iter *cursor;
	     Etk_Textblock_Iter *selection;
	     
	     cursor = etk_textblock_object_cursor_get(ETK_TEXT_VIEW(s->textview)->textblock_object);
	     selection = etk_textblock_object_selection_bound_get(ETK_TEXT_VIEW(s->textview)->textblock_object);
	     
	     etk_textblock_iter_backward_start(selection);
	     etk_textblock_iter_forward_end(cursor);
	  }
     }
}

static void
_e_sticky_mouse_down_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Mouse_Down *ev;
   E_Sticky *s;
   
   s = data;
   ev = event;

   _e_sticky_mouse_x = ev->canvas.x;
   _e_sticky_mouse_y = ev->canvas.y;   
   
   if(ev->button == 3)
     _e_sticky_menu_show(s);
   else if(ev->button == 1)
     _e_sticky_is_moving = ETK_TRUE;
}

static void
_e_sticky_mouse_up_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Mouse_Down *ev;
   E_Sticky *s;
   
   s = data;
   ev = event;
   
   if(ev->button == 1)
     _e_sticky_is_moving = ETK_FALSE;   
}

static void
_e_sticky_mouse_move_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Mouse_Move *ev;
   E_Sticky *s;
   
   s = data;
   ev = event;

   etk_window_geometry_get(ETK_WINDOW(s->win), &s->x, &s->y, NULL, NULL);
   
   if(_e_sticky_is_moving)
     {
	     int x = 0;
	     int y = 0;
	     ecore_x_pointer_last_xy_get(&x, &y);
	     _e_sticky_move(s,  x-_e_sticky_mouse_x,  y-_e_sticky_mouse_y);
     }
}

static void
_e_sticky_move_cb(Etk_Object *object, void *data)
{
   E_Sticky *s;
   
   s = data;
   etk_window_geometry_get(ETK_WINDOW(s->win), &s->x, &s->y, NULL, NULL);

     {
	/* FIXME:
	 * this bit of code forces estickies to only work on X11.
	 * we need to acquire this info through etk.
	 */
	Ecore_X_Window root;
	int x, y, w, h;
	
	root = ecore_x_window_root_first_get();
	ecore_x_window_geometry_get(root, &x, &y, &w, &h);
	
	if(!E_INTERSECTS(x, y, w, h, s->x, s->y, s->w, s->h))
	  {
	     _e_sticky_move(s, 0, 0);
	  }
     }
}

static void
_e_sticky_resize_cb(Etk_Object *object, void *data)
{
   E_Sticky *s;
   
   s = data;
   if(!_e_sticky_is_moving)
     etk_window_geometry_get(ETK_WINDOW(s->win), NULL, NULL, &s->w, &s->h);
}

static int
_e_sticky_delete_event_cb(Etk_Object *object, void *data)
{
   E_Sticky *s;
   
   s = data;
   ss->stickies = eina_list_remove(ss->stickies, s);
   
   return 1;
}   

static void
_e_sticky_focus_in_cb(Etk_Object *object, void *data)
{
   E_Sticky *s;
   Eina_List *l;
   Etk_Widget *w;

   s = data;
   EINA_LIST_FOREACH(etk_container_children_get(ETK_CONTAINER(s->buttonbox)), l, w)
     etk_widget_show_all(w);
}

static void
_e_sticky_focus_out_cb(Etk_Object *object, void *data)
{
   E_Sticky *s;
   Eina_List *l;
   Etk_Widget *w;

   s = data;
   EINA_LIST_FOREACH(etk_container_children_get(ETK_CONTAINER(s->buttonbox)), l, w)
     etk_widget_hide_all(w);
}

static void _e_sticky_sticky_cb(Etk_Object *object, const char *property_name, void *data)
{
   E_Sticky *s;
   
   s = data;

   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(s->stick_toggle),
				etk_window_sticky_get(ETK_WINDOW(object)));
   s->stick = etk_window_sticky_get(ETK_WINDOW(object));
}

static Etk_Widget *
_etk_menu_stock_item_new(const char *label, Etk_Stock_Id stock_id, Etk_Menu_Shell *menu_shell, Etk_Callback callback, void *data)
{
   Etk_Widget *menu_item;
   Etk_Widget *image;
   
   if (!menu_shell)
     return NULL;
   
   menu_item = etk_menu_item_image_new_with_label(label);
   image = etk_image_new_from_stock(stock_id, ETK_STOCK_SMALL);
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));
   
   if(callback)
     etk_signal_connect_swapped("activated", ETK_OBJECT(menu_item), callback, data);
   
   
   return menu_item;
}

void
_e_sticky_menu_show(E_Sticky *s)
{
   Etk_Widget *menu;

   menu = etk_menu_new();
   _etk_menu_stock_item_new("New", ETK_STOCK_DOCUMENT_OPEN, ETK_MENU_SHELL(menu), ETK_CALLBACK(_e_sticky_new_show_append), NULL);
   _etk_menu_stock_item_new("Save", ETK_STOCK_DOCUMENT_SAVE, ETK_MENU_SHELL(menu), ETK_CALLBACK(_e_config_save), ss);
   _etk_menu_stock_item_new("Export To File", ETK_STOCK_DOCUMENT_SAVE_AS, ETK_MENU_SHELL(menu), ETK_CALLBACK(_e_sticky_export_to), s);
   _etk_menu_stock_item_new("Delete", ETK_STOCK_EDIT_DELETE, ETK_MENU_SHELL(menu), ETK_CALLBACK(_e_sticky_delete_confirm), s);
   _etk_menu_stock_item_new("Options", ETK_STOCK_PREFERENCES_DESKTOP_THEME, ETK_MENU_SHELL(menu), ETK_CALLBACK(_e_theme_chooser_show), s);
   _etk_menu_stock_item_new("About", ETK_STOCK_DIALOG_INFORMATION, ETK_MENU_SHELL(menu), ETK_CALLBACK(_e_about_show), NULL);
   _etk_menu_stock_item_new("Quit", ETK_STOCK_DIALOG_CLOSE, ETK_MENU_SHELL(menu), ETK_CALLBACK(etk_main_quit), NULL);
   etk_menu_popup(ETK_MENU(menu));
}

E_Sticky *
_e_sticky_new()
{  
   E_Sticky *s;
   
   s = E_NEW(1, E_Sticky);
   s->theme = NULL;
   s->stick = 0;
   s->locked = 0;
   _e_sticky_window_add(s);
   return s;
}

void
_e_sticky_window_add(E_Sticky *s)
{
   Etk_Widget *vbox;
   Etk_Widget *hbox;
   char theme[PATH_MAX];
   
   if(!s->theme)
     s->theme = strdup(ss->theme);
   
   snprintf(theme, sizeof(theme), PACKAGE_DATA_DIR"/themes/%s", s->theme);
   
   s->win = etk_window_new();
//   etk_window_has_alpha_set(ETK_WINDOW(s->win), ETK_TRUE);
   etk_window_title_set(ETK_WINDOW(s->win), "estickies");
   etk_window_wmclass_set(ETK_WINDOW(s->win), "estickies", "estickies");
   etk_window_decorated_set(ETK_WINDOW(s->win), ETK_FALSE);
   etk_window_shaped_set(ETK_WINDOW(s->win), ETK_TRUE);
   etk_widget_theme_file_set(s->win, theme);
   etk_widget_size_request_set(s->win, 208, 206);
   etk_signal_connect("key-down", ETK_OBJECT(s->win), ETK_CALLBACK(_e_sticky_key_down_cb), s);
   etk_signal_connect("mouse-down", ETK_OBJECT(s->win), ETK_CALLBACK(_e_sticky_mouse_down_cb), s);
   etk_signal_connect("mouse-up", ETK_OBJECT(s->win), ETK_CALLBACK(_e_sticky_mouse_up_cb), s);
   etk_signal_connect("mouse-move", ETK_OBJECT(s->win), ETK_CALLBACK(_e_sticky_mouse_move_cb), s);
   etk_signal_connect("moved", ETK_OBJECT(s->win), ETK_CALLBACK(_e_sticky_move_cb), s);
   etk_signal_connect("resized", ETK_OBJECT(s->win), ETK_CALLBACK(_e_sticky_resize_cb), s);
   etk_signal_connect("delete-event", ETK_OBJECT(s->win), ETK_CALLBACK(_e_sticky_delete_event_cb), s);
   //etk_signal_connect("focus_in", ETK_OBJECT(s->win), ETK_CALLBACK(_e_sticky_focus_in_cb), s);
   //etk_signal_connect("focus_out", ETK_OBJECT(s->win), ETK_CALLBACK(_e_sticky_focus_out_cb), s);
   etk_object_notification_callback_add(ETK_OBJECT(s->win), "sticky", _e_sticky_sticky_cb, s);
   etk_object_data_set(ETK_OBJECT(s->win), "sticky", s);
   
   vbox = etk_vbox_new(ETK_FALSE, 0);      
   hbox = etk_hbox_new(ETK_FALSE, 2);
   s->buttonbox = hbox;
   
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);
   s->stick_toggle = etk_toggle_button_new();
   etk_object_properties_set(ETK_OBJECT(s->stick_toggle), 
			     "focusable", ETK_FALSE, NULL);
   etk_button_image_set(ETK_BUTTON(s->stick_toggle),
			ETK_IMAGE(etk_image_new_from_edje(theme,
						"button_stick")));
   etk_signal_connect_swapped("clicked", ETK_OBJECT(s->stick_toggle),
			      ETK_CALLBACK(_e_sticky_stick_toggle), s);
   //etk_tooltips_tip_set(button, "Make sticky visible on all desktops");
   etk_box_append(ETK_BOX(hbox), s->stick_toggle, ETK_BOX_START, ETK_BOX_NONE, 0);

   s->lock_toggle = etk_toggle_button_new();
   etk_object_properties_set(ETK_OBJECT(s->lock_toggle),
			     "focusable", ETK_FALSE, NULL);   
   etk_button_image_set(ETK_BUTTON(s->lock_toggle), 
			ETK_IMAGE(etk_image_new_from_edje(theme,
						"button_lock")));
   etk_signal_connect_swapped("toggled", ETK_OBJECT(s->lock_toggle),
			      ETK_CALLBACK(_e_sticky_lock_toggle), s);   
   //etk_tooltips_tip_set(button, "Lock sticky (read-only)");
   etk_box_append(ETK_BOX(hbox), s->lock_toggle, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   s->close_button = etk_button_new();
   etk_object_properties_set(ETK_OBJECT(s->close_button),
			     "focusable", ETK_FALSE, NULL);   
   etk_button_image_set(ETK_BUTTON(s->close_button), 
			ETK_IMAGE(etk_image_new_from_edje(theme,
							  "button_close")));
   etk_signal_connect_swapped("clicked", ETK_OBJECT(s->close_button), 
			      ETK_CALLBACK(_e_sticky_delete_confirm), s);
   //etk_tooltips_tip_set(button, "Delete this sticky");
   etk_box_append(ETK_BOX(hbox), s->close_button, ETK_BOX_START, ETK_BOX_NONE, 0);

   s->textview = etk_text_view_new();
   etk_widget_repeat_mouse_events_set(s->textview, ETK_TRUE);
   if(s->text)
     etk_textblock_text_set(etk_text_view_textblock_get(ETK_TEXT_VIEW(s->textview)), s->text,
			    ETK_TRUE);
   else
     etk_textblock_text_set(etk_text_view_textblock_get(ETK_TEXT_VIEW(s->textview)), " ",
		       ETK_TRUE);     
   etk_box_append(ETK_BOX(vbox), s->textview, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   etk_container_add(ETK_CONTAINER(s->win), vbox);
   etk_widget_focus(s->textview);
   etk_signal_connect("selection-received", ETK_OBJECT(s->win), ETK_CALLBACK(_e_sticky_selection_text_request_cb), s);

#if 0
   /* [TODO] [1] Disable transparency until we have a way to figure out if screen is composited */
   etk_widget_propagate_color_set(s->win, ETK_FALSE);   
   etk_widget_color_set(s->win, 160, 160, 160, 160);
   etk_widget_color_set(s->stick_toggle, 160, 160, 160, 160);
   etk_widget_color_set(s->lock_toggle, 160, 160, 160, 160);
   etk_widget_color_set(s->close_button, 160, 160, 160, 160);
#endif   
}

E_Sticky *
_e_sticky_new_show_append()
{
   E_Sticky *s;
   
   s = _e_sticky_new();
   ss->stickies = eina_list_append(ss->stickies, s);
   _e_sticky_show(s);
   return s;
}

void
_e_sticky_destroy(E_Sticky *s)
{
   etk_object_destroy(ETK_OBJECT(s->win));
   E_FREE(s);
}

void
_e_sticky_export_cb(void *data)
{
   E_Filedialog *fd = data;
   Eina_List *l;
   E_Sticky *s;
   FILE *fh;
   int c = 1;
   const char *basename;
   const char *dir;
   char file[PATH_MAX];
   char *text;

   basename = etk_entry_text_get(ETK_ENTRY(fd->entry));
   dir = etk_filechooser_widget_current_folder_get
     (ETK_FILECHOOSER_WIDGET(fd->filechooser));
   s = fd->s;
   
   if (!dir || !basename || !s) { printf("return!\n"); return;}

   etk_widget_hide(fd->dia);

   snprintf(file, sizeof(file), "%s/%s", dir, basename);
   if((fh = fopen(file, "w")) == NULL) return;
   
   if (!etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(fd->export_mode)))
     {
	/* save only current sticky */
	fprintf(fh, "Sticky %d\n=========================\n", c);
	text = strdup(etk_string_get(etk_textblock_text_get(
		      ETK_TEXT_VIEW(s->textview)->textblock, 
		      ETK_FALSE)));
	fprintf (fh, "%s\n\n", text);	
     }
   else
     {
	/* save all stickies */
        EINA_LIST_FOREACH(ss->stickies, l, s)
	  {	     
	     fprintf(fh, "Sticky %d\n=========================\n", c);
	     text = strdup(etk_string_get(etk_textblock_text_get(
			   ETK_TEXT_VIEW(s->textview)->textblock, 
			   ETK_FALSE)));
	     fprintf (fh, "%s\n\n", text);
	     ++c;
	  }	
     }
   
   fclose (fh);   
}

void
_e_sticky_export_to_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Key_Down *ev = event;

   if (!strcmp(ev->key, "Return") || !strcmp(ev->key, "KP_Enter"))
     _e_sticky_export_cb(data);
}

void
_e_sticky_export_to(E_Sticky *s)
{
   static E_Filedialog *fd = NULL;
   Etk_Widget *vbox;
   Etk_Widget *hbox;
   Etk_Widget *btn;
   Etk_Widget *label;
   char dflt_filename[64]; 

   if (!fd)
     {
        fd = calloc(1, sizeof(E_Filedialog));
        if (!fd) return;
	fd->s = NULL;
        fd->dia = NULL;
     }
   
   snprintf (dflt_filename, sizeof (dflt_filename), "estickies_export.txt");
   fd->s = s;
   
   /* Don't open more then one window */
   if (fd->dia)
     {
        /* Update the filename when we show the window again */
        etk_entry_text_set(ETK_ENTRY(fd->entry), dflt_filename);
        etk_widget_show_all(ETK_WIDGET(fd->dia));
        return;
     }

   fd->dia = etk_dialog_new();
   etk_window_title_set(ETK_WINDOW(fd->dia), "Estickies export to ..");
   etk_signal_connect("delete-event", ETK_OBJECT(fd->dia),
                      ETK_CALLBACK(etk_window_hide_on_delete), fd->dia);

   fd->filechooser = etk_filechooser_widget_new();
   etk_dialog_pack_in_main_area(ETK_DIALOG(fd->dia), fd->filechooser, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   vbox = etk_vbox_new(ETK_FALSE, 0);
   label = etk_label_new("Filename:");
   etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   fd->entry = etk_entry_new();
   etk_entry_text_set(ETK_ENTRY(fd->entry), dflt_filename);
   etk_box_append(ETK_BOX(vbox), fd->entry, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect("key-down", ETK_OBJECT(fd->entry),
                      ETK_CALLBACK(_e_sticky_export_to_cb), fd);

   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(vbox), hbox);

   btn = etk_button_new_with_label("Export");
   etk_box_append(ETK_BOX(hbox), btn, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_signal_connect_swapped("clicked", ETK_OBJECT(btn),
                              ETK_CALLBACK(_e_sticky_export_cb), fd);

   btn = etk_button_new_with_label("Cancel");
   etk_box_append(ETK_BOX(hbox), btn, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_signal_connect_swapped("clicked", ETK_OBJECT(btn),
                              ETK_CALLBACK(etk_widget_hide), fd->dia);

   fd->export_mode = etk_check_button_new_with_label("All Stickies");
   etk_box_append(ETK_BOX(hbox), fd->export_mode, ETK_BOX_START, ETK_BOX_FILL, 0);
   
   etk_dialog_pack_widget_in_action_area(ETK_DIALOG(fd->dia), vbox, 
					 ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   etk_widget_show_all(fd->dia);
}

void
_e_sticky_delete(E_Sticky *s)
{
   ss->stickies = eina_list_remove(ss->stickies, s);
   _e_sticky_destroy(s);
   if(!ss->stickies || eina_list_count(ss->stickies) == 0)
     etk_main_quit();
}

void
_e_sticky_delete_confirm(E_Sticky *s)
{
   Etk_Widget *dialog;
   
   dialog = etk_message_dialog_new(ETK_MESSAGE_DIALOG_QUESTION, 
				   ETK_MESSAGE_DIALOG_YES_NO,
				   "Are you sure you want to delete this sticky?");
   etk_signal_connect_swapped("delete-event", ETK_OBJECT(dialog), 
			      ETK_CALLBACK(etk_object_destroy), dialog);
   etk_signal_connect("response", ETK_OBJECT(dialog), 
		      ETK_CALLBACK(_e_sticky_delete_confirm_cb), s);
   etk_container_border_width_set(ETK_CONTAINER(dialog), 4);
   
   etk_window_title_set(ETK_WINDOW(dialog), "Confirm Deletion");
   etk_widget_show_all(dialog);      
}

void
_e_sticky_move(E_Sticky *s, int x, int y)
{
   etk_window_move(ETK_WINDOW(s->win), x, y);
}

void
_e_sticky_resize(E_Sticky *s, int w, int h)
{
   etk_window_resize(ETK_WINDOW(s->win), h, h);
}

Etk_Bool
_e_sticky_exists(E_Sticky *s)
{
   if(eina_list_data_find(ss->stickies, s))
     return ETK_TRUE;
   return ETK_FALSE;
}

void
_e_sticky_show(E_Sticky *s)
{
   if(s && s->win)
     etk_widget_show_all(s->win);
}

void
_e_sticky_move_resize(E_Sticky *s)
{
   if(!s || !s->win)
     return;
   
   etk_window_resize(ETK_WINDOW(s->win), s->w, s->h);
   etk_window_move(ETK_WINDOW(s->win), s->x, s->y);
}

void
_e_sticky_stick_toggle(E_Sticky *s)
{
   etk_window_sticky_set(ETK_WINDOW(s->win), 
			 !etk_window_sticky_get(ETK_WINDOW(s->win)));
   s->stick = !etk_window_sticky_get(ETK_WINDOW(s->win));
}

void
_e_sticky_lock_toggle(E_Sticky *s)
{   
   s->locked = etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(s->lock_toggle));   
   etk_textblock_object_cursor_visible_set(ETK_TEXT_VIEW(s->textview)->textblock_object,
					   !s->locked);
   etk_object_properties_set(ETK_OBJECT(s->textview),
			     "focusable", !s->locked, NULL);
   if(s->locked)
     {
	etk_object_properties_set(ETK_OBJECT(s->win),
				  "focusable", ETK_TRUE, NULL);
	etk_widget_unfocus(s->textview);
	etk_widget_focus(s->win);
     }
   else
     {
	etk_widget_focus(s->textview);
	etk_object_properties_set(ETK_OBJECT(s->win),
				  "focusable", ETK_FALSE, NULL);
     }
}

void
_e_sticky_load_from(E_Sticky *s)
{   
   _e_sticky_window_add(s);
}

void
_e_sticky_lock_set(E_Sticky *s, Etk_Bool on)
{
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(s->lock_toggle), on);
}

void
_e_sticky_properties_set(E_Sticky *s)
{   
   if(s->stick == 1)
     etk_window_sticky_set(ETK_WINDOW(s->win), ETK_TRUE);
   if(s->locked == 1)
     _e_sticky_lock_set(s, ETK_TRUE);
}

void
_e_sticky_theme_apply(E_Sticky *s, const char *theme)
{
   char theme_file[PATH_MAX];
   
   if(!theme)
     return;
   
   if(!strcmp(s->theme, theme))
     return;
   
   snprintf(theme_file, sizeof(theme_file), PACKAGE_DATA_DIR"/themes/%s", 
	    theme);   
   
   if(!ecore_file_exists(theme_file))
     return;
   
   E_FREE(s->theme);   
   s->theme = strdup(theme);
   
   etk_widget_theme_file_set(s->win, theme_file);
   
   etk_button_image_set(ETK_BUTTON(s->stick_toggle),
			ETK_IMAGE(etk_image_new_from_edje(theme_file,
							  "button_stick")));
   etk_button_image_set(ETK_BUTTON(s->lock_toggle),
			ETK_IMAGE(etk_image_new_from_edje(theme_file,
							  "button_lock")));
   etk_button_image_set(ETK_BUTTON(s->close_button),
			ETK_IMAGE(etk_image_new_from_edje(theme_file,
							  "button_close")));
   etk_widget_show_all(s->win);
}

void
_e_sticky_theme_apply_all(const char *theme)
{
   Eina_List *l;
   E_Sticky *data;

   EINA_LIST_FOREACH(ss->stickies, l, data)
     _e_sticky_theme_apply(data, theme);
}

static void 
_e_sticky_delete_confirm_cb(Etk_Object *obj, int response_id, void *data)
{
   switch(response_id)
     {
      case ETK_RESPONSE_YES:
	if(data)	  
	  _e_sticky_delete(data);
	break;
	
      default:
	break;
     }
   
   etk_object_destroy(obj);
}

static void _e_sticky_selection_text_request_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Selection_Event *ev = event;
   Etk_Textblock_Iter *cursor;
   Etk_Textblock_Iter *selection;   
   E_Sticky *s = data;
   
   if(!(ev = event) || (ev->type != ETK_SELECTION_TEXT))
     return;
	     
   cursor = etk_textblock_object_cursor_get(ETK_TEXT_VIEW(s->textview)->textblock_object);
   selection = etk_textblock_object_selection_bound_get(ETK_TEXT_VIEW(s->textview)->textblock_object);
   
   etk_textblock_delete_range(etk_text_view_textblock_get(ETK_TEXT_VIEW(s->textview)),
			      cursor, selection);      
   etk_textblock_insert(etk_text_view_textblock_get(ETK_TEXT_VIEW(s->textview)), cursor,
			ev->data.text, -1);
}

int main(int argc, char **argv)
{
   E_Sticky *s;

   ss = E_NEW(1, E_Stickies);
   ss->stickies = NULL;   
   
   eet_init();
   etk_init(argc, argv);
   //etk_tooltips_enable();
   _e_config_init();
   _e_config_load(ss);

   if(ss->stickies)
     {
	Eina_List *l;
	E_Sticky *data;

	EINA_LIST_FOREACH(ss->stickies, l, data)
	  {
	     _e_sticky_load_from(data);
	     _e_sticky_show(data);
	     _e_sticky_move_resize(data);
	     _e_sticky_properties_set(data);
	  }
     }
   else
     {
	s = _e_sticky_new();
	ss->stickies = eina_list_append(ss->stickies, s);
	_e_sticky_show(s);
     }
   
   etk_main();
   _e_config_save(ss);
   _e_config_shutdown();
   etk_shutdown();
   eet_shutdown();
   
   return 0;
}
