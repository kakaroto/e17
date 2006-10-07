#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Evas.h>
#include <Ecore_Evas.h>
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
Etk_Engine *engine_open();
void engine_close();

static Etk_Bool _engine_init();
static void _engine_shutdown();

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
static void _window_pointer_set(Etk_Window *window, Etk_Pointer_Type pointer_type);
  
/* Etk_Popup_Window functions  */
static void _popup_window_constructor(Etk_Popup_Window *popup_window);
static void _popup_window_popup(Etk_Popup_Window *popup_window);
static void _popup_window_popdown(Etk_Popup_Window *popup_window);

/* Event and mouse functions */
static void _event_callback_set(void (*callback)(Etk_Event_Type event, Etk_Event_Global event_info));
static int _event_input_handler_cb(void *data, int type, void *event);
static void _mouse_position_get(int *x, int *y);
static void _mouse_screen_geometry_get(int *x, int *y, int *w, int *h);

/* Etk_Selection functions */
static void _selection_text_set(Etk_Selection_Type selection, const char *text);
static void _selection_text_request(Etk_Selection_Type selection, Etk_Widget *target);
static void _selection_clear(Etk_Selection_Type selection);
static int _selection_notify_handler_cb(void *data, int type, void *event);


/* Etk_Drag functions */
static void _drag_constructor(Etk_Drag *drag);
static void _drag_begin(Etk_Drag *drag);
static int  _drag_mouse_up_cb(void *data, int type, void *event);
static int  _drag_mouse_move_cb(void *data, int type, void *event);

/* Etk_Dnd functions */
static void _dnd_container_get_widgets_at(Etk_Toplevel *top, int x, int y, int offx, int offy, Evas_List **list);
static int _dnd_enter_handler(void *data, int type, void *event);
static int _dnd_position_handler(void *data, int type, void *event);
static int _dnd_drop_handler(void *data, int type, void *event);
static int _dnd_leave_handler(void *data, int type, void *event);
static int _dnd_selection_handler(void *data, int type, void *event);
static int _dnd_status_handler(void *data, int type, void *event);
static int _dnd_finished_handler(void *data, int type, void *event);

/* Private functions */
static void _window_netwm_state_active_set(Etk_Window *window, Ecore_X_Window_State state, Etk_Bool active);
static Etk_Bool _window_netwm_state_active_get(Etk_Window *window, Ecore_X_Window_State state);
static void _event_global_modifiers_locks_wrap(int xmodifiers, Etk_Modifiers *modifiers, Etk_Locks *locks);


/* Private vars */
static Evas_List *_popup_window_popped_windows = NULL;
static Ecore_X_Window _popup_window_input_window = 0;

static void (*_event_callback)(Etk_Event_Type event, Etk_Event_Global event_info) = NULL;
static Ecore_Event_Handler *_event_input_handlers[NUM_INPUT_HANDLERS];

static Etk_Widget *_selection_widget = NULL;
static Ecore_Event_Handler *_selection_notify_handler = NULL;


static Ecore_Event_Handler *_drag_mouse_move_handler;
static Ecore_Event_Handler *_drag_mouse_up_handler;

extern Etk_Widget  *_etk_drag_widget;
static char       **_dnd_types          = NULL;
static int          _dnd_types_num      = 0;
static Etk_Widget  *_dnd_widget         = NULL;
static Evas_List   *_dnd_handlers       = NULL;
static int          _dnd_widget_accepts = 0;


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
   _window_skip_taskbar_hint_set,
   _window_skip_taskbar_hint_get,
   _window_skip_pager_hint_set,
   _window_skip_pager_hint_get,
   _window_pointer_set,

   _popup_window_constructor,
   _popup_window_popup,
   _popup_window_popdown,
   
   _event_callback_set,
   _mouse_position_get,
   _mouse_screen_geometry_get,
   
   _selection_text_set,
   _selection_text_request,
   _selection_clear,
   
   _drag_constructor,
   _drag_begin,
};

/**************************
 *
 * Engine general functions
 *
 **************************/

/* Called when the engine is loaded */
Etk_Engine *engine_open()
{
   engine_info.engine_data = NULL;
   engine_info.engine_name = strdup("ecore_evas_x11");
   etk_engine_inherit_from(&engine_info, "ecore_evas");
   return &engine_info;
}

/* Called when the engine is unloaded */
void engine_close()
{
   free(engine_info.engine_name);
}

