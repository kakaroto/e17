#include "eclair.h"
#include "../config.h"
#include <Ecore_X.h>
#include <Edje.h>

#ifndef ECLAIR_WIDGETS_THEME
#define ECLAIR_WIDGETS_THEME PACKAGE_DATA_DIR "/widget_themes/default.edj"
#endif

typedef enum _Eclair_Menu_Screen_Edge
{
   ECLAIR_MENU_NO_EDGE = 0,
   ECLAIR_MENU_LEFT_EDGE = (1 << 0),
   ECLAIR_MENU_RIGHT_EDGE = (1 << 1),
   ECLAIR_MENU_BOTTOM_EDGE = (1 << 2),
   ECLAIR_MENU_TOP_EDGE = (1 << 3)
} Eclair_Menu_Screen_Edge;

static void _eclair_menu_recalc(Eclair_Menu *menu);
static void _eclair_menu_resize_cb(Ecore_Evas *menu_window);
static Eclair_Menu_Screen_Edge _eclair_menu_over_screen_edge(Eclair_Menu *menu);
static Eclair_Menu_Screen_Edge _eclair_menu_mouse_on_screen_edge();
static int _eclair_menu_mouse_is_in(Eclair_Menu *menu);
static void _eclair_menu_update_slide_timer(Eclair_Menu *menu);
static int _eclair_menu_slide_timer_cb(void *data);
static int _eclair_menu_mouse_up_cb(void *data, int type, void *event);
static int _eclair_menu_mouse_move_cb(void *data, int type, void *event);
static void _eclair_menu_item_in_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _eclair_menu_item_out_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _eclair_menu_item_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

static Eclair_Menu *_eclair_menu_root = NULL;
static Ecore_X_Window _eclair_menu_input_window = 0;
static Eina_List *_eclair_menu_popped_menus = NULL;
static Ecore_Event_Handler *_eclair_menu_mouse_up_handler = NULL;
static Ecore_Event_Handler *_eclair_menu_mouse_move_handler = NULL;
static Ecore_Timer *_eclair_menu_slide_timer = NULL;
static int _eclair_menu_mouse_x = -100000;
static int _eclair_menu_mouse_y = -100000;

//------------------------------
// Eclair_Menu
//------------------------------

//Create a new menu
//The parent menu will have to be destroyed by eclair_menu_free
//Return NULL if failed
Eclair_Menu *eclair_menu_new(Eclair_Engine menu_engine)
{
   Eclair_Menu *menu;
   Ecore_X_Window_State window_states[2];

   menu = calloc(1, sizeof(Eclair_Menu));
   menu->engine = menu_engine;

   if (menu_engine == ECLAIR_GL)
   {
      menu->window = ecore_evas_gl_x11_new(NULL, 0, 0, 0, 0, 0);
      menu->x_window = ecore_evas_gl_x11_window_get(menu->window);
   }
   else
   {
      menu->window = ecore_evas_software_x11_new(NULL, 0, 0, 0, 0, 0);
      menu->x_window = ecore_evas_software_x11_window_get(menu->window);
   }
   
   ecore_evas_title_set(menu->window, "eclair menu");
   ecore_evas_name_class_set(menu->window, "eclair", "Eclair");
   window_states[0] = ECORE_X_WINDOW_STATE_SKIP_TASKBAR;
   window_states[1] = ECORE_X_WINDOW_STATE_SKIP_PAGER;
   ecore_x_netwm_window_state_set(menu->x_window, window_states, 2);
   ecore_evas_borderless_set(menu->window, 1);

   menu->evas = ecore_evas_get(menu->window);
   menu->edje_object = edje_object_add(menu->evas);
   if (!edje_object_file_set(menu->edje_object, ECLAIR_WIDGETS_THEME, "eclair_menu"))
   {
      evas_object_del(menu->edje_object);
      ecore_evas_free(menu->window);
      free(menu);
      return NULL;
   }
   evas_object_move(menu->edje_object, 0, 0);
   evas_object_show(menu->edje_object);
   ecore_evas_data_set(menu->window, "eclair_menu", menu);
   ecore_evas_callback_resize_set(menu->window, _eclair_menu_resize_cb);
   ecore_evas_hide(menu->window);

   return menu;
}

