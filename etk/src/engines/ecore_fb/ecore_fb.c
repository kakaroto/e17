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

#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <Evas.h>
#include <Edje.h>
#include <Ecore_Evas.h>
#include <Ecore_Fb.h>
#include <Ecore_File.h>
#include <Ecore_Input.h>
#if ENGINE_E_FB_X11_SUPPORT
#include <Ecore_X.h>
#endif
#include <Etk.h>
#include "Etk_Engine_Ecore_Fb.h"

#define DEFAULT_FB_WIDTH  1024
#define DEFAULT_FB_HEIGHT 768

typedef Etk_Engine_Ecore_Fb_Window_Data Etk_Engine_Window_Data;

/* General engine functions */
Etk_Engine *engine_open(void);
void engine_close(void);

static Etk_Bool _engine_init(void);
static void _engine_shutdown(void);

/* Etk_Window functions */
static void _window_constructor(Etk_Window *window);
static void _window_destructor(Etk_Window *window);
static void _window_show(Etk_Window *window);
static void _window_hide(Etk_Window *window);
static Evas *_window_evas_get(Etk_Window *window);
static void _window_title_set(Etk_Window *window, const char *title);
static const char *_window_title_get(Etk_Window *window);
static void _window_move(Etk_Window *window, int x, int y);
static void _window_resize(Etk_Window *window, int w, int h);
static void _window_min_size_set(Etk_Window *window, int w, int h);
static void _window_evas_position_get(Etk_Window *window, int *x, int *y);
static void _window_screen_position_get(Etk_Window *window, int *x, int *y);
static void _window_maximized_set(Etk_Window *window, Etk_Bool maximized);
static Etk_Bool _window_maximized_get(Etk_Window *window);
static void _window_size_get(Etk_Window *window, int *w, int *h);
static void _window_screen_geometry_get(Etk_Window *window, int *x, int *y, int *w, int *h);
static void _window_raise(Etk_Window *window);
static void _window_lower(Etk_Window *window);
static void _window_focused_set(Etk_Window *window, Etk_Bool focused);
static Etk_Bool _window_focused_get(Etk_Window *window);
static void _window_decorated_set(Etk_Window *window, Etk_Bool decorated);
static Etk_Bool _window_decorated_get(Etk_Window *window);
static void _window_pointer_set(Etk_Window *window, Etk_Pointer_Type pointer_type);

static void _window_realized_cb(Etk_Object *object, void *data);
static void _window_unrealized_cb(Etk_Object *object, void *data);
static void _window_titlebar_mouse_down_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _window_titlebar_mouse_up_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _window_resize_mouse_in_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _window_resize_mouse_out_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _window_resize_mouse_down_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _window_resize_mouse_up_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _window_maximize_mouse_up_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _window_close_mouse_up_cb(void *data, Evas_Object *obj, const char *emission, const char *source);

/* Event and mouse functions */
static void _event_callback_set(void (*callback)(Etk_Event_Type event, Etk_Event_Global event_info));
static void _mouse_position_get(int *x, int *y);
static void _mouse_screen_geometry_get(int *x, int *y, int *w, int *h);
static int _mouse_move_handler_cb(void *data, int ev_type, void *ev);
#if ENGINE_E_FB_X11_SUPPORT
static int _mouse_move_X_handler_cb(void *data, int ev_type, void *ev);
#endif

/* Private functions */
Etk_Window *_window_focus_find_other(Etk_Window *current);
static Etk_Cache *_pointer_cache_build(void);
static char *_theme_find(const char *theme_name);

/* Private vars */
static Etk_Bool _use_x11 = ETK_FALSE;
static char *_x11_engine = NULL;
static Ecore_Evas *_ecore_evas = NULL;
static Evas *_evas = NULL;
static int _fb_width = DEFAULT_FB_WIDTH;
static int _fb_height = DEFAULT_FB_HEIGHT;

static char *_wm_theme_file = NULL;
static Evas_Object *_background_object = NULL;
static Evas_Object *_pointer_object = NULL;
static Etk_Cache *_pointer_cache = NULL;
static char *_pointer_group = NULL;

static void (*_event_callback)(Etk_Event_Type event, Etk_Event_Global event_info) = NULL;
static int _mouse_x = 0;
static int _mouse_y = 0;

