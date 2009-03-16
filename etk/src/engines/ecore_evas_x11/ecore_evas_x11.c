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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Evas.h>
#include <Ecore_Evas.h>
#include <Ecore_Input.h>
#include <Ecore_X.h>
#include <Ecore_X_Cursor.h>
#include <Ecore_X_Atoms.h>
#include <Etk.h>
#include "Etk_Engine_Ecore_Evas.h"
#include "Etk_Engine_Ecore_Evas_X11.h"

/* TODO: window prop notify ? */

#define NUM_INPUT_HANDLERS 6

typedef Etk_Engine_Ecore_Evas_X11_Window_Data Etk_Engine_Window_Data;

/* General engine functions */
Etk_Engine *engine_open(void);
void engine_close(void);

static Etk_Bool _engine_init(void);
static void _engine_shutdown(void);

/* Etk_Window functions */
static void _window_constructor(Etk_Window *window);
static void _window_destructor(Etk_Window *window);
static void _window_screen_geometry_get(Etk_Window *window, int *x, int *y, int *w, int *h);
static void _window_modal_for_window(Etk_Window *window_to_modal, Etk_Window *window);
static void _window_stacking_set(Etk_Window *window, Etk_Window_Stacking stacking);
static Etk_Window_Stacking _window_stacking_get(Etk_Window *window);
static void _window_skip_taskbar_hint_set(Etk_Window *window, Etk_Bool skip_taskbar_hint);
static Etk_Bool _window_skip_taskbar_hint_get(Etk_Window *window);
static void _window_skip_pager_hint_set(Etk_Window *window, Etk_Bool skip_pager_hint);
static Etk_Bool _window_skip_pager_hint_get(Etk_Window *window);
static void _window_focusable_set(Etk_Window *window, Etk_Bool focusable);
static Etk_Bool _window_focusable_get(Etk_Window *window);
static void _window_pointer_set(Etk_Window *window, Etk_Pointer_Type pointer_type);
  
/* Etk_Popup_Window functions  */
static void _popup_window_constructor(Etk_Popup_Window *popup_window);
static void _popup_window_popup(Etk_Popup_Window *popup_window);
static void _popup_window_popdown(Etk_Popup_Window *popup_window);

/* Event functions */
static void _event_callback_set(void (*callback)(Etk_Event_Type event, Etk_Event_Global event_info));
static unsigned int _event_timestamp_get(void);
static int _event_input_handler_cb(void *data, int type, void *event);

/* Mouse functions */
static void _mouse_position_get(int *x, int *y);
static void _mouse_screen_geometry_get(int *x, int *y, int *w, int *h);

/* Etk_Selection functions */
static void _selection_text_set(Etk_Selection_Type selection, const char *text);
static void _selection_text_request(Etk_Selection_Type selection, Etk_Widget *target);
static void _selection_clear(Etk_Selection_Type selection);
static int _selection_notify_handler_cb(void *data, int type, void *event);

/* Private functions */
static void _window_netwm_state_active_set(Etk_Window *window, Ecore_X_Window_State state, Etk_Bool active);
static Etk_Bool _window_netwm_state_active_get(Etk_Window *window, Ecore_X_Window_State state);
static void _event_global_modifiers_locks_wrap(int xmodifiers, Etk_Modifiers *modifiers, Etk_Locks *locks);


/* Private vars */
static Eina_List *_popup_window_popped_windows = NULL;
static Ecore_X_Window _popup_window_input_window = 0;

static void (*_event_callback)(Etk_Event_Type event, Etk_Event_Global event_info) = NULL;
static Ecore_Event_Handler *_event_input_handlers[NUM_INPUT_HANDLERS];

static Etk_Widget *_selection_widget = NULL;
static Ecore_Event_Handler *_selection_notify_handler = NULL;


