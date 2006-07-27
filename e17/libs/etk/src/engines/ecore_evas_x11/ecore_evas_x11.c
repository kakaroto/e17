#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Evas.h>
#include <Ecore_Evas.h>
#include <Ecore_X.h>
#include <Ecore_X_Cursor.h>
#include <Ecore_X_Atoms.h>

#include "config.h"
#include "etk_types.h"
#include "etk_dnd.h"
#include "etk_engine.h"
#include "etk_utils.h"
#include "etk_toplevel_widget.h"
#include "etk_main.h"

#include "Etk_Engine_Ecore_Evas.h"
#include "Etk_Engine_Ecore_Evas_X11.h"

#define ETK_INSIDE(x, y, xx, yy, ww, hh) (((x) < ((xx) + (ww))) && ((y) < ((yy) + (hh))) && ((x) >= (xx)) && ((y) >= (yy)))

/* Engine specific data for Etk_Window
 * We do this to shorten the name for internal use
 */
typedef Etk_Engine_Ecore_Evas_X11_Window_Data Etk_Engine_Window_Data;
static Ecore_Event_Handler *_window_property_handler = NULL;
static int _window_property_cb(void *data, int ev_type, void *ev);
    
/* Engine specific data for Etk_Popup_Window */
static Ecore_X_Window _etk_popup_window_input_window = 0;
static Ecore_Event_Handler *_popup_window_key_down_handler = NULL;
static Ecore_Event_Handler *_popup_window_key_up_handler = NULL;
static Ecore_Event_Handler *_popup_window_mouse_up_handler = NULL;
static Ecore_Event_Handler *_popup_window_mouse_move_handler = NULL;
static int _popup_window_popup_timestamp = 0;
static int _popup_window_mouse_x = -100000;
static int _popup_window_mouse_y = -100000;
static Ecore_Timer *_popup_window_slide_timer = NULL;
static Evas_List *_popup_window_popped_windows = NULL;

/* General engine functions */
Etk_Engine *engine_init();

/* Etk_Window functions */
static void _window_constructor(Etk_Window *window);
static void _window_destructor(Etk_Window *window);
static void _window_center_on_window(Etk_Window *window_to_center, Etk_Window *window);
static void _window_move_to_mouse(Etk_Window *window);
static void _window_modal_for_window(Etk_Window *window_to_modal, Etk_Window *window);
static void _window_skip_taskbar_hint_set(Etk_Window *window, Etk_Bool skip_taskbar_hint);
static Etk_Bool _window_skip_taskbar_hint_get(Etk_Window *window);
static void _window_skip_pager_hint_set(Etk_Window *window, Etk_Bool skip_pager_hint);
static Etk_Bool _window_skip_pager_hint_get(Etk_Window *window);
static void _window_dnd_aware_set(Etk_Window *window, Etk_Bool on);
static void _window_pointer_set(Etk_Toplevel_Widget *toplevel_widget, Etk_Pointer_Type pointer_type);
  
/* Etk_Popup_Window functions */
static void _popup_window_constructor(Etk_Popup_Window *popup_window);
static void _popup_window_popup_at_xy(Etk_Popup_Window *popup_window, int x, int y);
static void _popup_window_popup(Etk_Popup_Window *popup_window);
static void _popup_window_popdown(Etk_Popup_Window *popup_window);  
static Evas_List **_popup_window_popped_get();

/* Etk_Popup_Window private functions */
static Etk_Popup_Window_Screen_Edge _popup_window_over_screen_edge_get(Etk_Popup_Window *popup_window);
static Etk_Popup_Window_Screen_Edge _popup_window_mouse_on_screen_edge_get();
static void _popup_window_slide_timer_update(Etk_Popup_Window *popup_window);
static int _popup_window_slide_timer_cb(void *data);
  
/* Etk_Popup_Window callbacks */
static int _popup_window_key_down_cb(void *data, int type, void *event);
static int _popup_window_key_up_cb(void *data, int type, void *event);
static int _popup_window_mouse_move_cb(void *data, int type, void *event);
static int _popup_window_mouse_up_cb(void *data, int type, void *event);

/* Etk_Drag functions */
static void _drag_constructor(Etk_Drag *drag);
static void _drag_begin(Etk_Drag *drag);


/* Etk_Drag event handlers */
static Ecore_Event_Handler *_drag_mouse_move_handler;
static Ecore_Event_Handler *_drag_mouse_up_handler;

/* Etk_Drag callbacks */
static int  _drag_mouse_up_cb(void *data, int type, void *event);
static int  _drag_mouse_move_cb(void *data, int type, void *event);

#define ETK_DND_INSIDE(x, y, xx, yy, ww, hh) (((x) < ((xx) + (ww))) && ((y) < ((yy) + (hh))) && ((x) >= (xx)) && ((y) >= (yy)))

/* Etk Dnd functions */
static Etk_Bool _dnd_init();
static void _dnd_shutdown();

/* Etk Dnd variables */
extern Etk_Widget  *_etk_selection_widget;
extern Etk_Widget  *_etk_drag_widget;
static char       **_dnd_types          = NULL;
static int          _dnd_types_num      = 0;
static Etk_Widget  *_dnd_widget         = NULL;
static Evas_List   *_dnd_handlers       = NULL;
static int          _dnd_widget_accepts = 0;

/* Etk Dnd callbacks */
static void _dnd_container_get_widgets_at(Etk_Toplevel_Widget *top, int x, int y, int offx, int offy, Evas_List **list);
static int _dnd_enter_handler(void *data, int type, void *event);
static int _dnd_position_handler(void *data, int type, void *event);
static int _dnd_drop_handler(void *data, int type, void *event);
static int _dnd_leave_handler(void *data, int type, void *event);
static int _dnd_selection_handler(void *data, int type, void *event);
static int _dnd_status_handler(void *data, int type, void *event);
static int _dnd_finished_handler(void *data, int type, void *event);

/* Etk Clipboard functions */
static void _clipboard_text_request(Etk_Widget *widget);
static void _clipboard_text_set(Etk_Widget *widget, const char *text, int length);

/* Etk Selection functions */
static void _selection_text_request(Etk_Widget *widget);
static void _selection_text_set(Etk_Widget *widget, const char *text, int length);
static void _selection_clear(void);