static Etk_Window *_window_to_drag = NULL;
static int _window_drag_offset_x = 0;
static int _window_drag_offset_y = 0;

static Etk_Window *_window_to_resize = NULL;
static int _window_resize_offset_x = 0;
static int _window_resize_offset_y = 0;

static Etk_Window *_focused_window = NULL;


static Etk_Engine engine_info = {
   
   NULL, /* engine specific data */
   NULL, /* engine name */
   NULL, /* super (parent) engine */
   NULL, /* DL handle */
   
   _engine_init,
   _engine_shutdown,
   
   _window_constructor,
   _window_destructor,
   _window_show,
   _window_hide,
   _window_evas_get,
   _window_title_set,
   _window_title_get,
   NULL, /* window_wmclass_set */
   _window_move,
   _window_resize,
   _window_min_size_set,
   _window_evas_position_get,
   _window_screen_position_get,
   _window_size_get,
   _window_screen_geometry_get,
   NULL, /* window_modal_for_window */
   NULL, /* window_iconified_set */
   NULL, /* window_iconified_get */
   _window_maximized_set,
   _window_maximized_get,
   NULL, /* window_fullscreen_set */
   NULL, /* window_fullscreen_get */
   _window_raise,
   _window_lower,
   NULL, /* window_stacking_set */
   NULL, /* window_stacking_get */
   NULL, /* window_sticky_set */
   NULL, /* window_sticky_get */
   _window_focused_set,
   _window_focused_get,
   _window_decorated_set,
   _window_decorated_get,
   NULL, /* window_shaped_set */
   NULL, /* window_shaped_get */
   NULL, /* window_has_alpha_set */
   NULL, /* window_has_alpha_get */     
   NULL, /* window_skip_taskbar_hint_set */
   NULL, /* window_skip_taskbar_hint_get */
   NULL, /* window_skip_pager_hint_set */
   NULL, /* window_skip_pager_hint_get */
   NULL, /* window_focusable_set */
   NULL, /* window_focusable_get */
   _window_pointer_set,

   NULL, /* popup_window_constructor */
   NULL, /* popup_window_popup */
   NULL, /* popup_window_popdown */
   
   _event_callback_set,
   NULL, /* event_timestamp_get */
   
   _mouse_position_get,
   _mouse_screen_geometry_get,
   
   NULL, /* selection_text_set */
   NULL, /* selection_text_request */
   NULL, /* selection_clear */
   
   NULL, /* drag_constructor */
   NULL, /* drag_begin */
};

/**************************
 *
 * General engine functions
 *
 **************************/

/* Called when the engine is loaded */
Etk_Engine *engine_open(void)
{
   char *use_x11_arg = NULL;
   char *fb_width_arg = NULL;
   char *fb_height_arg = NULL;
   
   _wm_theme_file = _theme_find(etk_config_wm_theme_get());
   if (!_wm_theme_file)
   {
      /* fallback to default theme */
      _wm_theme_file = _theme_find("default");
      if (!_wm_theme_file)
      {
         return NULL;
      }
   }

   engine_info.engine_data = NULL;
   engine_info.engine_name = strdup("ecore_fb");

#if ENGINE_E_FB_X11_SUPPORT
   /* Parse the arguments */
   etk_argument_value_get("ecore-fb-use-x11", 0, ETK_TRUE, &use_x11_arg);
   if (use_x11_arg)
   {
      _use_x11 = ETK_TRUE;

      etk_argument_value_get("ecore-fb-x11-engine", 0, ETK_TRUE, &_x11_engine);
      if (!_x11_engine)
      {
         _x11_engine = strdup("ecore_evas_software_x11");
      }

      /* Note that this values are only used when X11 is enabled */
      etk_argument_value_get("ecore-fb-width", 0, ETK_TRUE, &fb_width_arg);
      if (fb_width_arg)
      {
         _fb_width = (int) strtol(fb_width_arg, (char **) NULL, 10);
         if (_fb_width <= 0)
            _fb_width = DEFAULT_FB_WIDTH;
      }

      etk_argument_value_get("ecore-fb-height", 0, ETK_TRUE, &fb_height_arg);
      if (fb_height_arg)
      {
         _fb_height = (int) strtol(fb_height_arg, (char **) NULL, 10);
         if (_fb_height <= 0)
            _fb_height = DEFAULT_FB_HEIGHT;
      }
   }
#endif

   return &engine_info;
}

