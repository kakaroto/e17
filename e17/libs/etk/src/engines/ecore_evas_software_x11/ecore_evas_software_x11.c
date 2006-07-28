#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Evas.h>
#include <Ecore_Evas.h>
#include <Ecore_X.h>
#include <Ecore_X_Cursor.h>

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
   NULL, /* window_iconified_set */
   NULL, /* window_iconified_get */
   NULL, /* window_maximized_set */
   NULL, /* window_maximized_get */
   NULL, /* window_fullscreen_set */
   NULL, /* window_fullscreen_get */
   NULL, /* window_raise */
   NULL, /* window_lower */
   NULL, /* window_sticky_set */
   NULL, /* window_sticky_get */
   NULL, /* window_focused_set */
   NULL, /* window_focused_get */
   NULL, /* window_decorated_set */
   NULL, /* window_decorated_get */
   NULL, /* window_shaped_set */
   NULL, /* window_shaped_get */
   NULL, /* window_skip_taskbar_hint_set */
   NULL, /* window_skip_taskbar_hint_ge */
   NULL, /* window_skip_pager_hint_set */
   NULL, /* window_skip_pager_hint_get */
   NULL, /* window_dnd_aware_set */
   NULL, /* window_dnd_aware_get */
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
