#include <stdio.h>
#include <string.h>
#include <Ecore_Evas.h>

#include "etk_engine.h"
#include "etk_utils.h"

#include "Etk_Engine_Ecore_Evas.h"

typedef Etk_Engine_Ecore_Evas_Window_Data Etk_Engine_Window_Data;

/* General engine functions */
Etk_Engine *engine_open();

static Etk_Bool _engine_init();
static void _engine_shutdown();

/* Etk_Window functions */
static void _window_constructor(Etk_Window *window);
static void _window_destructor(Etk_Window *window);
static void _window_show(Etk_Window *window);
static void _window_hide(Etk_Window *window);
static Evas *_window_evas_get(Etk_Window *window);  
static void _window_title_set(Etk_Window *window, const char *title);
static const char *_window_title_get(Etk_Window *window);
static void _window_wmclass_set(Etk_Window *window, const char *window_name, const char *window_class);
static void _window_move(Etk_Window *window, int x, int y);
static void _window_resize(Etk_Window *window, int w, int h);
static void _window_size_min_set(Etk_Window *window, int w, int h);
static void _window_geometry_get(Etk_Window *window, int *x, int *y, int *w, int *h);
static void _window_iconify(Etk_Window *window);
static void _window_deiconify(Etk_Window *window);
static Etk_Bool _window_is_iconified(Etk_Window *window);
static void _window_maximize(Etk_Window *window);
static void _window_unmaximize(Etk_Window *window);
static Etk_Bool _window_is_maximized(Etk_Window *window);
static void _window_fullscreen(Etk_Window *window);
static void _window_unfullscreen(Etk_Window *window);
static Etk_Bool _window_is_fullscreen(Etk_Window *window);
static void _window_raise(Etk_Window *window);
static void _window_lower(Etk_Window *window);
static void _window_sticky_set(Etk_Window *window, Etk_Bool on);
static Etk_Bool _window_sticky_get(Etk_Window *window);
static void _window_focus(Etk_Window *window);
static void _window_unfocus(Etk_Window *window);  
static Etk_Bool _window_is_focused(Etk_Window *window);
static void _window_decorated_set(Etk_Window *window, Etk_Bool decorated);
static Etk_Bool _window_decorated_get(Etk_Window *window);
static void _window_shaped_set(Etk_Window *window, Etk_Bool shaped);  
static Etk_Bool _window_shaped_get(Etk_Window *window);
  
/* Etk_Window callbacks */
static void _window_move_cb(Ecore_Evas *ecore_evas);
static void _window_resize_cb(Ecore_Evas *ecore_evas);
static void _window_focus_in_cb(Ecore_Evas *ecore_evas);
static void _window_focus_out_cb(Ecore_Evas *ecore_evas);
static void _window_sticky_cb(Ecore_Evas *ecore_evas);
static void _window_unsticky_cb(Ecore_Evas *ecore_evas);
static void _window_delete_request_cb(Ecore_Evas *ecore_evas);

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
   _window_wmclass_set,
   _window_move,
   _window_resize,
   _window_size_min_set,     
   _window_geometry_get,
   NULL, /* window_center_on_window */
   NULL, /* window_move_to_mouse */
   NULL, /* window_modal_for_window */
   _window_iconify,
   _window_deiconify,
   _window_is_iconified,
   _window_maximize,
   _window_unmaximize,
   _window_is_maximized,
   _window_fullscreen,
   _window_unfullscreen,
   _window_is_fullscreen,
   _window_raise,
   _window_lower,
   _window_sticky_set,
   _window_sticky_get,
   _window_focus,
   _window_unfocus,
   _window_is_focused,
   _window_decorated_set,
   _window_decorated_get,
   _window_shaped_set,
   _window_shaped_get,
   NULL, /* window_skip_taskbar_hint_set */
   NULL, /* window_skip_taskbar_hint_get */
   NULL, /* window_skip_pager_hint_set */
   NULL, /* window_skip_pager_hint_get */
   NULL, /* window_dnd_aware_set */
   NULL, /* window_pointer_set */
   
   NULL, /* popup_window_constructor */
   NULL, /* popup_window_popup_at_xy */
   NULL, /* popup_window_popup */
   NULL, /* popup_window_popdown */
   NULL, /* popup_window_popped_get */
   
   NULL, /* drag_constructor */
   NULL, /* drag_begin */
   
   NULL, /* dnd_init */
   NULL, /* dnd_shutdown */
   
   NULL, /* clipboard_text_request */
   NULL, /* clipboard_text_set */
   
   NULL, /* selection_text_request */
   NULL, /* selection_text_set */
   NULL  /* selection_clear */     
};