/* Called when the engine is unloaded */
void engine_close(void)
{
   free(engine_info.engine_name);
   free(_wm_theme_file);
   free(_x11_engine);
}

/* Initializes the engine */
static Etk_Bool _engine_init(void)
{
#if ENGINE_E_FB_X11_SUPPORT
   if (_use_x11)
   {
      if (!ecore_x_init(NULL))
      {
         ETK_WARNING("Ecore_X initialization failed!");
         return ETK_FALSE;
      }
      if (!ecore_evas_init())
      {
         ETK_WARNING("Ecore_Evas initialization failed!");
         return ETK_FALSE;
      }

      ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE, _mouse_move_X_handler_cb, NULL);

      /* Create the evas where all the windows will be drawn */

      if (strcmp(_x11_engine, "ecore_evas_software_x11") == 0)
      {
        _ecore_evas = ecore_evas_software_x11_new(NULL, 0, 0, 0, _fb_width, _fb_height);
      }
      else if (strcmp(_x11_engine, "ecore_evas_gl_x11") == 0)
      {
        _ecore_evas = ecore_evas_gl_x11_new(NULL, 0, 0, 0, _fb_width, _fb_height);
      }
      else if (strcmp(_x11_engine, "ecore_evas_xrender_x11") == 0)
      {
        _ecore_evas = ecore_evas_xrender_x11_new(NULL, 0, 0, 0, _fb_width, _fb_height);
      }
      else if (strcmp(_x11_engine, "ecore_evas_software_x11_16") == 0)
      {
        _ecore_evas = ecore_evas_software_x11_16_new(NULL, 0, 0, 0, _fb_width, _fb_height);
      }
      else
      {
        _ecore_evas = ecore_evas_software_x11_new(NULL, 0, 0, 0, _fb_width, _fb_height);
      }
   }
   else
#endif
   {
      if (!ecore_fb_init(NULL))
      {
         ETK_WARNING("Ecore_FB initialization failed!");
         return ETK_FALSE;
      }
      if (!ecore_evas_init())
      {
         ETK_WARNING("Ecore_Evas initialization failed!");
         return ETK_FALSE;
      }

      ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE, _mouse_move_handler_cb, NULL);
      ecore_fb_size_get(&_fb_width, &_fb_height);

      /* Create the evas where all the windows will be drawn */
      _ecore_evas = ecore_evas_fb_new(NULL, 0, _fb_width, _fb_height);
   }

   if (!_ecore_evas)
   {
      ETK_WARNING("Unable to create a FB Ecore_Evas");
      return ETK_FALSE;
   }
   if (!(_evas = ecore_evas_get(_ecore_evas)))
   {
      ETK_WARNING("Unable to create a FB Evas");
      return ETK_FALSE;
   }
   
   ecore_evas_show(_ecore_evas);
   
   /* Create the background */
   _background_object = edje_object_add(_evas);
   edje_object_file_set(_background_object, _wm_theme_file, "etk/wm_background");
   evas_object_resize(_background_object, _fb_width, _fb_height);
   evas_object_show(_background_object);
   
   /* Cache the different mouse pointers and use the default one */
   _pointer_cache = _pointer_cache_build();
   _window_pointer_set(NULL, ETK_POINTER_DEFAULT);
#if ENGINE_E_FB_X11_SUPPORT
   ecore_evas_cursor_set(_ecore_evas, "", 1000, 32, 32);
#endif
  
   return ETK_TRUE;
}

/* Shutdowns the engine */
static void _engine_shutdown(void)
{
   etk_cache_destroy(_pointer_cache);
   free(_pointer_group);
   ecore_evas_free(_ecore_evas);
   
   ecore_evas_shutdown();
#if ENGINE_E_FB_X11_SUPPORT
   if (_use_x11)
   {
      ecore_x_shutdown();
   }
   else
#endif
   {
      ecore_fb_shutdown();
   }
}

/**************************
 *
 * Etk_Window's functions
 *
 **************************/

