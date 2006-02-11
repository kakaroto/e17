/** @file etk_window.c */
#include "etk_window.h"
#include <stdlib.h>
#include <string.h>
#include <Ecore_X.h>
#include <Ecore_X_Cursor.h>
#include "etk_main.h"
#include "etk_utils.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "config.h"

/**
 * @addtogroup Etk_Window
* @{
 */

enum _Etk_Widget_Signal_Id
{
   ETK_WINDOW_MOVE_SIGNAL,
   ETK_WINDOW_RESIZE_SIGNAL,
   ETK_WINDOW_FOCUS_IN_SIGNAL,
   ETK_WINDOW_FOCUS_OUT_SIGNAL,
   ETK_WINDOW_DELETE_EVENT_SIGNAL,
   ETK_WINDOW_NUM_SIGNALS
};

static void _etk_window_constructor(Etk_Window *window);
static void _etk_window_destructor(Etk_Window *window);
static void _etk_window_move_cb(Ecore_Evas *ecore_evas);
static void _etk_window_resize_cb(Ecore_Evas *ecore_evas);
static void _etk_window_focus_in_cb(Ecore_Evas *ecore_evas);
static void _etk_window_focus_out_cb(Ecore_Evas *ecore_evas);
static void _etk_window_delete_request_cb(Ecore_Evas *ecore_evas);
static void _etk_window_size_request_cb(Etk_Window *window, Etk_Size *requisition, void *data);
static Etk_Bool _etk_window_delete_event_handler(Etk_Window *window);
static void _etk_window_toplevel_geometry_get(Etk_Toplevel_Widget *toplevel, int *x, int *y, int *w, int *h);
static void _etk_window_pointer_set(Etk_Toplevel_Widget *toplevel_widget, Etk_Pointer_Type pointer_type);

