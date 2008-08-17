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

/** @file etk_drag.h */
#ifndef _ETK_DRAG_H_
#define _ETK_DRAG_H_

#include "etk_types.h"
#include "etk_window.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETK_DRAG_TYPE       (etk_drag_type_get())
#define ETK_DRAG(obj)       (ETK_OBJECT_CAST((obj), ETK_DRAG_TYPE, Etk_Drag))
#define ETK_IS_DRAG(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_DRAG_TYPE))

struct Etk_Drag
{
   /* private: */
   /* Inherit from Etk_Window */
   Etk_Window window;

   Etk_Widget *widget;

   char         **types;
   unsigned int   num_types;
   void          *data;
   int            data_size;
};

Etk_Type   *etk_drag_type_get(void);
Etk_Widget *etk_drag_new(Etk_Widget *widget);
void        etk_drag_types_set(Etk_Drag *drag, const char **types, unsigned int num_types);
void        etk_drag_data_set(Etk_Drag *drag, void *data, int size);
void        etk_drag_begin(Etk_Drag *drag);
void        etk_drag_parent_widget_set(Etk_Drag *drag, Etk_Widget *widget);
Etk_Widget *etk_drag_parent_widget_get(Etk_Drag *drag);


/** @} */

#ifdef __cplusplus
}
#endif

#endif