//Free the menu and its children
void eclair_menu_free(Eclair_Menu *menu)
{
   Eina_List *l;
   Eclair_Menu_Item *item;

   if (!menu)
      return;

   eclair_menu_pop_down(menu);
   for (l = menu->items; l; l = l->next)
   {
      if ((item = l->data))
      {
         item->parent = NULL;
         eclair_menu_item_free(item);
      }
   }
   eina_list_free(menu->items);

   evas_object_del(menu->edje_object);
   ecore_evas_free(menu->window);
   free(menu);
}

//Append the item to the menu
//Return 1 if success
int eclair_menu_append_item(Eclair_Menu *menu, Eclair_Menu_Item *item)
{
   if (!menu || !item)
      return 0;

   item->parent = menu;
   menu->items = eina_list_append(menu->items, item);

   _eclair_menu_recalc(menu);

   return 1;
}

//Attach a child menu to an item
void eclair_menu_attach_to_item(Eclair_Menu *menu, Eclair_Menu_Item *item)
{
   if (!item || !menu)
      return;

   menu->parent = item;
   if (item->child)
      item->child->parent = NULL;
   item->child = menu;

   _eclair_menu_recalc(item->parent);
}

//Popup the menu at the mouse pointer position
void eclair_menu_popup(Eclair_Menu *menu)
{
   int x, y;

   ecore_x_pointer_last_xy_get(&x, &y);
   eclair_menu_popup_at_xy(menu, x, y);
}

//Popup the menu at the position (x, y)
void eclair_menu_popup_at_xy(Eclair_Menu *menu, int x, int y)
{
   if (!menu)
      return;

   if (!_eclair_menu_input_window)
   {
      Ecore_X_Window root, parent;
      int root_x, root_y, root_w, root_h;

      root = menu->x_window;
      while ((parent = ecore_x_window_parent_get(root)) != 0)
         root = parent;
 
      ecore_x_window_geometry_get(root, &root_x, &root_y, &root_w, &root_h);
      _eclair_menu_input_window = ecore_x_window_input_new(root, root_x, root_y, root_w, root_h);
   }
   if (!_eclair_menu_root)
   {
      _eclair_menu_mouse_up_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_UP, _eclair_menu_mouse_up_cb, menu);
      _eclair_menu_mouse_move_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_MOVE, _eclair_menu_mouse_move_cb, menu);
      ecore_x_window_show(_eclair_menu_input_window);
      ecore_x_keyboard_grab(_eclair_menu_input_window);
      ecore_x_pointer_confine_grab(_eclair_menu_input_window);

      _eclair_menu_root = menu;
   }
   ecore_evas_move(menu->window, x, y);
   ecore_evas_show(menu->window);
   evas_event_feed_mouse_move(menu->evas, -100000, -100000, ecore_x_current_time_get(), NULL);
   evas_event_feed_mouse_in(menu->evas, ecore_x_current_time_get(), NULL);
   _eclair_menu_popped_menus = eina_list_append(_eclair_menu_popped_menus, menu);

   _eclair_menu_update_slide_timer(menu);
}

//Pop down the menu and its childrend
void eclair_menu_pop_down(Eclair_Menu *menu)
{
   Eina_List *l;
   Eclair_Menu_Item *item;

   if (!menu)
      return;

   for (l = menu->items; l; l = l->next)
   {
      if (!(item = l->data))
         continue;

      eclair_menu_item_deactivate(item);
   }

   ecore_evas_hide(menu->window);
   if (menu == _eclair_menu_root)
   {
      ecore_x_keyboard_ungrab();
      ecore_x_pointer_ungrab();
      ecore_x_window_hide(_eclair_menu_input_window);
      ecore_event_handler_del(_eclair_menu_mouse_up_handler);
      ecore_event_handler_del(_eclair_menu_mouse_move_handler);
      _eclair_menu_root = NULL;
   }

   _eclair_menu_popped_menus = eina_list_remove(_eclair_menu_popped_menus, menu);
}

//Create a new menu item
//This item will be automatically freed by calling eclair_menu_free on the parent menu
//Return NULL if failed
Eclair_Menu_Item *eclair_menu_add_item(Eclair_Menu *menu, const char *label)
{
   return eclair_menu_item_new(label, menu, ECLAIR_MENU_ITEM_NORMAL_ITEM, NULL);
}

//Create a new menu item with an icon
//This item will be automatically freed by calling eclair_menu_free on the parent menu
//Return NULL if failed
Eclair_Menu_Item *eclair_menu_add_item_with_icon(Eclair_Menu *menu, const char *label, const char *icon_path)
{
   return eclair_menu_item_new(label, menu, ECLAIR_MENU_ITEM_ICON_ITEM, icon_path);
}