/* Initializes the engine */
static Etk_Bool _engine_init()
{
   if (!ecore_x_init(NULL))
   {
      ETK_WARNING("Ecore_X initialization failed!");
      return ETK_FALSE;
   }
   
   _event_input_handlers[0] = ecore_event_handler_add(ECORE_X_EVENT_KEY_DOWN, _event_input_handler_cb, NULL);
   _event_input_handlers[1] = ecore_event_handler_add(ECORE_X_EVENT_KEY_UP, _event_input_handler_cb, NULL);
   _event_input_handlers[2] = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_DOWN, _event_input_handler_cb, NULL);
   _event_input_handlers[3] = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_UP, _event_input_handler_cb, NULL);
   _event_input_handlers[4] = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_MOVE, _event_input_handler_cb, NULL);
   _event_input_handlers[5] = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_WHEEL, _event_input_handler_cb, NULL);
   
   _selection_notify_handler = ecore_event_handler_add(ECORE_X_EVENT_SELECTION_NOTIFY, _selection_notify_handler_cb, NULL);
   
   _dnd_handlers = evas_list_append(_dnd_handlers, ecore_event_handler_add(ECORE_X_EVENT_XDND_ENTER, _dnd_enter_handler, NULL));
   _dnd_handlers = evas_list_append(_dnd_handlers, ecore_event_handler_add(ECORE_X_EVENT_XDND_POSITION, _dnd_position_handler, NULL));
   _dnd_handlers = evas_list_append(_dnd_handlers, ecore_event_handler_add(ECORE_X_EVENT_XDND_DROP, _dnd_drop_handler, NULL));
   _dnd_handlers = evas_list_append(_dnd_handlers, ecore_event_handler_add(ECORE_X_EVENT_XDND_LEAVE, _dnd_leave_handler, NULL));
   _dnd_handlers = evas_list_append(_dnd_handlers, ecore_event_handler_add(ECORE_X_EVENT_SELECTION_NOTIFY, _dnd_selection_handler, NULL));
   _dnd_handlers = evas_list_append(_dnd_handlers, ecore_event_handler_add(ECORE_X_EVENT_XDND_STATUS, _dnd_status_handler, NULL));
   _dnd_handlers = evas_list_append(_dnd_handlers, ecore_event_handler_add(ECORE_X_EVENT_XDND_FINISHED, _dnd_finished_handler, NULL));
   
   return ETK_TRUE;
}

/* Shutdowns the engine */
static void _engine_shutdown()
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
   
   while (_dnd_handlers)
   {
      ecore_event_handler_del(_dnd_handlers->data);
      _dnd_handlers = evas_list_remove_list(_dnd_handlers, _dnd_handlers);
   }
   
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
   etk_object_notify(ETK_OBJECT(window), "skip_taskbar");
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
   etk_object_notify(ETK_OBJECT(window), "skip_pager");
}

/* Gets whether the window appears in the pager */
static Etk_Bool _window_skip_pager_hint_get(Etk_Window *window)
{
   return _window_netwm_state_active_get(window, ECORE_X_WINDOW_STATE_SKIP_PAGER);
}

/* Sets the mouse pointer to use when the mouse is inside the window */
static void _window_pointer_set(Etk_Window *window, Etk_Pointer_Type pointer_type)
{
   int x_pointer_type = ECORE_X_CURSOR_LEFT_PTR;
   Ecore_X_Cursor cursor;
   Etk_Engine_Window_Data *engine_data;
   
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
      printf("Engine: Popup: %d %d %d %d\n", root_x, root_y, root_w, root_h);
      _popup_window_input_window = ecore_x_window_input_new(root, root_x, root_y, root_w, root_h);
      ecore_x_window_show(_popup_window_input_window);
      ecore_x_pointer_confine_grab(_popup_window_input_window);
      ecore_x_keyboard_grab(_popup_window_input_window);
   }
   _popup_window_popped_windows = evas_list_append(_popup_window_popped_windows, popup_window);
}