static Etk_Engine engine_info = {
   
   NULL, /* engine specific data */
   NULL, /* engine name */
   NULL, /* super (parent) engine */
   NULL, /* DL handle */
   
   _engine_init,
   _engine_shutdown,
   
   _window_constructor,
   _window_destructor,
   NULL, /* window_show */
   NULL, /* window_hide */
   NULL, /* window_evas_get */
   NULL, /* window_title_set */
   NULL, /* window_title_get */
   NULL, /* window_wmclass_set */
   NULL, /* window_move */
   NULL, /* window_resize */
   NULL, /* window_size_min_get */
   NULL, /* window_evas_position_get */
   NULL, /* window_screen_position_get */
   NULL, /* window_size_get */
   _window_screen_geometry_get,
   _window_modal_for_window,
   NULL, /* window_iconified_set */
   NULL, /* window_iconified_get */
   NULL, /* window_maximized_set */
   NULL, /* window_maximized_get */
   NULL, /* window_fullscreen_set */
   NULL, /* window_fullscreen_get */
   NULL, /* window_raise */
   NULL, /* window_lower */
   _window_stacking_set,
   _window_stacking_get,
   NULL, /* window_sticky_set */
   NULL, /* window_sticky_get */
   NULL, /* window_focused_set */
   NULL, /* window_focused_get */
   NULL, /* window_decorated_set */
   NULL, /* window_decorated_get */
   NULL, /* window_shaped_set */
   NULL, /* window_shaped_get */
   NULL, /* window_has_alpha_set */
   NULL, /* window_has_alpha_get */     
   _window_skip_taskbar_hint_set,
   _window_skip_taskbar_hint_get,
   _window_skip_pager_hint_set,
   _window_skip_pager_hint_get,
   _window_focusable_set,
   _window_focusable_get,
   _window_pointer_set,

   _popup_window_constructor,
   _popup_window_popup,
   _popup_window_popdown,
   
   _event_callback_set,
   _event_timestamp_get,
   
   _mouse_position_get,
   _mouse_screen_geometry_get,
   
   _selection_text_set,
   _selection_text_request,
   _selection_clear,
   
   NULL, /* drag_constructor */
   NULL, /* drag_begin */
};

/**************************
 *
 * Engine general functions
 *
 **************************/

/* Called when the engine is loaded */
Etk_Engine *engine_open(void)
{
   engine_info.engine_data = NULL;
   engine_info.engine_name = strdup("ecore_evas_x11");
   etk_engine_inherit_from(&engine_info, "ecore_evas");
   return &engine_info;
}

/* Called when the engine is unloaded */
void engine_close(void)
{
   free(engine_info.engine_name);
}

/* Initializes the engine */
static Etk_Bool _engine_init(void)
{
   if (!ecore_x_init(NULL))
   {
      ETK_WARNING("Ecore_X initialization failed!");
      return ETK_FALSE;
   }
   
   _event_input_handlers[0] = ecore_event_handler_add(ECORE_EVENT_KEY_DOWN, _event_input_handler_cb, NULL);
   _event_input_handlers[1] = ecore_event_handler_add(ECORE_EVENT_KEY_UP, _event_input_handler_cb, NULL);
   _event_input_handlers[2] = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_DOWN, _event_input_handler_cb, NULL);
   _event_input_handlers[3] = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_UP, _event_input_handler_cb, NULL);
   _event_input_handlers[4] = ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE, _event_input_handler_cb, NULL);
   _event_input_handlers[5] = ecore_event_handler_add(ECORE_EVENT_MOUSE_WHEEL, _event_input_handler_cb, NULL);
   
   _selection_notify_handler = ecore_event_handler_add(ECORE_X_EVENT_SELECTION_NOTIFY, _selection_notify_handler_cb, NULL);
   
   return ETK_TRUE;
}

/* Shutdowns the engine */
static void _engine_shutdown(void)
{
   int i;
   
   for (i = 0; i < NUM_INPUT_HANDLERS; i++)
   {
      if (_event_input_handlers[i])
      {
         ecore_event_handler_del(_event_input_handlers[i]);
         _event_input_handlers[i] = NULL;
      }
   }
   
   ecore_event_handler_del(_selection_notify_handler);
   _selection_notify_handler = NULL;
   
   ecore_x_shutdown();
}

/**************************
 *
 * Etk_Window's functions
 *
 **************************/

/* Initializes the created window */
static void _window_constructor(Etk_Window *window)
{
   /* We expect the engine that extends this one to initialize and create
    * the engine_data, ecore_evas and to get us the x_window. */
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;
   ecore_x_dnd_aware_set(engine_data->x_window, 1);
   engine_info.super->window_constructor(window);
}

/* Destroys the window */
static void _window_destructor(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;
   engine_info.super->window_destructor(window);
   free(engine_data);
   window->engine_data = NULL;
}

