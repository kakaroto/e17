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

/** @file etk_alignment.h */
#ifndef _ETK_ALIGNMENT_H_
#define _ETK_ALIGNMENT_H_

#include "etk_bin.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Alignment Etk_Alignment
 * @brief The Etk_Alignment widget is a container that can contain
 * one child which can be aligned and scaled inside the container
 * @{
 */

/** Gets the type of an alignment */
#define ETK_ALIGNMENT_TYPE       (etk_alignment_type_get())
/** Casts the object to an Etk_Alignment */
#define ETK_ALIGNMENT(obj)       (ETK_OBJECT_CAST((obj), ETK_ALIGNMENT_TYPE, Etk_Alignment))
/** Checks if the object is an Etk_Alignment */
#define ETK_IS_ALIGNMENT(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_ALIGNMENT_TYPE))


/**
 * @brief @widget A container containing one child which can be aligned and scaled
 * @structinfo
 */
struct Etk_Alignment
{
   /* private: */
   /* Inherit from Etk_Bin */
   Etk_Bin bin;

   float xalign;
   float yalign;
   float xscale;
   float yscale;
};


Etk_Type   *etk_alignment_type_get(void);
Etk_Widget *etk_alignment_new(float xalign, float yalign, float xscale, float yscale);

void        etk_alignment_set(Etk_Alignment *alignment, float xalign, float yalign, float xscale, float yscale);
void        etk_alignment_get(Etk_Alignment *alignment, float *xalign, float *yalign, float *xscale, float *yscale);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