static Etk_Engine engine_info = {
   
   NULL, /* engine specific data */
   NULL, /* engine name */
   NULL, /* super (parent) engine */
     
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
   NULL, /* window_geometry_get */
   _window_center_on_window,
   _window_move_to_mouse,
   _window_modal_for_window,     
   NULL, /* window_iconify */
   NULL, /* window_deiconify */
   NULL, /* window_is_iconified */
   NULL, /* window_maximize */
   NULL, /* window_unmaximize */
   NULL, /* window_is_maximized */
   NULL, /* window_fullscreen */
   NULL, /* window_unfullscreen */
   NULL, /* window_is_fullscreen */
   NULL, /* window_raise */
   NULL, /* window_lower */
   NULL, /* window_sticky_set */
   NULL, /* window_sticky_get */
   NULL, /* window_focus */
   NULL, /* window_unfocus */
   NULL, /* window_if_focised */
   NULL, /* window_decorated_set */
   NULL, /* window_decorated_get */
   NULL, /* window_shaped_set */
   NULL, /* window_shaped_get */
   _window_skip_taskbar_hint_set,
   _window_skip_taskbar_hint_get,
   _window_skip_pager_hint_set,
   _window_skip_pager_hint_get,
   _window_dnd_aware_set,
   _window_pointer_set,

   _popup_window_constructor,
   _popup_window_popup_at_xy,
   _popup_window_popup,
   _popup_window_popdown,
   _popup_window_popped_get,
     
   _drag_constructor,
   _drag_begin,
     
   _dnd_init,
   _dnd_shutdown,
     
   _clipboard_text_request,
   _clipboard_text_set,
     
   _selection_text_request,
   _selection_text_set,
   _selection_clear
};

Etk_Engine * engine_init()
{
   engine_info.engine_data = NULL;
   engine_info.engine_name = strdup("ecore_evas_x11");
   etk_engine_inherit_from(&engine_info, "ecore_evas");
   return &engine_info;
}

static void _window_constructor(Etk_Window *window)
{
   /* We expect the engine that extends this one to initialize and create
    * the engine_data, ecore_evas and to get us the x_window.
    */   
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;
   /* TODO: perhaps this should be able to return with an error somehow */
   if(!engine_data)
     return;
   
   ecore_x_dnd_aware_set(engine_data->x_window, 1);
   _window_property_handler = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_PROPERTY, _window_property_cb, window);
   engine_info.super->window_constructor(window);
}

static void _window_destructor(Etk_Window *window)
{
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;
   engine_info.super->window_destructor(window);
   free(engine_data);
   ecore_event_handler_del(_window_property_handler);
   window->engine_data = NULL;
}

static void _window_center_on_window(Etk_Window *window_to_center, Etk_Window *window)
{
   int x, y, w, h;
   int cw, ch;
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   if (!window_to_center)
     return;
   
   if (window_to_center->wait_size_request)
   {
      window_to_center->center = ETK_TRUE;
      window_to_center->center_on_window = window;
      if (window)
         etk_object_weak_pointer_add(ETK_OBJECT(window), (void **)(&window_to_center->center_on_window));
   }
   else
   {
      if (window)
      {
         etk_window_geometry_get(window, &x, &y, &w, &h);
         if (window->wait_size_request)
         {
            Etk_Size size_requisition;
            
            etk_widget_size_request(ETK_WIDGET(window), &size_requisition);
            w = size_requisition.w;
            h = size_requisition.h;
         }
      }

      else
      {
#if HAVE_ECORE_X	 
         Ecore_X_Window root;
	 int screens;
	 
	 screens = ecore_x_xinerama_screen_count_get();	 
	 if (screens > 0)
	 {
	    int i;
	    
	    for (root = engine_data->x_window; ecore_x_window_parent_get(root) != 0; root = ecore_x_window_parent_get(root));
	    ecore_x_pointer_xy_get(root, &x, &y);
	    for (i = 0; i < screens; i++)
	    {	       
	       int rx, ry, rw, rh;
	       
	       root = ecore_x_xinerama_screen_geometry_get(i, &rx, &ry, &rw, &rh);
	       if (ETK_INSIDE(x, y, rx, ry, rw, rh))
	       {
		  x = rx; y = ry;
		  w = rw; h = rh;
		  goto END;
	       }	       
	    }
	    goto DEFAULT;
	 }
	 else
	 {
DEFAULT:	    
	    for (root = engine_data->x_window; ecore_x_window_parent_get(root) != 0; root = ecore_x_window_parent_get(root));
	    ecore_x_window_geometry_get(root, &x, &y, &w, &h);
	 }
#else
	 /* this is the case where we DONT have ecore_x and
	  * our window is NULL, we cant do anything.
	  */
	 return;
#endif	 
      }

END:      
      etk_window_geometry_get(window_to_center, NULL, NULL, &cw, &ch);
      ecore_evas_move(ETK_ENGINE_ECORE_EVAS_WINDOW_DATA(engine_data)->ecore_evas, x + (w - cw) / 2, y + (h - ch) / 2);
   }
}

static void _window_move_to_mouse(Etk_Window *window)
{
#if HAVE_ECORE_X   
   int x, y;
   Ecore_X_Window root;
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   
   if (!window)
     return;
   
   for (root = engine_data->x_window; ecore_x_window_parent_get(root) != 0; root = ecore_x_window_parent_get(root));
   ecore_x_pointer_xy_get(root, &x, &y);
   etk_window_move(window, x, y);
#endif      
}

static void _window_modal_for_window(Etk_Window *window_to_modal, Etk_Window *window)
{
#if HAVE_ECORE_X
   int x, y, w, h;
   int cw, ch;
   Etk_Engine_Window_Data *engine_data;
   Etk_Engine_Window_Data *engine_data2;   
   
   engine_data = window_to_modal->engine_data;
   engine_data2 = window->engine_data;   
   if (!window_to_modal)
     return;
   
   if (window_to_modal->wait_size_request)
   {
      window_to_modal->modal = ETK_TRUE;
      window_to_modal->modal_for_window = window;
      if (window)
	etk_object_weak_pointer_add(ETK_OBJECT(window), (void **)(&window_to_modal->modal_for_window));
   }
   else
   {
      if (window)
      {
	 Ecore_X_Window_State states[] = {ECORE_X_WINDOW_STATE_MODAL};
	 
	 ecore_x_icccm_transient_for_set(engine_data->x_window,
					 engine_data2->x_window);
	 
	 ecore_x_netwm_window_state_set(engine_data->x_window, states, 1);
      }
   }
#endif
}

