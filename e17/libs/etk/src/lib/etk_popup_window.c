/** @file etk_popup_window.c */
#include "etk_popup_window.h"
#include <stdlib.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include "etk_utils.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"

/**
 * @addtogroup Etk_Popup_Window
 * @{
 */

#define ETK_POPUP_WINDOW_MIN_POP_TIME 400

enum _Etk_Popup_Window_Signal_Id
{
   ETK_POPUP_WINDOW_POPPED_DOWN_SIGNAL,
   ETK_POPUP_WINDOW_POPPED_UP_SIGNAL,
   ETK_POPUP_WINDOW_NUM_SIGNALS
};

typedef enum _Etk_Popup_Window_Screen_Edge
{
   ETK_POPUP_WINDOW_NO_EDGE = 0,
   ETK_POPUP_WINDOW_LEFT_EDGE = (1 << 0),
   ETK_POPUP_WINDOW_RIGHT_EDGE = (1 << 1),
   ETK_POPUP_WINDOW_BOTTOM_EDGE = (1 << 2),
   ETK_POPUP_WINDOW_TOP_EDGE = (1 << 3)
} Etk_Popup_Window_Screen_Edge;

static void _etk_popup_window_constructor(Etk_Popup_Window *popup_window);

static int _etk_popup_window_key_down_cb(void *data, int type, void *event);
static int _etk_popup_window_key_up_cb(void *data, int type, void *event);
static int _etk_popup_window_mouse_move_cb(void *data, int type, void *event);
static int _etk_popup_window_mouse_up_cb(void *data, int type, void *event);

static Etk_Popup_Window_Screen_Edge _etk_popup_window_over_screen_edge_get(Etk_Popup_Window *popup_window);
static Etk_Popup_Window_Screen_Edge _etk_popup_window_mouse_on_screen_edge_get();
static void _etk_popup_window_slide_timer_update(Etk_Popup_Window *popup_window);
static int _etk_popup_window_slide_timer_cb(void *data);

static Ecore_X_Window _etk_popup_window_input_window = 0;
static Evas_List *_etk_popup_window_popped_windows = NULL;
static Etk_Popup_Window *_etk_popup_window_focused_window = NULL;
static int _etk_popup_window_popup_timestamp = 0;
static int _etk_popup_window_mouse_x = -100000;
static int _etk_popup_window_mouse_y = -100000;
static Ecore_Timer *_etk_popup_window_slide_timer = NULL;

static Ecore_Event_Handler *_etk_popup_window_key_down_handler = NULL;
static Ecore_Event_Handler *_etk_popup_window_key_up_handler = NULL;
static Ecore_Event_Handler *_etk_popup_window_mouse_move_handler = NULL;
static Ecore_Event_Handler *_etk_popup_window_mouse_up_handler = NULL;