/* Called when the popup window is popped down */
static void _popup_window_popdown(Etk_Popup_Window *popup_window)
{
   _popup_window_popped_windows = evas_list_remove(_popup_window_popped_windows, popup_window);
   
   if (!_popup_window_popped_windows)
   {
      printf("Engine: Popdown\n");
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
   Etk_Engine_Window_Data *engine_data;
   Etk_Toplevel *toplevel;
   
   if (!(toplevel = etk_widget_toplevel_parent_get(target)) || !ETK_IS_WINDOW(toplevel))
      return;
   
   _selection_widget = target;
   engine_data = ETK_WINDOW(toplevel)->engine_data;
   if (selection == ETK_SELECTION_PRIMARY)
      ecore_x_selection_primary_request(engine_data->x_window, ECORE_X_SELECTION_TARGET_UTF8_STRING);
   else if (selection == ETK_SELECTION_SECONDARY)
      ecore_x_selection_secondary_request(engine_data->x_window, ECORE_X_SELECTION_TARGET_UTF8_STRING);
   else if (selection == ETK_SELECTION_CLIPBOARD)
      ecore_x_selection_clipboard_request(engine_data->x_window, ECORE_X_SELECTION_TARGET_UTF8_STRING);
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
 * Etk_Drag's functions
 *
 **************************/

/* TODOC */
static void _drag_constructor(Etk_Drag *drag)
{
   Etk_Engine_Window_Data *engine_data;
   Ecore_X_Window x_window;
   
   engine_data = ETK_WINDOW(drag)->engine_data;
   x_window = engine_data->x_window;
   ecore_x_dnd_aware_set(x_window, 1);
}

/* TODOC */
static void _drag_begin(Etk_Drag *drag)
{
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
   
   if (type == ECORE_X_EVENT_MOUSE_MOVE)
   {
      Ecore_X_Event_Mouse_Move *xev = event;
      
      ecore_x_window_geometry_get(xev->win, &x, &y, NULL, NULL);
      _event_global_modifiers_locks_wrap(xev->modifiers, &ev.mouse_move.modifiers, &ev.mouse_move.locks);
      ev.mouse_move.pos.x = xev->x + x;
      ev.mouse_move.pos.y = xev->y + y;
      ev.mouse_move.timestamp = xev->time;
      _event_callback(ETK_EVENT_MOUSE_MOVE, ev);
   }
   else if (type == ECORE_X_EVENT_MOUSE_BUTTON_DOWN)
   {
      Ecore_X_Event_Mouse_Button_Down *xev = event;
      
      ecore_x_window_geometry_get(xev->win, &x, &y, NULL, NULL);
      _event_global_modifiers_locks_wrap(xev->modifiers, &ev.mouse_down.modifiers, &ev.mouse_down.locks);
      ev.mouse_down.flags = ETK_MOUSE_NONE;
      if (xev->double_click)
         ev.mouse_down.flags |= ETK_MOUSE_DOUBLE_CLICK;
      if (xev->triple_click)
         ev.mouse_down.flags |= ETK_MOUSE_TRIPLE_CLICK;
      ev.mouse_down.button = xev->button;
      ev.mouse_down.pos.x = xev->x + x;
      ev.mouse_down.pos.y = xev->y + y;
      ev.mouse_down.timestamp = xev->time;
      _event_callback(ETK_EVENT_MOUSE_DOWN, ev);
   }
   else if (type == ECORE_X_EVENT_MOUSE_BUTTON_UP)
   {
      Ecore_X_Event_Mouse_Button_Up *xev = event;
      
      ecore_x_window_geometry_get(xev->win, &x, &y, NULL, NULL);
      _event_global_modifiers_locks_wrap(xev->modifiers, &ev.mouse_up.modifiers, &ev.mouse_up.locks);
      ev.mouse_up.flags = ETK_MOUSE_NONE;
      if (xev->double_click)
         ev.mouse_up.flags |= ETK_MOUSE_DOUBLE_CLICK;
      if (xev->triple_click)
         ev.mouse_up.flags |= ETK_MOUSE_TRIPLE_CLICK;
      ev.mouse_up.button = xev->button;
      ev.mouse_up.pos.x = xev->x + x;
      ev.mouse_up.pos.y = xev->y + y;
      ev.mouse_up.timestamp = xev->time;
      printf("Engine: Mouse Up: %d %d (%d %d)\n", ev.mouse_up.pos.x, ev.mouse_up.pos.y, x, y);
      _event_callback(ETK_EVENT_MOUSE_UP, ev);
   }
   else if (type == ECORE_X_EVENT_MOUSE_WHEEL)
   {
      Ecore_X_Event_Mouse_Wheel *xev = event;
      
      ecore_x_window_geometry_get(xev->win, &x, &y, NULL, NULL);
      _event_global_modifiers_locks_wrap(xev->modifiers, &ev.mouse_wheel.modifiers, &ev.mouse_wheel.locks);
      ev.mouse_wheel.direction = (xev->direction == 0) ? ETK_WHEEL_VERTICAL : ETK_WHEEL_HORIZONTAL;
      ev.mouse_wheel.z = xev->z;
      ev.mouse_wheel.pos.x = xev->x + x;
      ev.mouse_wheel.pos.y = xev->y + y;
      ev.mouse_wheel.timestamp = xev->time;
      _event_callback(ETK_EVENT_MOUSE_WHEEL, ev);
   }
   else if (type == ECORE_X_EVENT_KEY_DOWN)
   {
      Ecore_X_Event_Key_Down *xev = event;
      
      _event_global_modifiers_locks_wrap(xev->modifiers, &ev.key_down.modifiers, &ev.key_down.locks);
      ev.key_down.keyname = xev->keyname;
      ev.key_down.key = xev->keysymbol;
      ev.key_down.string = xev->key_compose;
      ev.key_down.timestamp = xev->time;
      _event_callback(ETK_EVENT_KEY_DOWN, ev);
   }
   else if (type == ECORE_X_EVENT_KEY_UP)
   {
      Ecore_X_Event_Key_Up *xev = event;
      
      _event_global_modifiers_locks_wrap(xev->modifiers, &ev.key_up.modifiers, &ev.key_up.locks);
      ev.key_up.keyname = xev->keyname;
      ev.key_up.key = xev->keysymbol;
      ev.key_up.string = xev->key_compose;
      ev.key_up.timestamp = xev->time;
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
      
      etk_signal_emit_by_name("selection_received", ETK_OBJECT(_selection_widget), NULL, &etk_event);
   }
   
   return 1;
}

/* TODOC */
static int _drag_mouse_up_cb(void *data, int type, void *event)
{
   Etk_Drag *drag;
   
   drag = data;
   etk_widget_hide_all(ETK_WIDGET(drag));
   ecore_event_handler_del(_drag_mouse_move_handler);
   ecore_event_handler_del(_drag_mouse_up_handler);
   ecore_x_dnd_drop();   
   etk_widget_drag_end(ETK_WIDGET(drag));   
   etk_toplevel_pointer_push(etk_widget_toplevel_parent_get(drag->widget), ETK_POINTER_DEFAULT);
   
   return 1;
}

/* TODOC */
static int _drag_mouse_move_cb(void *data, int type, void *event)
{
   Ecore_X_Event_Mouse_Move *ev;
   Etk_Drag *drag;
   
   drag = data;
   ev = event;
   
   etk_window_move(ETK_WINDOW(drag), ev->root.x + 2, ev->root.y + 2);
   
   return 1;
}

/* Searchs the container recursively for the widget that accepts xdnd */
static void _dnd_container_get_widgets_at(Etk_Toplevel *top, int x, int y, int offx, int offy, Evas_List **list)
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
      if (ETK_INSIDE(x, y, wx + offx, wy + offy, ww, wh))
	 *list = evas_list_append(*list, widget);
   }
}