/* Gets the geometry of the screen containing the window */
static void _window_screen_geometry_get(Etk_Window *window, int *x, int *y, int *w, int *h)
{
   Etk_Engine_Window_Data *engine_data;
   Ecore_X_Window root;
   int num_screens;
   int sx, sy, sw, sh;
   int cx, cy, cw, ch;
   int i;
   
   engine_data = window->engine_data;
   
   num_screens = ecore_x_xinerama_screen_count_get();
   etk_window_geometry_get(window, &cx, &cy, &cw, &ch);
   cx += (cw / 2);
   cy += (ch / 2);
   
   for (i = 0; i < num_screens; i++)
   {
      ecore_x_xinerama_screen_geometry_get(i, &sx, &sy, &sw, &sh);
      if (ETK_INSIDE(cx, cy, sx, sy, sw, sh))
      {
         if (x)   *x = sx;
         if (y)   *y = sy;
         if (w)   *w = sw;
         if (h)   *h = sh;
         return;
      }
   }
   
   for (root = engine_data->x_window; ecore_x_window_parent_get(root) != 0; root = ecore_x_window_parent_get(root));
   ecore_x_window_geometry_get(root, x, y, w, h);
}

/* Makes the window modal for another window */
static void _window_modal_for_window(Etk_Window *window_to_modal, Etk_Window *window)
{
   Etk_Engine_Window_Data *win_to_modal_data;
   
   if (!window_to_modal)
     return;
   
   win_to_modal_data = window_to_modal->engine_data;
   
   if (window)
   {
      Etk_Engine_Window_Data *win_data = window->engine_data;
      
      ecore_x_icccm_transient_for_set(win_to_modal_data->x_window, win_data->x_window);
      _window_netwm_state_active_set(window_to_modal, ECORE_X_WINDOW_STATE_MODAL, ETK_TRUE);
   }
   else
   {
      ecore_x_icccm_transient_for_unset(win_to_modal_data->x_window);
      _window_netwm_state_active_set(window_to_modal, ECORE_X_WINDOW_STATE_MODAL, ETK_FALSE);
   }
}

/* Sets the stacking layer of the window ("normal", "always on top" or "always below") */
static void _window_stacking_set(Etk_Window *window, Etk_Window_Stacking stacking)
{
   Etk_Engine_Ecore_Evas_Window_Data *engine_data;
   
   engine_data = window->engine_data;
   if (stacking == ETK_WINDOW_ABOVE)
      ecore_evas_layer_set(engine_data->ecore_evas, ECORE_X_WINDOW_LAYER_ABOVE);
   else if (stacking == ETK_WINDOW_BELOW)
      ecore_evas_layer_set(engine_data->ecore_evas, ECORE_X_WINDOW_LAYER_BELOW);
   else
      ecore_evas_layer_set(engine_data->ecore_evas, ECORE_X_WINDOW_LAYER_NORMAL);
}

/* Gets the stacking layer of the window */
static Etk_Window_Stacking _window_stacking_get(Etk_Window *window)
{
   Etk_Engine_Ecore_Evas_Window_Data *engine_data;
   int layer;
   
   engine_data = window->engine_data;
   layer = ecore_evas_layer_get(engine_data->ecore_evas);
   if (layer <= ECORE_X_WINDOW_LAYER_BELOW)
      return ETK_WINDOW_BELOW;
   else if (layer >= ECORE_X_WINDOW_LAYER_ABOVE)
      return ETK_WINDOW_ABOVE;
   else
      return ETK_WINDOW_NORMAL;
}

/* Sets whether or not the window should appear in the taskbar */
static void _window_skip_taskbar_hint_set(Etk_Window *window, Etk_Bool skip_taskbar_hint)
{
   if (!window)
     return;
   
   _window_netwm_state_active_set(window, ECORE_X_WINDOW_STATE_SKIP_TASKBAR, skip_taskbar_hint);
   etk_object_notify(ETK_OBJECT(window), "skip-taskbar");
}

/* Gets whether the window appears in the taskbar */
static Etk_Bool _window_skip_taskbar_hint_get(Etk_Window *window)
{
   return _window_netwm_state_active_get(window, ECORE_X_WINDOW_STATE_SKIP_TASKBAR);
}

/* Sets whether or not the window should appear in the pager */
static void _window_skip_pager_hint_set(Etk_Window *window, Etk_Bool skip_pager_hint)
{
   if (!window)
     return;
   
   _window_netwm_state_active_set(window, ECORE_X_WINDOW_STATE_SKIP_PAGER, skip_pager_hint);
   etk_object_notify(ETK_OBJECT(window), "skip-pager");
}

