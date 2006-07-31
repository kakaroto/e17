#include "stickies.h"

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
static void _e_sticky_focus_in_cb(Etk_Object *object, void *data);
static void _e_sticky_focus_out_cb(Etk_Object *object, void *data);
static void _e_sticky_sticky_cb(Etk_Object *object, const char *property_name, void *data);

static void
_e_sticky_key_down_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Key_Up_Down *ev;
   E_Sticky *s;
   
   s = data;
   ev = event;
   
   if(evas_key_modifier_is_set(ev->modifiers, "Control"))
     {
	if(!strcmp(ev->key, "n"))
	  {
	     E_Sticky *sn;
	     
	     sn = _e_sticky_new();
	     ss->stickies = evas_list_append(ss->stickies, sn);
	     _e_sticky_show(sn);
	     //printf("appending stick, total = %d\n", evas_list_count(ss->stickies));
	  }
	else if(!strcmp(ev->key, "d"))
	  {
	     _e_sticky_delete(s);
	  }
	else if(!strcmp(ev->key, "q"))
	  {
	     etk_main_quit();
	  }	
     }
}

static void
_e_sticky_mouse_down_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Mouse_Up_Down *ev;
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
   Etk_Event_Mouse_Up_Down *ev;
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
   
   if(_e_sticky_is_moving)
     {
	_e_sticky_move(s, s->x + ev->cur.canvas.x - _e_sticky_mouse_x,
		       s->y + ev->cur.canvas.y - _e_sticky_mouse_y);
     }
}

static void
_e_sticky_move_cb(Etk_Object *object, void *data)
{
   E_Sticky *s;
   
   s = data;
   etk_window_geometry_get(ETK_WINDOW(s->win), &s->x, &s->y, NULL, NULL);
   
     {
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
   etk_window_geometry_get(ETK_WINDOW(s->win), NULL, NULL, &s->w, &s->h);
}

static int
_e_sticky_delete_event_cb(Etk_Object *object, void *data)
{
   E_Sticky *s;
   
   s = data;
   ss->stickies = evas_list_remove(ss->stickies, s);
   
   return 1;
}   

static void
_e_sticky_focus_in_cb(Etk_Object *object, void *data)
{
   E_Sticky *s;
   Evas_List *l;
   
   s = data;   
   for(l = etk_container_children_get(ETK_CONTAINER(s->buttonbox)); 
       l; l = l->next)
     {
	Etk_Widget *w;
	
	w = l->data;
	etk_widget_show_all(w);
     }
}

static void
_e_sticky_focus_out_cb(Etk_Object *object, void *data)
{
   E_Sticky *s;
   Evas_List *l;
   
   s = data;   
   for(l = etk_container_children_get(ETK_CONTAINER(s->buttonbox)); 
       l; l = l->next)
     {
	Etk_Widget *w;
	
	w = l->data;
	etk_widget_hide_all(w);
     }
}

static void _e_sticky_sticky_cb(Etk_Object *object, const char *property_name, void *data)
{
   E_Sticky *s;
   
   s = data;
   
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(s->stick_toggle),
				etk_window_sticky_get(ETK_WINDOW(object)));
}

static Etk_Widget *
_etk_menu_stock_item_new(const char *label, Etk_Stock_Id stock_id, Etk_Menu_Shell *menu_shell, Etk_Signal_Callback_Function callback, void *data)
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
   _etk_menu_stock_item_new("Delete", ETK_STOCK_EDIT_DELETE, ETK_MENU_SHELL(menu), ETK_CALLBACK(_e_sticky_delete), s);
   _etk_menu_stock_item_new("Options", ETK_STOCK_PREFERENCES_DESKTOP_THEME, ETK_MENU_SHELL(menu), ETK_CALLBACK(_e_theme_chooser_show), s);
   _etk_menu_stock_item_new("Quit", ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(etk_main_quit), NULL);
   etk_menu_popup(ETK_MENU(menu));
}

