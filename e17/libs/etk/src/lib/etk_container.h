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

/** @file etk_container.h */
#ifndef _ETK_CONTAINER_H_
#define _ETK_CONTAINER_H_

#include <Evas.h>

#include "etk_types.h"
#include "etk_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Container Etk_Container
 * @brief An Etk_Container is a widget that can contain other widgets
 * @{
 */

/** Gets the type of a container */
#define ETK_CONTAINER_TYPE       (etk_container_type_get())
/** Casts the object to an Etk_Container */
#define ETK_CONTAINER(obj)       (ETK_OBJECT_CAST((obj), ETK_CONTAINER_TYPE, Etk_Container))
/** Checks if the object is an Etk_Container */
#define ETK_IS_CONTAINER(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_CONTAINER_TYPE))

extern int ETK_CONTAINER_CHILD_ADDED_SIGNAL;
extern int ETK_CONTAINER_CHILD_REMOVED_SIGNAL;

/**
 * @brief @widget A widget that can contain other widgets
 * @structinfo
 */
struct Etk_Container
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   void (*child_add)(Etk_Container *container, Etk_Widget *widget);
   void (*child_remove)(Etk_Container *container, Etk_Widget *widget);
   Evas_List *(*children_get)(Etk_Container *container);

   int border_width;
};


Etk_Type  *etk_container_type_get(void);

void       etk_container_add(Etk_Container *container, Etk_Widget *widget);
void       etk_container_remove(Etk_Widget *widget);
void       etk_container_remove_all(Etk_Container *container);

void       etk_container_border_width_set(Etk_Container *container, int border_width);
int        etk_container_border_width_get(Etk_Container *container);

Evas_List *etk_container_children_get(Etk_Container *container);
Etk_Bool   etk_container_is_child(Etk_Container *container, Etk_Widget *widget);
void       etk_container_for_each(Etk_Container *container, void (*for_each_cb)(Etk_Widget *child));
void       etk_container_for_each_data(Etk_Container *container, void (*for_each_cb)(Etk_Widget *child, void *data), void *data);

void       etk_container_child_space_fill(Etk_Widget *child, Etk_Geometry *child_space, Etk_Bool hfill, Etk_Bool vfill, float xalign, float yalign);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
