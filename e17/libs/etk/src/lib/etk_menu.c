/** @file etk_menu.c */
#include "etk_menu.h"
#include <stdlib.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include "etk_menu_item.h"
#include "etk_vbox.h"
#include "etk_utils.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"

/**
 * @addtogroup Etk_Menu
* @{
 */

#define ETK_MENU_MIN_POP_TIME 250

typedef enum _Etk_Menu_Screen_Edge
{
   ETK_MENU_NO_EDGE = 0,
   ETK_MENU_LEFT_EDGE = (1 << 0),
   ETK_MENU_RIGHT_EDGE = (1 << 1),
   ETK_MENU_BOTTOM_EDGE = (1 << 2),
   ETK_MENU_TOP_EDGE = (1 << 3)
} Etk_Menu_Screen_Edge;

enum _Etk_Widget_Signal_Id
{
   ETK_MENU_NUM_SIGNALS
};

static void _etk_menu_constructor(Etk_Menu *menu);
static void _etk_menu_destructor(Etk_Menu *menu);
static int _etk_menu_mouse_up_cb(void *data, int type, void *event);
static int _etk_menu_mouse_move_cb(void *data, int type, void *event);
static void _etk_menu_item_enter_cb(Etk_Object *object, void *data);
static void _etk_menu_item_leave_cb(Etk_Object *object, void *data);
static void _etk_menu_item_up_cb(Etk_Object *object, void *event, void *data);
static void _etk_menu_item_select_cb(Etk_Object *object, void *data);
static void _etk_menu_item_deselect_cb(Etk_Object *object, void *data);

static Etk_Menu_Screen_Edge _etk_menu_over_screen_edge(Etk_Menu *menu);
static Etk_Menu_Screen_Edge _etk_menu_mouse_on_screen_edge();
static void _etk_menu_slide_timer_update(Etk_Menu *menu);
static int _etk_menu_slide_timer_cb(void *data);

static Ecore_X_Window _etk_menu_input_window = 0;
static Etk_Menu *_etk_menu_root = NULL;
static Evas_List *_etk_menu_popped_menus = NULL;
static Ecore_Event_Handler *_etk_menu_mouse_up_handler = NULL;
static Ecore_Event_Handler *_etk_menu_mouse_move_handler = NULL;
static int _etk_menu_root_popup_timestamp = 0;
static int _etk_menu_mouse_x = -100000;
static int _etk_menu_mouse_y = -100000;
static Ecore_Timer *_etk_menu_slide_timer = NULL;

static Etk_Signal *_etk_menu_signals[ETK_MENU_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Menu
 * @return Returns the type on an Etk_Menu
 */
Etk_Type *etk_menu_type_get()
{
   static Etk_Type *menu_type = NULL;

   if (!menu_type)
   {
      menu_type = etk_type_new("Etk_Menu", ETK_WINDOW_TYPE, sizeof(Etk_Menu), ETK_CONSTRUCTOR(_etk_menu_constructor), ETK_DESTRUCTOR(_etk_menu_destructor));
   }

   return menu_type;
}

/**
 * @brief Creates a new menu
 * @return Returns the new menu widget
 */
Etk_Widget *etk_menu_new()
{
   return etk_widget_new(ETK_MENU_TYPE, "theme_group", "menu", NULL);
}

/** 
 * @brief Adds a menu item at the end of the menu
 * @param menu a menu
 * @param item the menu item to add
 */
void etk_menu_append(Etk_Menu *menu, Etk_Menu_Item *item)
{
   if (!menu || !item)
      return;
   
   etk_box_pack_start(ETK_BOX(menu->vbox), ETK_WIDGET(item), FALSE, FALSE, 0);
   item->parent = menu;
   etk_signal_connect("enter", ETK_OBJECT(item), ETK_CALLBACK(_etk_menu_item_enter_cb), NULL);
   etk_signal_connect("leave", ETK_OBJECT(item), ETK_CALLBACK(_etk_menu_item_leave_cb), NULL);
   etk_signal_connect("mouse_up", ETK_OBJECT(item), ETK_CALLBACK(_etk_menu_item_up_cb), NULL);
   etk_signal_connect("select", ETK_OBJECT(item), ETK_CALLBACK(_etk_menu_item_select_cb), NULL);
   etk_signal_connect("deselect", ETK_OBJECT(item), ETK_CALLBACK(_etk_menu_item_deselect_cb), NULL);
}

/**
 * @brief Popups the menu at the position (x, y)
 * @param menu a menu
 * @param x the x component of the location where to popup the menu
 * @param y the y component of the location where to popup the menu
 */
void etk_menu_popup_at_xy(Etk_Menu *menu, int x, int y)
{
   if (!menu)
      return;

   if (_etk_menu_input_window == 0)
   {
      Ecore_X_Window root, parent;
      int root_x, root_y, root_w, root_h;

      root = ETK_WINDOW(menu)->x_window;
      while ((parent = ecore_x_window_parent_get(root)) != 0)
         root = parent;
 
      ecore_x_window_geometry_get(root, &root_x, &root_y, &root_w, &root_h);
      _etk_menu_input_window = ecore_x_window_input_new(root, root_x, root_y, root_w, root_h);
   }
   if (!_etk_menu_root)
   {
      _etk_menu_mouse_up_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_UP, _etk_menu_mouse_up_cb, menu);
      _etk_menu_mouse_move_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_MOVE, _etk_menu_mouse_move_cb, menu);
      ecore_x_window_show(_etk_menu_input_window);
      ecore_x_keyboard_grab(_etk_menu_input_window);
      ecore_x_pointer_confine_grab(_etk_menu_input_window);

      _etk_menu_root_popup_timestamp = ecore_x_current_time_get();
      _etk_menu_root = menu;
   }
   etk_window_move(ETK_WINDOW(menu), x, y);
   etk_widget_show(ETK_WIDGET(menu));
   evas_event_feed_mouse_move(ETK_TOPLEVEL_WIDGET(menu)->evas, -100000, -100000, ecore_x_current_time_get(), NULL);
   evas_event_feed_mouse_in(ETK_TOPLEVEL_WIDGET(menu)->evas, ecore_x_current_time_get(), NULL);
   _etk_menu_popped_menus = evas_list_append(_etk_menu_popped_menus, menu);

   _etk_menu_slide_timer_update(menu);
}