E_Sticky *
_e_sticky_new()
{  
   E_Sticky *s;
   
   s = E_NEW(1, E_Sticky);
   s->theme = NULL;
   
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
   etk_window_title_set(ETK_WINDOW(s->win), "estickies");
   etk_window_wmclass_set(ETK_WINDOW(s->win), "_estickies", "_estickies");
   etk_window_decorated_set(ETK_WINDOW(s->win), ETK_FALSE);
   etk_window_shaped_set(ETK_WINDOW(s->win), ETK_TRUE);
   etk_widget_theme_file_set(s->win, theme);
   etk_widget_size_request_set(s->win, 208, 206);
   etk_signal_connect("key_down", ETK_OBJECT(s->win), ETK_CALLBACK(_e_sticky_key_down_cb), s);
   etk_signal_connect("mouse_down", ETK_OBJECT(s->win), ETK_CALLBACK(_e_sticky_mouse_down_cb), s);
   etk_signal_connect("mouse_up", ETK_OBJECT(s->win), ETK_CALLBACK(_e_sticky_mouse_up_cb), s);
   etk_signal_connect("mouse_move", ETK_OBJECT(s->win), ETK_CALLBACK(_e_sticky_mouse_move_cb), s);
   etk_signal_connect("move", ETK_OBJECT(s->win), ETK_CALLBACK(_e_sticky_move_cb), s);
   etk_signal_connect("resize", ETK_OBJECT(s->win), ETK_CALLBACK(_e_sticky_resize_cb), s);
   etk_signal_connect("delete_event", ETK_OBJECT(s->win), ETK_CALLBACK(_e_sticky_delete_event_cb), s);
   //etk_signal_connect("focus_in", ETK_OBJECT(s->win), ETK_CALLBACK(_e_sticky_focus_in_cb), s);
   //etk_signal_connect("focus_out", ETK_OBJECT(s->win), ETK_CALLBACK(_e_sticky_focus_out_cb), s);
   etk_object_notification_callback_add(ETK_OBJECT(s->win), "sticky", _e_sticky_sticky_cb, s);
   etk_object_data_set(ETK_OBJECT(s->win), "sticky", s);
   
   vbox = etk_vbox_new(ETK_FALSE, 0);      
   hbox = etk_hbox_new(ETK_FALSE, 2);
   s->buttonbox = hbox;
   
   etk_box_pack_start(ETK_BOX(vbox), hbox, ETK_FALSE, ETK_FALSE, 0);
   s->stick_toggle = etk_toggle_button_new();
   etk_object_properties_set(ETK_OBJECT(s->stick_toggle), 
			     "focusable", ETK_FALSE, NULL);
   etk_button_image_set(ETK_BUTTON(s->stick_toggle),
			ETK_IMAGE(etk_image_new_from_edje(theme,
						"button_stick")));
   etk_signal_connect_swapped("clicked", ETK_OBJECT(s->stick_toggle),
			      ETK_CALLBACK(_e_sticky_stick_toggle), s);
   //etk_tooltips_tip_set(button, "Make sticky visible on all desktops");   
   etk_box_pack_start(ETK_BOX(hbox), s->stick_toggle, ETK_FALSE, ETK_FALSE, 0);
   
   s->lock_button = etk_button_new();
   etk_object_properties_set(ETK_OBJECT(s->lock_button),
			     "focusable", ETK_FALSE, NULL);   
   etk_button_image_set(ETK_BUTTON(s->lock_button), 
			ETK_IMAGE(etk_image_new_from_edje(theme,
						"button_lock")));
   //etk_tooltips_tip_set(button, "Lock sticky (read-only)");   
   etk_box_pack_start(ETK_BOX(hbox), s->lock_button, ETK_FALSE, ETK_FALSE, 0);
   
   s->close_button = etk_button_new();
   etk_object_properties_set(ETK_OBJECT(s->close_button),
			     "focusable", ETK_FALSE, NULL);   
   etk_button_image_set(ETK_BUTTON(s->close_button), 
			ETK_IMAGE(etk_image_new_from_edje(theme,
							  "button_close")));
   etk_signal_connect_swapped("clicked", ETK_OBJECT(s->close_button), 
			      ETK_CALLBACK(_e_sticky_delete), s);
   //etk_tooltips_tip_set(button, "Delete this sticky");
   etk_box_pack_start(ETK_BOX(hbox), s->close_button, ETK_FALSE, ETK_FALSE, 0);

   s->textview = etk_text_view_new();
   if(s->text)
     etk_textblock_text_set(ETK_TEXT_VIEW(s->textview)->textblock, s->text,
			    ETK_TRUE);
   else
     etk_textblock_text_set(ETK_TEXT_VIEW(s->textview)->textblock, " ",
		       ETK_TRUE);     
   etk_box_pack_start(ETK_BOX(vbox), s->textview, ETK_TRUE, ETK_TRUE, 0);
   
   etk_container_add(ETK_CONTAINER(s->win), vbox);
   etk_widget_focus(s->textview);   
}

E_Sticky *
_e_sticky_new_show_append()
{
   E_Sticky *s;
   
   s = _e_sticky_new();
   ss->stickies = evas_list_append(ss->stickies, s);
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
_e_sticky_delete(E_Sticky *s)
{
   ss->stickies = evas_list_remove(ss->stickies, s);
   _e_sticky_destroy(s);
   if(!ss->stickies || evas_list_count(ss->stickies) == 0)
     etk_main_quit();
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
   if(evas_list_find(ss->stickies, s))
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
}

void
_e_sticky_load_from(E_Sticky *s)
{   
   _e_sticky_window_add(s);
}

void
_e_sticky_theme_apply(E_Sticky *s, char *theme)
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
   etk_button_image_set(ETK_BUTTON(s->lock_button),
			ETK_IMAGE(etk_image_new_from_edje(theme_file,
							  "button_lock")));
   etk_button_image_set(ETK_BUTTON(s->close_button),
			ETK_IMAGE(etk_image_new_from_edje(theme_file,
							  "button_close")));
   etk_widget_show_all(s->win);
}

void
_e_sticky_theme_apply_all(char *theme)
{
   Evas_List *l;
   
   for(l = ss->stickies; l; l = l->next)
     _e_sticky_theme_apply(l->data, theme);   
}

int main(int argc, char **argv)
{
   E_Sticky *s;

   ss = E_NEW(1, E_Stickies);
   ss->stickies = NULL;   
   
   eet_init();
   etk_init("ecore_evas_software_x11");
   //etk_tooltips_enable();
   _e_config_init();
   _e_config_load(ss);

   if(ss->stickies)
     {
	Evas_List *l;
	//printf("loading %d stickies!\n", evas_list_count(ss->stickies));
	for(l = ss->stickies; l; l = l->next)
	  {
	     _e_sticky_load_from(l->data);
	     _e_sticky_show(l->data);
	     _e_sticky_move_resize(l->data);
	  }
     }
   else
     {
	//printf("no stickies found\n");
	s = _e_sticky_new();
	ss->stickies = evas_list_append(ss->stickies, s);
	_e_sticky_show(s);
     }
   
   etk_main();
   _e_config_save(ss);
   _e_config_shutdown();
   etk_shutdown();
   eet_shutdown();
   
   return 0;
}