/* Gets whether the window appears in the pager */
static Etk_Bool _window_skip_pager_hint_get(Etk_Window *window)
{
   Etk_Engine_Window_Data *win_data;
   int accepts_focus, is_urgent;
   Ecore_X_Window_State_Hint initial_state;
   Ecore_X_Pixmap icon_pixmap, icon_mask;
   Ecore_X_Window icon_window, window_group;

   if (!window)
     return ETK_FALSE;

   win_data = window->engine_data;
   ecore_x_icccm_hints_get(win_data->x_window,
			   &accepts_focus,
			   &initial_state,
			   &icon_pixmap,
			   &icon_mask,
			   &icon_window,
			   &window_group,
			   &is_urgent);

   return accepts_focus;
}

/* Sets whether or not the window should appear in the pager */
static void _window_focusable_set(Etk_Window *window, Etk_Bool focusable)
{
   Etk_Engine_Window_Data *win_data;
   int accepts_focus, is_urgent;
   Ecore_X_Window_State_Hint initial_state;
   Ecore_X_Pixmap icon_pixmap, icon_mask;
   Ecore_X_Window icon_window, window_group;

   if (!window)
     return;

   win_data = window->engine_data;
   ecore_x_icccm_hints_get(win_data->x_window,
			   &accepts_focus,
			   &initial_state,
			   &icon_pixmap,
			   &icon_mask,
			   &icon_window,
			   &window_group,
			   &is_urgent);

   if (accepts_focus == focusable)
     return;

   accepts_focus = focusable;

   ecore_x_icccm_hints_set(win_data->x_window,
			   accepts_focus,
			   initial_state,
			   icon_pixmap,
			   icon_mask,
			   icon_window,
			   window_group,
			   is_urgent);

   etk_object_notify(ETK_OBJECT(window), "window-focusable");
}

/* Gets whether the window appears in the pager */
static Etk_Bool _window_focusable_get(Etk_Window *window)
{
   return _window_netwm_state_active_get(window, ECORE_X_WINDOW_STATE_SKIP_PAGER);
}

/* Sets the mouse pointer to use when the mouse is inside the window */
static void _window_pointer_set(Etk_Window *window, Etk_Pointer_Type pointer_type)
{
   int x_pointer_type = ECORE_X_CURSOR_LEFT_PTR;
   Ecore_Evas *ecore_evas;
   Ecore_X_Cursor cursor;
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;
   ecore_evas = ETK_ENGINE_ECORE_EVAS_WINDOW_DATA(engine_data)->ecore_evas;

   switch (pointer_type)
   {
      case ETK_POINTER_DND_DROP:
         x_pointer_type = ECORE_X_CURSOR_PLUS;
         break;
      case ETK_POINTER_MOVE:
         x_pointer_type = ECORE_X_CURSOR_FLEUR;
         break;
      case ETK_POINTER_H_DOUBLE_ARROW:
         x_pointer_type = ECORE_X_CURSOR_SB_H_DOUBLE_ARROW;
         break;
      case ETK_POINTER_V_DOUBLE_ARROW:
         x_pointer_type = ECORE_X_CURSOR_SB_V_DOUBLE_ARROW;
         break;
      case ETK_POINTER_RESIZE:
         x_pointer_type = ECORE_X_CURSOR_SIZING;
         break;
      case ETK_POINTER_RESIZE_TL:
         x_pointer_type = ECORE_X_CURSOR_TOP_LEFT_CORNER;
         break;
      case ETK_POINTER_RESIZE_T:
         x_pointer_type = ECORE_X_CURSOR_TOP_SIDE;
         break;
      case ETK_POINTER_RESIZE_TR:
         x_pointer_type = ECORE_X_CURSOR_TOP_RIGHT_CORNER;
         break;
      case ETK_POINTER_RESIZE_R:
         x_pointer_type = ECORE_X_CURSOR_RIGHT_SIDE;
         break;
      case ETK_POINTER_RESIZE_BR:
         x_pointer_type = ECORE_X_CURSOR_BOTTOM_RIGHT_CORNER;
         break;
      case ETK_POINTER_RESIZE_B:
         x_pointer_type = ECORE_X_CURSOR_BOTTOM_SIDE;
         break;
      case ETK_POINTER_RESIZE_BL:
         x_pointer_type = ECORE_X_CURSOR_BOTTOM_LEFT_CORNER;
         break;
      case ETK_POINTER_RESIZE_L:
         x_pointer_type = ECORE_X_CURSOR_LEFT_SIDE;
         break;
      case ETK_POINTER_TEXT_EDIT:
         x_pointer_type = ECORE_X_CURSOR_XTERM;
         break;
      case ETK_POINTER_DEFAULT:
      default:
         x_pointer_type = ECORE_X_CURSOR_LEFT_PTR;
         break;
   }
   
   if (pointer_type == ETK_POINTER_NONE)
      ecore_x_window_cursor_set(ecore_evas_software_x11_window_get(ecore_evas), 0);
   else if ((cursor = ecore_x_cursor_shape_get(x_pointer_type)))
      ecore_x_window_cursor_set(ecore_evas_software_x11_window_get(ecore_evas), cursor);
   else
      ETK_WARNING("Unable to find the X cursor \"%d\"", pointer_type);
}

