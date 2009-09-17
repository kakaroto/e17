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

/** @file etk_label.h */
#ifndef _ETK_LABEL_H_
#define _ETK_LABEL_H_

#include <Evas.h>

#include "etk_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Label Etk_Label
 * @brief The Etk_Label widget is a simple widget that can display text
 * @{
 */

/** Gets the type of a label */
#define ETK_LABEL_TYPE        (etk_label_type_get())
/** Casts the object to an Etk_Label */
#define ETK_LABEL(obj)        (ETK_OBJECT_CAST((obj), ETK_LABEL_TYPE, Etk_Label))
/** Check if the object is an Etk_Label */
#define ETK_IS_LABEL(obj)     (ETK_OBJECT_CHECK_TYPE((obj), ETK_LABEL_TYPE))


/**
 * @brief @widget A simple widget that can display text
 * @structinfo
 */
struct Etk_Label
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   char *text;

   float xalign;
   float yalign;
};


Etk_Type   *etk_label_type_get(void);
Etk_Widget *etk_label_new(const char *text);

void        etk_label_set(Etk_Label *label, const char *text);
const char *etk_label_get(Etk_Label *label);
void        etk_label_alignment_set(Etk_Label *label, float xalign, float yalign);
void        etk_label_alignment_get(Etk_Label *label, float *xalign, float *yalign);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