/* The event handler for when a drag enters our window */
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

/* The event handler for when a drag is moving in our window */
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
   for (l = etk_toplevel_widgets_get(); l; l = l->next)
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
      _dnd_container_get_widgets_at(ETK_TOPLEVEL(window), ev->position.x, ev->position.y, x, y, &children);
      
      /* check if we're leaving a widget */
      if (_dnd_widget)
      {
         etk_widget_geometry_get(_dnd_widget, &wx, &wy, &ww, &wh);
         if (!ETK_INSIDE(ev->position.x, ev->position.y, wx + x, wy + y, ww, wh))
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

/* TODOC */
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

/* TODOC */
static int _dnd_leave_handler(void *data, int type, void *event)
{
   //printf("leave window\n");
      
   return 1;
}

/* TODOC */
static int _dnd_selection_handler(void *data, int type, void *event)
{
   Ecore_X_Event_Selection_Notify *ev;
   Ecore_X_Selection_Data_Files *files;
   Ecore_X_Selection_Data_Text *text;
   //int i;

   //printf("selection\n"); 
   ev = event;
   switch (ev->selection) 
   {
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
         
      default:
         break;
   }
   
   return 1;
}

/* TODOC */
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
      etk_toplevel_pointer_push(etk_widget_toplevel_parent_get(etk_drag_parent_widget_get(ETK_DRAG(_etk_drag_widget))), ETK_POINTER_DEFAULT);
      return 1;
   }
   
   etk_toplevel_pointer_push(etk_widget_toplevel_parent_get(etk_drag_parent_widget_get(ETK_DRAG(_etk_drag_widget))), ETK_POINTER_DND_DROP);
   return 1;
}

/* TODOC */
static int _dnd_finished_handler(void *data, int type, void *event)
{
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
      int cur_num, new_num;
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
      if (xmodifiers & ECORE_X_MODIFIER_SHIFT)
         *modifiers |= ETK_MODIFIER_SHIFT;
      if (xmodifiers & ECORE_X_MODIFIER_CTRL)
         *modifiers |= ETK_MODIFIER_CTRL;
      if (xmodifiers & ECORE_X_MODIFIER_ALT)
         *modifiers |= ETK_MODIFIER_ALT;
      if (xmodifiers & ECORE_X_MODIFIER_WIN)
         *modifiers |= ETK_MODIFIER_WIN;
   }
   
   if (modifiers)
   {
      *locks = ETK_LOCK_NONE;
      if (xmodifiers & ECORE_X_LOCK_SCROLL)
         *locks |= ETK_LOCK_SCROLL;
      if (xmodifiers & ECORE_X_LOCK_NUM)
         *locks |= ETK_LOCK_NUM;
      if (xmodifiers & ECORE_X_LOCK_CAPS)
         *locks |= ETK_LOCK_CAPS;
   }
}
