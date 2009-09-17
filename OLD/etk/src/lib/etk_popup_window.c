/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

/** @file etk_popup_window.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_popup_window.h"
#include <stdlib.h>
#include "etk_engine.h"
#include "etk_event.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Popup_Window
 * @{
 */

#define ETK_POPUP_WINDOW_MIN_POP_TIME    400
#define ETK_POPUP_WINDOW_SLIDE_RATE      (1.0 / 60.0)

typedef enum Etk_Popup_Window_Screen_Edge
{
   ETK_POPUP_WINDOW_NO_EDGE = 0,
   ETK_POPUP_WINDOW_LEFT_EDGE = (1 << 0),
   ETK_POPUP_WINDOW_RIGHT_EDGE = (1 << 1),
   ETK_POPUP_WINDOW_BOTTOM_EDGE = (1 << 2),
   ETK_POPUP_WINDOW_TOP_EDGE = (1 << 3)
} Etk_Popup_Window_Screen_Edge;

int ETK_POPUP_WINDOW_POPPED_DOWN_SIGNAL;
int ETK_POPUP_WINDOW_POPPED_UP_SIGNAL;

static void _etk_popup_window_constructor(Etk_Popup_Window *popup_window);

static void _etk_popup_window_key_down_cb(Etk_Event_Global event_info, void *data);
static void _etk_popup_window_key_up_cb(Etk_Event_Global event_info, void *data);
static void _etk_popup_window_mouse_move_cb(Etk_Event_Global event_info, void *data);
static void _etk_popup_window_mouse_up_cb(Etk_Event_Global event_info, void *data);
static int _etk_popup_window_slide_timer_cb(void *data);

static void _etk_popup_window_slide_timer_update(Etk_Popup_Window *popup_window);
static Etk_Popup_Window_Screen_Edge _etk_popup_window_edge_get(Etk_Popup_Window *popup_window);
static Etk_Popup_Window_Screen_Edge _etk_popup_window_mouse_edge_get(void);

static unsigned int _etk_popup_window_popup_timestamp = 0;
static Ecore_Timer *_etk_popup_window_slide_timer = NULL;
static Eina_List *_etk_popup_window_popped_parents = NULL;
static Etk_Popup_Window *_etk_popup_window_focused_window = NULL;


/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Popup_Window
 * @return Returns the type of an Etk_Popup_Window
 */
Etk_Type *etk_popup_window_type_get(void)
{
   static Etk_Type *popup_window_type = NULL;

   if (!popup_window_type)
   {
      const Etk_Signal_Description signals[] = {
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_POPUP_WINDOW_POPPED_UP_SIGNAL,
            "popped-up", etk_marshaller_VOID),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_POPUP_WINDOW_POPPED_DOWN_SIGNAL,
            "popped-down", etk_marshaller_VOID),
         ETK_SIGNAL_DESCRIPTION_SENTINEL
      };

      popup_window_type = etk_type_new("Etk_Popup_Window", ETK_WINDOW_TYPE,
         sizeof(Etk_Popup_Window),
         ETK_CONSTRUCTOR(_etk_popup_window_constructor), NULL, signals);
   }

   return popup_window_type;
}

/**
 * @brief Sets the parent of the popup window.
 * It is used by popup windows that can make other windows pop up, such as the menus
 * @param popup_window a popup_window
 * @param parent the parent of the popup_window
 */
void etk_popup_window_parent_set(Etk_Popup_Window *popup_window, Etk_Popup_Window *parent)
{
   if (!popup_window)
      return;

   if (popup_window->parent)
   {
      popup_window->parent->children = eina_list_remove(popup_window->parent->children, popup_window);
      if (popup_window->parent->popped_child)
         popup_window->parent->popped_child = NULL;
      popup_window->parent = NULL;
      if (popup_window->popped_up)
         _etk_popup_window_popped_parents = eina_list_append(_etk_popup_window_popped_parents, popup_window);
   }

   if (parent)
   {
      if (popup_window->popped_up)
      {
         _etk_popup_window_popped_parents = eina_list_remove(_etk_popup_window_popped_parents, popup_window);
         if (parent->popped_child)
            etk_popup_window_popdown(popup_window);
         else
            parent->popped_child = popup_window;
      }
      parent->children = eina_list_append(parent->children, popup_window);
      popup_window->parent = parent;
   }
}

