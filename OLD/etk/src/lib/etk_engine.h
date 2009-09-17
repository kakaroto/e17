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

/** @file etk_engine.h */
#ifndef _ETK_ENGINE_H_
#define _ETK_ENGINE_H_

#include <Evas.h>

#include "etk_event.h"
#include "etk_selection.h"
#include "etk_toplevel.h"
#include "etk_types.h"
#include "etk_window.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Engine Etk_Engine
 * @brief Etk is based on engines that are used for specific tasks such as creating a window or handling copy/paste...
 * @{
 */


/**
 * @brief The structure of an engine regrouping the different methods to call
 * @structinfo
 */
struct Etk_Engine
{
   void *engine_data;
   char *engine_name;
   Etk_Engine *super;
   void *handle;

   Etk_Bool (*engine_init)(void);
   void (*engine_shutdown)(void);

   void (*window_constructor)(Etk_Window *window);
   void (*window_destructor)(Etk_Window *window);
   void (*window_show)(Etk_Window *window);
   void (*window_hide)(Etk_Window *window);
   Evas *(*window_evas_get)(Etk_Window *window);
   void (*window_title_set)(Etk_Window *window, const char *title);
   const char *(*window_title_get)(Etk_Window *window);
   void (*window_wmclass_set)(Etk_Window *window, const char *window_name, const char *window_class);
   void (*window_move)(Etk_Window *window, int x, int y);
   void (*window_resize)(Etk_Window *window, int w, int h);
   void (*window_size_min_set)(Etk_Window *window, int w, int h);
   void (*window_evas_position_get)(Etk_Window *window, int *x, int *y);
   void (*window_screen_position_get)(Etk_Window *window, int *x, int *y);
   void (*window_size_get)(Etk_Window *window, int *w, int *h);
   void (*window_screen_geometry_get)(Etk_Window *window, int *x, int *y, int *w, int *h);
   void (*window_modal_for_window)(Etk_Window *window_to_modal, Etk_Window *window);
   void (*window_iconified_set)(Etk_Window *window, Etk_Bool iconified);
   Etk_Bool (*window_iconified_get)(Etk_Window *window);
   void (*window_maximized_set)(Etk_Window *window, Etk_Bool maximized);
   Etk_Bool (*window_maximized_get)(Etk_Window *window);
   void (*window_fullscreen_set)(Etk_Window *window, Etk_Bool fullscreen);
   Etk_Bool (*window_fullscreen_get)(Etk_Window *window);
   void (*window_raise)(Etk_Window *window);
   void (*window_lower)(Etk_Window *window);
   void (*window_stacking_set)(Etk_Window *window, Etk_Window_Stacking stacking);
   Etk_Window_Stacking (*window_stacking_get)(Etk_Window *window);
   void (*window_sticky_set)(Etk_Window *window, Etk_Bool sticky);
   Etk_Bool (*window_sticky_get)(Etk_Window *window);
   void (*window_focused_set)(Etk_Window *window, Etk_Bool focused);
   Etk_Bool (*window_focused_get)(Etk_Window *window);
   void (*window_decorated_set)(Etk_Window *window, Etk_Bool decorated);
   Etk_Bool (*window_decorated_get)(Etk_Window *window);
   void (*window_shaped_set)(Etk_Window *window, Etk_Bool shaped);
   Etk_Bool (*window_shaped_get)(Etk_Window *window);
   void (*window_has_alpha_set)(Etk_Window *window, Etk_Bool has_alpha);
   Etk_Bool (*window_has_alpha_get)(Etk_Window *window);
   void (*window_skip_taskbar_hint_set)(Etk_Window *window, Etk_Bool skip_taskbar_hint);
   Etk_Bool (*window_skip_taskbar_hint_get)(Etk_Window *window);
   void (*window_skip_pager_hint_set)(Etk_Window *window, Etk_Bool skip_pager_hint);
   Etk_Bool (*window_skip_pager_hint_get)(Etk_Window *window);
   void (*window_focusable_set)(Etk_Window *window, Etk_Bool focusable);
   Etk_Bool (*window_focusable_get)(Etk_Window *window);
   void (*window_pointer_set)(Etk_Window *window, Etk_Pointer_Type pointer_type);

   void (*popup_window_constructor)(Etk_Popup_Window *popup_window);
   void (*popup_window_popup)(Etk_Popup_Window *popup_window);
   void (*popup_window_popdown)(Etk_Popup_Window *popup_window);

   void (*event_callback_set)(void (*callback)(Etk_Event_Type event, Etk_Event_Global event_info));
   unsigned int (*event_timestamp_get)(void);

   void (*mouse_position_get)(int *x, int *y);
   void (*mouse_screen_geometry_get)(int *x, int *y, int *w, int *h);

   void (*selection_text_set)(Etk_Selection_Type selection, const char *text);
   void (*selection_text_request)(Etk_Selection_Type selection, Etk_Widget *target);
   void (*selection_clear)(Etk_Selection_Type selection);

   void (*drag_constructor)(Etk_Drag *drag);
   void (*drag_begin)(Etk_Drag *drag);
};


Etk_Bool     etk_engine_init(void);
void         etk_engine_shutdown(void);

Eina_List   *etk_engine_list_get(void);
Etk_Bool     etk_engine_exists(const char *engine_name);
Etk_Engine  *etk_engine_get(void);

Etk_Engine  *etk_engine_load(const char *engine_name);
void         etk_engine_unload(Etk_Engine *engine);
Etk_Bool     etk_engine_inherit_from(Etk_Engine *engine, const char * inherit_name);

void         etk_engine_window_constructor(Etk_Window *window);
void         etk_engine_window_destructor(Etk_Window *window);
void         etk_engine_window_show(Etk_Window *window);
void         etk_engine_window_hide(Etk_Window *window);
Evas        *etk_engine_window_evas_get(Etk_Window *window);
void         etk_engine_window_title_set(Etk_Window *window, const char *title);
const char  *etk_engine_window_title_get(Etk_Window *window);
void         etk_engine_window_wmclass_set(Etk_Window *window, const char *window_name, const char *window_class);
void         etk_engine_window_move(Etk_Window *window, int x, int y);
void         etk_engine_window_resize(Etk_Window *window, int w, int h);
void         etk_engine_window_size_min_set(Etk_Window *window, int w, int h);
void         etk_engine_window_evas_position_get(Etk_Window *window, int *x, int *y);
void         etk_engine_window_screen_position_get(Etk_Window *window, int *x, int *y);
void         etk_engine_window_size_get(Etk_Window *window, int *w, int *h);
void         etk_engine_window_screen_geometry_get(Etk_Window *window, int *x, int *y, int *w, int *h);
void         etk_engine_window_modal_for_window(Etk_Window *window_to_modal, Etk_Window *window);
void         etk_engine_window_iconified_set(Etk_Window *window, Etk_Bool iconified);
Etk_Bool     etk_engine_window_iconified_get(Etk_Window *window);
void         etk_engine_window_maximized_set(Etk_Window *window, Etk_Bool maximized);
Etk_Bool     etk_engine_window_maximized_get(Etk_Window *window);
void         etk_engine_window_fullscreen_set(Etk_Window *window, Etk_Bool fullscreen);
Etk_Bool     etk_engine_window_fullscreen_get(Etk_Window *window);
void         etk_engine_window_raise(Etk_Window *window);
void         etk_engine_window_lower(Etk_Window *window);
void         etk_engine_window_stacking_set(Etk_Window *window, Etk_Window_Stacking stacking);
Etk_Window_Stacking etk_engine_window_stacking_get(Etk_Window *window);
void         etk_engine_window_sticky_set(Etk_Window *window, Etk_Bool on);
Etk_Bool     etk_engine_window_sticky_get(Etk_Window *window);
void         etk_engine_window_focused_set(Etk_Window *window, Etk_Bool focused);
Etk_Bool     etk_engine_window_focused_get(Etk_Window *window);
void         etk_engine_window_decorated_set(Etk_Window *window, Etk_Bool decorated);
Etk_Bool     etk_engine_window_decorated_get(Etk_Window *window);
void         etk_engine_window_shaped_set(Etk_Window *window, Etk_Bool shaped);
Etk_Bool     etk_engine_window_shaped_get(Etk_Window *window);
void         etk_engine_window_has_alpha_set(Etk_Window *window, Etk_Bool has_alpha);
Etk_Bool     etk_engine_window_has_alpha_get(Etk_Window *window);
void         etk_engine_window_skip_taskbar_hint_set(Etk_Window *window, Etk_Bool skip_taskbar_hint);
Etk_Bool     etk_engine_window_skip_taskbar_hint_get(Etk_Window *window);
void         etk_engine_window_skip_pager_hint_set(Etk_Window *window, Etk_Bool skip_pager_hint);
Etk_Bool     etk_engine_window_skip_pager_hint_get(Etk_Window *window);
void         etk_engine_window_focusable_set(Etk_Window *window, Etk_Bool focusable);
Etk_Bool     etk_engine_window_focusable_get(Etk_Window *window);
void         etk_engine_window_pointer_set(Etk_Window *window, Etk_Pointer_Type pointer_type);

void         etk_engine_popup_window_constructor(Etk_Popup_Window *popup_window);
void         etk_engine_popup_window_popup(Etk_Popup_Window *popup_window);
void         etk_engine_popup_window_popdown(Etk_Popup_Window *popup_window);

void         etk_engine_event_callback_set(void (*callback)(Etk_Event_Type event, Etk_Event_Global event_info));
unsigned int etk_engine_event_timestamp_get(void);

void         etk_engine_mouse_position_get(int *x, int *y);
void         etk_engine_mouse_screen_geometry_get(int *x, int *y, int *w, int *h);

void         etk_engine_selection_text_set(Etk_Selection_Type selection, const char *text);
void         etk_engine_selection_text_request(Etk_Selection_Type selection, Etk_Widget *target);
void         etk_engine_selection_clear(Etk_Selection_Type selection);

void         etk_engine_drag_constructor(Etk_Drag *drag);
void         etk_engine_drag_begin(Etk_Drag *drag);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