/* Initializes the new window */
static void _window_constructor(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = malloc(sizeof(Etk_Engine_Window_Data));
   engine_data->border_position.x = 0;
   engine_data->border_position.y = 0;
   engine_data->size.w = 32;
   engine_data->size.h = 32;
   engine_data->min_size.w = 0;
   engine_data->min_size.h = 0;
   engine_data->visible = ETK_FALSE;
   engine_data->maximized = ETK_FALSE;
   engine_data->title = NULL;
   engine_data->borderless = ETK_FALSE;
   engine_data->border = NULL;
   window->engine_data = engine_data;
   
   ETK_TOPLEVEL(window)->evas = _evas;
   etk_signal_connect("realized", ETK_OBJECT(window), ETK_CALLBACK(_window_realized_cb), NULL);
   etk_signal_connect("unrealized", ETK_OBJECT(window), ETK_CALLBACK(_window_unrealized_cb), NULL);
}

/* Cleans up the window */
static void _window_destructor(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   Etk_Window *other;
   
   engine_data = window->engine_data;
   
   /* Move focus to another window if this was the focused one */
   if (window == _focused_window)
   {
      other = _window_focus_find_other(window);
      if (other)
         etk_window_focused_set(other, ETK_TRUE);
      else
         _focused_window = NULL;
   }
   
   free(engine_data->title);
   free(engine_data);
}

/* Shows the window */
static void _window_show(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;
   engine_data->visible = ETK_TRUE;
   if (engine_data->border)
      evas_object_show(engine_data->border);
}

/* Hides the window */
static void _window_hide(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   Etk_Window *other;

   engine_data = window->engine_data;
   engine_data->visible = ETK_FALSE;
   if (engine_data->border)
      evas_object_hide(engine_data->border);

   /* Move focus to another window if this was the focused one */
   if (window == _focused_window)
   {
      other = _window_focus_find_other(window);
      if (other)
         etk_window_focused_set(other, ETK_TRUE);
      else
         _focused_window = NULL;
   }
}

/* Gets the evas where the window is drawn */
static Evas *_window_evas_get(Etk_Window *window)
{
   return _evas;
}

/* Sets the title of the window */
static void _window_title_set(Etk_Window *window, const char *title)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;
   if (engine_data->title != title)
   {
      free(engine_data->title);
      engine_data->title = strdup(title);
      
      if (engine_data->border)
         edje_object_part_text_set(engine_data->border, "etk.text.title", title ? title : "");
      
      etk_object_notify(ETK_OBJECT(window), "title");
   }
}

/* Returns the title of the window */
static const char *_window_title_get(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data; 
   return engine_data->title;
}

/* Moves the window at the given position */
static void _window_move(Etk_Window *window, int x, int y)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;
   engine_data->border_position.x = x;
   engine_data->border_position.y = y;
   if (engine_data->border && !engine_data->maximized)
      evas_object_move(engine_data->border, x, y);
}

/* Resizes the window */
static void _window_resize(Etk_Window *window, int w, int h)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;
   engine_data->size.w = ETK_MAX(engine_data->min_size.w, w);
   engine_data->size.h = ETK_MAX(engine_data->min_size.h, h);
   if (engine_data->border && ETK_WIDGET(window)->smart_object && !engine_data->maximized)
   {
      int border_w, border_h;
      
      edje_extern_object_min_size_set(ETK_WIDGET(window)->smart_object, engine_data->size.w, engine_data->size.h);
      edje_object_part_swallow(engine_data->border, "etk.swallow.content", ETK_WIDGET(window)->smart_object);
      edje_object_size_min_calc(engine_data->border, &border_w, &border_h);
      evas_object_resize(engine_data->border, border_w, border_h);
   }
}

/* Sets the min size of the window */
static void _window_min_size_set(Etk_Window *window, int w, int h)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;
   engine_data->min_size.w = w;
   engine_data->min_size.h = h;
   if (engine_data->size.w < w || engine_data->size.h < h)
      _window_resize(window, engine_data->size.w, engine_data->size.h);
}

/* Gets the position of the window, relative to the top left corner of the evas where the window is drawn */
static void _window_evas_position_get(Etk_Window *window, int *x, int *y)
{
   if (ETK_WIDGET(window)->smart_object)
      evas_object_geometry_get(ETK_WIDGET(window)->smart_object, x, y, NULL, NULL);
   else
   {
      if (x)   *x = 0;
      if (y)   *y = 0;
   }
}