/**************************
 *
 * Etk_Popup_Window's functions
 *
 **************************/

/* Initializes the created popup window */
static void _popup_window_constructor(Etk_Popup_Window *popup_window)
{   
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = ETK_WINDOW(popup_window)->engine_data;   
   ecore_x_netwm_window_type_set(engine_data->x_window, ECORE_X_WINDOW_TYPE_MENU);
   ecore_evas_override_set(ETK_ENGINE_ECORE_EVAS_WINDOW_DATA(engine_data)->ecore_evas, 1);
   ecore_evas_ignore_events_set(ETK_ENGINE_ECORE_EVAS_WINDOW_DATA(engine_data)->ecore_evas, 1);
}

/* Called when the popup window is popped up */
static void _popup_window_popup(Etk_Popup_Window *popup_window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = ETK_WINDOW(popup_window)->engine_data;
   
   if (_popup_window_input_window == 0)
   {
      Ecore_X_Window root, win;
      int root_x, root_y, root_w, root_h;
      int mx, my;
      
      /* Release all the buttons of the mouse */
      /* TODO: There should be a better way to do that... */
      ecore_x_pointer_last_xy_get(&mx, &my);
      if ((win = ecore_x_window_at_xy_get(mx, my)) != 0)
      {
         int i;
         
         ecore_x_pointer_xy_get(win, &mx, &my);
         for (i = 1; i <= 32; i++)
            ecore_x_mouse_up_send(win, mx, my, i);
      }
      
      /* Create the input window */
      root = engine_data->x_window;
      while ((win = ecore_x_window_parent_get(root)) != 0)
	 root = win;
      
      ecore_x_window_geometry_get(root, &root_x, &root_y, &root_w, &root_h);
      _popup_window_input_window = ecore_x_window_input_new(root, root_x, root_y, root_w, root_h);
      ecore_x_window_show(_popup_window_input_window);
      ecore_x_pointer_confine_grab(_popup_window_input_window);
      ecore_x_keyboard_grab(_popup_window_input_window);
   }
   _popup_window_popped_windows = eina_list_append(_popup_window_popped_windows, popup_window);
}

/* Called when the popup window is popped down */
static void _popup_window_popdown(Etk_Popup_Window *popup_window)
{
   _popup_window_popped_windows = eina_list_remove(_popup_window_popped_windows, popup_window);
   
   if (!_popup_window_popped_windows)
   {
      ecore_x_pointer_ungrab();
      ecore_x_keyboard_ungrab();
      ecore_x_window_del(_popup_window_input_window);
      _popup_window_input_window = 0;
   }
}

/**************************
 *
 * Etk_Event's functions
 *
 **************************/

/* Sets the function to call when an input event is received */
static void _event_callback_set(void (*callback)(Etk_Event_Type event, Etk_Event_Global event_info))
{
   _event_callback = callback;
}

/* Gets the current event timestamp */
static unsigned int _event_timestamp_get(void)
{
   return ecore_x_current_time_get();
}

/**************************
 *
 * Mouse's functions
 *
 **************************/

/* Gets the position of the mouse pointer */
static void _mouse_position_get(int *x, int *y)
{
   ecore_x_pointer_last_xy_get(x, y);
}

/* Gets the geometry of the screen containing the mouse pointer */
static void _mouse_screen_geometry_get(int *x, int *y, int *w, int *h)
{
   int num_screens;
   int sx, sy, sw, sh;
   int mx, my;
   
   num_screens = ecore_x_xinerama_screen_count_get();	 
   if (num_screens > 0)
   {
      int i;
      
      ecore_x_pointer_last_xy_get(&mx, &my);
      for (i = 0; i < num_screens; i++)
      {
         ecore_x_xinerama_screen_geometry_get(i, &sx, &sy, &sw, &sh);
         if (ETK_INSIDE(mx, my, sx, sy, sw, sh))
         {
            if (x)   *x = sx;
            if (y)   *y = sy;
            if (w)   *w = sw;
            if (h)   *h = sh;
            return;
         }
      }
   }
   
   ecore_x_window_geometry_get(ecore_x_window_root_first_get(), x, y, w, h);
}

