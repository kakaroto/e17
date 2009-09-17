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

/** @file etk_canvas.h */
#ifndef _ETK_CANVAS_H_
#define _ETK_CANVAS_H_

#include <Evas.h>

#include "etk_container.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Canvas Etk_Canvas
 * @brief The Etk_Canvas widget is a container which allows you to position widgets at canvas coordinates
 * @{
 */

/** Gets the type of a canvas */
#define ETK_CANVAS_TYPE       (etk_canvas_type_get())
/** Casts the object to an Etk_Canvas */
#define ETK_CANVAS(obj)       (ETK_OBJECT_CAST((obj), ETK_CANVAS_TYPE, Etk_Canvas))
/** Checks if the object is an Etk_Canvas */
#define ETK_IS_CANVAS(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_CANVAS_TYPE))

/**
 * @brief @widget A container which allows you to position widgets at canvas coordinates
 * @structinfo
 */
struct Etk_Canvas
{
   /* private: */
   /* Inherit from Etk_Container */
   Etk_Container container;

   Eina_List *children;
   Evas_Object *clip;
};

Etk_Type   *etk_canvas_type_get(void);
Etk_Widget *etk_canvas_new(void);

void        etk_canvas_put(Etk_Canvas *canvas, Etk_Widget *widget, int x, int y);
void        etk_canvas_move(Etk_Canvas *canvas, Etk_Widget *widget, int x, int y);
void        etk_canvas_child_position_get(Etk_Canvas *canvas, Etk_Widget *widget, int *x, int *y);

Etk_Widget *etk_canvas_object_add(Etk_Canvas *canvas, Evas_Object *evas_object);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
