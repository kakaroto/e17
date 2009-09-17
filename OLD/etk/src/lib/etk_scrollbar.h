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

/** @file etk_scrollbar.h */
#ifndef _ETK_SCROLLBAR_H_
#define _ETK_SCROLLBAR_H_

#include <Ecore.h>

#include "etk_range.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TODO/FIXME list:
 * - the value of the scrollbar should be changed when the trough is clicked
 */

/**
 * @defgroup Etk_Scrollbar Etk_Scrollbar
 * @brief A scrollbar is a widget with a cursor that can be moved to change the scrolling value
 * @{
 */

/** Gets the type of a scrollbar */
#define ETK_SCROLLBAR_TYPE       (etk_scrollbar_type_get())
/** Casts the object to an Etk_Scrollbar */
#define ETK_SCROLLBAR(obj)       (ETK_OBJECT_CAST((obj), ETK_SCROLLBAR_TYPE, Etk_Scrollbar))
/** Checks if the object is an Etk_Scrollbar */
#define ETK_IS_SCROLLBAR(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_SCROLLBAR_TYPE))

/** Gets the type of a horizontal scrollbar */
#define ETK_HSCROLLBAR_TYPE       (etk_hscrollbar_type_get())
/** Casts the object to an Etk_HScrollbar */
#define ETK_HSCROLLBAR(obj)       (ETK_OBJECT_CAST((obj), ETK_HSCROLLBAR_TYPE, Etk_HScrollbar))
/** Checks if the object is an Etk_HScrollbar */
#define ETK_IS_HSCROLLBAR(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_HSCROLLBAR_TYPE))

/** Gets the type of a vertical scrollbar */
#define ETK_VSCROLLBAR_TYPE       (etk_vscrollbar_type_get())
/** Casts the object to an Etk_VScrollbar */
#define ETK_VSCROLLBAR(obj)       (ETK_OBJECT_CAST((obj), ETK_VSCROLLBAR_TYPE, Etk_VScrollbar))
/** Checks if the object is an Etk_VScrollbar */
#define ETK_IS_VSCROLLBAR(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_VSCROLLBAR_TYPE))


/**
 * @brief @widget The structure of a scrollbar
 * @structinfo
 */
struct Etk_Scrollbar
{
   /* private: */
   /* Inherit from Etk_Range */
   Etk_Range range;

   Ecore_Timer *scrolling_timer;
   Etk_Bool first_scroll:1;
   Etk_Bool dragging:1;
};

/**
 * @brief @widget The structure of a hscrollbar
 * @structinfo
 */
struct Etk_HScrollbar
{
   /* private: */
   /* Inherit from Etk_Scrollbar */
   Etk_Scrollbar scrollbar;
};

/**
 * @brief @widget The structure of a vscrollbar
 * @structinfo
 */
struct Etk_VScrollbar
{
   /* private: */
   /* Inherit from Etk_Scrollbar */
   Etk_Scrollbar scrollbar;
};


Etk_Type   *etk_scrollbar_type_get(void);
Etk_Type   *etk_hscrollbar_type_get(void);
Etk_Type   *etk_vscrollbar_type_get(void);

Etk_Widget *etk_hscrollbar_new(double lower, double upper, double value, double step_increment, double page_increment, double page_size);
Etk_Widget *etk_vscrollbar_new(double lower, double upper, double value, double step_increment, double page_increment, double page_size);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