//Create a new menu separator
//This separator will be automatically freed by calling eclair_menu_free on the parent menu
//Return NULL if failed
Eclair_Menu_Item *eclair_menu_add_seperator(Eclair_Menu *menu)
{
   return eclair_menu_item_new(NULL, menu, ECLAIR_MENU_ITEM_SEPARATOR_ITEM, NULL);
}

//Calculate the size of the menu, resize it and display the arrows if needed
static void _eclair_menu_recalc(Eclair_Menu *menu)
{
   Eina_List *l;
   Eclair_Menu_Item *item;
   int w = 0, h = 0, item_w;
   int display_arrows = 0, display_icons = 0;

   if (!menu)
      return;

   //First we show/hide the arrows/icons
   for (l = menu->items; l; l = l->next)
   {
      if (!(item = l->data))
         continue;

      display_arrows |= (item->child != NULL);
      display_icons |= (item->type != ECLAIR_MENU_ITEM_NORMAL_ITEM && item->type != ECLAIR_MENU_ITEM_SEPARATOR_ITEM);
   }
   for (l = menu->items; l; l = l->next)
   {
      if (!(item = l->data))
         continue;

      if (!display_arrows)
         edje_object_signal_emit(item->edje_object, "signal_arrow_invisible", "eclair_bin");
      else if (item->child)
         edje_object_signal_emit(item->edje_object, "signal_arrow_visible", "eclair_bin");
      else
         edje_object_signal_emit(item->edje_object, "signal_arrow_spacer", "eclair_bin");

      if (display_icons)
         edje_object_signal_emit(item->edje_object, "signal_icon_visible", "eclair_bin");
      else
         edje_object_signal_emit(item->edje_object, "signal_icon_invisible", "eclair_bin");
   }

   //Then we resize the menu
   for (l = menu->items; l; l = l->next)
   {
      if (!(item = l->data))
         continue;

      edje_object_size_min_calc(item->edje_object, &item_w, &item->height);
      if (w < item_w)
         w = item_w;
      h += item->height;
   }
   ecore_evas_resize(menu->window, w, h);
}

//Called when the menu is resized
static void _eclair_menu_resize_cb(Ecore_Evas *menu_window)
{
   Eclair_Menu *menu;
   Eina_List *l;
   Eclair_Menu_Item *item;
   Evas_Coord w, h, y;
   int i;
   
   if (!(menu = ecore_evas_data_get(menu_window, "eclair_menu")))
      return;

   ecore_evas_geometry_get(menu_window, NULL, NULL, &w, &h);
   evas_object_resize(menu->edje_object, w, h);
   y = 0;
   for (l = menu->items, i = 0; l; l = l->next, i++)
   {
      if (!(item = l->data))
         continue;

      evas_object_move(item->edje_object, 0, y);
      evas_object_resize(item->edje_object, w, item->height);
      y += item->height;
   }
}

//Return a flag incating on which edges of the screen the menu is over
static Eclair_Menu_Screen_Edge _eclair_menu_over_screen_edge(Eclair_Menu *menu)
{
   int root_x, root_y, root_w, root_h;
   int menu_x, menu_y, menu_w, menu_h;
   Eclair_Menu_Screen_Edge result = ECLAIR_MENU_NO_EDGE;

   if (!menu)
      return ECLAIR_MENU_NO_EDGE;

   ecore_x_window_geometry_get(_eclair_menu_input_window, &root_x, &root_y, &root_w, &root_h);
   ecore_evas_geometry_get(menu->window, &menu_x, &menu_y, &menu_w, &menu_h);

   if (menu_x < root_x)
   {
      result |= ECLAIR_MENU_LEFT_EDGE;
   }
   if (menu_x + menu_w > root_x + root_w)
      result |= ECLAIR_MENU_RIGHT_EDGE;
   if (menu_y < root_y)
      result |= ECLAIR_MENU_TOP_EDGE;
   if (menu_y + menu_h > root_y + root_h)
      result |= ECLAIR_MENU_BOTTOM_EDGE;
      
   return result;
}