/**
 * @brief Gets the parent of the popup window
 * @param popup_window a popup_window
 * @return Returns the parent of the popup window
 */
Etk_Popup_Window *etk_popup_window_parent_get(Etk_Popup_Window *popup_window)
{
   if (!popup_window)
      return NULL;
   return popup_window->parent;
}

/**
 * @brief Sets the focused popup window. The focused window is the one which will receive the keyboard events.
 * When a popup window is popped up, it is automatically focused
 * @param popup_window the popup window to focus. If NULL, the last popped window will be automatically focused
 * @note The popup window should be popped up to be focused
 */
void etk_popup_window_focused_window_set(Etk_Popup_Window *popup_window)
{
   Etk_Popup_Window *pop;
   Eina_List *l;

   if (popup_window && popup_window->popped_up)
      _etk_popup_window_focused_window = popup_window;
   else
   {
      pop = ETK_POPUP_WINDOW(eina_list_data_get(eina_list_last(_etk_popup_window_popped_parents)));
      while (pop && pop->popped_child)
         pop = pop->popped_child;
      _etk_popup_window_focused_window = pop;
   }

   /* Raise the new focused window - if its a normal window, dont */
   if (popup_window && !ETK_IS_POPUP_WINDOW(popup_window))
     return;

   for (pop = popup_window; pop; pop = pop->parent)
   {
      if ((l = eina_list_data_find_list(_etk_popup_window_popped_parents, pop)))
      {
         _etk_popup_window_popped_parents = eina_list_remove_list(_etk_popup_window_popped_parents, l);
         _etk_popup_window_popped_parents = eina_list_append(_etk_popup_window_popped_parents, pop);

         for ( ; pop; pop = pop->popped_child)
            etk_window_raise(ETK_WINDOW(pop));

         break;
      }
   }
}

/**
 * @brief Gets the focused popup window
 * @return Returns the focused popup window
 */
Etk_Popup_Window *etk_popup_window_focused_window_get(void)
{
   return _etk_popup_window_focused_window;
}

/**
 * @brief Pops up the popup window at the mouse pointer position
 * @param popup_window a popup window
 * @note This is equivalent to etk_popup_window_popup_in_direction(popup_window, ETK_POPUP_BELOW_RIGHT)
 */
void etk_popup_window_popup(Etk_Popup_Window *popup_window)
{
   etk_popup_window_popup_in_direction(popup_window, ETK_POPUP_BELOW_RIGHT);
}

/**
 * @brief Pops up the popup window at the mouse pointer position, in the given direction
 * @param popup_window a popup window
 * @param direction the direction to which the window should be popped up
 */
void etk_popup_window_popup_in_direction(Etk_Popup_Window *popup_window, Etk_Popup_Direction direction)
{
   int x, y;

   etk_engine_mouse_position_get(&x, &y);
   etk_popup_window_popup_at_xy_in_direction(popup_window, x + 2, y + 2, direction);
}

/**
 * @brief Pops up the popup window at the position (x, y). If the parent of the popup window has already a child which
 * is popped up, the child will be automatically popped down
 * @param popup_window a popup window
 * @param x the x position where to pop up the popup window
 * @param y the y position where to pop up the popup window
 * @note This is equivalent to etk_popup_window_popup_at_xy_in_direction(popup_window, x, y, ETK_POPUP_BELOW_RIGHT)
 */
