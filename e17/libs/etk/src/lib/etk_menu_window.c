/** @file etk_menu_window.c */
#include "etk_menu_window.h"
#include <stdlib.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include "etk_utils.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"

/**
 * @addtogroup Etk_Menu_Window
 * @{
 */

#define ETK_MENU_WINDOW_MIN_POP_TIME 400

enum _Etk_Menu_Window_Signal_Id
{
   ETK_MENU_WINDOW_POPPED_DOWN_SIGNAL,
   ETK_MENU_WINDOW_POPPED_UP_SIGNAL,
   ETK_MENU_WINDOW_NUM_SIGNALS
};

typedef enum _Etk_Menu_Window_Screen_Edge
{
   ETK_MENU_WINDOW_NO_EDGE = 0,
   ETK_MENU_WINDOW_LEFT_EDGE = (1 << 0),
   ETK_MENU_WINDOW_RIGHT_EDGE = (1 << 1),
   ETK_MENU_WINDOW_BOTTOM_EDGE = (1 << 2),
   ETK_MENU_WINDOW_TOP_EDGE = (1 << 3)
} Etk_Menu_Window_Screen_Edge;

static void _etk_menu_window_constructor(Etk_Menu_Window *menu_window);
static int _etk_menu_window_mouse_up_cb(void *data, int type, void *event);
static int _etk_menu_window_mouse_move_cb(void *data, int type, void *event);

static Etk_Menu_Window_Screen_Edge _etk_menu_window_over_screen_edge_get(Etk_Menu_Window *menu_window);
static Etk_Menu_Window_Screen_Edge _etk_menu_window_mouse_on_screen_edge_get();
static void _etk_menu_window_slide_timer_update(Etk_Menu_Window *menu_window);
static int _etk_menu_window_slide_timer_cb(void *data);

static Ecore_X_Window _etk_menu_window_input_window = 0;
static Evas_List *_etk_menu_window_popped_windows = NULL;
static Ecore_Event_Handler *_etk_menu_window_mouse_up_handler = NULL;
static Ecore_Event_Handler *_etk_menu_window_mouse_move_handler = NULL;
static int _etk_menu_window_popup_timestamp = 0;
static int _etk_menu_window_mouse_x = -100000;
static int _etk_menu_window_mouse_y = -100000;
static Ecore_Timer *_etk_menu_window_slide_timer = NULL;