//Return a flag incating on which edges of the screen the mouse pointer is
static Eclair_Menu_Screen_Edge _eclair_menu_mouse_on_screen_edge()
{
   int root_x, root_y, root_w, root_h;
   Eclair_Menu_Screen_Edge result = ECLAIR_MENU_NO_EDGE;

   ecore_x_window_geometry_get(_eclair_menu_input_window, &root_x, &root_y, &root_w, &root_h);
   if (_eclair_menu_mouse_x - root_x + 1 >= root_w)
      result |= ECLAIR_MENU_RIGHT_EDGE;
   if (_eclair_menu_mouse_x <= root_x)
      result |= ECLAIR_MENU_LEFT_EDGE;
   if (_eclair_menu_mouse_y - root_y + 1 >= root_h)
      result |= ECLAIR_MENU_BOTTOM_EDGE;
   if (_eclair_menu_mouse_y <= root_y)
      result |= ECLAIR_MENU_TOP_EDGE;

   return result;
}

//Return 1 is the mouse is in the menu
static int _eclair_menu_mouse_is_in(Eclair_Menu *menu)
{
   int menu_x, menu_y, menu_w, menu_h;

   if (!menu)
      return 0;

   ecore_evas_geometry_get(menu->window, &menu_x, &menu_y, &menu_w, &menu_h);
   return (_eclair_menu_mouse_x >= menu_x && _eclair_menu_mouse_x <= menu_x + menu_w
      && _eclair_menu_mouse_y >= menu_y && _eclair_menu_mouse_y <= menu_y + menu_h);
}

//Start the slide timer if needed
static void _eclair_menu_update_slide_timer(Eclair_Menu *menu)
{
   Eclair_Menu_Screen_Edge mouse_on_edge, menu_over_edge;

   if (!menu)
      return;

   mouse_on_edge = _eclair_menu_mouse_on_screen_edge();
   menu_over_edge = _eclair_menu_over_screen_edge(menu);
   if (!_eclair_menu_slide_timer && (mouse_on_edge & menu_over_edge) != ECLAIR_MENU_NO_EDGE)
      _eclair_menu_slide_timer = ecore_timer_add(1.0 / 60.0, _eclair_menu_slide_timer_cb, NULL);
}

//Slide the menu (called every 1/60 sec)
static int _eclair_menu_slide_timer_cb(void *data)
{
   Eclair_Menu *menu = NULL, *m;
   Eina_List *l;
   Eclair_Menu_Screen_Edge mouse_edge, menu_edge = ECLAIR_MENU_NO_EDGE;
   int root_x, root_y, root_w, root_h;
   int menu_x, menu_y, menu_w, menu_h;
   int x, y;
   int dx = 0, dy = 0, max_delta = (int)(1.0 / 60.0 * 800);

   //TODO: FIXME: We could have problem with menus bigger than the screen width

   //We first look the menu that is over an edge of the screen
   for (l = _eclair_menu_popped_menus; l; l = l->next)
   {
      if (!(m = l->data))
         continue;

      if ((menu_edge = _eclair_menu_over_screen_edge(m)) != ECLAIR_MENU_NO_EDGE)
      {
         menu = m;
         break;
      }
   }
   if (!menu)
   {
      _eclair_menu_slide_timer = NULL;
      return 0;
   }

   //Then we move all the menu windows in the right direction
   mouse_edge = _eclair_menu_mouse_on_screen_edge();
   ecore_x_window_geometry_get(_eclair_menu_input_window, &root_x, &root_y, &root_w, &root_h);
   ecore_evas_geometry_get(menu->window, &menu_x, &menu_y, &menu_w, &menu_h);
   if (mouse_edge & menu_edge & ECLAIR_MENU_LEFT_EDGE)
   {
      if (max_delta < root_x - menu_x)
         dx = max_delta;
      else
         dx = root_x - menu_x;
   }
   if (mouse_edge & menu_edge & ECLAIR_MENU_RIGHT_EDGE)
   {
      if (max_delta < menu_w - root_x - root_w + menu_x)
         dx = -max_delta;
      else
         dx = -menu_w + root_x + root_w - menu_x;
   }
   if (mouse_edge & menu_edge & ECLAIR_MENU_TOP_EDGE)
   {
      if (max_delta < root_y - menu_y)
         dy = max_delta;
      else
         dy = root_y - menu_y;
   }
   if (mouse_edge & menu_edge & ECLAIR_MENU_BOTTOM_EDGE)
   {
      if (max_delta < menu_h - root_y - root_h + menu_y)
         dy = -max_delta;
      else
         dy = -menu_h + root_y + root_h - menu_y;
   }

   if (dx == 0 && dy == 0)
   {
      _eclair_menu_slide_timer = NULL;
      return 0;
   }

   for (l = _eclair_menu_popped_menus; l; l = l->next)
   {
      if (!(m = l->data))
         continue;

      ecore_evas_geometry_get(m->window, &x, &y, NULL, NULL);
      ecore_evas_move(m->window, x + dx, y + dy);

      //We feed a mouse move event since the relative position between
      //the mouse pointer and the menu window has changed
      evas_event_feed_mouse_move(m->evas, _eclair_menu_mouse_x - x, _eclair_menu_mouse_y - y, ecore_x_current_time_get(), NULL);
   }

   return 1;
}