static void _window_skip_taskbar_hint_set(Etk_Window *window, Etk_Bool skip_taskbar_hint)
{
#if HAVE_ECORE_X   
   Etk_Engine_Window_Data *engine_data;
      
   if (!window || skip_taskbar_hint == etk_window_skip_taskbar_hint_get(window))
     return;
   
   engine_data = window->engine_data;   
   if (skip_taskbar_hint)
     {
	if (etk_window_skip_pager_hint_get(window))
	  {
	     Ecore_X_Window_State states[2];
	     states[0] = ECORE_X_WINDOW_STATE_SKIP_TASKBAR;
	     states[1] = ECORE_X_WINDOW_STATE_SKIP_PAGER;
	     ecore_x_netwm_window_state_set(engine_data->x_window, states, 2);
	  }
	else
	  {
	     Ecore_X_Window_State state[1];
	     state[0] = ECORE_X_WINDOW_STATE_SKIP_TASKBAR;
	     ecore_x_netwm_window_state_set(engine_data->x_window, state, 1);
	  }
     }
   else
     {
	if (etk_window_skip_pager_hint_get(window))
	  {
	     Ecore_X_Window_State state[1];
	     state[0] = ECORE_X_WINDOW_STATE_SKIP_PAGER;
	     ecore_x_netwm_window_state_set(engine_data->x_window, state, 1);
	  }
	else
	  ecore_x_netwm_window_state_set(engine_data->x_window, NULL, 0);
     }
   etk_object_notify(ETK_OBJECT(window), "skip_taskbar");
#endif
}

static Etk_Bool _window_skip_taskbar_hint_get(Etk_Window *window)
{
#if HAVE_ECORE_X   
   unsigned int num_states, i;
   Ecore_X_Window_State *states;
   Etk_Engine_Window_Data *engine_data;
   
   if (!window)
     return ETK_FALSE;
   
   engine_data = window->engine_data;   
   ecore_x_netwm_window_state_get(engine_data->x_window, &states, &num_states);
   for (i = 0; i < num_states; i++)
   {
      if (states[i] == ECORE_X_WINDOW_STATE_SKIP_TASKBAR)
      {
	 free(states);
	 return ETK_TRUE;
      }
   }
   if (num_states > 0)
     free(states);
#endif   
   return ETK_FALSE;   
}

static void _window_skip_pager_hint_set(Etk_Window *window, Etk_Bool skip_pager_hint)
{
#if HAVE_ECORE_X
   Etk_Engine_Window_Data *engine_data;
        
   if (!window || skip_pager_hint == etk_window_skip_pager_hint_get(window))
     return;
   
   engine_data = window->engine_data;   
   if (skip_pager_hint)
   {
      if (etk_window_skip_taskbar_hint_get(window))
       {
	  Ecore_X_Window_State states[2];
	  states[0] = ECORE_X_WINDOW_STATE_SKIP_TASKBAR;
	  states[1] = ECORE_X_WINDOW_STATE_SKIP_PAGER;
	  ecore_x_netwm_window_state_set(engine_data->x_window, states, 2);
       }
      else
      {
	 Ecore_X_Window_State state[1];
	 state[0] = ECORE_X_WINDOW_STATE_SKIP_PAGER;
	 ecore_x_netwm_window_state_set(engine_data->x_window, state, 1);
      }
   }
   else
   {
      if (etk_window_skip_taskbar_hint_get(window))
      {
	 Ecore_X_Window_State state[1];
	 state[0] = ECORE_X_WINDOW_STATE_SKIP_TASKBAR;
	 ecore_x_netwm_window_state_set(engine_data->x_window, state, 1);
      }
      else
	ecore_x_netwm_window_state_set(engine_data->x_window, NULL, 0);
   }
   etk_object_notify(ETK_OBJECT(window), "skip_pager");
#endif   
}

static Etk_Bool _window_skip_pager_hint_get(Etk_Window *window)
{
#if HAVE_ECORE_X   
   unsigned int num_states, i;
   Ecore_X_Window_State *states;
   Etk_Engine_Window_Data *engine_data;
     
   if (!window)
     return ETK_FALSE;
   
   engine_data = window->engine_data;   
   ecore_x_netwm_window_state_get(engine_data->x_window, &states, &num_states);
   for (i = 0; i < num_states; i++)
   {
      if (states[i] == ECORE_X_WINDOW_STATE_SKIP_PAGER)
      {
	 free(states);
	 return ETK_TRUE;
      }
   }
   if (num_states > 0)
     free(states);
#endif   
   return ETK_FALSE;
}

static void _window_dnd_aware_set(Etk_Window *window, Etk_Bool on)
{
#if HAVE_ECORE_X
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = window->engine_data;   
   ecore_x_dnd_aware_set(engine_data->x_window, on);
#endif
}

static void _window_pointer_set(Etk_Toplevel_Widget *toplevel_widget, Etk_Pointer_Type pointer_type)
{
#if 0
   /* TODO: for now this is totally done in the parent engine, we need to fix */
   
#if HAVE_ECORE_X
   Etk_Window *window;
   int x_pointer_type = ECORE_X_CURSOR_LEFT_PTR;
   Ecore_X_Cursor cursor;
   Etk_Engine_Window_Data *engine_data;

   /* TODO: do we want to move the following line to etk_window ? */
   if (!(window = ETK_WINDOW(toplevel_widget)))
      return;
   
   engine_data = window->engine_data;

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

   if ((cursor = ecore_x_cursor_shape_get(x_pointer_type)))
      ecore_x_window_cursor_set(ecore_evas_software_x11_window_get(ETK_ENGINE_ECORE_EVAS_WINDOW_DATA(engine_data)->ecore_evas), cursor);
   else
      ETK_WARNING("Unable to find the X cursor \"%d\"", pointer_type);
#endif   
#endif   
}  

static void _popup_window_constructor(Etk_Popup_Window *popup_window)
{   
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = ETK_WINDOW(popup_window)->engine_data;   
   ecore_x_netwm_window_type_set(engine_data->x_window, ECORE_X_WINDOW_TYPE_MENU);
   /* TODO: this can be done using ecore_evas */
   ecore_x_window_override_set(engine_data->x_window, 1);
}