void etk_popup_window_popup_at_xy(Etk_Popup_Window *popup_window, int x, int y)
{
   Etk_Size size;

   if (!popup_window)
      return;

   if (!popup_window->popped_up)
   {
      etk_engine_popup_window_popup(popup_window);

      evas_event_feed_mouse_move(ETK_TOPLEVEL(popup_window)->evas, -100000, -100000, 0, NULL);
      evas_event_feed_mouse_in(ETK_TOPLEVEL(popup_window)->evas, 0, NULL);

      if (!_etk_popup_window_popped_parents)
      {
         etk_event_global_callback_add(ETK_EVENT_KEY_DOWN, _etk_popup_window_key_down_cb, NULL);
         etk_event_global_callback_add(ETK_EVENT_KEY_UP, _etk_popup_window_key_up_cb, NULL);
         etk_event_global_callback_add(ETK_EVENT_MOUSE_MOVE, _etk_popup_window_mouse_move_cb, NULL);
         etk_event_global_callback_add(ETK_EVENT_MOUSE_UP, _etk_popup_window_mouse_up_cb, NULL);
      }

      if (!popup_window->parent || !popup_window->parent->popped_up)
      {
         _etk_popup_window_popped_parents = eina_list_append(_etk_popup_window_popped_parents, popup_window);
         _etk_popup_window_popup_timestamp = etk_engine_event_timestamp_get();
      }
      if (popup_window->parent)
      {
         if (popup_window->parent->popped_child)
            etk_popup_window_popdown(popup_window->parent->popped_child);
         popup_window->parent->popped_child = popup_window;
      }
      popup_window->popped_up = ETK_TRUE;
   }

   etk_popup_window_focused_window_set(popup_window);

   etk_widget_size_request_full(ETK_WIDGET(popup_window), &size, ETK_FALSE);
   etk_window_move(ETK_WINDOW(popup_window), x, y);
   etk_window_resize(ETK_WINDOW(popup_window), size.w, size.h);
   etk_widget_show(ETK_WIDGET(popup_window));

   _etk_popup_window_slide_timer_update(popup_window);
   etk_signal_emit(ETK_POPUP_WINDOW_POPPED_UP_SIGNAL, ETK_OBJECT(popup_window));
}

/**
 * @brief Pops up the popup window at the position (x, y). If the parent of the popup window has already a child which
 * is popped up, the child will be automatically popped down
 * @param popup_window a popup window
 * @param x the x position where to pop up the popup window
 * @param y the y position where to pop up the popup window
 * @param direction the direction to which the window should be popped up
 * @note This is equivalent to etk_popup_window_popup_at_xy_in_direction(popup_window, x, y, ETK_POPUP_BELOW_RIGHT)
 */
void etk_popup_window_popup_at_xy_in_direction(Etk_Popup_Window *popup_window, int x, int y, Etk_Popup_Direction direction)
{
   Etk_Size size;

   if (!popup_window)
      return;

   etk_widget_size_request_full(ETK_WIDGET(popup_window), &size, ETK_FALSE);
   switch (direction)
   {
      case ETK_POPUP_BELOW_LEFT:
         x -= size.w;
         break;
      case ETK_POPUP_ABOVE_RIGHT:
         y -= size.h;
         break;
      case ETK_POPUP_ABOVE_LEFT:
         x -= size.w;
         y -= size.h;
         break;
      default:
         break;
   }
   etk_popup_window_popup_at_xy(popup_window, x, y);
}


/**
 * @brief Pops down the popup window and its children
 * @param popup_window a popup window
 */
void etk_popup_window_popdown(Etk_Popup_Window *popup_window)
{
   if (!popup_window || !popup_window->popped_up)
      return;

   if (popup_window->popped_child)
      etk_popup_window_popdown(popup_window->popped_child);

   etk_engine_popup_window_popdown(popup_window);
   popup_window->popped_up = ETK_FALSE;
   if (popup_window->parent && popup_window->parent->popped_child == popup_window)
      popup_window->parent->popped_child = NULL;

   _etk_popup_window_popped_parents = eina_list_remove(_etk_popup_window_popped_parents, popup_window);
   if (_etk_popup_window_focused_window == popup_window)
      etk_popup_window_focused_window_set(popup_window->parent);

   if (!_etk_popup_window_popped_parents)
   {
      etk_event_global_callback_del(ETK_EVENT_KEY_DOWN, _etk_popup_window_key_down_cb);
      etk_event_global_callback_del(ETK_EVENT_KEY_UP, _etk_popup_window_key_up_cb);
      etk_event_global_callback_del(ETK_EVENT_MOUSE_MOVE, _etk_popup_window_mouse_move_cb);
      etk_event_global_callback_del(ETK_EVENT_MOUSE_UP, _etk_popup_window_mouse_up_cb);
   }

   etk_widget_hide(ETK_WIDGET(popup_window));
   etk_signal_emit(ETK_POPUP_WINDOW_POPPED_DOWN_SIGNAL, ETK_OBJECT(popup_window));
}