//Pop down the menus if needed and feed the mouse up event to the menu window
static int _eclair_menu_mouse_up_cb(void *data, int type, void *event)
{
   Eclair_Menu *menu, *m;
   Eina_List *l;
   Ecore_X_Event_Mouse_Button_Up *mouse_event;
   int pointer_over_menu = 0;

   mouse_event = event;
   if (!(menu = data) || mouse_event->win != _eclair_menu_input_window)
      return 1;

   //If we are not clicking on a popped menu window, we pop down the menu root and all its children
   for (l = _eclair_menu_popped_menus; l; l = l->next)
   {
      if (!(m = l->data))
         continue;

      if (_eclair_menu_mouse_is_in(m))
      {
         pointer_over_menu = 1;
         evas_event_feed_mouse_up(m->evas, mouse_event->button, EVAS_BUTTON_NONE, mouse_event->time, NULL);
      }
   }
   if (!pointer_over_menu)
      eclair_menu_pop_down(_eclair_menu_root);

   return 1;
}

//Feed the mouse move, in and out events to the menu windows and start to slide the windows if needed
static int _eclair_menu_mouse_move_cb(void *data, int type, void *event)
{
   Eclair_Menu *menu, *m;
   Eina_List *l;
   Ecore_X_Event_Mouse_Move *mouse_event;
   int menu_x, menu_y;

   mouse_event = event;
   if (!(menu = data) || mouse_event->win != _eclair_menu_input_window)
      return 1;

   _eclair_menu_mouse_x = mouse_event->x;
   _eclair_menu_mouse_y = mouse_event->y;

   for (l = _eclair_menu_popped_menus; l; l = l->next)
   {
      if (!(m = l->data))
         continue;

      ecore_evas_geometry_get(m->window, &menu_x, &menu_y, NULL, NULL);
      evas_event_feed_mouse_move(m->evas, mouse_event->x - menu_x, mouse_event->y - menu_y, mouse_event->time, NULL);
      
      //Start to slide the menu window if we need
      _eclair_menu_update_slide_timer(m);
   }

   return 1;
}


//------------------------------
// Eclair_Menu_Item
//------------------------------

//Create a new menu item of type "item_type"
//This item will be automatically freed by calling eclair_menu_free on the parent menu
//Return NULL if failed
Eclair_Menu_Item *eclair_menu_item_new(const char *label, Eclair_Menu *parent, Eclair_Menu_Item_Type item_type, const char *icon_path)
{
   Eclair_Menu_Item *item;
   int result;

   if (!parent)
      return NULL;

   item = calloc(1, sizeof(Eclair_Menu_Item));
   item->type = item_type;

   //We load the edje object of the item
   item->edje_object = edje_object_add(parent->evas);
   if (item->type == ECLAIR_MENU_ITEM_SEPARATOR_ITEM)
      result = edje_object_file_set(item->edje_object, ECLAIR_WIDGETS_THEME, "eclair_menu_separator");
   else
      result = edje_object_file_set(item->edje_object, ECLAIR_WIDGETS_THEME, "eclair_menu_item");
   if (!result)
   {
      evas_object_del(item->edje_object);
      item->edje_object = NULL;
      return 0;
   }
   evas_object_event_callback_add(item->edje_object, EVAS_CALLBACK_MOUSE_IN, _eclair_menu_item_in_cb, item);
   evas_object_event_callback_add(item->edje_object, EVAS_CALLBACK_MOUSE_OUT, _eclair_menu_item_out_cb, item);
   evas_object_event_callback_add(item->edje_object, EVAS_CALLBACK_MOUSE_UP, _eclair_menu_item_up_cb, item);
   evas_object_show(item->edje_object);

   //We load the icon
   if (item->type == ECLAIR_MENU_ITEM_ICON_ITEM)
   {
      char *icon_size_str;
      int icon_size;

      item->icon_object = evas_object_image_add(parent->evas);
      evas_object_image_file_set(item->icon_object, icon_path, NULL);
      if (evas_object_image_load_error_get(item->icon_object))
      {
         evas_object_del(item->icon_object);
         item->icon_object = NULL;
         item->type = ECLAIR_MENU_ITEM_NORMAL_ITEM;
      }
      else
      {
         if ((icon_size_str = edje_file_data_get(ECLAIR_WIDGETS_THEME, "icon_size")))
         {
            icon_size = atoi(icon_size_str);
            evas_object_image_fill_set(item->icon_object, 0, 0, icon_size, icon_size);
            free(icon_size_str);
         }
         else
            evas_object_image_fill_set(item->icon_object, 0, 0, 16, 16);
         edje_object_part_swallow(item->edje_object, "icon", item->icon_object);
         evas_object_pass_events_set(item->icon_object, 1);
         evas_object_show(item->edje_object);
      }
   }

   eclair_menu_append_item(parent, item);

   if (item->type != ECLAIR_MENU_ITEM_SEPARATOR_ITEM)
      eclair_menu_item_label_set(item, label);

   return item;
}