/**************************
 *
 * Etk_Selection's functions
 *
 **************************/

/* Sets the text of the given selection */
static void _selection_text_set(Etk_Selection_Type selection, const char *text)
{
   int len;
   Ecore_X_Window win;
   
   if (!text)
      return;
   
   win = ecore_x_window_root_first_get();
   len = strlen(text) + 1;
   if (selection == ETK_SELECTION_PRIMARY)
      ecore_x_selection_primary_set(win, text, len);
   else if (selection == ETK_SELECTION_SECONDARY)
      ecore_x_selection_secondary_set(win, text, len);
   else if (selection == ETK_SELECTION_CLIPBOARD)
      ecore_x_selection_clipboard_set(win, text, len);
   
}  

/* Requests the text from a selection */
static void _selection_text_request(Etk_Selection_Type selection, Etk_Widget *target)
{
   Etk_Toplevel *toplevel;
   Ecore_X_Window w;

   if (!(toplevel = etk_widget_toplevel_parent_get(target)))
      return;

   if (ETK_IS_WINDOW(toplevel)) {
      Etk_Engine_Window_Data *engine_data = ETK_WINDOW(toplevel)->engine_data;
      w = engine_data->x_window;
   }
   else if (ETK_IS_EMBED(toplevel))
   {
      Ecore_Evas *ee = ecore_evas_ecore_evas_get(ETK_TOPLEVEL(toplevel)->evas);
      if (!ee) return;
      w = ecore_evas_software_x11_window_get(ee);
      if (!w) return;
   }
   else
   {
      return;
   }

   _selection_widget = target;
   if (selection == ETK_SELECTION_PRIMARY)
      ecore_x_selection_primary_request(w, ECORE_X_SELECTION_TARGET_UTF8_STRING);
   else if (selection == ETK_SELECTION_SECONDARY)
      ecore_x_selection_secondary_request(w, ECORE_X_SELECTION_TARGET_UTF8_STRING);
   else if (selection == ETK_SELECTION_CLIPBOARD)
      ecore_x_selection_clipboard_request(w, ECORE_X_SELECTION_TARGET_UTF8_STRING);
}