Etk_Engine *engine_open()
{
   engine_info.engine_data = NULL;
   engine_info.engine_name = strdup("ecore_evas");
   return &engine_info;
}

static Etk_Bool _engine_init()
{
   if (!ecore_evas_init())
   {
      ETK_WARNING("Ecore_Evas initialization failed!");
      return ETK_FALSE;
   }
   return ETK_TRUE;
}

static void _engine_shutdown()
{
   ecore_evas_shutdown();
}

static void _window_constructor(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;

   ETK_TOPLEVEL_WIDGET(window)->evas = ecore_evas_get(engine_data->ecore_evas);
   ecore_evas_data_set(engine_data->ecore_evas, "etk_window", window);
   ecore_evas_callback_move_set(engine_data->ecore_evas, _window_move_cb);
   ecore_evas_callback_resize_set(engine_data->ecore_evas, _window_resize_cb);
   ecore_evas_callback_focus_in_set(engine_data->ecore_evas, _window_focus_in_cb);
   ecore_evas_callback_focus_out_set(engine_data->ecore_evas, _window_focus_out_cb);
   ecore_evas_callback_sticky_set(engine_data->ecore_evas, _window_sticky_cb);
   ecore_evas_callback_unsticky_set(engine_data->ecore_evas, _window_unsticky_cb);
   ecore_evas_callback_delete_request_set(engine_data->ecore_evas, _window_delete_request_cb);
}

static void _window_destructor(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;
   ecore_evas_free(engine_data->ecore_evas);
}

static void _window_show(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;
   ecore_evas_show(engine_data->ecore_evas);
}

static void _window_hide(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;
   ecore_evas_hide(engine_data->ecore_evas);
}

static Evas *_window_evas_get(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;
   return ecore_evas_get(engine_data->ecore_evas);
}

static void _window_title_set(Etk_Window *window, const char *title)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   ecore_evas_title_set(engine_data->ecore_evas, title);
}

static const char *_window_title_get(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;  
   return ecore_evas_title_get(engine_data->ecore_evas);
}

static void _window_wmclass_set(Etk_Window *window, const char *window_name, const char *window_class)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   ecore_evas_name_class_set(engine_data->ecore_evas, window_name, window_class);
}

static void _window_move(Etk_Window *window, int x, int y)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   ecore_evas_move(engine_data->ecore_evas, x, y);
}

static void _window_resize(Etk_Window *window, int w, int h)
{
   Etk_Engine_Window_Data *engine_data;
   int min_w, min_h;
   
   engine_data = window->engine_data;            
   ecore_evas_size_min_get(engine_data->ecore_evas, &min_w, &min_h);
   window->width = ETK_MAX(w, min_w);
   window->height = ETK_MAX(h, min_h);
   ecore_evas_resize(engine_data->ecore_evas, window->width, window->height);
}

static void _window_size_min_set(Etk_Window *window, int w, int h)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;      
   ecore_evas_size_min_set(engine_data->ecore_evas, w, h);
}

static void _window_geometry_get(Etk_Window *window, int *x, int *y, int *w, int *h)
{   
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   ecore_evas_geometry_get(engine_data->ecore_evas, x, y, NULL, NULL);
   if (w)
      *w = window->width;
   if (h)
      *h = window->height;
}

static void _window_iconify(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   ecore_evas_iconified_set(engine_data->ecore_evas, 1);
}

static void _window_deiconify(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   ecore_evas_iconified_set(engine_data->ecore_evas, 0);
}

static Etk_Bool _window_is_iconified(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   return ecore_evas_iconified_get(engine_data->ecore_evas);
}

static void _window_maximize(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   ecore_evas_maximized_set(engine_data->ecore_evas, 1);
}

