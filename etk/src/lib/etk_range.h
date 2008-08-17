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

/** @file etk_range.h */
#ifndef _ETK_RANGE_H_
#define _ETK_RANGE_H_

#include "etk_types.h"
#include "etk_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Range Etk_Range
 * @brief Etk_Range is an abstract class used by widgets like scrollbars or sliders
 * @{
 */

/** Gets the type of a range */
#define ETK_RANGE_TYPE       (etk_range_type_get())
/** Casts the object to an Etk_Range */
#define ETK_RANGE(obj)       (ETK_OBJECT_CAST((obj), ETK_RANGE_TYPE, Etk_Range))
/** Checks if the object is an Etk_Range */
#define ETK_IS_RANGE(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_RANGE_TYPE))

extern int ETK_RANGE_VALUE_CHANGED_SIGNAL;

/**
 * @brief An abstract class used by widgets like scrollbars or sliders
 * @structinfo
 */
struct Etk_Range
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   double lower;
   double upper;
   double value;

   double step_increment;
   double page_increment;
   double page_size;

   Etk_Bool (*value_changed_handler)(Etk_Range *range, double value);
};


Etk_Type *etk_range_type_get(void);
Etk_Bool  etk_range_value_set(Etk_Range *range, double value);
double    etk_range_value_get(Etk_Range *range);
void      etk_range_range_set(Etk_Range *range, double lower, double upper);
void      etk_range_range_get(Etk_Range *range, double *lower, double *upper);
void      etk_range_increments_set(Etk_Range *range, double step, double page);
void      etk_range_increments_get(Etk_Range *range, double *step, double *page);
void      etk_range_page_size_set(Etk_Range *range, double page_size);
double    etk_range_page_size_get(Etk_Range *range);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