/* Gets the position of the window, relative to the screen */
static void _window_screen_position_get(Etk_Window *window, int *x, int *y)
{
   if (ETK_WIDGET(window)->smart_object)
      evas_object_geometry_get(ETK_WIDGET(window)->smart_object, x, y, NULL, NULL);
   else
   {
      if (x)   *x = 0;
      if (y)   *y = 0;
   }
}

/* Gets the size of the window */
static void _window_size_get(Etk_Window *window, int *w, int *h)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;
   if (engine_data->maximized && ETK_WIDGET(window)->smart_object)
      evas_object_geometry_get(ETK_WIDGET(window)->smart_object, NULL, NULL, w, h);
   else
   {
      if (w)   *w = engine_data->size.w;
      if (h)   *h = engine_data->size.h;
   }
}

/* Gets the geometry of the screen containing the window */
static void _window_screen_geometry_get(Etk_Window *window, int *x, int *y, int *w, int *h)
{
   if (x)   *x = 0;
   if (y)   *y = 0;
   if (w)   *w = _fb_width;
   if (h)   *h = _fb_height;
}

/* Sets whether or not the window is maximized */
static void _window_maximized_set(Etk_Window *window, Etk_Bool maximized)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;
   if (engine_data->maximized != maximized)
   {
      engine_data->maximized = maximized;
      if (engine_data->border)
      {
         if (maximized)
         {
            evas_object_move(engine_data->border, 0, 0);
            evas_object_resize(engine_data->border, _fb_width, _fb_height);
         }
         else
         {
            evas_object_move(engine_data->border, engine_data->border_position.x, engine_data->border_position.y);
            etk_window_resize(window, engine_data->size.w, engine_data->size.h);
         }
         /* TODO: notify geometry.. */
      }
      else
      
      etk_object_notify(ETK_OBJECT(window), "maximized");
   }
}

/* Gets whether or not the window is maximized */
Etk_Bool _window_maximized_get(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;
   return engine_data->maximized;
}

/* Raises the window */
static void _window_raise(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;
   if (engine_data->border)
   {
      evas_object_raise(engine_data->border);
      if (_pointer_object)
         evas_object_raise(_pointer_object);
   }
}

/* Lowers the window */
static void _window_lower(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;

   engine_data = window->engine_data;
   if (engine_data->border)
      evas_object_lower(engine_data->border);
}

/* Sets the window as the current focused window in the system, or unfocus it.  
 * When unfocusing, the next window available in the windows list will be focused. 
 * If no other window is available, focus will remain on current window. */
void _window_focused_set(Etk_Window *window, Etk_Bool focused)
{
   Etk_Engine_Window_Data *engine_data;
   Etk_Window *other;

   engine_data = window->engine_data;
   if (focused) 
   {
      _focused_window = window;
      evas_object_focus_set(ETK_WIDGET(window)->smart_object, 1);
   }
   else 
   {
      if (_focused_window != window)
         return;

      other = _window_focus_find_other(window);
      if (other) 
      {
         _focused_window = other;
         evas_object_focus_set(ETK_WIDGET(other)->smart_object, 1);
      }
   }
}

/* Is this window the current focused window in the system ? */
Etk_Bool _window_focused_get(Etk_Window *window)
{
   return (_focused_window == window);
}

/* Sets whether or not the window is decorated (i.e. the window has a border) */
static void _window_decorated_set(Etk_Window *window, Etk_Bool decorated)
{
   Etk_Engine_Window_Data *engine_data;

   engine_data = window->engine_data;
   if (engine_data->borderless == !decorated)
      return;
   
   engine_data->borderless = !decorated;
   if (engine_data->border)
   {
      /* Recreate the border */
      _window_unrealized_cb(ETK_OBJECT(window), NULL);
      _window_realized_cb(ETK_OBJECT(window), NULL);
   }
   etk_object_notify(ETK_OBJECT(window), "decorated");
}

/* Gets whether or not the window is decorated */
static Etk_Bool _window_decorated_get(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;

   engine_data = window->engine_data;
   return !engine_data->borderless;
}