//Destroy the item and its child
void eclair_menu_item_free(Eclair_Menu_Item *menu_item)
{
   if (!menu_item)
      return;

   free(menu_item->label);
   evas_object_del(menu_item->edje_object);
   if (menu_item->icon_object)
      evas_object_del(menu_item->icon_object);
   if (menu_item->icon_object)
      evas_object_del(menu_item->icon_object);
   if (menu_item->child)
   {
      menu_item->child->parent = NULL;
      eclair_menu_free(menu_item->child);
   }

   if (menu_item->parent)
   {
      menu_item->parent->items = eina_list_remove(menu_item->parent->items, menu_item);
      _eclair_menu_recalc(menu_item->parent);
   }
   free(menu_item);
}

//Set the label of the menu item
void eclair_menu_item_label_set(Eclair_Menu_Item *item, const char *label)
{
   if (!item)
      return;

   free(item->label);
   if (!label)
   {
      item->label = NULL;
      edje_object_part_text_set(item->edje_object, "label", "");
   }
   else
   {
      item->label = strdup(label);
      edje_object_part_text_set(item->edje_object, "label", item->label);
   }
   _eclair_menu_recalc(item->parent);
}

//Activate the menu item: deactivate other items and pop up the child menu of this item
void eclair_menu_item_activate(Eclair_Menu_Item *item)
{
   Eclair_Menu *menu;
   Eclair_Menu_Item *i;
   int menu_x, menu_y, menu_w, item_y = 0;
   int item_met = 0;
   Eina_List *l;

   if (!item || !(menu = item->parent) || item->is_active)
      return;

   //First, we deactivate all the items that are on the same menu than the item...
   for (l = menu->items; l; l = l->next)
   {
      if (!(i = l->data))
         continue;

      eclair_menu_item_deactivate(i);

      if (i == item)
         item_met = 1;
      if (!item_met)
         item_y += i->height;
   }

   edje_object_signal_emit(item->edje_object, "signal_activate", "eclair_bin");
   //...then we popup the child menu
   ecore_evas_geometry_get(menu->window, &menu_x, &menu_y, &menu_w, NULL);
   eclair_menu_popup_at_xy(item->child, menu_x + menu_w, menu_y + item_y);

   item->is_active = 1;
}

//Deactivate the menu item
void eclair_menu_item_deactivate(Eclair_Menu_Item *item)
{
   if (!item || !item->is_active)
      return;

   edje_object_signal_emit(item->edje_object, "signal_deactivate", "eclair_bin");
   eclair_menu_pop_down(item->child);
   item->is_active = 0;
}

//Set the callback called when the item is triggered
void eclair_menu_item_callback_set(Eclair_Menu_Item *item, void (*item_cb)(Eclair_Menu_Item *item, void *data), void *data)
{
   if (!item)
      return;

   item->item_cb = item_cb;
   item->cb_data = data;
}

//Called when the mouse pointer enters the item  
static void _eclair_menu_item_in_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   eclair_menu_item_activate(data);
}

//Called when the mouse pointer leaves the item
static void _eclair_menu_item_out_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eclair_Menu_Item *item;

   if (!(item = data))
      return;

   if (!item->child)
      eclair_menu_item_deactivate(data);
}

//Called when the user has clicked on the item
static void _eclair_menu_item_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eclair_Menu_Item *item;

   if (!(item = data))
      return;

   if (!item->child)
   {
      eclair_menu_pop_down(_eclair_menu_root);
      if (item->item_cb)
         item->item_cb(item, item->cb_data);
   }
}
  