static void _popup_window_popup_at_xy(Etk_Popup_Window *popup_window, int x, int y)
{
#if HAVE_ECORE_X
   Etk_Engine_Window_Data *engine_data;
   
   engine_data = ETK_WINDOW(popup_window)->engine_data;
   
   if (_etk_popup_window_input_window == 0)
   {
      Ecore_X_Window root, parent;
      int root_x, root_y, root_w, root_h;
      
      root = engine_data->x_window;

      while ((parent = ecore_x_window_parent_get(root)) != 0)
	root = parent;
      
      ecore_x_window_geometry_get(root, &root_x, &root_y, &root_w, &root_h);
      _etk_popup_window_input_window = ecore_x_window_input_new(root, root_x, root_y, root_w, root_h);
      ecore_x_window_show(_etk_popup_window_input_window);
      /* TODO: fixme pointer_grab!! */
      /* ecore_x_pointer_confine_grab(_etk_popup_window_input_window); */
      ecore_x_keyboard_grab(_etk_popup_window_input_window);
      
      _popup_window_key_down_handler = ecore_event_handler_add(ECORE_X_EVENT_KEY_DOWN, _popup_window_key_down_cb, popup_window);
      _popup_window_key_up_handler = ecore_event_handler_add(ECORE_X_EVENT_KEY_UP, _popup_window_key_up_cb, popup_window);
      _popup_window_mouse_up_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_UP, _popup_window_mouse_up_cb, popup_window);
      _popup_window_mouse_move_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_MOVE, _popup_window_mouse_move_cb, popup_window);
      
      _popup_window_popup_timestamp = ecore_x_current_time_get();
   }
   
   etk_window_move(ETK_WINDOW(popup_window), x, y);
   etk_widget_show(ETK_WIDGET(popup_window));
   evas_event_feed_mouse_move(ETK_TOPLEVEL_WIDGET(popup_window)->evas, -100000, -100000, ecore_x_current_time_get(), NULL);
   evas_event_feed_mouse_in(ETK_TOPLEVEL_WIDGET(popup_window)->evas, ecore_x_current_time_get(), NULL);
   _popup_window_popped_windows = evas_list_append(_popup_window_popped_windows, popup_window);
   
   etk_popup_window_focused_window_set(popup_window);
   /* TODO: this still doesnt work with TWM */
   etk_window_raise(ETK_WINDOW(popup_window));
   _popup_window_slide_timer_update(popup_window);   
#endif
}

static void _popup_window_popup(Etk_Popup_Window *popup_window)
{
#if HAVE_ECORE_X   
   int x, y;
   
   ecore_x_pointer_last_xy_get(&x, &y);
   etk_popup_window_popup_at_xy(popup_window, x + 1, y + 1);
#endif   
}

static void _popup_window_popdown(Etk_Popup_Window *popup_window)
{
#if HAVE_ECORE_X      
   if (!_popup_window_popped_windows)
   {
      /* TODO: pointer ungrab, fixme!! */
      /* ecore_x_pointer_ungrab(); */
      ecore_x_keyboard_ungrab();
      ecore_x_window_del(_etk_popup_window_input_window);
      _etk_popup_window_input_window = 0;
      
      ecore_event_handler_del(_popup_window_key_down_handler);
      ecore_event_handler_del(_popup_window_key_up_handler);
      ecore_event_handler_del(_popup_window_mouse_up_handler);
      ecore_event_handler_del(_popup_window_mouse_move_handler);
      _popup_window_key_down_handler = NULL;
      _popup_window_key_up_handler = NULL;
      _popup_window_mouse_up_handler = NULL;
      _popup_window_mouse_move_handler = NULL;
   }   
#endif   
}

static Evas_List **_popup_window_popped_get()
{
   return &_popup_window_popped_windows;
}  

/* Returns a flag incating on which edges of the screen the popup window is over */
static Etk_Popup_Window_Screen_Edge _popup_window_over_screen_edge_get(Etk_Popup_Window *popup_window)
{
   Etk_Popup_Window_Screen_Edge result = ETK_POPUP_WINDOW_NO_EDGE;
#if HAVE_ECORE_X   
   int rx, ry, rw, rh;
   int px, py, pw, ph;

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
#endif
   return result;
}

/* Returns a flag incating on which edges of the screen the mouse pointer is */
static Etk_Popup_Window_Screen_Edge _etk_popup_window_mouse_on_screen_edge_get()
{
   Etk_Popup_Window_Screen_Edge result = ETK_POPUP_WINDOW_NO_EDGE;   
#if HAVE_ECORE_X   
   int rx, ry, rw, rh;

   if (_etk_popup_window_input_window == 0)
      return ETK_POPUP_WINDOW_NO_EDGE;

   ecore_x_window_geometry_get(_etk_popup_window_input_window, &rx, &ry, &rw, &rh);
   if (_popup_window_mouse_x - rx + 1 >= rw)
      result |= ETK_POPUP_WINDOW_RIGHT_EDGE;
   if (_popup_window_mouse_x <= rx)
      result |= ETK_POPUP_WINDOW_LEFT_EDGE;
   if (_popup_window_mouse_y - ry + 1 >= rh)
      result |= ETK_POPUP_WINDOW_BOTTOM_EDGE;
   if (_popup_window_mouse_y <= ry)
      result |= ETK_POPUP_WINDOW_TOP_EDGE;
#endif
   return result;
}

/* Starts the slide timer if needed */
static void _popup_window_slide_timer_update(Etk_Popup_Window *popup_window)
{
   Etk_Popup_Window_Screen_Edge mouse_on_edge, window_over_edge;

   if (!popup_window)
      return;

   mouse_on_edge = _etk_popup_window_mouse_on_screen_edge_get();
   window_over_edge = _popup_window_over_screen_edge_get(popup_window);
   if (!_popup_window_slide_timer && (mouse_on_edge & window_over_edge) != ETK_POPUP_WINDOW_NO_EDGE)
      _popup_window_slide_timer = ecore_timer_add(1.0 / 60.0, _popup_window_slide_timer_cb, NULL);
}