/* Sets the mouse pointer to use */
static void _window_pointer_set(Etk_Window *window, Etk_Pointer_Type pointer_type)
{
   char *group;
   
   switch (pointer_type)
   {
      case ETK_POINTER_DND_DROP:
         group = "etk/wm_pointer_dnd_drop";
         break;
      case ETK_POINTER_MOVE:
         group = "etk/wm_pointer_dnd_move";
         break;
      case ETK_POINTER_H_DOUBLE_ARROW:
         group = "etk/wm_pointer_h_double_arrow";
         break;
      case ETK_POINTER_V_DOUBLE_ARROW:
         group = "etk/wm_pointer_v_double_arrow";
         break;
      case ETK_POINTER_RESIZE:
         group = "etk/wm_pointer_resize";
         break;
      case ETK_POINTER_RESIZE_TL:
         group = "etk/wm_pointer_resize_tl";
         break;
      case ETK_POINTER_RESIZE_T:
         group = "etk/wm_pointer_resize_t";
         break;
      case ETK_POINTER_RESIZE_TR:
         group = "etk/wm_pointer_resize_tr";
         break;
      case ETK_POINTER_RESIZE_R:
         group = "etk/wm_pointer_resize_r";
         break;
      case ETK_POINTER_RESIZE_BR:
         group = "etk/wm_pointer_resize_br";
         break;
      case ETK_POINTER_RESIZE_B:
         group = "etk/wm_pointer_resize_b";
         break;
      case ETK_POINTER_RESIZE_BL:
         group = "etk/wm_pointer_resize_bl";
         break;
      case ETK_POINTER_RESIZE_L:
         group = "etk/wm_pointer_resize_l";
         break;
      case ETK_POINTER_TEXT_EDIT:
         group = "etk/wm_pointer_text_edit";
         break;
      case ETK_POINTER_DEFAULT:
      default:
         group = "etk/wm_pointer_default";
         break;
   }
   
   if (_pointer_object)
      etk_cache_add(_pointer_cache, _pointer_object, _wm_theme_file, _pointer_group);
   free(_pointer_group);
   _pointer_group = NULL;
   
   if ((_pointer_object = etk_cache_find(_pointer_cache, _wm_theme_file, group)))
      _pointer_group = strdup(group);
   else if ((_pointer_object = etk_cache_find(_pointer_cache, _wm_theme_file, "etk/wm_pointer_default")))
      _pointer_group = strdup("etk/wm_pointer_default");
   
   if (_pointer_object)
   {
      evas_object_move(_pointer_object, _mouse_x, _mouse_y);
      evas_object_show(_pointer_object);
      evas_object_raise(_pointer_object);
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

/* Gets the position of the mouse pointer */
static void _mouse_position_get(int *x, int *y)
{
   if (x)   *x = _mouse_x;
   if (y)   *y = _mouse_y;
}

/* Gets the geometry of the screen containing the mouse pointer */
static void _mouse_screen_geometry_get(int *x, int *y, int *w, int *h)
{
   if (x)   *x = 0;
   if (y)   *y = 0;
   if (w)   *w = _fb_width;
   if (h)   *h = _fb_height;
}

/**************************
 *
 * Handlers and callbacks
 *
 **************************/

/* Called when the window is realized: it creates the border */
static void _window_realized_cb(Etk_Object *object, void *data)
{
   Etk_Window *window;
   Etk_Engine_Window_Data *engine_data;
   int border_w, border_h;
   
   if (!(window = ETK_WINDOW(object)))
      return;
   engine_data = window->engine_data;
   
   engine_data->border = edje_object_add(_evas);
   if (engine_data->borderless)
      edje_object_file_set(engine_data->border, _wm_theme_file, "etk/wm_borderless");
   else
      edje_object_file_set(engine_data->border, _wm_theme_file, "etk/wm_border");
   edje_object_part_text_set(engine_data->border, "etk.text.title", engine_data->title ? engine_data->title : "");
   
   edje_extern_object_min_size_set(ETK_WIDGET(window)->smart_object, engine_data->size.w, engine_data->size.h);
   edje_object_part_swallow(engine_data->border, "etk.swallow.content", ETK_WIDGET(window)->smart_object);
   edje_object_size_min_calc(engine_data->border, &border_w, &border_h);
   
   if (engine_data->maximized)
   {
      evas_object_move(engine_data->border, 0, 0);
      evas_object_resize(engine_data->border, _fb_width, _fb_height);
   }
   else
   {
      evas_object_move(engine_data->border, engine_data->border_position.x, engine_data->border_position.y);
      evas_object_resize(engine_data->border, border_w, border_h);
   }
   
   if (engine_data->visible)
      evas_object_show(engine_data->border);
   
   edje_object_signal_callback_add(engine_data->border, "mouse,down,1*", "etk.event.titlebar",
      _window_titlebar_mouse_down_cb, window);
   edje_object_signal_callback_add(engine_data->border, "mouse,up,1*", "etk.event.titlebar",
      _window_titlebar_mouse_up_cb, window);
   
   edje_object_signal_callback_add(engine_data->border, "mouse,in", "etk.event.resize",
      _window_resize_mouse_in_cb, window);
   edje_object_signal_callback_add(engine_data->border, "mouse,out", "etk.event.resize",
      _window_resize_mouse_out_cb, window);
   edje_object_signal_callback_add(engine_data->border, "mouse,down,1*", "etk.event.resize",
      _window_resize_mouse_down_cb, window);
   edje_object_signal_callback_add(engine_data->border, "mouse,up,1*", "etk.event.resize",
      _window_resize_mouse_up_cb, window);
   
   edje_object_signal_callback_add(engine_data->border, "mouse,clicked,1*", "etk.event.maximize",
      _window_maximize_mouse_up_cb, window);
   edje_object_signal_callback_add(engine_data->border, "mouse,clicked,1*", "etk.event.close",
      _window_close_mouse_up_cb, window);
   
   if (_pointer_object)
      evas_object_raise(_pointer_object);
}

/* Called when the window is unrealized: it destroys the border */
static void _window_unrealized_cb(Etk_Object *object, void *data)
{
   Etk_Window *window;
   Etk_Engine_Window_Data *engine_data;
   
   if (!(window = ETK_WINDOW(object)))
      return;
   engine_data = window->engine_data;
   
   if (engine_data->border)
   {
      evas_object_del(engine_data->border);
      engine_data->border = NULL;
   }
}

/* Called when the titlebar of the window is pressed */
static void _window_titlebar_mouse_down_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Etk_Window *window;
   Etk_Engine_Window_Data *engine_data;
   
   if (!(window = ETK_WINDOW(data)))
      return;
   engine_data = window->engine_data;
   
   if (!engine_data->maximized)
   {
      _window_to_drag = window;
      _window_drag_offset_x = _mouse_x - engine_data->border_position.x;
      _window_drag_offset_y = _mouse_y - engine_data->border_position.y;
   }
   
   etk_window_raise(window);
}

/* Called when the titlebar of the window is released */
static void _window_titlebar_mouse_up_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   _window_to_drag = NULL;
}