static void _window_unmaximize(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   ecore_evas_maximized_set(engine_data->ecore_evas, 0);
}

Etk_Bool _window_is_maximized(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   return ecore_evas_maximized_get(engine_data->ecore_evas);
}

static void _window_fullscreen(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   ecore_evas_fullscreen_set(engine_data->ecore_evas, 1);
}

static void _window_unfullscreen(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   ecore_evas_fullscreen_set(engine_data->ecore_evas, 0);
}

static Etk_Bool _window_is_fullscreen(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   return ecore_evas_fullscreen_get(engine_data->ecore_evas);
}

static void _window_raise(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   return ecore_evas_raise(engine_data->ecore_evas);
}

static void _window_lower(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   return ecore_evas_lower(engine_data->ecore_evas);
}

static void _window_sticky_set(Etk_Window *window, Etk_Bool on)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   ecore_evas_sticky_set(engine_data->ecore_evas, on);
}

static Etk_Bool _window_sticky_get(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   return ecore_evas_sticky_get(engine_data->ecore_evas);
}

static void _window_focus(Etk_Window *window)
{   
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   ecore_evas_focus_set(engine_data->ecore_evas, 1);
}

static void _window_unfocus(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   ecore_evas_focus_set(engine_data->ecore_evas, 0);
}

static Etk_Bool _window_is_focused(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   return ecore_evas_focus_get(engine_data->ecore_evas);
}

static void _window_decorated_set(Etk_Window *window, Etk_Bool decorated)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   ecore_evas_borderless_set(engine_data->ecore_evas, !decorated);
}

static Etk_Bool _window_decorated_get(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   return !ecore_evas_borderless_get(engine_data->ecore_evas);   
}

static void _window_shaped_set(Etk_Window *window, Etk_Bool shaped)
{   
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;
   ecore_evas_shaped_set(engine_data->ecore_evas, shaped);   
}

static Etk_Bool _window_shaped_get(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;
   return ecore_evas_shaped_get(engine_data->ecore_evas);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the window is moved */
static void _window_move_cb(Ecore_Evas *ecore_evas)
{
   Etk_Window *window;
   
   if (!(window = ETK_WINDOW(ecore_evas_data_get(ecore_evas, "etk_window"))))
     return;
   window->move_cb(window);
}
      
/* Called when the window is resized */
static void _window_resize_cb(Ecore_Evas *ecore_evas)
{
   Etk_Window *window;
   
   if (!(window = ETK_WINDOW(ecore_evas_data_get(ecore_evas, "etk_window"))))
     return;
   
   ecore_evas_geometry_get(ecore_evas, NULL, NULL, &window->width, &window->height);
   window->resize_cb(window);
}

/* Called when the window is focused in */
static void _window_focus_in_cb(Ecore_Evas *ecore_evas)
{
   Etk_Window *window;
   
   if (!(window = ETK_WINDOW(ecore_evas_data_get(ecore_evas, "etk_window"))))
     return;
   window->focus_in_cb(window);
}

/* Called when the window is focused out */
static void _window_focus_out_cb(Ecore_Evas *ecore_evas)
{
   Etk_Window *window;
   
   if (!(window = ETK_WINDOW(ecore_evas_data_get(ecore_evas, "etk_window"))))
     return;
   window->focus_out_cb(window);
}

/* Called when the window is made sticky */
static void _window_sticky_cb(Ecore_Evas *ecore_evas)
{
   Etk_Window *window;
   
   if (!(window = ETK_WINDOW(ecore_evas_data_get(ecore_evas, "etk_window"))))
     return;
   window->sticky_cb(window);
}

/* Called when the window is made unsticky */
static void _window_unsticky_cb(Ecore_Evas *ecore_evas)
{
   Etk_Window *window;
   
   if (!(window = ETK_WINDOW(ecore_evas_data_get(ecore_evas, "etk_window"))))
     return;
   window->unsticky_cb(window);
}

/* Called when the user wants to close the window */
static void _window_delete_request_cb(Ecore_Evas *ecore_evas)
{
   Etk_Window *window;
   
   if (!(window = ETK_WINDOW(ecore_evas_data_get(ecore_evas, "etk_window"))))
     return;
   window->delete_request_cb(window);   
}
