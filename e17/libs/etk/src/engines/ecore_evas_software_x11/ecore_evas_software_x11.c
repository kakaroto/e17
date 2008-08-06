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

#include <string.h>
#include <stdlib.h>
#include <Ecore_Evas.h>
#include <Etk.h>
#include "Etk_Engine_Ecore_Evas.h"
#include "Etk_Engine_Ecore_Evas_X11.h"

typedef Etk_Engine_Ecore_Evas_X11_Window_Data Etk_Engine_Window_Data;

/* General engine functions */
Etk_Engine *engine_open(void);
void engine_close(void);

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
   NULL, /* window_evas_position_get */
   NULL, /* window_screen_position_get */
   NULL, /* window_size_get */
   NULL, /* window_screen_geometry_get */
   NULL, /* window_modal_for_window */
   NULL, /* window_iconified_set */
   NULL, /* window_iconified_get */
   NULL, /* window_maximized_set */
   NULL, /* window_maximized_get */
   NULL, /* window_fullscreen_set */
   NULL, /* window_fullscreen_get */
   NULL, /* window_raise */
   NULL, /* window_lower */
   NULL, /* window_stacking_set */
   NULL, /* window_stacking_get */
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
   NULL, /* window_skip_taskbar_hint_set */
   NULL, /* window_skip_taskbar_hint_ge */
   NULL, /* window_skip_pager_hint_set */
   NULL, /* window_skip_pager_hint_get */
   NULL, /* window_pointer_set */
   
   NULL, /* popup_window_constructor */
   NULL, /* popup_window_popup */
   NULL, /* popup_window_popdown */
   
   NULL, /* event_callback_set */
   NULL, /* event_timestamp_get */
   
   NULL, /* mouse_position_get */
   NULL, /* mouse_screen_geometry_get */
   
   NULL, /* selection_text_set */
   NULL, /* selection_text_request */
   NULL, /* selection_clear */
   
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
   engine_info.engine_name = strdup("ecore_evas_software_x11");
   etk_engine_inherit_from(&engine_info, "ecore_evas_x11");
   return &engine_info;
}

/* Called when the engine is unloaded */
void engine_close(void)
{
   free(engine_info.engine_name);
}

/**************************
 *
 * Etk_Window's functions
 *
 **************************/

/* Initializes the created window */
static void _window_constructor(Etk_Window *window)
{
   /* We _MUST_ initialize and create the ecore_evas and the x_window
    * variables in the engine_data since they are used by the "ecore_evas_x11" engine */
   Etk_Engine_Window_Data *engine_data; 

   engine_data = malloc(sizeof(Etk_Engine_Window_Data));
   window->engine_data = engine_data;   
   ETK_ENGINE_ECORE_EVAS_WINDOW_DATA(engine_data)->ecore_evas = ecore_evas_software_x11_new(NULL, 0, 0, 0, 0, 0);
   engine_data->x_window = ecore_evas_software_x11_window_get(ETK_ENGINE_ECORE_EVAS_WINDOW_DATA(engine_data)->ecore_evas);
   engine_info.super->window_constructor(window);
}