/**
 * @brief Pops down all the popped windows
 */
void etk_popup_window_popdown_all(void)
{
   while (_etk_popup_window_popped_parents)
      etk_popup_window_popdown(ETK_POPUP_WINDOW(_etk_popup_window_popped_parents->data));
}

/**
 * @brief Gets whether the popup window is popped up
 * @param popup_window a popup window
 * @return Returns ETK_TRUE if the popup window is popped up, ETK_FALSE otherwise
 */
Etk_Bool etk_popup_window_is_popped_up(Etk_Popup_Window *popup_window)
{
   if (!popup_window)
      return ETK_FALSE;
   return popup_window->popped_up;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the popup window */
static void _etk_popup_window_constructor(Etk_Popup_Window *popup_window)
{
   if (!popup_window)
      return;

   popup_window->popped_up = ETK_FALSE;
   popup_window->parent = NULL;
   popup_window->children = NULL;
   popup_window->popped_child = NULL;
   etk_window_stacking_set(ETK_WINDOW(popup_window), ETK_WINDOW_ABOVE);
   etk_engine_popup_window_constructor(popup_window);
}

/**************************
 *
 * Handlers and callbacks
 *
 **************************/

/* Called when a key is pressed, if a popup window is popped up */
static void _etk_popup_window_key_down_cb(Etk_Event_Global event_info, void *data)
{
   if (!_etk_popup_window_focused_window)
      return;

   evas_event_feed_key_down(ETK_TOPLEVEL(_etk_popup_window_focused_window)->evas, event_info.key_down.keyname,
      event_info.key_down.key, event_info.key_down.string, NULL, event_info.key_down.timestamp, NULL);
}


/* Called when a key is released, if a popup window is popped up */
static void _etk_popup_window_key_up_cb(Etk_Event_Global event_info, void *data)
{
   if (!_etk_popup_window_focused_window)
      return;

   evas_event_feed_key_up(ETK_TOPLEVEL(_etk_popup_window_focused_window)->evas, event_info.key_up.keyname,
      event_info.key_up.key, event_info.key_up.string, NULL, event_info.key_up.timestamp, NULL);
}

/* Called when the mouse is moved over the screen:
 * It feeds the mouse move event to the popup windows and starts to make the popup windows slide if needed */
static void _etk_popup_window_mouse_move_cb(Etk_Event_Global event_info, void *data)
{
   Etk_Popup_Window *pop;
   int px, py;

   pop = ETK_POPUP_WINDOW(eina_list_data_get(eina_list_last(_etk_popup_window_popped_parents)));
   for ( ; pop; pop = pop->popped_child)
   {
      etk_window_geometry_get(ETK_WINDOW(pop), &px, &py, NULL, NULL);
      evas_event_feed_mouse_move(ETK_TOPLEVEL(pop)->evas, event_info.mouse_move.pos.x - px,
         event_info.mouse_move.pos.y - py, event_info.mouse_move.timestamp, NULL);

      /* Start to make the popup window slide if needed */
      _etk_popup_window_slide_timer_update(pop);
   }
}

/* Called when the user releases a mouse button:
 * it pops down the opened popup windows if needed and feeds the mouse up event to the popup window */
static void _etk_popup_window_mouse_up_cb(Etk_Event_Global event_info, void *data)
{
   Etk_Popup_Window *pop;
   Etk_Bool pointer_over_window = ETK_FALSE;

   /* If the user clicks on a popped window, we feed the event */
   pop = ETK_POPUP_WINDOW(eina_list_data_get(eina_list_last(_etk_popup_window_popped_parents)));
   for ( ; pop; pop = pop->popped_child)
   {
      int px, py, pw, ph;

      etk_window_geometry_get(ETK_WINDOW(pop), &px, &py, &pw, &ph);
      if (ETK_INSIDE(event_info.mouse_up.pos.x, event_info.mouse_up.pos.y, px, py, pw, ph))
      {
	 pointer_over_window = ETK_TRUE;
	 evas_event_feed_mouse_up(ETK_TOPLEVEL(pop)->evas, event_info.mouse_up.button,
            EVAS_BUTTON_NONE, event_info.mouse_up.timestamp, NULL);
	 break;
      }
   }

   /* Otherwise, we pop down the popup windows */
   if (!pointer_over_window && event_info.mouse_up.timestamp >= _etk_popup_window_popup_timestamp
      && (event_info.mouse_up.timestamp - _etk_popup_window_popup_timestamp) >= ETK_POPUP_WINDOW_MIN_POP_TIME)
   {
      pop = ETK_POPUP_WINDOW(eina_list_data_get(eina_list_last(_etk_popup_window_popped_parents)));
      etk_popup_window_popdown(pop);
   }
}

/* Makes the popup windows slide (called every 1/60 sec) */
static int _etk_popup_window_slide_timer_cb(void *data)
{
   Etk_Popup_Window *popup_window = NULL, *pwin;
   Etk_Popup_Window_Screen_Edge mouse_edge, window_edge;
   int sx, sy, sw, sh;
   int px, py, pw, ph;
   int x, y;
   int dx = 0, dy = 0, max_delta = (int)(ETK_POPUP_WINDOW_SLIDE_RATE * 800);

   /* We first look for the popup window that is over an edge of the screen */
   pwin = ETK_POPUP_WINDOW(eina_list_data_get(eina_list_last(_etk_popup_window_popped_parents)));
   for ( ; pwin; pwin = pwin->popped_child)
   {
      if ((window_edge = _etk_popup_window_edge_get(pwin)) != ETK_POPUP_WINDOW_NO_EDGE)
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
   mouse_edge = _etk_popup_window_mouse_edge_get();
   etk_engine_window_screen_geometry_get(ETK_WINDOW(popup_window), &sx, &sy, &sw, &sh);
   etk_window_geometry_get(ETK_WINDOW(popup_window), &px, &py, &pw, &ph);
   if (mouse_edge & window_edge & ETK_POPUP_WINDOW_LEFT_EDGE)
   {
      if (max_delta < sx - px)
         dx = max_delta;
      else
         dx = sx - px;
   }
   if (mouse_edge & window_edge & ETK_POPUP_WINDOW_RIGHT_EDGE)
   {
      if (max_delta < pw - sx - sw + px)
         dx = -max_delta;
      else
         dx = -pw + sx + sw - px;
   }
   if (mouse_edge & window_edge & ETK_POPUP_WINDOW_TOP_EDGE)
   {
      if (max_delta < sy - py)
         dy = max_delta;
      else
         dy = sy - py;
   }
   if (mouse_edge & window_edge & ETK_POPUP_WINDOW_BOTTOM_EDGE)
   {
      if (max_delta < ph - sy - sh + py)
         dy = -max_delta;
      else
         dy = -ph + sy + sh - py;
   }

   if (dx == 0 && dy == 0)
   {
      _etk_popup_window_slide_timer = NULL;
      return 0;
   }

   pwin = ETK_POPUP_WINDOW(eina_list_data_get(eina_list_last(_etk_popup_window_popped_parents)));
   for ( ; pwin; pwin = pwin->popped_child)
   {
      int mx, my;

      etk_window_geometry_get(ETK_WINDOW(pwin), &x, &y, NULL, NULL);
      etk_window_move(ETK_WINDOW(pwin), x + dx, y + dy);

      /* We feed a mouse-move event since the relative position between the mouse pointer
       * and the popup window has changed */
      etk_engine_mouse_position_get(&mx, &my);
      evas_event_feed_mouse_move(ETK_TOPLEVEL(pwin)->evas, mx - x, my - y, etk_engine_event_timestamp_get(), NULL);
   }

   return 1;
}

/**************************
 *
 * Private function
 *
 **************************/

/* Starts the slide timer if needed */
static void _etk_popup_window_slide_timer_update(Etk_Popup_Window *popup_window)
{
   Etk_Popup_Window_Screen_Edge mouse_on_edge, window_over_edge;

   if (!popup_window)
      return;

   mouse_on_edge = _etk_popup_window_mouse_edge_get();
   window_over_edge = _etk_popup_window_edge_get(popup_window);
   if (!_etk_popup_window_slide_timer && (mouse_on_edge & window_over_edge) != ETK_POPUP_WINDOW_NO_EDGE)
      _etk_popup_window_slide_timer = ecore_timer_add(ETK_POPUP_WINDOW_SLIDE_RATE, _etk_popup_window_slide_timer_cb, NULL);
}

/* Returns a flag incating on which edges of the screen the popup window is */
static Etk_Popup_Window_Screen_Edge _etk_popup_window_edge_get(Etk_Popup_Window *popup_window)
{
   Etk_Popup_Window_Screen_Edge result = ETK_POPUP_WINDOW_NO_EDGE;
   int sx, sy, sw, sh;
   int px, py, pw, ph;

   if (!popup_window)
      return ETK_POPUP_WINDOW_NO_EDGE;

   etk_engine_window_screen_geometry_get(ETK_WINDOW(popup_window), &sx, &sy, &sw, &sh);
   etk_window_geometry_get(ETK_WINDOW(popup_window), &px, &py, &pw, &ph);

   if (px < sx)
      result |= ETK_POPUP_WINDOW_LEFT_EDGE;
   if (px + pw > sx + sw)
      result |= ETK_POPUP_WINDOW_RIGHT_EDGE;
   if (py < sy)
      result |= ETK_POPUP_WINDOW_TOP_EDGE;
   if (py + ph > sy + sh)
      result |= ETK_POPUP_WINDOW_BOTTOM_EDGE;

   return result;
}

/* Returns a flag incating on which edges of the screen the mouse pointer is */
/* TODO: add margin */
static Etk_Popup_Window_Screen_Edge _etk_popup_window_mouse_edge_get(void)
{
   Etk_Popup_Window_Screen_Edge result = ETK_POPUP_WINDOW_NO_EDGE;
   int sx, sy, sw, sh;
   int mx, my;

   etk_engine_mouse_screen_geometry_get(&sx, &sy, &sw, &sh);
   etk_engine_mouse_position_get(&mx, &my);
   if (mx - sx + 1 >= sw)
      result |= ETK_POPUP_WINDOW_RIGHT_EDGE;
   if (mx <= sx)
      result |= ETK_POPUP_WINDOW_LEFT_EDGE;
   if (my - sy + 1 >= sh)
      result |= ETK_POPUP_WINDOW_BOTTOM_EDGE;
   if (my <= sy)
      result |= ETK_POPUP_WINDOW_TOP_EDGE;

   return result;
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Popup_Window
 *
 * When a popup window is popped up, it grabs the keyboard and the mouse input so the user won't be able to manipulate
 * the other windows. To pop down the window, the user has to click outside of it. @n
 * If the popup window intersects one of the edges of the screen, the popup window will slide smoothly when the mouse
 * pointer reaches this edge. @n
 * You usually do not need to directly create a popup window in your programs, use Etk_Menu or Etk_Combobox instead.
 * However, Etk_Popup_Window can be useful if you are creating a new widget.
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Bin
 *         - Etk_Toplevel
 *           - Etk_Window
 *             - Etk_Popup_Window
 *
 * \par Signals:
 * @signal_name "popped-up": Emitted when the popup window is popped up
 * @signal_cb Etk_Bool callback(Etk_Popup_Window *popup_window, void *data)
 * @signal_arg popup_window: the popup window which has been popped up
 * @signal_data
 * \par
 * @signal_name "popped-down": Emitted when the popup window is popped down
 * @signal_cb Etk_Bool callback(Etk_Popup_Window *popup_window, void *data)
 * @signal_arg popup_window: the popup window which has been popped down
 * @signal_data
 */