static Etk_Signal *_etk_popup_window_signals[ETK_POPUP_WINDOW_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Popup_Window
 * @return Returns the type on an Etk_Popup_Window
 */
Etk_Type *etk_popup_window_type_get()
{
   static Etk_Type *popup_window_type = NULL;

   if (!popup_window_type)
   {
      popup_window_type = etk_type_new("Etk_Popup_Window", ETK_WINDOW_TYPE, sizeof(Etk_Popup_Window), ETK_CONSTRUCTOR(_etk_popup_window_constructor), NULL);

      _etk_popup_window_signals[ETK_POPUP_WINDOW_POPPED_UP_SIGNAL] = etk_signal_new("popped_up", popup_window_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_popup_window_signals[ETK_POPUP_WINDOW_POPPED_DOWN_SIGNAL] = etk_signal_new("popped_down", popup_window_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
   }

   return popup_window_type;
}

/**
 * @brief Popups the popup window at the position (x, y)
 * @param popup_window a popup window
 * @param x the x component of the location where to popup the popup window
 * @param y the y component of the location where to popup the popup window
 */
void etk_popup_window_popup_at_xy(Etk_Popup_Window *popup_window, int x, int y)
{
   if (!popup_window)
      return;

   if (_etk_popup_window_input_window == 0)
   {
      Ecore_X_Window root, parent;
      int root_x, root_y, root_w, root_h;

      root = ETK_WINDOW(popup_window)->x_window;
      while ((parent = ecore_x_window_parent_get(root)) != 0)
         root = parent;

      ecore_x_window_geometry_get(root, &root_x, &root_y, &root_w, &root_h);
      _etk_popup_window_input_window = ecore_x_window_input_new(root, root_x, root_y, root_w, root_h);
      ecore_x_window_show(_etk_popup_window_input_window);
      /* TODO: fixme pointer_grab!! */
      /* ecore_x_pointer_confine_grab(_etk_popup_window_input_window); */
      ecore_x_keyboard_grab(_etk_popup_window_input_window);

      _etk_popup_window_key_down_handler = ecore_event_handler_add(ECORE_X_EVENT_KEY_DOWN, _etk_popup_window_key_down_cb, popup_window);
      _etk_popup_window_key_up_handler = ecore_event_handler_add(ECORE_X_EVENT_KEY_UP, _etk_popup_window_key_up_cb, popup_window);
      _etk_popup_window_mouse_up_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_UP, _etk_popup_window_mouse_up_cb, popup_window);
      _etk_popup_window_mouse_move_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_MOVE, _etk_popup_window_mouse_move_cb, popup_window);

      _etk_popup_window_popup_timestamp = ecore_x_current_time_get();
   }

   etk_window_move(ETK_WINDOW(popup_window), x, y);
   etk_widget_show(ETK_WIDGET(popup_window));
   evas_event_feed_mouse_move(ETK_TOPLEVEL_WIDGET(popup_window)->evas, -100000, -100000, ecore_x_current_time_get(), NULL);
   evas_event_feed_mouse_in(ETK_TOPLEVEL_WIDGET(popup_window)->evas, ecore_x_current_time_get(), NULL);
   _etk_popup_window_popped_windows = evas_list_append(_etk_popup_window_popped_windows, popup_window);

   etk_popup_window_focused_window_set(popup_window);
   _etk_popup_window_slide_timer_update(popup_window);

   etk_signal_emit(_etk_popup_window_signals[ETK_POPUP_WINDOW_POPPED_UP_SIGNAL], ETK_OBJECT(popup_window), NULL);
}

/**
 * @brief Popups the popup window at the mouse pointer position
 * @param popup_window a popup window
 */
void etk_popup_window_popup(Etk_Popup_Window *popup_window)
{
   int x, y;

   ecore_x_pointer_last_xy_get(&x, &y);
   etk_popup_window_popup_at_xy(popup_window, x + 1, y + 1);
}

/**
 * @brief Pops down the popup window
 * @param popup_window a popup window
 */
void etk_popup_window_popdown(Etk_Popup_Window *popup_window)
{
   Evas_List *l, *last;

   if (!popup_window || !(l = evas_list_find_list(_etk_popup_window_popped_windows, popup_window)))
      return;

   etk_widget_hide(ETK_WIDGET(popup_window));
   _etk_popup_window_popped_windows = evas_list_remove_list(_etk_popup_window_popped_windows, l);
   last = evas_list_last(_etk_popup_window_popped_windows);
   if (last)
      etk_popup_window_focused_window_set(ETK_POPUP_WINDOW(last->data));

   if (!_etk_popup_window_popped_windows)
   {
      /* TODO: pointer ungrab, fixme!! */
      /* ecore_x_pointer_ungrab(); */
      ecore_x_keyboard_ungrab();
      ecore_x_window_del(_etk_popup_window_input_window);
      _etk_popup_window_input_window = 0;

      ecore_event_handler_del(_etk_popup_window_key_down_handler);
      ecore_event_handler_del(_etk_popup_window_key_up_handler);
      ecore_event_handler_del(_etk_popup_window_mouse_up_handler);
      ecore_event_handler_del(_etk_popup_window_mouse_move_handler);
      _etk_popup_window_key_down_handler = NULL;
      _etk_popup_window_key_up_handler = NULL;
      _etk_popup_window_mouse_up_handler = NULL;
      _etk_popup_window_mouse_move_handler = NULL;
   }

   etk_signal_emit(_etk_popup_window_signals[ETK_POPUP_WINDOW_POPPED_DOWN_SIGNAL], ETK_OBJECT(popup_window), NULL);
}


/**
 * @brief Pops down all the popped windows
 */
void etk_popup_window_popdown_all()
{
   while (_etk_popup_window_popped_windows)
      etk_popup_window_popdown(ETK_POPUP_WINDOW(_etk_popup_window_popped_windows->data));
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_popup_window_constructor(Etk_Popup_Window *popup_window)
{
   if (!popup_window)
      return;

   etk_window_decorated_set(ETK_WINDOW(popup_window), ETK_FALSE);
   etk_window_skip_taskbar_hint_set(ETK_WINDOW(popup_window), ETK_TRUE);
   etk_window_skip_pager_hint_set(ETK_WINDOW(popup_window), ETK_TRUE);
   ecore_x_netwm_window_type_set(ETK_WINDOW(popup_window)->x_window, ECORE_X_WINDOW_TYPE_MENU);
}

/**
 * @brief Gets whether the popup window is popped up
 * @param popup_window a popup window
 * @return Returns ETK_TRUE if the popup window is popped up, ETK_FALSE otherwise
 */
Etk_Bool etk_popup_window_is_popped_up(Etk_Popup_Window *popup_window)
{
   if (!popup_window || !evas_list_find(_etk_popup_window_popped_windows, popup_window))
      return ETK_FALSE;
   return ETK_TRUE;
}

/**
 * @brief Sets the focused popup window. The focused window is the one which will receive the keyboard events. @n
 * When a new popup window is popped up, the popup window is automatically focused
 * @param popup_window the popup window to focus
 * @note The poppup window should be popped up to be focused
 */
void etk_popup_window_focused_window_set(Etk_Popup_Window *popup_window)
{
   if (popup_window)
   {
      if (etk_popup_window_is_popped_up(popup_window))
         _etk_popup_window_focused_window = popup_window;
   }
   else if (!_etk_popup_window_popped_windows)
      _etk_popup_window_focused_window = NULL;
}

/**
 * @brief Gets the focused popup window
 * @return Returns the focused popup window
 */
Etk_Popup_Window *etk_popup_window_focused_window_get()
{
   return _etk_popup_window_focused_window;
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called wgen the user presses a key on the input window: we just feed it */
static int _etk_popup_window_key_down_cb(void *data, int type, void *event)
{
   Etk_Popup_Window *popup_window;
   Ecore_X_Event_Key_Down *key_event;

   if (!(popup_window = ETK_POPUP_WINDOW(data)) || !(key_event = event) || key_event->win != _etk_popup_window_input_window)
      return 1;
   if (!_etk_popup_window_focused_window)
      return 1;

   evas_event_feed_key_down(ETK_TOPLEVEL_WIDGET(_etk_popup_window_focused_window)->evas, key_event->keyname,
      key_event->keysymbol, key_event->key_compose, NULL, key_event->time, NULL);
   return 1;
}

/* Called wgen the user releases a key on the input window: we just feed it */
static int _etk_popup_window_key_up_cb(void *data, int type, void *event)
{
   Etk_Popup_Window *popup_window;
   Ecore_X_Event_Key_Up *key_event;

   if (!(popup_window = ETK_POPUP_WINDOW(data)) || !(key_event = event) || key_event->win != _etk_popup_window_input_window)
      return 1;
   if (!_etk_popup_window_focused_window)
      return 1;

   evas_event_feed_key_up(ETK_TOPLEVEL_WIDGET(_etk_popup_window_focused_window)->evas, key_event->keyname,
      key_event->keysymbol, key_event->key_compose, NULL, key_event->time, NULL);
   return 1;
}

/*
 * Called when the user clicks on the input window:
 * it pops down the opened popup windows if needed and feeds the mouse up event to the popup window
 */
static int _etk_popup_window_mouse_up_cb(void *data, int type, void *event)
{
   Etk_Popup_Window *popup_window, *pwin;
   Evas_List *l;
   Ecore_X_Event_Mouse_Button_Up *mouse_event;
   Etk_Bool pointer_over_window = ETK_FALSE;

   if (!(popup_window = ETK_POPUP_WINDOW(data)) || !(mouse_event = event) || mouse_event->win != _etk_popup_window_input_window)
      return 1;

   /* If the user clicks on a popped window, we feed the event */
   for (l = _etk_popup_window_popped_windows; l; l = l->next)
   {
      int px, py, pw, ph;

      pwin = ETK_POPUP_WINDOW(l->data);
      etk_window_geometry_get(ETK_WINDOW(pwin), &px, &py, &pw, &ph);
      if (_etk_popup_window_mouse_x >= px && _etk_popup_window_mouse_x <= px + pw && _etk_popup_window_mouse_y >= py && _etk_popup_window_mouse_y <= py + ph)
      {
         pointer_over_window = ETK_TRUE;
         evas_event_feed_mouse_up(ETK_TOPLEVEL_WIDGET(pwin)->evas, mouse_event->button, EVAS_BUTTON_NONE, mouse_event->time, NULL);
         break;
      }
   }
   /* Otherwize, we pop down all the popup windows */
   if (!pointer_over_window && (mouse_event->time - _etk_popup_window_popup_timestamp) >= ETK_POPUP_WINDOW_MIN_POP_TIME)
      etk_popup_window_popdown_all();

   return 1;
}

/*
 * Called when the user moves the mouse above the popup input window:
 * It feeds the mouse move, in and out events to the popup windows and starts to make the popup windows slide if needed
 */
static int _etk_popup_window_mouse_move_cb(void *data, int type, void *event)
{
   Etk_Popup_Window *popup_window, *pwin;
   Evas_List *l;
   Ecore_X_Event_Mouse_Move *mouse_event;
   int px, py;

   if (!(popup_window = ETK_POPUP_WINDOW(data)) || !(mouse_event = event) || mouse_event->win != _etk_popup_window_input_window)
      return 1;

   _etk_popup_window_mouse_x = mouse_event->x;
   _etk_popup_window_mouse_y = mouse_event->y;

   for (l = _etk_popup_window_popped_windows; l; l = l->next)
   {
      pwin = ETK_POPUP_WINDOW(l->data);
      etk_window_geometry_get(ETK_WINDOW(pwin), &px, &py, NULL, NULL);
      evas_event_feed_mouse_move(ETK_TOPLEVEL_WIDGET(pwin)->evas, mouse_event->x - px, mouse_event->y - py, mouse_event->time, NULL);

      /* Start to make the popup window slide if needed */
      _etk_popup_window_slide_timer_update(pwin);
   }

   return 1;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Returns a flag incating on which edges of the screen the popup window is over */
static Etk_Popup_Window_Screen_Edge _etk_popup_window_over_screen_edge_get(Etk_Popup_Window *popup_window)
{
   int rx, ry, rw, rh;
   int px, py, pw, ph;
   Etk_Popup_Window_Screen_Edge result = ETK_POPUP_WINDOW_NO_EDGE;

   if (!popup_window || _etk_popup_window_input_window == 0)
      return ETK_POPUP_WINDOW_NO_EDGE;

   ecore_x_window_geometry_get(_etk_popup_window_input_window, &rx, &ry, &rw, &rh);
   etk_window_geometry_get(ETK_WINDOW(popup_window), &px, &py, &pw, &ph);

   if (px < rx)
      result |= ETK_POPUP_WINDOW_LEFT_EDGE;
   if (px + pw > rx + rw)
      result |= ETK_POPUP_WINDOW_RIGHT_EDGE;
   if (py < ry)
      result |= ETK_POPUP_WINDOW_TOP_EDGE;
   if (py + ph > ry + rh)
      result |= ETK_POPUP_WINDOW_BOTTOM_EDGE;

   return result;
}

/* Returns a flag incating on which edges of the screen the mouse pointer is */
static Etk_Popup_Window_Screen_Edge _etk_popup_window_mouse_on_screen_edge_get()
{
   int rx, ry, rw, rh;
   Etk_Popup_Window_Screen_Edge result = ETK_POPUP_WINDOW_NO_EDGE;

   if (_etk_popup_window_input_window == 0)
      return ETK_POPUP_WINDOW_NO_EDGE;

   ecore_x_window_geometry_get(_etk_popup_window_input_window, &rx, &ry, &rw, &rh);
   if (_etk_popup_window_mouse_x - rx + 1 >= rw)
      result |= ETK_POPUP_WINDOW_RIGHT_EDGE;
   if (_etk_popup_window_mouse_x <= rx)
      result |= ETK_POPUP_WINDOW_LEFT_EDGE;
   if (_etk_popup_window_mouse_y - ry + 1 >= rh)
      result |= ETK_POPUP_WINDOW_BOTTOM_EDGE;
   if (_etk_popup_window_mouse_y <= ry)
      result |= ETK_POPUP_WINDOW_TOP_EDGE;

   return result;
}

/* Starts the slide timer if needed */
static void _etk_popup_window_slide_timer_update(Etk_Popup_Window *popup_window)
{
   Etk_Popup_Window_Screen_Edge mouse_on_edge, window_over_edge;

   if (!popup_window)
      return;

   mouse_on_edge = _etk_popup_window_mouse_on_screen_edge_get();
   window_over_edge = _etk_popup_window_over_screen_edge_get(popup_window);
   if (!_etk_popup_window_slide_timer && (mouse_on_edge & window_over_edge) != ETK_POPUP_WINDOW_NO_EDGE)
      _etk_popup_window_slide_timer = ecore_timer_add(1.0 / 60.0, _etk_popup_window_slide_timer_cb, NULL);
}

/* Makes the popup windows slide (called every 1/60 sec) */
static int _etk_popup_window_slide_timer_cb(void *data)
{
   Etk_Popup_Window *popup_window = NULL, *pwin;
   Evas_List *l;
   Etk_Popup_Window_Screen_Edge mouse_edge, window_edge;
   int rx, ry, rw, rh;
   int px, py, pw, ph;
   int x, y;
   int dx = 0, dy = 0, max_delta = (int)(1.0 / 60.0 * 800);

   /* We first look for the popup window that is over an edge of the screen */
   for (l = _etk_popup_window_popped_windows; l; l = l->next)
   {
      pwin = ETK_POPUP_WINDOW(l->data);
      if ((window_edge = _etk_popup_window_over_screen_edge_get(pwin)) != ETK_POPUP_WINDOW_NO_EDGE)
      {
         popup_window = pwin;
         break;
      }
   }
   if (!popup_window)
   {
      _etk_popup_window_slide_timer = NULL;
      return 0;
   }

   /* Then we move all the popup windows in the right direction */
   mouse_edge = _etk_popup_window_mouse_on_screen_edge_get();
   ecore_x_window_geometry_get(_etk_popup_window_input_window, &rx, &ry, &rw, &rh);
   etk_window_geometry_get(ETK_WINDOW(popup_window), &px, &py, &pw, &ph);
   if (mouse_edge & window_edge & ETK_POPUP_WINDOW_LEFT_EDGE)
   {
      if (max_delta < rx - px)
         dx = max_delta;
      else
         dx = rx - px;
   }
   if (mouse_edge & window_edge & ETK_POPUP_WINDOW_RIGHT_EDGE)
   {
      if (max_delta < pw - rx - rw + px)
         dx = -max_delta;
      else
         dx = -pw + rx + rw - px;
   }
   if (mouse_edge & window_edge & ETK_POPUP_WINDOW_TOP_EDGE)
   {
      if (max_delta < ry - py)
         dy = max_delta;
      else
         dy = ry - py;
   }
   if (mouse_edge & window_edge & ETK_POPUP_WINDOW_BOTTOM_EDGE)
   {
      if (max_delta < ph - ry - rh + py)
         dy = -max_delta;
      else
         dy = -ph + ry + rh - py;
   }

   if (dx == 0 && dy == 0)
   {
      _etk_popup_window_slide_timer = NULL;
      return 0;
   }

   for (l = _etk_popup_window_popped_windows; l; l = l->next)
   {
      pwin = ETK_POPUP_WINDOW(l->data);
      etk_window_geometry_get(ETK_WINDOW(pwin), &x, &y, NULL, NULL);
      etk_window_move(ETK_WINDOW(pwin), x + dx, y + dy);

      /* We feed a mouse move event since the relative position between the mouse pointer and the popup window has changed */
      evas_event_feed_mouse_move(ETK_TOPLEVEL_WIDGET(pwin)->evas, _etk_popup_window_mouse_x - x, _etk_popup_window_mouse_y - y, ecore_x_current_time_get(), NULL);
   }

   return 1;
}

/** @} */