/* Called when the mouse pointer enters the resize-rect of the window's border */
static void _window_resize_mouse_in_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Etk_Window *window;
   
   if (!(window = ETK_WINDOW(data)))
      return;
   etk_toplevel_pointer_push(ETK_TOPLEVEL(window), ETK_POINTER_RESIZE_BR);
}

/* Called when the mouse pointer leaves the resize-rect of the window's border */
static void _window_resize_mouse_out_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Etk_Window *window;
   
   if (!(window = ETK_WINDOW(data)))
      return;
   etk_toplevel_pointer_pop(ETK_TOPLEVEL(window), ETK_POINTER_RESIZE_BR);
}

/* Called when the mouse presses the resize-rect of the window's border */
static void _window_resize_mouse_down_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Etk_Window *window;
   Etk_Engine_Window_Data *engine_data;
   
   if (!(window = ETK_WINDOW(data)))
      return;
   engine_data = window->engine_data;
   
   if (!engine_data->maximized)
   {
      _window_to_resize = window;
      _window_resize_offset_x = _mouse_x - engine_data->size.w;
      _window_resize_offset_y = _mouse_y - engine_data->size.h;
   }
}

/* Called when the mouse releases the resize-rect of the window's border */
static void _window_resize_mouse_up_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   _window_to_resize = NULL;
}

