/** @file etk_window.c */
#include "etk_window.h"
#include <stdlib.h>
#include <Ecore_X.h>
#include <Ecore_X_Cursor.h>
#include "etk_main.h"
#include "etk_utils.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "../../config.h"

/**
 * @addtogroup Etk_Window
* @{
 */

enum _Etk_Widget_Signal_Id
{
   ETK_WINDOW_DELETE_EVENT_SIGNAL,
   ETK_WINDOW_NUM_SIGNALS
};

static void _etk_window_constructor(Etk_Window *window);
static void _etk_window_destructor(Etk_Window *window);
static void _etk_window_resize_cb(Ecore_Evas *ecore_evas);
static void _etk_window_delete_request_cb(Ecore_Evas *ecore_evas);
static void _etk_window_size_request_cb(Etk_Window *window, Etk_Size *requisition, void *data);
static Etk_Bool _etk_window_delete_event_handler(Etk_Window *window);
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
      window_type = etk_type_new("Etk_Window", ETK_TOPLEVEL_WIDGET_TYPE, sizeof(Etk_Window), ETK_CONSTRUCTOR(_etk_window_constructor), ETK_DESTRUCTOR(_etk_window_destructor), NULL);
   
      _etk_window_signals[ETK_WINDOW_DELETE_EVENT_SIGNAL] = etk_signal_new("delete_event", window_type, ETK_MEMBER_OFFSET(Etk_Window, delete_event), etk_marshaller_BOOL__VOID, etk_accumulator_bool_or, NULL);
   }

   return window_type;
}

/**
 * @brief Creates a new window
 * @return Returns the new window widget */
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

   ecore_evas_name_class_set(window, window_name, window_class);
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
   ETK_TOPLEVEL_WIDGET(window)->evas = ecore_evas_get(window->ecore_evas);
   ETK_TOPLEVEL_WIDGET(window)->pointer_set = _etk_window_pointer_set;

   /* TODO */
   evas_font_path_append(ETK_TOPLEVEL_WIDGET(window)->evas, PACKAGE_DATA_DIR "/fonts/");
   ecore_evas_data_set(window->ecore_evas, "etk_window", window);
   ecore_evas_callback_resize_set(window->ecore_evas, _etk_window_resize_cb);
   ecore_evas_callback_delete_request_set(window->ecore_evas, _etk_window_delete_request_cb);

   etk_signal_connect("size_request", ETK_OBJECT(window), ETK_CALLBACK(_etk_window_size_request_cb), NULL);
   etk_signal_connect_swapped("show", ETK_OBJECT(window), ETK_CALLBACK(ecore_evas_show), window->ecore_evas);
   etk_signal_connect_swapped("hide", ETK_OBJECT(window), ETK_CALLBACK(ecore_evas_hide), window->ecore_evas);
}

/* Destroys the window */
static void _etk_window_destructor(Etk_Window *window)
{
   if (!window)
      return;

   etk_widget_parent_set(etk_bin_child_get(ETK_BIN(window)), NULL);
   ecore_evas_free(window->ecore_evas);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the window is resized */
static void _etk_window_resize_cb(Ecore_Evas *ecore_evas)
{
   Etk_Window *window;
   int w, h;

   if (!(window = ETK_WINDOW(ecore_evas_data_get(ecore_evas, "etk_window"))))
      return;

   ecore_evas_geometry_get(window->ecore_evas, NULL, NULL, &w, &h);
   ETK_TOPLEVEL_WIDGET(window)->width = w;
   ETK_TOPLEVEL_WIDGET(window)->height = h;
   etk_widget_resize_queue(ETK_WIDGET(window));
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
      etk_toplevel_widget_size_get(ETK_TOPLEVEL_WIDGET(window), &w, &h);
      if (w < requisition->w || h < requisition->h)
         ecore_evas_resize(window->ecore_evas, ETK_MAX(w, requisition->w), ETK_MAX(h, requisition->h));
      ecore_evas_size_min_set(window->ecore_evas, requisition->w, requisition->h);
   }
}

/* Default handler for the "delete_event" signal */
static Etk_Bool _etk_window_delete_event_handler(Etk_Window *window)
{
   return FALSE;
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
