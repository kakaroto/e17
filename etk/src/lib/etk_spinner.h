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

/** @file etk_spinner.h */
#ifndef _ETK_SPINNER_H_
#define _ETK_SPINNER_H_

#include <Ecore.h>

#include "etk_range.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Spinner Etk_Spinner
 * @brief A spinner is a widget that allows the user to set the value of a setting
 * @{
 */

/** Gets the type of a spinner */
#define ETK_SPINNER_TYPE       (etk_spinner_type_get())
/** Casts the object to an Etk_Spinner */
#define ETK_SPINNER(obj)       (ETK_OBJECT_CAST((obj), ETK_SPINNER_TYPE, Etk_Spinner))
/** Checks if the object is an Etk_Spinner */
#define ETK_IS_SPINNER(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_SPINNER_TYPE))


/**
 * @brief @widget A widget that allows the user to set the value of a setting
 * @structinfo
 */
struct Etk_Spinner
{
   /* private: */
   /* Inherit from Etk_Range */
   Etk_Range range;

   Evas_Object *editable_object;
   Ecore_Timer *step_timer;

   int successive_steps;
   int digits;
   char value_format[16];
   Etk_Bool snap_to_ticks:1;
   Etk_Bool wrap:1;
   Etk_Bool selection_dragging:1;
};


Etk_Type   *etk_spinner_type_get(void);
Etk_Widget *etk_spinner_new(double lower, double upper, double value, double step_increment, double page_increment);

void        etk_spinner_digits_set(Etk_Spinner *spinner, int digits);
int         etk_spinner_digits_get(Etk_Spinner *spinner);
void        etk_spinner_snap_to_ticks_set(Etk_Spinner *spinner, Etk_Bool snap_to_ticks);
Etk_Bool    etk_spinner_snap_to_ticks_get(Etk_Spinner *spinner);
void        etk_spinner_wrap_set(Etk_Spinner *spinner, Etk_Bool wrap);
Etk_Bool    etk_spinner_wrap_get(Etk_Spinner *spinner);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