static Etk_Signal *_etk_menu_window_signals[ETK_MENU_WINDOW_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Menu_Window
 * @return Returns the type on an Etk_Menu_Window
 */
Etk_Type *etk_menu_window_type_get()
{
   static Etk_Type *menu_window_type = NULL;

   if (!menu_window_type)
   {
      menu_window_type = etk_type_new("Etk_Menu_Window", ETK_WINDOW_TYPE, sizeof(Etk_Menu_Window), ETK_CONSTRUCTOR(_etk_menu_window_constructor), NULL);
      
      _etk_menu_window_signals[ETK_MENU_WINDOW_POPPED_UP_SIGNAL] = etk_signal_new("popped_up", menu_window_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_menu_window_signals[ETK_MENU_WINDOW_POPPED_DOWN_SIGNAL] = etk_signal_new("popped_down", menu_window_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
   }

   return menu_window_type;
}

/**
 * @brief Popups the menu window at the position (x, y)
 * @param menu_window a menu window
 * @param x the x component of the location where to popup the menu window
 * @param y the y component of the location where to popup the menu window
 */
void etk_menu_window_popup_at_xy(Etk_Menu_Window *menu_window, int x, int y)
{
   if (!menu_window)
      return;

   if (_etk_menu_window_input_window == 0)
   {
      Ecore_X_Window root, parent;
      int root_x, root_y, root_w, root_h;

      root = ETK_WINDOW(menu_window)->x_window;
      while ((parent = ecore_x_window_parent_get(root)) != 0)
         root = parent;
 
      ecore_x_window_geometry_get(root, &root_x, &root_y, &root_w, &root_h);
      _etk_menu_window_input_window = ecore_x_window_input_new(root, root_x, root_y, root_w, root_h);
      ecore_x_window_show(_etk_menu_window_input_window);
      /* TODO: fixme pointer_grab!! */
      /* ecore_x_pointer_confine_grab(_etk_menu_window_input_window); */
      ecore_x_keyboard_grab(_etk_menu_window_input_window);
      
      _etk_menu_window_mouse_up_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_UP, _etk_menu_window_mouse_up_cb, menu_window);
      _etk_menu_window_mouse_move_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_MOVE, _etk_menu_window_mouse_move_cb, menu_window);

      _etk_menu_window_popup_timestamp = ecore_x_current_time_get();
   }
   
   etk_window_move(ETK_WINDOW(menu_window), x, y);
   etk_widget_show(ETK_WIDGET(menu_window));
   evas_event_feed_mouse_move(ETK_TOPLEVEL_WIDGET(menu_window)->evas, -100000, -100000, ecore_x_current_time_get(), NULL);
   evas_event_feed_mouse_in(ETK_TOPLEVEL_WIDGET(menu_window)->evas, ecore_x_current_time_get(), NULL);
   _etk_menu_window_popped_windows = evas_list_append(_etk_menu_window_popped_windows, menu_window);

   _etk_menu_window_slide_timer_update(menu_window);
   
   etk_signal_emit(_etk_menu_window_signals[ETK_MENU_WINDOW_POPPED_UP_SIGNAL], ETK_OBJECT(menu_window), NULL);
}

/**
 * @brief Popups the menu window at the mouse pointer position
 * @param menu_window a menu window
 */
void etk_menu_window_popup(Etk_Menu_Window *menu_window)
{
   int x, y;

   ecore_x_pointer_last_xy_get(&x, &y);
   etk_menu_window_popup_at_xy(menu_window, x + 1, y + 1);
}

/**
 * @brief Pops down the menu window
 * @param menu_window a menu window
 */
void etk_menu_window_popdown(Etk_Menu_Window *menu_window)
{
   Evas_List *l;

   if (!menu_window || !(l = evas_list_find_list(_etk_menu_window_popped_windows, menu_window)))
      return;

   etk_widget_hide(ETK_WIDGET(menu_window));
   _etk_menu_window_popped_windows = evas_list_remove_list(_etk_menu_window_popped_windows, l);
   
   if (!_etk_menu_window_popped_windows)
   {
      /* TODO: pointer ungrab, fixme!! */
      /* ecore_x_pointer_ungrab(); */
      ecore_x_keyboard_ungrab();
      ecore_x_window_del(_etk_menu_window_input_window);
      ecore_event_handler_del(_etk_menu_window_mouse_up_handler);
      ecore_event_handler_del(_etk_menu_window_mouse_move_handler);
      _etk_menu_window_input_window = 0;
      _etk_menu_window_mouse_up_handler = NULL;
      _etk_menu_window_mouse_move_handler = NULL;
   }
   
   etk_signal_emit(_etk_menu_window_signals[ETK_MENU_WINDOW_POPPED_DOWN_SIGNAL], ETK_OBJECT(menu_window), NULL);
}


/**
 * @brief Pops down all the popped menu windows
 */
void etk_menu_window_popdown_all()
{
   while (_etk_menu_window_popped_windows)
      etk_menu_window_popdown(ETK_MENU_WINDOW(_etk_menu_window_popped_windows->data));
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_menu_window_constructor(Etk_Menu_Window *menu_window)
{
   if (!menu_window)
      return;
   
   etk_window_decorated_set(ETK_WINDOW(menu_window), ETK_FALSE);
   etk_window_skip_taskbar_hint_set(ETK_WINDOW(menu_window), ETK_TRUE);
   etk_window_skip_pager_hint_set(ETK_WINDOW(menu_window), ETK_TRUE);
   /* TODO */
   ecore_x_netwm_window_type_set(ETK_WINDOW(menu_window)->x_window, ECORE_X_WINDOW_TYPE_MENU);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/*
 * Called when the user clicks on the input window:
 * it pops down the opened menu windows if needed and feeds the mouse up event to the menu window
 */
static int _etk_menu_window_mouse_up_cb(void *data, int type, void *event)
{
   Etk_Menu_Window *menu_window, *mwin;
   Evas_List *l;
   Ecore_X_Event_Mouse_Button_Up *mouse_event;
   Etk_Bool pointer_over_menu = ETK_FALSE;
   
   if (!(menu_window = ETK_MENU_WINDOW(data)) || !(mouse_event = event) || mouse_event->win != _etk_menu_window_input_window)
      return 1;

   /* If the user clicks on a popped menu window, we feed the event */
   for (l = _etk_menu_window_popped_windows; l; l = l->next)
   {
      int mx, my, mw, mh;
      
      mwin = ETK_MENU_WINDOW(l->data);
      etk_window_geometry_get(ETK_WINDOW(mwin), &mx, &my, &mw, &mh);
      if (_etk_menu_window_mouse_x >= mx && _etk_menu_window_mouse_x <= mx + mw && _etk_menu_window_mouse_y >= my && _etk_menu_window_mouse_y <= my + mh)
      {
         pointer_over_menu = ETK_TRUE;
         evas_event_feed_mouse_up(ETK_TOPLEVEL_WIDGET(mwin)->evas, mouse_event->button, EVAS_BUTTON_NONE, mouse_event->time, NULL);
         break;
      }
   }
   /* Otherwize, we pop down all the menu windows */
   if (!pointer_over_menu && (mouse_event->time - _etk_menu_window_popup_timestamp) >= ETK_MENU_WINDOW_MIN_POP_TIME)
      etk_menu_window_popdown_all();

   return 1;
}

/*
 * Called when the user moves the mouse above the menu input window:
 * It feeds the mouse move, in and out events to the menu windows and starts to make the menu windows slide if needed
 */
static int _etk_menu_window_mouse_move_cb(void *data, int type, void *event)
{
   Etk_Menu_Window *menu_window, *mwin;
   Evas_List *l;
   Ecore_X_Event_Mouse_Move *mouse_event;
   int mx, my;

   if (!(menu_window = ETK_MENU_WINDOW(data)) || !(mouse_event = event) || mouse_event->win != _etk_menu_window_input_window)
      return 1;

   _etk_menu_window_mouse_x = mouse_event->x;
   _etk_menu_window_mouse_y = mouse_event->y;

   for (l = _etk_menu_window_popped_windows; l; l = l->next)
   {
      mwin = ETK_MENU_WINDOW(l->data);
      etk_window_geometry_get(ETK_WINDOW(mwin), &mx, &my, NULL, NULL);
      evas_event_feed_mouse_move(ETK_TOPLEVEL_WIDGET(mwin)->evas, mouse_event->x - mx, mouse_event->y - my, mouse_event->time, NULL);
      
      /* Start to make the menu window slide if needed */
      _etk_menu_window_slide_timer_update(mwin);
   }
   
   return 1;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Returns a flag incating on which edges of the screen the menu window is over */
static Etk_Menu_Window_Screen_Edge _etk_menu_window_over_screen_edge_get(Etk_Menu_Window *menu_window)
{
   int rx, ry, rw, rh;
   int mx, my, mw, mh;
   Etk_Menu_Window_Screen_Edge result = ETK_MENU_WINDOW_NO_EDGE;

   if (!menu_window || _etk_menu_window_input_window == 0)
      return ETK_MENU_WINDOW_NO_EDGE;

   ecore_x_window_geometry_get(_etk_menu_window_input_window, &rx, &ry, &rw, &rh);
   etk_window_geometry_get(ETK_WINDOW(menu_window), &mx, &my, &mw, &mh);

   if (mx < rx)
      result |= ETK_MENU_WINDOW_LEFT_EDGE;
   if (mx + mw > rx + rw)
      result |= ETK_MENU_WINDOW_RIGHT_EDGE;
   if (my < ry)
      result |= ETK_MENU_WINDOW_TOP_EDGE;
   if (my + mh > ry + rh)
      result |= ETK_MENU_WINDOW_BOTTOM_EDGE;
      
   return result;
}

/* Returns a flag incating on which edges of the screen the mouse pointer is */
static Etk_Menu_Window_Screen_Edge _etk_menu_window_mouse_on_screen_edge_get()
{
   int rx, ry, rw, rh;
   Etk_Menu_Window_Screen_Edge result = ETK_MENU_WINDOW_NO_EDGE;

   if (_etk_menu_window_input_window == 0)
      return ETK_MENU_WINDOW_NO_EDGE;
   
   ecore_x_window_geometry_get(_etk_menu_window_input_window, &rx, &ry, &rw, &rh);
   if (_etk_menu_window_mouse_x - rx + 1 >= rw)
      result |= ETK_MENU_WINDOW_RIGHT_EDGE;
   if (_etk_menu_window_mouse_x <= rx)
      result |= ETK_MENU_WINDOW_LEFT_EDGE;
   if (_etk_menu_window_mouse_y - ry + 1 >= rh)
      result |= ETK_MENU_WINDOW_BOTTOM_EDGE;
   if (_etk_menu_window_mouse_y <= ry)
      result |= ETK_MENU_WINDOW_TOP_EDGE;

   return result;
}

/* Starts the slide timer if needed */
static void _etk_menu_window_slide_timer_update(Etk_Menu_Window *menu_window)
{
   Etk_Menu_Window_Screen_Edge mouse_on_edge, menu_over_edge;

   if (!menu_window)
      return;

   mouse_on_edge = _etk_menu_window_mouse_on_screen_edge_get();
   menu_over_edge = _etk_menu_window_over_screen_edge_get(menu_window);
   if (!_etk_menu_window_slide_timer && (mouse_on_edge & menu_over_edge) != ETK_MENU_WINDOW_NO_EDGE)
      _etk_menu_window_slide_timer = ecore_timer_add(1.0 / 60.0, _etk_menu_window_slide_timer_cb, NULL);
}

/* Makes the menu slide (called every 1/60 sec) */
static int _etk_menu_window_slide_timer_cb(void *data)
{
   Etk_Menu_Window *menu_window = NULL, *mwin;
   Evas_List *l;
   Etk_Menu_Window_Screen_Edge mouse_edge, menu_edge;
   int rx, ry, rw, rh;
   int mx, my, mw, mh;
   int x, y;
   int dx = 0, dy = 0, max_delta = (int)(1.0 / 60.0 * 800);

   /* TODO: FIXME */
   /* We first look for the menu that is over an edge of the screen */
   for (l = _etk_menu_window_popped_windows; l; l = l->next)
   {
      mwin = ETK_MENU_WINDOW(l->data);
      if ((menu_edge = _etk_menu_window_over_screen_edge_get(mwin)) != ETK_MENU_WINDOW_NO_EDGE)
      {
         menu_window = mwin;
         break;
      }
   }
   if (!menu_window)
   {
      _etk_menu_window_slide_timer = NULL;
      return 0;
   }

   /* Then we move all the menu windows in the right direction */
   mouse_edge = _etk_menu_window_mouse_on_screen_edge_get();
   ecore_x_window_geometry_get(_etk_menu_window_input_window, &rx, &ry, &rw, &rh);
   etk_window_geometry_get(ETK_WINDOW(menu_window), &mx, &my, &mw, &mh);
   if (mouse_edge & menu_edge & ETK_MENU_WINDOW_LEFT_EDGE)
   {
      if (max_delta < rx - mx)
         dx = max_delta;
      else
         dx = rx - mx;
   }
   if (mouse_edge & menu_edge & ETK_MENU_WINDOW_RIGHT_EDGE)
   {
      if (max_delta < mw - rx - rw + mx)
         dx = -max_delta;
      else
         dx = -mw + rx + rw - mx;
   }
   if (mouse_edge & menu_edge & ETK_MENU_WINDOW_TOP_EDGE)
   {
      if (max_delta < ry - my)
         dy = max_delta;
      else
         dy = ry - my;
   }
   if (mouse_edge & menu_edge & ETK_MENU_WINDOW_BOTTOM_EDGE)
   {
      if (max_delta < mh - ry - rh + my)
         dy = -max_delta;
      else
         dy = -mh + ry + rh - my;
   }

   if (dx == 0 && dy == 0)
   {
      _etk_menu_window_slide_timer = NULL;
      return 0;
   }

   for (l = _etk_menu_window_popped_windows; l; l = l->next)
   {
      mwin = ETK_MENU_WINDOW(l->data);
      etk_window_geometry_get(ETK_WINDOW(mwin), &x, &y, NULL, NULL);
      etk_window_move(ETK_WINDOW(mwin), x + dx, y + dy);

      /* We feed a mouse move event since the relative position between the mouse pointer and the menu window has changed */
      evas_event_feed_mouse_move(ETK_TOPLEVEL_WIDGET(mwin)->evas, _etk_menu_window_mouse_x - x, _etk_menu_window_mouse_y - y, ecore_x_current_time_get(), NULL);
   }

   return 1;
}

/** @} */