/* Makes the popup windows slide (called every 1/60 sec) */
static int _popup_window_slide_timer_cb(void *data)
{
#if HAVE_ECORE_X   
   Etk_Popup_Window *popup_window = NULL, *pwin;
   Evas_List *l;
   Etk_Popup_Window_Screen_Edge mouse_edge, window_edge;
   int rx, ry, rw, rh;
   int px, py, pw, ph;
   int x, y;
   int dx = 0, dy = 0, max_delta = (int)(1.0 / 60.0 * 800);

   /* We first look for the popup window that is over an edge of the screen */
   for (l = _popup_window_popped_windows; l; l = l->next)
   {
      pwin = ETK_POPUP_WINDOW(l->data);
      if ((window_edge = _popup_window_over_screen_edge_get(pwin)) != ETK_POPUP_WINDOW_NO_EDGE)
      {
         popup_window = pwin;
         break;
      }
   }
   if (!popup_window)
   {
      _popup_window_slide_timer = NULL;
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
      _popup_window_slide_timer = NULL;
      return 0;
   }

   for (l = _popup_window_popped_windows; l; l = l->next)
   {
      pwin = ETK_POPUP_WINDOW(l->data);
      etk_window_geometry_get(ETK_WINDOW(pwin), &x, &y, NULL, NULL);
      etk_window_move(ETK_WINDOW(pwin), x + dx, y + dy);

      /* We feed a mouse move event since the relative position between the mouse pointer and the popup window has changed */
      evas_event_feed_mouse_move(ETK_TOPLEVEL_WIDGET(pwin)->evas, _popup_window_mouse_x - x, _popup_window_mouse_y - y, ecore_x_current_time_get(), NULL);
   }
#endif
   return 1;
}

static void _drag_constructor(Etk_Drag *drag)
{
#if HAVE_ECORE_X   
   Etk_Engine_Window_Data *engine_data;
   Ecore_X_Window x_window;
   
   engine_data = ETK_WINDOW(drag)->engine_data;
   x_window = engine_data->x_window;
   ecore_x_dnd_aware_set(x_window, 1);
#endif   
}

static void _drag_begin(Etk_Drag *drag)
{
#if HAVE_ECORE_X
   Etk_Engine_Window_Data *engine_data;
   Ecore_Evas *ecore_evas;
   Ecore_X_Window x_window;
   
   engine_data = ETK_WINDOW(drag)->engine_data;   
   ecore_evas = ETK_ENGINE_ECORE_EVAS_WINDOW_DATA(engine_data)->ecore_evas;
   x_window = engine_data->x_window;
   
   ecore_evas_ignore_events_set(ecore_evas, 1);
   ecore_x_dnd_types_set(x_window, drag->types, drag->num_types);
   ecore_x_dnd_begin(x_window, drag->data, drag->data_size);

   _drag_mouse_move_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_MOVE, _drag_mouse_move_cb, drag);
   _drag_mouse_up_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_UP, _drag_mouse_up_cb, drag);
#endif   
}

static Etk_Bool _dnd_init()
{
   if (_dnd_handlers)
     return ETK_TRUE;
   
#if HAVE_ECORE_X
   _dnd_handlers = evas_list_append(_dnd_handlers, ecore_event_handler_add(ECORE_X_EVENT_XDND_ENTER, _dnd_enter_handler, NULL));
   _dnd_handlers = evas_list_append(_dnd_handlers, ecore_event_handler_add(ECORE_X_EVENT_XDND_POSITION, _dnd_position_handler, NULL));
   _dnd_handlers = evas_list_append(_dnd_handlers, ecore_event_handler_add(ECORE_X_EVENT_XDND_DROP, _dnd_drop_handler, NULL));
   _dnd_handlers = evas_list_append(_dnd_handlers, ecore_event_handler_add(ECORE_X_EVENT_XDND_LEAVE, _dnd_leave_handler, NULL));
   _dnd_handlers = evas_list_append(_dnd_handlers, ecore_event_handler_add(ECORE_X_EVENT_SELECTION_NOTIFY, _dnd_selection_handler, NULL));
   _dnd_handlers = evas_list_append(_dnd_handlers, ecore_event_handler_add(ECORE_X_EVENT_XDND_STATUS, _dnd_status_handler, NULL));
   _dnd_handlers = evas_list_append(_dnd_handlers, ecore_event_handler_add(ECORE_X_EVENT_XDND_FINISHED, _dnd_finished_handler, NULL));
#endif
   
   return ETK_TRUE;   
}

static void _dnd_shutdown()
{
#if HAVE_ECORE_X
   while (_dnd_handlers)
   {
      ecore_event_handler_del(_dnd_handlers->data);
      _dnd_handlers = evas_list_remove(_dnd_handlers, _dnd_handlers->data);
   }
#endif      
}

static void _clipboard_text_request(Etk_Widget *widget)
{
#if HAVE_ECORE_X
   Etk_Engine_Window_Data *engine_data;
   Ecore_X_Window win;
   
   engine_data = ETK_WINDOW(widget->toplevel_parent)->engine_data;
   win = engine_data->x_window;   
   _etk_selection_widget = widget;
   ecore_x_selection_clipboard_request(win, ECORE_X_SELECTION_TARGET_UTF8_STRING);
#endif
}  

static void _clipboard_text_set(Etk_Widget *widget, const char *text, int length)
{  
#if HAVE_ECORE_X
   Etk_Engine_Window_Data *engine_data;
   Ecore_X_Window win;
   
   engine_data = ETK_WINDOW(widget->toplevel_parent)->engine_data;
   win = engine_data->x_window;
   ecore_x_selection_clipboard_set(win, (char *)text, length);
#endif
}

static void _selection_text_request(Etk_Widget *widget)
{
#if HAVE_ECORE_X
   Etk_Engine_Window_Data *engine_data;
   Ecore_X_Window win;
   
   engine_data = ETK_WINDOW(widget->toplevel_parent)->engine_data;
   win = engine_data->x_window;   
   _etk_selection_widget = widget;
   ecore_x_selection_primary_request(win, ECORE_X_SELECTION_TARGET_UTF8_STRING);
#endif
}  

static void _selection_text_set(Etk_Widget *widget, const char *text, int length)
{  
#if HAVE_ECORE_X
   Etk_Engine_Window_Data *engine_data;
   Ecore_X_Window win;
   
   engine_data = ETK_WINDOW(widget->toplevel_parent)->engine_data;
   win = engine_data->x_window;
   ecore_x_selection_primary_set(win, (char *)text, length);
#endif
}

