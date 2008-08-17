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

/** @file etk_frame.h */
#ifndef _ETK_FRAME_H_
#define _ETK_FRAME_H_

#include "etk_bin.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Frame Etk_Frame
 * @brief A frame is a bin container with a label. It may be useful to group widgets that are logically related
 * @{
 */

/** Gets the type of an frame */
#define ETK_FRAME_TYPE       (etk_frame_type_get())
/** Casts the object to an Etk_Frame */
#define ETK_FRAME(obj)       (ETK_OBJECT_CAST((obj), ETK_FRAME_TYPE, Etk_Frame))
/**  Checks if the object is an Etk_Frame */
#define ETK_IS_FRAME(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_FRAME_TYPE))


/**
 * @brief @widget A bin container with a label
 * @structinfo
 */
struct Etk_Frame
{
   /* private: */
   /* Inherit from Etk_Bin */
   Etk_Bin bin;

   char *label;
};


Etk_Type   *etk_frame_type_get(void);
Etk_Widget *etk_frame_new(const char *label);

void        etk_frame_label_set(Etk_Frame *frame, const char *label);
const char *etk_frame_label_get(Etk_Frame *frame);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