static Etk_Signal *_etk_window_signals[ETK_WINDOW_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Window
 * @return Returns the type on an Etk_Window
 */
Etk_Type *etk_window_type_get()
{
   static Etk_Type *window_type = NULL;

   if (!window_type)
   {
      window_type = etk_type_new("Etk_Window", ETK_TOPLEVEL_WIDGET_TYPE, sizeof(Etk_Window), ETK_CONSTRUCTOR(_etk_window_constructor), ETK_DESTRUCTOR(_etk_window_destructor));
   
      _etk_window_signals[ETK_WINDOW_MOVE_SIGNAL] = etk_signal_new("move", window_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_window_signals[ETK_WINDOW_RESIZE_SIGNAL] = etk_signal_new("resize", window_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_window_signals[ETK_WINDOW_FOCUS_IN_SIGNAL] = etk_signal_new("focus_in", window_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_window_signals[ETK_WINDOW_FOCUS_OUT_SIGNAL] = etk_signal_new("focus_out", window_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_window_signals[ETK_WINDOW_DELETE_EVENT_SIGNAL] = etk_signal_new("delete_event", window_type, ETK_MEMBER_OFFSET(Etk_Window, delete_event), etk_marshaller_BOOL__VOID, etk_accumulator_bool_or, NULL);
   }

   return window_type;
}

/**
 * @brief Creates a new window
 * @return Returns the new window widget
 */
Etk_Widget *etk_window_new()
{
   return etk_widget_new(ETK_WINDOW_TYPE, "theme_group", "window", NULL);
}

/**
 * @brief Sets the title of the window
 * @param window the window
 * @param title the title to set
 */ 
void etk_window_title_set(Etk_Window *window, const char *title)
{
   if (!window)
      return;
   ecore_evas_title_set(window->ecore_evas, title);
}

/**
 * @brief Sets the window name and the window class of a window
 * @param window a window
 * @param window_name the window name to set
 * @param window_class the window class to set
 */
void etk_window_wmclass_set(Etk_Window *window, const char *window_name, const char *window_class)
{
   if (!window)
      return;
   ecore_evas_name_class_set(window->ecore_evas, window_name, window_class);
}

/**
 * @brief Moves the window to the position (x, y)
 * @param window a window
 * @param x the x position
 * @param y the y position
 */
void etk_window_move(Etk_Window *window, int x, int y)
{
   if (!window)
      return;
   ecore_evas_move(window->ecore_evas, x, y);
}

/**
 * @brief Resizes the window
 * @param window a window
 * @param w the new width of the window
 * @param h the new height of the window
 */
void etk_window_resize(Etk_Window *window, int w, int h)
{
   int min_w, min_h;
   
   if (!window)
      return;
   
   ecore_evas_size_min_get(window->ecore_evas, &min_w, &min_h);
   ecore_evas_resize(window->ecore_evas, ETK_MAX(w, min_w), ETK_MAX(h, min_h));
}

/**
 * @brief Gets the geometry of the window
 * @param window a window
 * @param x the location where to set the x position the window
 * @param y the location where to set the y position the window
 * @param w the location where to set the width of the window
 * @param h the location where to set the height of the window
 */
void etk_window_geometry_get(Etk_Window *window, int *x, int *y, int *w, int *h)
{
   if (!window)
      return;
   ecore_evas_geometry_get(window->ecore_evas, x, y, w, h);
}

/**
 * @brief Iconifies (i.e. minimize) the window
 * @param window a window
 */
void etk_window_iconify(Etk_Window *window)
{
   if (!window)
      return;
   ecore_evas_iconified_set(window->ecore_evas, 1);
}

/**
 * @brief Deiconifies (i.e. unminimize) the window
 * @param window a window
 */
void etk_window_deiconify(Etk_Window *window)
{
   if (!window)
      return;
   ecore_evas_iconified_set(window->ecore_evas, 0);
}

/**
 * @brief Maximizes the window
 * @param window a window
 */
void etk_window_maximize(Etk_Window *window)
{
   if (!window)
      return;
   ecore_evas_maximized_set(window->ecore_evas, 1);
}

/**
 * @brief Unmaximizes the window
 * @param window a window
 */
void etk_window_unmaximize(Etk_Window *window)
{
   if (!window)
      return;
   ecore_evas_maximized_set(window->ecore_evas, 0);
}

/**
 * @brief Places the window in the fullscreen state
 * @param window a window
 */
void etk_window_fullscreen(Etk_Window *window)
{
   if (!window)
      return;
   ecore_evas_fullscreen_set(window->ecore_evas, 1);
}

/**
 * @brief Toggles off the fullscreen state for the window
 * @param window a window
 */
void etk_window_unfullscreen(Etk_Window *window)
{
   if (!window)
      return;
   ecore_evas_fullscreen_set(window->ecore_evas, 0);
}

/**
 * @brief Sticks the window: it will appear on all the virtual desktops
 * @param window a window
 */
void etk_window_stick(Etk_Window *window)
{
   if (!window)
      return;
   ecore_evas_sticky_set(window->ecore_evas, 1);
}

/**
 * @brief Unsticks the window: it will appear on only one virtual desktop
 * @param window a window
 */
void etk_window_unstick(Etk_Window *window)
{
   if (!window)
      return;
   ecore_evas_sticky_set(window->ecore_evas, 0);
}

/**
 * @brief Focuses the window
 * @param window a window
 */
void etk_window_focus(Etk_Window *window)
{
   if (!window)
      return;
   ecore_evas_focus_set(window->ecore_evas, 1);
}

/**
 * @brief Unfocuses the window
 * @param window a window
 */
void etk_window_unfocus(Etk_Window *window)
{
   if (!window)
      return;
   ecore_evas_focus_set(window->ecore_evas, 0);
}

/**
 * @brief Sets wheter the window is decorated
 * @param window a window
 * @param decorated if @a decorated is ETK_FALSE, the border of the window will be hidden
 */
void etk_window_decorated_set(Etk_Window *window, Etk_Bool decorated)
{
   if (!window)
      return;
   ecore_evas_borderless_set(window->ecore_evas, !decorated);
}

/**
 * @brief Gets whether the window is decorated (i.e. whether the border of the window is shown)
 * @param window a window
 * @return Returns ETK_TRUE if the window is decorated
 */
Etk_Bool etk_window_decorated_get(Etk_Window *window)
{
   if (!window)
      return ETK_TRUE;
   return !ecore_evas_borderless_get(window->ecore_evas);
}

/**
 * @brief Sets wheter the window is shaped
 * @param window a window
 * @param shaped the shaped setting
 */
void etk_window_shaped_set(Etk_Window *window, Etk_Bool shaped)
{
   if (!window)
      return;
   ecore_evas_shaped_set(window->ecore_evas, shaped);
}

/**
 * @brief Gets whether the window is shaped
 * @param window a window
 * @return Returns ETK_TRUE if the window is shaped
 */
Etk_Bool etk_window_shaped_get(Etk_Window *window)
{
   if (!window)
      return ETK_TRUE;
   return ecore_evas_shaped_get(window->ecore_evas);
}

/**
 * @brief Sets whether the window should not be shown in the taskbar
 * @param window a window
 * @param skip_taskbar_hint if @a skip_taskbar_hint == ETK_TRUE, the window should not be shown in the taskbar
 */
void etk_window_skip_taskbar_hint_set(Etk_Window *window, Etk_Bool skip_taskbar_hint)
{
   if (!window || skip_taskbar_hint == etk_window_skip_taskbar_hint_get(window))
      return;
   
   if (skip_taskbar_hint)
   {
      if (etk_window_skip_pager_hint_get(window))
      {
         Ecore_X_Window_State states[2];
         states[0] = ECORE_X_WINDOW_STATE_SKIP_TASKBAR;
         states[1] = ECORE_X_WINDOW_STATE_SKIP_PAGER;
         ecore_x_netwm_window_state_set(window->x_window, states, 2);
      }
      else
      {
         Ecore_X_Window_State state[1];
         state[0] = ECORE_X_WINDOW_STATE_SKIP_TASKBAR;
         ecore_x_netwm_window_state_set(window->x_window, state, 1);
      }
   }
   else
   {
      if (etk_window_skip_pager_hint_get(window))
      {
         Ecore_X_Window_State state[1];
         state[0] = ECORE_X_WINDOW_STATE_SKIP_PAGER;
         ecore_x_netwm_window_state_set(window->x_window, state, 1);
      }
      else
         ecore_x_netwm_window_state_set(window->x_window, NULL, 0);
   }
}

/**
 * @brief Gets whether the window should not be shown in the taskbar
 * @param window a window
 * @return Returns ETK_FALSE if the window is shown in the taskbar
 */
Etk_Bool etk_window_skip_taskbar_hint_get(Etk_Window *window)
{
   int num_states, i;
   Ecore_X_Window_State *states;
   
   if (!window)
      return ETK_FALSE;
   
   ecore_x_netwm_window_state_get(window->x_window, &states, &num_states);
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
   return ETK_FALSE;
}

/**
 * @brief Sets whether the window should not be shown in the pager
 * @param window a window
 * @param skip_pager_hint if @a skip_pager_hint == ETK_TRUE, the window should not be shown in the pager
 */
void etk_window_skip_pager_hint_set(Etk_Window *window, Etk_Bool skip_pager_hint)
{
   if (!window || skip_pager_hint == etk_window_skip_pager_hint_get(window))
      return;

   if (skip_pager_hint)
   {
      if (etk_window_skip_taskbar_hint_get(window))
      {
         Ecore_X_Window_State states[2];
         states[0] = ECORE_X_WINDOW_STATE_SKIP_TASKBAR;
         states[1] = ECORE_X_WINDOW_STATE_SKIP_PAGER;
         ecore_x_netwm_window_state_set(window->x_window, states, 2);
      }
      else
      {
         Ecore_X_Window_State state[1];
         state[0] = ECORE_X_WINDOW_STATE_SKIP_PAGER;
         ecore_x_netwm_window_state_set(window->x_window, state, 1);
      }
   }
   else
   {
      if (etk_window_skip_taskbar_hint_get(window))
      {
         Ecore_X_Window_State state[1];
         state[0] = ECORE_X_WINDOW_STATE_SKIP_TASKBAR;
         ecore_x_netwm_window_state_set(window->x_window, state, 1);
      }
      else
         ecore_x_netwm_window_state_set(window->x_window, NULL, 0);
   }
}

/**
 * @brief Gets whether the window should not be shown in the pager
 * @param window a window
 * @return Returns ETK_TRUE if the window should not be shown in the pager
 */
Etk_Bool etk_window_skip_pager_hint_get(Etk_Window *window)
{
   int num_states, i;
   Ecore_X_Window_State *states;
   
   if (!window)
      return ETK_FALSE;
   
   ecore_x_netwm_window_state_get(window->x_window, &states, &num_states);
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
   return ETK_FALSE;
}

/**
 * @brief Sets whether the window is dnd-aware (true by default)
 * @param window a window
 * @param on ETK_TTUE if to set the window dnd-aware
 */
void etk_window_xdnd_aware_set(Etk_Window *window, Etk_Bool on)
{
#if HAVE_ECORE_X
   ecore_x_dnd_aware_set(window->x_window, on);
#endif
}

/**
 * @brief A utility function to use as a callback for the "delete_event" signal. It will hide the window and return ETK_TRUE to prevent the program to quit
 * @param window the window to hide
 * @param data the data passed when the signal is emitted - unused
 * @return Return ETK_TRUE so the the program won't quit
 */
Etk_Bool etk_window_hide_on_delete(Etk_Object *window, void *data)
{
   etk_widget_hide(ETK_WIDGET(window));
   return ETK_TRUE;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members and build the window */
static void _etk_window_constructor(Etk_Window *window)
{
   if (!window)
      return;

   window->delete_event = _etk_window_delete_event_handler;

   window->ecore_evas = ecore_evas_software_x11_new(NULL, 0, 0, 0, 0, 0);
   window->x_window = ecore_evas_software_x11_window_get(window->ecore_evas);
   
/* TODO: free!! */
#if HAVE_ECORE_X      
   {
      const char *types[] = { "*" };
      char **drop_types;
      int i;
      
      ecore_x_dnd_aware_set(window->x_window, 1);	
      drop_types = calloc(1, sizeof(char *));
      
      for (i = 0; i < 1; i++)
         drop_types[i] = strdup(types[i]);
      
      ecore_x_dnd_types_set(window->x_window, drop_types , 1);	
   }
#endif   
   
   ETK_TOPLEVEL_WIDGET(window)->evas = ecore_evas_get(window->ecore_evas);
   ETK_TOPLEVEL_WIDGET(window)->pointer_set = _etk_window_pointer_set;
   ETK_TOPLEVEL_WIDGET(window)->geometry_get = _etk_window_toplevel_geometry_get;

   /* TODO: font path */
   evas_font_path_append(ETK_TOPLEVEL_WIDGET(window)->evas, PACKAGE_DATA_DIR "/fonts/");
   ecore_evas_data_set(window->ecore_evas, "etk_window", window);
   ecore_evas_callback_resize_set(window->ecore_evas, _etk_window_move_cb);
   ecore_evas_callback_resize_set(window->ecore_evas, _etk_window_resize_cb);
   ecore_evas_callback_focus_in_set(window->ecore_evas, _etk_window_focus_in_cb);
   ecore_evas_callback_focus_out_set(window->ecore_evas, _etk_window_focus_out_cb);
   ecore_evas_callback_delete_request_set(window->ecore_evas, _etk_window_delete_request_cb);

   etk_signal_connect("size_request", ETK_OBJECT(window), ETK_CALLBACK(_etk_window_size_request_cb), NULL);
   etk_signal_connect_swapped("show", ETK_OBJECT(window), ETK_CALLBACK(ecore_evas_show), window->ecore_evas);
   etk_signal_connect_swapped("hide", ETK_OBJECT(window), ETK_CALLBACK(ecore_evas_hide), window->ecore_evas);
   
   etk_widget_realize(ETK_WIDGET(window));
}

/* Destroys the window */
static void _etk_window_destructor(Etk_Window *window)
{
   if (!window)
      return;
   ecore_evas_free(window->ecore_evas);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the window is moved */
static void _etk_window_move_cb(Ecore_Evas *ecore_evas)
{
   Etk_Window *window;

   if (!(window = ETK_WINDOW(ecore_evas_data_get(ecore_evas, "etk_window"))))
      return;
   etk_signal_emit(_etk_window_signals[ETK_WINDOW_MOVE_SIGNAL], ETK_OBJECT(window), NULL);
}

/* Called when the window is resized */
static void _etk_window_resize_cb(Ecore_Evas *ecore_evas)
{
   Etk_Window *window;

   if (!(window = ETK_WINDOW(ecore_evas_data_get(ecore_evas, "etk_window"))))
      return;
   etk_signal_emit(_etk_window_signals[ETK_WINDOW_RESIZE_SIGNAL], ETK_OBJECT(window), NULL);
   etk_widget_redraw_queue(ETK_WIDGET(window));
}

/* Called when the window is focused in */
static void _etk_window_focus_in_cb(Ecore_Evas *ecore_evas)
{
   Etk_Window *window;

   if (!(window = ETK_WINDOW(ecore_evas_data_get(ecore_evas, "etk_window"))))
      return;
   etk_signal_emit(_etk_window_signals[ETK_WINDOW_FOCUS_IN_SIGNAL], ETK_OBJECT(window), NULL);
}

/* Called when the window is focused out */
static void _etk_window_focus_out_cb(Ecore_Evas *ecore_evas)
{
   Etk_Window *window;

   if (!(window = ETK_WINDOW(ecore_evas_data_get(ecore_evas, "etk_window"))))
      return;
   etk_signal_emit(_etk_window_signals[ETK_WINDOW_FOCUS_OUT_SIGNAL], ETK_OBJECT(window), NULL);
}

/* Called when the user want to close the window */
static void _etk_window_delete_request_cb(Ecore_Evas *ecore_evas)
{
   Etk_Bool result;
   Etk_Object *window;

   if (!(window = ETK_OBJECT(ecore_evas_data_get(ecore_evas, "etk_window"))))
      return;

   etk_signal_emit(_etk_window_signals[ETK_WINDOW_DELETE_EVENT_SIGNAL], window, &result);
   if (!result)
      etk_object_destroy(window);
}

/* Called when a size request signal is emitted */
static void _etk_window_size_request_cb(Etk_Window *window, Etk_Size *requisition, void *data)
{
   int w, h;

   if (window && requisition && requisition->w >= 0 && requisition->h >= 0)
   {
      ecore_evas_geometry_get(window->ecore_evas, NULL, NULL, &w, &h);
      if (w < requisition->w || h < requisition->h)
         ecore_evas_resize(window->ecore_evas, ETK_MAX(w, requisition->w), ETK_MAX(h, requisition->h));
      ecore_evas_size_min_set(window->ecore_evas, requisition->w, requisition->h);
   }
}

/* Default handler for the "delete_event" signal */
static Etk_Bool _etk_window_delete_event_handler(Etk_Window *window)
{
   return ETK_FALSE;
}

/* Gets the geometry of the window toplevel widget */
static void _etk_window_toplevel_geometry_get(Etk_Toplevel_Widget *toplevel, int *x, int *y, int *w, int *h)
{
   etk_window_geometry_get(ETK_WINDOW(toplevel), x, y, w, h);
}

/* Sets the mouse pointer of the window */
static void _etk_window_pointer_set(Etk_Toplevel_Widget *toplevel_widget, Etk_Pointer_Type pointer_type)
{
   Etk_Window *window;
   int x_pointer_type = ECORE_X_CURSOR_LEFT_PTR;
   Ecore_X_Cursor cursor;

   if (!(window = ETK_WINDOW(toplevel_widget)))
      return;

   switch (pointer_type)
   {
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
      ecore_x_window_cursor_set(ecore_evas_software_x11_window_get(window->ecore_evas), cursor);
   else
      ETK_WARNING("Unable to find the X cursor \"%d\"", pointer_type);
}

/** @} */