static void _selection_clear()
{
#if HAVE_ECORE_X
   ecore_x_selection_primary_clear();
#endif
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

static int _window_property_cb(void *data, int ev_type, void *event)
{
   Etk_Window *window;
   Ecore_X_Event_Window_Property *ev;
   int sticky;
   
   if (!(window = ETK_WINDOW(data)) || !(ev = event))
     return 1;
   
   if (ev->atom == ECORE_X_ATOM_NET_WM_STATE)
   {
      unsigned int i, num;
      Ecore_X_Window_State *state;
      Etk_Engine_Window_Data *engine_data;
      Ecore_X_Window win;
      
      engine_data = window->engine_data;
      win = engine_data->x_window;      
      ecore_x_netwm_window_state_get(win, &state, &num);
      
      sticky = 0;      
      
      if (state)
      {
	 for (i = 0; i < num; i++)
	 {
	    switch (state[i])
	    {
	       case ECORE_X_WINDOW_STATE_STICKY:
	          window->sticky = 1;
	          sticky = 1;
	          etk_object_notify(ETK_OBJECT(window), "sticky");
	          break;
	    }
	 }
      }
      
      if (window->sticky == 1 && sticky == 0)
      {
	 window->sticky = 0;
	 etk_object_notify(ETK_OBJECT(window), "sticky");
      }
   }
}

/* Called when the user presses a key on the input window: we just feed it */
static int _popup_window_key_down_cb(void *data, int type, void *event)
{
#if HAVE_ECORE_X   
   Etk_Popup_Window *popup_window;
   Ecore_X_Event_Key_Down *key_event;
   
   if (!(popup_window = ETK_POPUP_WINDOW(data)) || !(key_event = event) || key_event->win != _etk_popup_window_input_window)
     return 1;
   if (!etk_popup_window_focused_window_get())
     return 1;
   
   evas_event_feed_key_down(ETK_TOPLEVEL_WIDGET(etk_popup_window_focused_window_get())->evas, key_event->keyname,
			    key_event->keysymbol, key_event->key_compose, NULL, key_event->time, NULL);
#endif   
   return 1;
}

/* Called wgen the user releases a key on the input window: we just feed it */
static int _popup_window_key_up_cb(void *data, int type, void *event)
{
#if HAVE_ECORE_X   
   Etk_Popup_Window *popup_window;
   Ecore_X_Event_Key_Up *key_event;
   
   if (!(popup_window = ETK_POPUP_WINDOW(data)) || !(key_event = event) || key_event->win != _etk_popup_window_input_window)
     return 1;
   if (!etk_popup_window_focused_window_get())
     return 1;
   
   evas_event_feed_key_up(ETK_TOPLEVEL_WIDGET(etk_popup_window_focused_window_get())->evas, key_event->keyname,
			  key_event->keysymbol, key_event->key_compose, NULL, key_event->time, NULL);
#endif   
   return 1;
}

/*
 * Called when the user clicks on the input window:
 * it pops down the opened popup windows if needed and feeds the mouse up event to the popup window
 */

static int _popup_window_mouse_up_cb(void *data, int type, void *event)
{
#if HAVE_ECORE_X   
   Etk_Popup_Window *popup_window, *pwin;
   Evas_List *l;
   Ecore_X_Event_Mouse_Button_Up *mouse_event;
   Etk_Bool pointer_over_window = ETK_FALSE;
   
   if (!(popup_window = ETK_POPUP_WINDOW(data)) || !(mouse_event = event) || mouse_event->win != _etk_popup_window_input_window)
     return 1;
   
   /* If the user clicks on a popped window, we feed the event */
   for (l = _popup_window_popped_windows; l; l = l->next)
   {
      int px, py, pw, ph;
      
      pwin = ETK_POPUP_WINDOW(l->data);
      etk_window_geometry_get(ETK_WINDOW(pwin), &px, &py, &pw, &ph);

      if (_popup_window_mouse_x >= px && _popup_window_mouse_x <= px + pw && _popup_window_mouse_y >= py && _popup_window_mouse_y <= py + ph)
      {
	 pointer_over_window = ETK_TRUE;
	 evas_event_feed_mouse_up(ETK_TOPLEVEL_WIDGET(pwin)->evas, mouse_event->button, EVAS_BUTTON_NONE, mouse_event->time, NULL);
	 break;
      }
   }
   /* Otherwize, we pop down all the popup windows */
   if (!pointer_over_window && (mouse_event->time - _popup_window_popup_timestamp) >= ETK_POPUP_WINDOW_MIN_POP_TIME)
     etk_popup_window_popdown_all();
#endif
   return 1;
}

/*
 * Called when the user moves the mouse above the popup input window:
 * It feeds the mouse move, in and out events to the popup windows and starts to make the popup windows slide if needed
 */
static int _popup_window_mouse_move_cb(void *data, int type, void *event)
{
#if HAVE_ECORE_X   
   Etk_Popup_Window *popup_window, *pwin;
   Evas_List *l;
   Ecore_X_Event_Mouse_Move *mouse_event;
   int px, py;
   
   if (!(popup_window = ETK_POPUP_WINDOW(data)) || !(mouse_event = event) || mouse_event->win != _etk_popup_window_input_window)
     return 1;
   
   _popup_window_mouse_x = mouse_event->x;
   _popup_window_mouse_y = mouse_event->y;
   
   for (l = _popup_window_popped_windows; l; l = l->next)
     {
	pwin = ETK_POPUP_WINDOW(l->data);
	etk_window_geometry_get(ETK_WINDOW(pwin), &px, &py, NULL, NULL);
	evas_event_feed_mouse_move(ETK_TOPLEVEL_WIDGET(pwin)->evas, mouse_event->x - px, mouse_event->y - py, mouse_event->time, NULL);
	
	/* Start to make the popup window slide if needed */
	_popup_window_slide_timer_update(pwin);
     }
#endif
   return 1;
}

static int _drag_mouse_up_cb(void *data, int type, void *event)
{
#if HAVE_ECORE_X   
   Etk_Drag *drag;
   
   drag = data;
   etk_widget_hide_all(ETK_WIDGET(drag));
   ecore_event_handler_del(_drag_mouse_move_handler);
   ecore_event_handler_del(_drag_mouse_up_handler);
   ecore_x_dnd_drop();   
   etk_widget_drag_end(ETK_WIDGET(drag));   
   etk_toplevel_widget_pointer_push(etk_widget_toplevel_parent_get(drag->widget), ETK_POINTER_DEFAULT);
#endif   
   return 1;
}

static int _drag_mouse_move_cb(void *data, int type, void *event)
{
#if HAVE_ECORE_X   
   Ecore_X_Event_Mouse_Move *ev;
   Etk_Drag *drag;
   
   drag = data;
   ev = event;
   
   etk_window_move(ETK_WINDOW(drag), ev->root.x + 2, ev->root.y + 2);
#endif   
   return 1;
}

#if HAVE_ECORE_X
/**
 * @brief Search the container recursively for the widget that accepts xdnd
 * @param top top level widget
 * @param x the x coord we're searching under
 * @param y the y coord we're searching under
 * @param xoff the x offset for the window
 * @param yoff the y offset for the window
 * @param list the evas list we're going to append widgets to
 */
static void _dnd_container_get_widgets_at(Etk_Toplevel_Widget *top, int x, int y, int offx, int offy, Evas_List **list)
{

   Evas_List *l;
   int wx, wy, ww, wh;
   
   if (!top || !list)
      return;
   
   for (l = etk_widget_dnd_dest_widgets_get(); l; l = l->next)
   {
      Etk_Widget *widget;
      
      if (!(widget = ETK_WIDGET(l->data)) || etk_widget_toplevel_parent_get(widget) != top)
         continue;
      
      etk_widget_geometry_get(widget, &wx, &wy, &ww, &wh);
      if (ETK_DND_INSIDE(x, y, wx + offx, wy + offy, ww, wh))
	 *list = evas_list_append(*list, widget);
   }
}

/**
 * @brief The event handler for when a drag enters our window
 */
static int _dnd_enter_handler(void *data, int type, void *event)
{
   Ecore_X_Event_Xdnd_Enter *ev;
   int i;
   
   ev = event;
   
   //printf("enter window!\n");   
   //for (i = 0; i < ev->num_types; i++)
   //  printf("type: %s\n", ev->types[i]);   
   
   if(_dnd_types != NULL && _dnd_types_num >= 0)
   {
      for (i = 0; i < _dnd_types_num; i++)
         if(_dnd_types[i]) free(_dnd_types[i]);
   }
   
   if(_dnd_types != NULL) free(_dnd_types);
   _dnd_types_num = 0;
   
   if(ev->num_types > 0)
   {
      _dnd_types = calloc(ev->num_types, sizeof(char*));
      for (i = 0; i < ev->num_types; i++)
	_dnd_types[i] = strdup(ev->types[i]);

      _dnd_types_num = ev->num_types;
   }
   
   return 1;
}

/**
 * @brief The event handler for when a drag is moving in our window
 */
static int _dnd_position_handler(void *data, int type, void *event)
{
   Ecore_X_Event_Xdnd_Position *ev;
   Etk_Window *window;
   Evas_List *l;
   Evas_List *children = NULL;
   Etk_Widget *widget;
   int x = 0, y = 0;
   int wx, wy, ww, wh;
   Etk_Engine_Window_Data *engine_data;
   Ecore_X_Window x_window;      
   
   ev = event;

   //printf("position!\n");
   
   /* loop top level widgets (windows) */
   for (l = etk_main_toplevel_widgets_get(); l; l = l->next)
   {
      if (!ETK_IS_WINDOW(l->data))
         continue;
      window = ETK_WINDOW(l->data);
	
      /* if this isnt the active window, dont waste time */
      engine_data = window->engine_data;
      x_window = engine_data->x_window;
      if (ev->win != x_window)
         continue;
      
      etk_window_geometry_get(window, &x, &y, NULL, NULL);

      /* find the widget we want to drop on */
      _dnd_container_get_widgets_at(ETK_TOPLEVEL_WIDGET(window), ev->position.x, ev->position.y, x, y, &children);
      
      /* check if we're leaving a widget */
      if (_dnd_widget)
      {
         etk_widget_geometry_get(_dnd_widget, &wx, &wy, &ww, &wh);
         if (!ETK_DND_INSIDE(ev->position.x, ev->position.y, wx + x, wy + y, ww, wh))
         {
            etk_widget_drag_leave(_dnd_widget);
            _dnd_widget = NULL;
	    _dnd_widget_accepts = 0;
         }
      }
      
      break;
   }
   
   /* if we found a widget, emit signals */
   if (children != NULL)
   {	
      Ecore_X_Rectangle rect;
      int i;
      
      widget = (evas_list_last(children))->data;
      etk_widget_geometry_get(widget, &wx, &wy, &ww, &wh);

      rect.x = wx;
      rect.y = wy;
      rect.width = ww;
      rect.height = wh;
      
      if(_dnd_widget == widget && _dnd_widget_accepts)
      {
	 etk_widget_drag_motion(widget);
	 ecore_x_dnd_send_status(1, 1, rect, ECORE_X_DND_ACTION_PRIVATE);
      }
      else
      {      
	 _dnd_widget = widget;
      
	 /* first case - no specific types, so just accept */
	 if(_dnd_widget->dnd_types == NULL || _dnd_widget->dnd_types_num <= 0)
	 {
	    ecore_x_dnd_send_status(1, 1, rect, ECORE_X_DND_ACTION_PRIVATE);
	    _dnd_widget_accepts = 1;
	    etk_widget_drag_enter(widget);
	    return 1;
	 }
	 
	 /* second case - we found matching types, accept */
	 for(i = 0; i < _dnd_types_num; i++)
	   {
	      int j;
	      
	      for(j = 0; j < _dnd_widget->dnd_types_num; j++)
		{
		   if(!strcmp(_dnd_widget->dnd_types[j], _dnd_types[i]))
		   {
		      ecore_x_dnd_send_status(1, 1, rect, ECORE_X_DND_ACTION_PRIVATE);
		      _dnd_widget_accepts = 1;
		      etk_widget_drag_enter(widget);
		      return 1;
		   }
		}
	   }
	 
	 /* third case - no matches at all, dont accept */
	 ecore_x_dnd_send_status(0, 1, rect, ECORE_X_DND_ACTION_PRIVATE);
	 _dnd_widget_accepts = 0;
      }
   }
   else
   {
      /* tell the source we wont accept it here */
      Ecore_X_Rectangle rect;
      
      rect.x = 0;
      rect.y = 0;
      rect.width = 0;
      rect.height = 0;
      ecore_x_dnd_send_status(0, 1, rect, ECORE_X_DND_ACTION_PRIVATE);
   }
   return 1;
}

/* TODO: doc */
static int _dnd_drop_handler(void *data, int type, void *event)
{
   Ecore_X_Event_Xdnd_Drop *ev;
   int i;
   
   //printf("drop\n");
   ev = event;
   
	if (!_dnd_widget)
		return 0;

   /* first case - if we dont have a type preferece, send everyting */
   if(_dnd_widget->dnd_types == NULL || _dnd_widget->dnd_types_num <= 0)
   {
      for(i = 0; i < _dnd_types_num; i++)
	ecore_x_selection_xdnd_request(ev->win, _dnd_types[i]);
   }
   /* second case - send only our preferred types */
   else
   {
      for(i = 0; i < _dnd_widget->dnd_types_num; i++)
	ecore_x_selection_xdnd_request(ev->win, _dnd_widget->dnd_types[i]);
   }
   
   return 1;
}

/* TODO: doc */
static int _dnd_leave_handler(void *data, int type, void *event)
{
   //printf("leave window\n");
      
   return 1;
}

/* TODO: doc */
static int _dnd_selection_handler(void *data, int type, void *event)
{
   Ecore_X_Event_Selection_Notify *ev;
   Ecore_X_Selection_Data *sel;
   Ecore_X_Selection_Data_Files *files;
   Ecore_X_Selection_Data_Text *text;
   Ecore_X_Selection_Data_Targets *targets;
   //int i;

   //printf("selection\n"); 
   ev = event;
   switch (ev->selection) 
   {
      case ECORE_X_SELECTION_PRIMARY:
         if (!strcmp(ev->target, ECORE_X_SELECTION_TARGET_TARGETS)) 
	 {
	    /* printf("primary: %s\n", ev->target); */
	    targets = ev->data;
	    /* 
	    for (i = 0; i < targets->num_targets; i++)
	       printf("target: %s\n", targets->targets[i]);
	    */
         } 
	 else 
	 {	    
	    /* emit signal to widget that the clipboard text is sent to it */
	    Etk_Event_Selection_Request event;
	    Etk_Selection_Data_Text     event_text;
	    
	    text = ev->data;
	   
	    if (!_etk_selection_widget)
	      break;	    
	    
	    event_text.text = text->text;
	    event_text.data.data = text->data.data;
	    event_text.data.length = text->data.length;
	    event_text.data.free = text->data.free;
	    
	    event.data = &event_text;
	    event.content = ETK_SELECTION_CONTENT_TEXT;	    	    
	    
	    etk_widget_selection_received(_etk_selection_widget, &event);
	 }
	 break;
      
      case ECORE_X_SELECTION_SECONDARY:
         sel = ev->data;
         //printf("secondary: %s %s\n", ev->target, sel->data);
         break;
      
      case ECORE_X_SELECTION_XDND:
        if(!strcmp(ev->target, "text/uri-list"))
	 {
	    Etk_Event_Selection_Request event;	    
	    Etk_Selection_Data_Files    event_files;
	    
	    files = ev->data;
	
	    if (!_dnd_widget || files->num_files < 1)
	      break;		    
	    
	    event_files.files = files->files;
	    event_files.num_files = files->num_files;
	    event_files.data.data = files->data.data;
	    event_files.data.length = files->data.length;
	    event_files.data.free = files->data.free;
	    
	    event.data = &event_files;
	    event.content = ETK_SELECTION_CONTENT_FILES;	   
	    
	    /* emit the drop signal so the widget can react */
	    etk_widget_drag_drop(_dnd_widget, &event);
	 }
         else if(!strcmp(ev->target, "text/plain") || 
		 !strcmp(ev->target, ECORE_X_SELECTION_TARGET_UTF8_STRING))
	 {
	    Etk_Event_Selection_Request event;
	    Etk_Selection_Data_Text     event_text;
	    
	    text = ev->data;
	   
	    if (!_dnd_widget)
	      break;	    
	    
	    event_text.text = text->text;
	    event_text.data.data = text->data.data;
	    event_text.data.length = text->data.length;
	    event_text.data.free = text->data.free;
	    
	    event.data = &event_text;
	    event.content = ETK_SELECTION_CONTENT_TEXT;	    
	    
	    /* emit the drop signal so the widget can react */
	    etk_widget_drag_drop(_dnd_widget, &event);
	 }
         else
	 {
	    /* couldnt find any data type that etk supports, send raw data */
	    Etk_Event_Selection_Request event;
	    
	    event.data = ev->data;
	    event.content = ETK_SELECTION_CONTENT_CUSTOM;
	    
	    /* emit the drop signal so the widget can react */
	    etk_widget_drag_drop(_dnd_widget, &event);
	 }
      
	 _dnd_widget = NULL;	 
	 
         ecore_x_dnd_send_finished();
         break;
	
      case ECORE_X_SELECTION_CLIPBOARD:
         if (!strcmp(ev->target, ECORE_X_SELECTION_TARGET_TARGETS)) 
	 {
	    /* REDO THIS CODE!!!
	    
	    Etk_Event_Selection_Get event;
	    Etk_Selection_Data_Targets _targets;
	    
	    event.content = ETK_SELECTION_CONTENT_TARGETS;

	    targets = ev->data;
	    
            _targets.num_targets = targets->num_targets;
	    _targets.targets = targets->targets;
	    
	    //printf("clipboard: %s\n", ev->target);
	    //for (i = 0; i < targets->num_targets; i++)
	    //  printf("target: %s\n", targets->targets[i]);
	    
	    */
         } 
         else 
	 {
	    /* emit signal to widget that the clipboard text is sent to it */
	    Etk_Event_Selection_Request event;
	    Etk_Selection_Data_Text     event_text;
	    
	    text = ev->data;
	   
	    if (!_etk_selection_widget)
	      break;	    
	    
	    event_text.text = text->text;
	    event_text.data.data = text->data.data;
	    event_text.data.length = text->data.length;
	    event_text.data.free = text->data.free;
	    
	    event.data = &event_text;
	    event.content = ETK_SELECTION_CONTENT_TEXT;	    
	    
	    etk_widget_clipboard_received(_etk_selection_widget, &event);
	 }
	 break;
         
      default:
         break;
   }
   
   return 1;
}

static int _dnd_status_handler(void *data, int type, void *event)
{
   Ecore_X_Event_Xdnd_Status *ev;
   Etk_Engine_Window_Data *engine_data;  
   Ecore_X_Window x_window;
      
   engine_data = ETK_WINDOW(_etk_drag_widget)->engine_data;   
   x_window = engine_data->x_window;   
   ev = event;
       
   if (ev->win != x_window) return 1;	  
   if(!ev->will_accept)
   {
      etk_toplevel_widget_pointer_push(etk_widget_toplevel_parent_get(etk_drag_parent_widget_get(ETK_DRAG(_etk_drag_widget))), ETK_POINTER_DEFAULT);
      return 1;
   }
   
   etk_toplevel_widget_pointer_push(etk_widget_toplevel_parent_get(etk_drag_parent_widget_get(ETK_DRAG(_etk_drag_widget))), ETK_POINTER_DND_DROP);
   return 1;
}

static int _dnd_finished_handler(void *data, int type, void *event)
{
   return 1;
}
#endif