/* Called when the mouse releases the maximize button */
static void _window_maximize_mouse_up_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Etk_Window *window;
   
   if (!(window = ETK_WINDOW(data)))
      return;
   etk_window_maximized_set(window, !etk_window_maximized_get(window));
}

/* Called when the mouse releases the close button */
static void _window_close_mouse_up_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Etk_Window *window;
   
   if (!(window = ETK_WINDOW(data)))
      return;
   etk_window_delete_request(window);
}

/* Called when the mouse is moved */
static int _mouse_move_handler_cb(void *data, int ev_type, void *ev)
{
   Ecore_Event_Mouse_Move *event = ev;
   
   _mouse_x = event->x;
   _mouse_y = event->y;
   if (_pointer_object)
      evas_object_move(_pointer_object, _mouse_x, _mouse_y);
   
   /* Move the window to drag */
   if (_window_to_drag)
      etk_window_move(_window_to_drag, _mouse_x - _window_drag_offset_x, _mouse_y - _window_drag_offset_y);
   /* Or resize the window to resize */
   else if (_window_to_resize)
      etk_window_resize(_window_to_resize, _mouse_x - _window_resize_offset_x, _mouse_y - _window_resize_offset_y);
   
   return 1;
}

#if ENGINE_E_FB_X11_SUPPORT
/* Called when the mouse is moved (X11 debug version) */
static int _mouse_move_X_handler_cb(void *data, int ev_type, void *ev)
{
   Ecore_Event_Mouse_Move *event = ev;
   
   _mouse_x = event->x;
   _mouse_y = event->y;
   if (_pointer_object)
      evas_object_move(_pointer_object, _mouse_x, _mouse_y);
   
   /* Move the window to drag */
   if (_window_to_drag)
      etk_window_move(_window_to_drag, _mouse_x - _window_drag_offset_x, _mouse_y - _window_drag_offset_y);
   /* Or resize the window to resize */
   else if (_window_to_resize)
      etk_window_resize(_window_to_resize, _mouse_x - _window_resize_offset_x, _mouse_y - _window_resize_offset_y);
   
   return 1;
}
#endif

/**************************
 *
 * Private functions
 *
 **************************/

/* Find a window to focus that is different from the current one */
/* TODO: Maybe replace this with a focus stack or something similar, later */
Etk_Window *_window_focus_find_other(Etk_Window *current)
{
   Eina_List *toplevels;
   Etk_Window *other;

   /* We just return the first other window we find */
   for (toplevels = etk_toplevel_widgets_get(); toplevels; toplevels = toplevels->next)
   {
      if (!ETK_IS_WINDOW(toplevels->data))
         continue;
      
      other = ETK_WINDOW(toplevels->data);
      if (other != current)
         return other;
   }
   return NULL;
}

/* Cache the different mouse pointers */
static Etk_Cache *_pointer_cache_build(void)
{
   Etk_Cache *cache;
   Eina_List *groups, *l;
   Evas_Object *pointer;
   char *group;
   int w, h;
   
   cache = etk_cache_new(50);
   groups = edje_file_collection_list(_wm_theme_file);
   for (l = groups; l; l = l->next)
   {
      group = l->data;
      if (strncmp(group, "etk/wm_pointer_", 15) == 0)
      {
         pointer = edje_object_add(_evas);
         evas_object_pass_events_set(pointer, 1);
   
         edje_object_file_set(pointer, _wm_theme_file, group);
         edje_object_size_min_get(pointer, &w, &h);
         evas_object_resize(pointer, w, h);
         etk_cache_add(cache, pointer, _wm_theme_file, group);
      }
   }
   edje_file_collection_list_free(groups);
   
   return cache;
}

/* Finds the theme called "theme_name" in the subdir wm and returns its path, or NULL on failure */
static char *_theme_find(const char *theme_name)
{
   char path[PATH_MAX];
   char *home;

   if (!theme_name)
      return NULL;

   if ((home = getenv("HOME")))
   {
      snprintf(path, PATH_MAX, "%s/.e/etk/wm/%s.edj", home, theme_name);
      if (ecore_file_exists(path))
         return strdup(path);
   }

   snprintf(path, PATH_MAX, PACKAGE_DATA_DIR "/wm/%s.edj", theme_name);
   if (ecore_file_exists(path))
      return strdup(path);

   return NULL;
}