/**
 * @brief Popups the menu at the mouse pointer position
 * @param menu a menu
 */
void etk_menu_popup(Etk_Menu *menu)
{
   int x, y;

   ecore_x_pointer_last_xy_get(&x, &y);
   etk_menu_popup_at_xy(menu, x + 1, y + 1);
}

/**
 * @brief Pops down the menu and its children
 * @param menu a menu
 */
void etk_menu_popdown(Etk_Menu *menu)
{
   Evas_List *items, *l;
   Etk_Menu_Item *item;

   if (!menu)
      return;

   items = ETK_CONTAINER(menu->vbox)->children;
   for (l = items; l; l = l->next)
   {
      item = ETK_MENU_ITEM(l->data);
      etk_menu_item_deselect(item);
   }

   etk_widget_hide(ETK_WIDGET(menu));
   if (menu == _etk_menu_root)
   {
      ecore_x_keyboard_ungrab();
      ecore_x_pointer_ungrab();
      ecore_x_window_hide(_etk_menu_input_window);
      ecore_event_handler_del(_etk_menu_mouse_up_handler);
      ecore_event_handler_del(_etk_menu_mouse_move_handler);
      _etk_menu_root = NULL;
   }

   _etk_menu_popped_menus = evas_list_remove(_etk_menu_popped_menus, menu);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_menu_constructor(Etk_Menu *menu)
{
   if (!menu)
      return;
   
   etk_window_decorated_set(ETK_WINDOW(menu), FALSE);
   etk_window_skip_taskbar_hint_set(ETK_WINDOW(menu), TRUE);
   etk_window_skip_pager_hint_set(ETK_WINDOW(menu), TRUE);
   
   menu->vbox = etk_vbox_new(FALSE, 0);
   etk_container_add(ETK_CONTAINER(menu), menu->vbox);
   etk_widget_show(menu->vbox);
   
   /* TODO */
   ETK_CONTAINER(menu)->child_add = NULL;
}

/* Destroys the menu */
static void _etk_menu_destructor(Etk_Menu *menu)
{
   if (!menu)
      return;

   /* TODO */
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/*
 * Called when the user clicks on the menu input window:
 * it pops down the menus if needed and feeds the mouse up event to the menu window
 */
static int _etk_menu_mouse_up_cb(void *data, int type, void *event)
{
   Etk_Menu *menu, *m;
   Evas_List *l;
   Ecore_X_Event_Mouse_Button_Up *mouse_event;
   Etk_Bool pointer_over_menu = FALSE;
   
   if (!(menu = ETK_MENU(data)) || !(mouse_event = event) || mouse_event->win != _etk_menu_input_window)
      return 1;

   /* If the user clicks on a popped menu window, we feed the event */
   for (l = _etk_menu_popped_menus; l; l = l->next)
   {
      int mx, my, mw, mh;
      
      m = ETK_MENU(l->data);
      etk_window_geometry_get(ETK_WINDOW(menu), &mx, &my, &mw, &mh);
      if (_etk_menu_mouse_x >= mx && _etk_menu_mouse_x <= mx + mw && _etk_menu_mouse_y >= my && _etk_menu_mouse_y <= my + mh)
      {
         pointer_over_menu = TRUE;
         evas_event_feed_mouse_up(ETK_TOPLEVEL_WIDGET(m)->evas, mouse_event->button, EVAS_BUTTON_NONE, mouse_event->time, NULL);
         if (!_etk_menu_popped_menus)
            break;
      }
   }
   /* Otherwize, we pop down the menu root and all its children */
   if (!pointer_over_menu && (mouse_event->time - _etk_menu_root_popup_timestamp) >= ETK_MENU_MIN_POP_TIME)
      etk_menu_popdown(_etk_menu_root);

   return 1;
}

/*
 * Called when the user moves the mouse above the menu input window:
 * It feeds the mouse move, in and out events to the menu windows and starts to make the menu windows slide if needed
 */
static int _etk_menu_mouse_move_cb(void *data, int type, void *event)
{
   Etk_Menu *menu, *m;
   Evas_List *l;
   Ecore_X_Event_Mouse_Move *mouse_event;
   int mx, my;

   if (!(menu = ETK_MENU(data)) || !(mouse_event = event) || mouse_event->win != _etk_menu_input_window)
      return 1;

   _etk_menu_mouse_x = mouse_event->x;
   _etk_menu_mouse_y = mouse_event->y;

   for (l = _etk_menu_popped_menus; l; l = l->next)
   {
      m = ETK_MENU(l->data);
      etk_window_geometry_get(ETK_WINDOW(menu), &mx, &my, NULL, NULL);
      evas_event_feed_mouse_move(ETK_TOPLEVEL_WIDGET(m)->evas, mouse_event->x - mx, mouse_event->y - my, mouse_event->time, NULL);
      
      /* Start to make the menu window slide if needed */
      _etk_menu_slide_timer_update(m);
   }
   
   return 1;
}

/* Called when the mouse pointer enters the item */ 
static void _etk_menu_item_enter_cb(Etk_Object *object, void *data)
{
   etk_menu_item_select(ETK_MENU_ITEM(object));
}

/* Called when the mouse pointer leaves the item */
static void _etk_menu_item_leave_cb(Etk_Object *object, void *data)
{
   Etk_Menu_Item *item;
   
   if (!(item = ETK_MENU_ITEM(object)) || item->child)
      return;
   etk_menu_item_deselect(item);
}

/* Called when the user has clicked on the item */
static void _etk_menu_item_up_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Menu_Item *item;
   
   if (!(item = ETK_MENU_ITEM(object)) || item->child)
      return;

   etk_menu_item_activate(item);
   etk_menu_popdown(_etk_menu_root);
}

/* Called when the item is selected */
static void _etk_menu_item_select_cb(Etk_Object *object, void *data)
{
   Etk_Menu_Item *item;
   Etk_Menu *menu;
   Etk_Menu_Item *i;
   Evas_List *l;
   
   if (!(item = ETK_MENU_ITEM(object)) || !(menu = item->parent))
      return;

   /* First, we deactivate all the items that are on the same menu than the item */
   for (l = ETK_CONTAINER(menu->vbox)->children; l; l = l->next)
   {
      i = ETK_MENU_ITEM(l->data);
      /* TODO */
      etk_menu_item_deselect(i);
   }
   
   /* Then we popup the child menu */
   if (item->child)
   {
      int mx, my, mw, item_y;
      
      item_y = ETK_WIDGET(item)->geometry.h;
      etk_window_geometry_get(ETK_WINDOW(menu), &mx, &my, &mw, NULL);
      etk_menu_popup_at_xy(item->child, mx + mw, my + item_y);
   }
}

/* Called when the item is deselected */
static void _etk_menu_item_deselect_cb(Etk_Object *object, void *data)
{
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Returns a flag incating on which edges of the screen the menu is over */
static Etk_Menu_Screen_Edge _etk_menu_over_screen_edge(Etk_Menu *menu)
{
   int rx, ry, rw, rh;
   int mx, my, mw, mh;
   Etk_Menu_Screen_Edge result = ETK_MENU_NO_EDGE;

   if (!menu || _etk_menu_input_window == 0)
      return ETK_MENU_NO_EDGE;

   ecore_x_window_geometry_get(_etk_menu_input_window, &rx, &ry, &rw, &rh);
   etk_window_geometry_get(ETK_WINDOW(menu), &mx, &my, &mw, &mh);

   if (mx < rx)
      result |= ETK_MENU_LEFT_EDGE;
   if (mx + mw > rx + rw)
      result |= ETK_MENU_RIGHT_EDGE;
   if (my < ry)
      result |= ETK_MENU_TOP_EDGE;
   if (my + mh > ry + rh)
      result |= ETK_MENU_BOTTOM_EDGE;
      
   return result;
}

/* Returns a flag incating on which edges of the screen the mouse pointer is */
static Etk_Menu_Screen_Edge _etk_menu_mouse_on_screen_edge()
{
   int rx, ry, rw, rh;
   Etk_Menu_Screen_Edge result = ETK_MENU_NO_EDGE;

   if (_etk_menu_input_window == 0)
      return ETK_MENU_NO_EDGE;
   
   ecore_x_window_geometry_get(_etk_menu_input_window, &rx, &ry, &rw, &rh);
   if (_etk_menu_mouse_x - rx + 1 >= rw)
      result |= ETK_MENU_RIGHT_EDGE;
   if (_etk_menu_mouse_x <= rx)
      result |= ETK_MENU_LEFT_EDGE;
   if (_etk_menu_mouse_y - ry + 1 >= rh)
      result |= ETK_MENU_BOTTOM_EDGE;
   if (_etk_menu_mouse_y <= ry)
      result |= ETK_MENU_TOP_EDGE;

   return result;
}

/* Starts the slide timer if needed */
static void _etk_menu_slide_timer_update(Etk_Menu *menu)
{
   Etk_Menu_Screen_Edge mouse_on_edge, menu_over_edge;

   if (!menu)
      return;

   mouse_on_edge = _etk_menu_mouse_on_screen_edge();
   menu_over_edge = _etk_menu_over_screen_edge(menu);
   if (!_etk_menu_slide_timer && (mouse_on_edge & menu_over_edge) != ETK_MENU_NO_EDGE)
      _etk_menu_slide_timer = ecore_timer_add(1.0 / 60.0, _etk_menu_slide_timer_cb, NULL);
}

/* Makes the menu slide (called every 1/60 sec) */
static int _etk_menu_slide_timer_cb(void *data)
{
   Etk_Menu *menu = NULL, *m;
   Evas_List *l;
   Etk_Menu_Screen_Edge mouse_edge, menu_edge;
   int rx, ry, rw, rh;
   int mx, my, mw, mh;
   int x, y;
   int dx = 0, dy = 0, max_delta = (int)(1.0 / 60.0 * 800);

   /* TODO: FIXME */
   /* We first look for the menu that is over an edge of the screen */
   for (l = _etk_menu_popped_menus; l; l = l->next)
   {
      m = ETK_MENU(l->data);
      if ((menu_edge = _etk_menu_over_screen_edge(m)) != ETK_MENU_NO_EDGE)
      {
         menu = m;
         break;
      }
   }
   if (!menu)
   {
      _etk_menu_slide_timer = NULL;
      return 0;
   }

   /* Then we move all the menu windows in the right direction */
   mouse_edge = _etk_menu_mouse_on_screen_edge();
   ecore_x_window_geometry_get(_etk_menu_input_window, &rx, &ry, &rw, &rh);
   etk_window_geometry_get(ETK_WINDOW(menu), &mx, &my, &mw, &mh);
   if (mouse_edge & menu_edge & ETK_MENU_LEFT_EDGE)
   {
      if (max_delta < rx - mx)
         dx = max_delta;
      else
         dx = rx - mx;
   }
   if (mouse_edge & menu_edge & ETK_MENU_RIGHT_EDGE)
   {
      if (max_delta < mw - rx - rw + mx)
         dx = -max_delta;
      else
         dx = -mw + rx + rw - mx;
   }
   if (mouse_edge & menu_edge & ETK_MENU_TOP_EDGE)
   {
      if (max_delta < ry - my)
         dy = max_delta;
      else
         dy = ry - my;
   }
   if (mouse_edge & menu_edge & ETK_MENU_BOTTOM_EDGE)
   {
      if (max_delta < mh - ry - rh + my)
         dy = -max_delta;
      else
         dy = -mh + ry + rh - my;
   }

   if (dx == 0 && dy == 0)
   {
      _etk_menu_slide_timer = NULL;
      return 0;
   }

   for (l = _etk_menu_popped_menus; l; l = l->next)
   {
      m = ETK_MENU(l->data);
      etk_window_geometry_get(ETK_WINDOW(m), &x, &y, NULL, NULL);
      etk_window_move(ETK_WINDOW(m), x + dx, y + dy);

      /* We feed a mouse move event since the relative position between the mouse pointer and the menu window has changed */
      evas_event_feed_mouse_move(ETK_TOPLEVEL_WIDGET(m)->evas, _etk_menu_mouse_x - x, _etk_menu_mouse_y - y, ecore_x_current_time_get(), NULL);
   }

   return 1;
}

/** @} */