/* Clears the given selection */
static void _selection_clear(Etk_Selection_Type selection)
{
   if (selection == ETK_SELECTION_PRIMARY)
      ecore_x_selection_primary_clear();
   else if (selection == ETK_SELECTION_SECONDARY)
      ecore_x_selection_secondary_clear();
   else if (selection == ETK_SELECTION_CLIPBOARD)
      ecore_x_selection_clipboard_clear();
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when an input event is received */
static int _event_input_handler_cb(void *data, int type, void *event)
{
   Etk_Event_Global ev;
   int x, y;
   
   if (!_event_callback)
      return 1;
   
   if (type == ECORE_EVENT_MOUSE_MOVE)
   {
      Ecore_Event_Mouse_Move *xev = event;
      
      ecore_x_window_geometry_get(xev->window, &x, &y, NULL, NULL);
      _event_global_modifiers_locks_wrap(xev->modifiers, &ev.mouse_move.modifiers, &ev.mouse_move.locks);
      ev.mouse_move.pos.x = xev->x + x;
      ev.mouse_move.pos.y = xev->y + y;
      ev.mouse_move.timestamp = xev->timestamp;
      _event_callback(ETK_EVENT_MOUSE_MOVE, ev);
   }
   else if (type == ECORE_EVENT_MOUSE_BUTTON_DOWN)
   {
      Ecore_Event_Mouse_Button *xev = event;
      
      ecore_x_window_geometry_get(xev->window, &x, &y, NULL, NULL);
      _event_global_modifiers_locks_wrap(xev->modifiers, &ev.mouse_down.modifiers, &ev.mouse_down.locks);
      ev.mouse_down.flags = ETK_MOUSE_NONE;
      if (xev->double_click)
         ev.mouse_down.flags |= ETK_MOUSE_DOUBLE_CLICK;
      if (xev->triple_click)
         ev.mouse_down.flags |= ETK_MOUSE_TRIPLE_CLICK;
      ev.mouse_down.button = xev->buttons;
      ev.mouse_down.pos.x = xev->x + x;
      ev.mouse_down.pos.y = xev->y + y;
      ev.mouse_down.timestamp = xev->timestamp;
      _event_callback(ETK_EVENT_MOUSE_DOWN, ev);
   }
   else if (type == ECORE_EVENT_MOUSE_BUTTON_UP)
   {
      Ecore_Event_Mouse_Button *xev = event;
      
      ecore_x_window_geometry_get(xev->window, &x, &y, NULL, NULL);
      _event_global_modifiers_locks_wrap(xev->modifiers, &ev.mouse_up.modifiers, &ev.mouse_up.locks);
      ev.mouse_up.flags = ETK_MOUSE_NONE;
      if (xev->double_click)
         ev.mouse_up.flags |= ETK_MOUSE_DOUBLE_CLICK;
      if (xev->triple_click)
         ev.mouse_up.flags |= ETK_MOUSE_TRIPLE_CLICK;
      ev.mouse_up.button = xev->buttons;
      ev.mouse_up.pos.x = xev->x + x;
      ev.mouse_up.pos.y = xev->y + y;
      ev.mouse_up.timestamp = xev->timestamp;
      _event_callback(ETK_EVENT_MOUSE_UP, ev);
   }
   else if (type == ECORE_EVENT_MOUSE_WHEEL)
   {
      Ecore_Event_Mouse_Wheel *xev = event;
      
      ecore_x_window_geometry_get(xev->window, &x, &y, NULL, NULL);
      _event_global_modifiers_locks_wrap(xev->modifiers, &ev.mouse_wheel.modifiers, &ev.mouse_wheel.locks);
      ev.mouse_wheel.direction = (xev->direction == 0) ? ETK_WHEEL_VERTICAL : ETK_WHEEL_HORIZONTAL;
      ev.mouse_wheel.z = xev->z;
      ev.mouse_wheel.pos.x = xev->x + x;
      ev.mouse_wheel.pos.y = xev->y + y;
      ev.mouse_wheel.timestamp = xev->timestamp;
      _event_callback(ETK_EVENT_MOUSE_WHEEL, ev);
   }
   else if (type == ECORE_EVENT_KEY_DOWN)
   {
      Ecore_Event_Key *xev = event;
      
      _event_global_modifiers_locks_wrap(xev->modifiers, &ev.key_down.modifiers, &ev.key_down.locks);
      ev.key_down.keyname = xev->keyname; /* FIXME: const gone ? */
      ev.key_down.key = xev->key;
      ev.key_down.string = xev->string;
      ev.key_down.timestamp = xev->timestamp;
      _event_callback(ETK_EVENT_KEY_DOWN, ev);
   }
   else if (type == ECORE_EVENT_KEY_UP)
   {
      Ecore_Event_Key *xev = event;
      
      _event_global_modifiers_locks_wrap(xev->modifiers, &ev.key_up.modifiers, &ev.key_up.locks);
      ev.key_up.keyname = xev->keyname; /* FIXME: const gone ? */
      ev.key_up.key = xev->key;
      ev.key_up.string = xev->string;
      ev.key_up.timestamp = xev->timestamp;
      _event_callback(ETK_EVENT_KEY_UP, ev);
   }

   return 1;
}

/* Called when the content of the selection/clipboard has been received */
static int _selection_notify_handler_cb(void *data, int type, void *event)
{
   Ecore_X_Event_Selection_Notify *ev;
   Ecore_X_Selection_Data *sel_data;
   Etk_Selection_Event etk_event;
   
   if (!_selection_widget)
      return 1;
   
   ev = event;
   sel_data = ev->data;
   
   if (ev->selection == ECORE_X_SELECTION_PRIMARY)
      etk_event.from = ETK_SELECTION_PRIMARY;
   else if (ev->selection == ECORE_X_SELECTION_SECONDARY)
      etk_event.from = ETK_SELECTION_SECONDARY;
   else if (ev->selection == ECORE_X_SELECTION_CLIPBOARD)
      etk_event.from = ETK_SELECTION_CLIPBOARD;
   else
      return 1;
   
   if (sel_data->content == ECORE_X_SELECTION_CONTENT_TEXT)
   {
      Ecore_X_Selection_Data_Text *text_data = (Ecore_X_Selection_Data_Text *)sel_data;
      
      etk_event.type = ETK_SELECTION_TEXT;
      etk_event.data.text = text_data->text;
      
      etk_signal_emit_by_name("selection-received", ETK_OBJECT(_selection_widget), &etk_event);
   }
   
   return 1;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Sets whether or not the given netwm state is active */
static void _window_netwm_state_active_set(Etk_Window *window, Ecore_X_Window_State state, Etk_Bool active)
{
   Etk_Engine_Window_Data *engine_data;
   Ecore_Evas *ecore_evas;
   
   if (!window)
     return;
   
   engine_data = window->engine_data;
   ecore_evas = ETK_ENGINE_ECORE_EVAS_WINDOW_DATA(engine_data)->ecore_evas;
   
   if (ecore_evas_visibility_get(ecore_evas))
   {
      Ecore_X_Window root, parent;
      
      root = engine_data->x_window;
      while ((parent = ecore_x_window_parent_get(root)) != 0)
         root = parent;
      
      ecore_x_netwm_state_request_send(engine_data->x_window, root, state, -1, active);
   }
   else
   {
      unsigned int cur_num, new_num;
      int i, j;
      Ecore_X_Window_State *cur_state, *new_state;
      Etk_Bool is_set = ETK_FALSE;
      
      ecore_x_netwm_window_state_get(engine_data->x_window, &cur_state, &cur_num);
      if (cur_state)
      {
         for (i = 0; i < cur_num; i++)
         {
            if (cur_state[i] == state)
            {
               is_set = ETK_TRUE;
               break;
            }
         }
         
         if (is_set != active)
         {
            new_num = active ? (cur_num + 1) : (cur_num - 1);
            if (new_num > 0)
            {
               new_state = malloc(new_num * sizeof(Ecore_X_Window_State));
               
               for (i = 0, j = 0; i < cur_num; i++)
               {
                  if (cur_state[i] == state)
                     continue;
                  
                  new_state[j] = cur_state[i];
                  j++;
               }
               
               if (active)
                  new_state[new_num - 1] = state;
               
               ecore_x_netwm_window_state_set(engine_data->x_window, new_state, new_num);
               free(new_state);
            }
            else
               ecore_x_netwm_window_state_set(engine_data->x_window, NULL, 0);
         }
         
         free(cur_state);
      } else {
	 new_state = malloc(sizeof(Ecore_X_Window_State));
	 new_state[0] = state;
	 ecore_x_netwm_window_state_set(engine_data->x_window, new_state, 1);
	 free(new_state);
      }
   }
}

/* Gets whether or not the given netwm state is active */
static Etk_Bool _window_netwm_state_active_get(Etk_Window *window, Ecore_X_Window_State state)
{
   unsigned int num, i;
   Ecore_X_Window_State *cur_state;
   Etk_Engine_Window_Data *engine_data;
   
   if (!window)
     return ETK_FALSE;
   
   engine_data = window->engine_data;
   ecore_x_netwm_window_state_get(engine_data->x_window, &cur_state, &num);
   if (cur_state)
   {
      for (i = 0; i < num; i++)
      {
         if (cur_state[i] == state)
         {
            free(cur_state);
            return ETK_TRUE;
         }
      }
      free(cur_state);
   }
   
   return ETK_FALSE;
}

/* Converts the Ecore_X modifiers bits to Etk_Modifiers and Etk_Locks */
static void _event_global_modifiers_locks_wrap(int xmodifiers, Etk_Modifiers *modifiers, Etk_Locks *locks)
{
   if (modifiers)
   {
      *modifiers = ETK_MODIFIER_NONE;
      if (xmodifiers & ECORE_EVENT_MODIFIER_SHIFT)
         *modifiers |= ETK_MODIFIER_SHIFT;
      if (xmodifiers & ECORE_EVENT_MODIFIER_CTRL)
         *modifiers |= ETK_MODIFIER_CTRL;
      if (xmodifiers & ECORE_EVENT_MODIFIER_ALT)
         *modifiers |= ETK_MODIFIER_ALT;
      if (xmodifiers & ECORE_EVENT_MODIFIER_WIN)
         *modifiers |= ETK_MODIFIER_WIN;
   }
   
   if (modifiers)
   {
      *locks = ETK_LOCK_NONE;
      if (xmodifiers & ECORE_EVENT_LOCK_SCROLL)
         *locks |= ETK_LOCK_SCROLL;
      if (xmodifiers & ECORE_EVENT_LOCK_NUM)
         *locks |= ETK_LOCK_NUM;
      if (xmodifiers & ECORE_EVENT_LOCK_CAPS)
         *locks |= ETK_LOCK_CAPS;
   }
}
