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

/** @file etk_window.h */
#ifndef _ETK_WINDOW_H_
#define _ETK_WINDOW_H_

#include <Ecore.h>

#include "etk_toplevel.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Window Etk_Window
 * @brief The most common toplevel-widget
 * @{
 */

/** Gets the type of a window */
#define ETK_WINDOW_TYPE       (etk_window_type_get())
/** Casts the object to an Etk_Window */
#define ETK_WINDOW(obj)       (ETK_OBJECT_CAST((obj), ETK_WINDOW_TYPE, Etk_Window))
/** Check if the object is an Etk_Window */
#define ETK_IS_WINDOW(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_WINDOW_TYPE))


/** @brief Describes how the window is stacked */
typedef enum
{
   ETK_WINDOW_NORMAL,      /**< The window is stacked in the default layer */
   ETK_WINDOW_ABOVE,       /**< The window is stacked above all the other windows */
   ETK_WINDOW_BELOW        /**< The window is stacked below all the other windows */
} Etk_Window_Stacking;

extern int ETK_WINDOW_MOVED_SIGNAL;
extern int ETK_WINDOW_RESIZED_SIGNAL;
extern int ETK_WINDOW_FOCUSED_IN_SIGNAL;
extern int ETK_WINDOW_FOCUSED_OUT_SIGNAL;
extern int ETK_WINDOW_DELETE_EVENT_SIGNAL;

/**
 * @brief @widget The most common toplevel-widget
 * @structinfo
 */
struct Etk_Window
{
   /* private: */
   /* Inherit from Etk_Toplevel */
   Etk_Toplevel toplevel;

   void *engine_data;

   Etk_Bool (*delete_event)(Etk_Window *window);
   Etk_Window *center_on_window;
   Etk_Bool center_queued;
   Etk_Bool wait_size_request:1;
};


Etk_Type   *etk_window_type_get(void);
Etk_Widget *etk_window_new(void);
void        etk_window_delete_request(Etk_Window *window);

void        etk_window_title_set(Etk_Window *window, const char *title);
const char *etk_window_title_get(Etk_Window *window);
void        etk_window_wmclass_set(Etk_Window *window, const char *window_name, const char *window_class);

void        etk_window_move(Etk_Window *window, int x, int y);
void        etk_window_resize(Etk_Window *window, int w, int h);
void        etk_window_geometry_get(Etk_Window *window, int *x, int *y, int *w, int *h);
void        etk_window_center_on_window(Etk_Window *window_to_center, Etk_Window *window);
void        etk_window_move_to_mouse(Etk_Window *window);
void        etk_window_modal_for_window(Etk_Window *window_to_modal, Etk_Window *window);

void        etk_window_raise(Etk_Window *window);
void        etk_window_lower(Etk_Window *window);
void        etk_window_stacking_set(Etk_Window *window, Etk_Window_Stacking stacking);
Etk_Window_Stacking etk_window_stacking_get(Etk_Window *window);

void        etk_window_iconified_set(Etk_Window *window, Etk_Bool iconified);
Etk_Bool    etk_window_iconified_get(Etk_Window *window);
void        etk_window_maximized_set(Etk_Window *window, Etk_Bool maximized);
Etk_Bool    etk_window_maximized_get(Etk_Window *window);
void        etk_window_fullscreen_set(Etk_Window *window, Etk_Bool fullscreen);
Etk_Bool    etk_window_fullscreen_get(Etk_Window *window);

void        etk_window_focused_set(Etk_Window *window, Etk_Bool focused);
Etk_Bool    etk_window_focused_get(Etk_Window *window);

void        etk_window_sticky_set(Etk_Window *window, Etk_Bool sticky);
Etk_Bool    etk_window_sticky_get(Etk_Window *window);
void        etk_window_decorated_set(Etk_Window *window, Etk_Bool decorated);
Etk_Bool    etk_window_decorated_get(Etk_Window *window);
void        etk_window_shaped_set(Etk_Window *window, Etk_Bool shaped);
Etk_Bool    etk_window_shaped_get(Etk_Window *window);
void        etk_window_has_alpha_set(Etk_Window *window, Etk_Bool has_alpha);
Etk_Bool    etk_window_has_alpha_get(Etk_Window *window);

void        etk_window_skip_taskbar_hint_set(Etk_Window *window, Etk_Bool skip_taskbar_hint);
Etk_Bool    etk_window_skip_taskbar_hint_get(Etk_Window *window);
void        etk_window_skip_pager_hint_set(Etk_Window *window, Etk_Bool skip_pager_hint);
Etk_Bool    etk_window_skip_pager_hint_get(Etk_Window *window);

Etk_Bool    etk_window_hide_on_delete(Etk_Object *window, void *data);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
