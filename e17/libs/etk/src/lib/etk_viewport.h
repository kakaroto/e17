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

/** @file etk_viewport.h */
#ifndef _ETK_VIEWPORT_H_
#define _ETK_VIEWPORT_H_

#include "etk_bin.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Viewport Etk_Viewport
 * @{
 */

/** Gets the type of an viewport */
#define ETK_VIEWPORT_TYPE       (etk_viewport_type_get())
/** Casts the object to an Etk_Viewport */
#define ETK_VIEWPORT(obj)       (ETK_OBJECT_CAST((obj), ETK_VIEWPORT_TYPE, Etk_Viewport))
/** Checks if the object is an Etk_Viewport */
#define ETK_IS_VIEWPORT(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_VIEWPORT_TYPE))

/**
 * @struct Etk_Viewport
 * @brief A viewport is a bin container which has the ability to scroll. Thus, all the widgets that @n
 * have no scrolling ability have to be placed in a viewport, in order to be scrollable in an Etk_Scrolled_View
 */
struct Etk_Viewport
{
   /* private: */
   /* Inherit from Etk_Bin */
   Etk_Bin bin;

   Evas_Object *clip, *event;
   int xscroll, yscroll;
   Etk_Bool hold_events;
};

Etk_Type   *etk_viewport_type_get(void);
Etk_Widget *etk_viewport_new(void);
void        etk_viewport_hold_events_set(Etk_Viewport *viewport, Etk_Bool hold_events);
Etk_Bool    etk_viewport_hold_events_get(Etk_Viewport *viewport);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
