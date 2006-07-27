#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Evas.h>
#include <Ecore_Evas.h>
#include <Ecore_X.h>
#include <Ecore_X_Cursor.h>

#include "config.h"
#include "etk_types.h"
#include "etk_dnd.h"
#include "etk_engine.h"
#include "etk_utils.h"
#include "etk_toplevel_widget.h"
#include "etk_main.h"

#include "Etk_Engine_Ecore_Evas.h"
#include "Etk_Engine_Ecore_Evas_X11.h"

/* Etk_Window engine data
 * we do another typedef to shorten the name for internal use
 */
typedef Etk_Engine_Ecore_Evas_X11_Window_Data Etk_Engine_Window_Data;

/* General engine functions */
Etk_Engine *engine_open();

static Etk_Bool _engine_init();
static void _engine_shutdown();

/* Etk_Window functions */
static void _window_constructor(Etk_Window *window);
static void _window_pointer_set(Etk_Toplevel_Widget *toplevel_widget, Etk_Pointer_Type pointer_type);

static Etk_Engine engine_info = {
   
   NULL, /* engine specific data */
   NULL, /* engine name */
   NULL, /* super (parent) engine */
   NULL, /* DL handle */
     
   NULL, /* engine_init */
   NULL, /* engine_shutdown */
   
   _window_constructor,
   NULL, /* window_destructor */
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
   NULL, /* window_center_on_window */
   NULL, /* window_move_to_mouse */
   NULL, /* window_modal_for_window */
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
   NULL, /* window_skip_taskbar_hint_set */
   NULL, /* window_skip_taskbar_hint_ge */
   NULL, /* window_skip_pager_hint_set */
   NULL, /* window_skip_pager_hint_get */
   NULL, /* window_dnd_aware_set */
   _window_pointer_set,

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
   engine_info.engine_name = strdup("ecore_evas_software_x11");
   etk_engine_inherit_from(&engine_info, "ecore_evas_x11");
   return &engine_info;
}

static void _window_constructor(Etk_Window *window)
{
   /* We _MUST_ initialize and create the ecore_evas and the x_window
    * variables in the engine_data.
    */
   Etk_Engine_Window_Data *engine_data;

   engine_data = malloc(sizeof(Etk_Engine_Window_Data));
   window->engine_data = engine_data;   
   ETK_ENGINE_ECORE_EVAS_WINDOW_DATA(engine_data)->ecore_evas = ecore_evas_software_x11_new(0, 0, 0, 0, 0, 0);
   engine_data->x_window = ecore_evas_software_x11_window_get(ETK_ENGINE_ECORE_EVAS_WINDOW_DATA(engine_data)->ecore_evas);
   engine_info.super->window_constructor(window);   
}

static void _window_pointer_set(Etk_Toplevel_Widget *toplevel_widget, Etk_Pointer_Type pointer_type)
{
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
}  
