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

/** @file etk_shadow.h */
#ifndef _ETK_SHADOW_H_
#define _ETK_SHADOW_H_

#include "etk_bin.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Shadow Etk_Shadow
 * @brief The Etk_Shadow widget is a container that can contain one child, and
 * that can cast a shadow or have a border
 * @{
 */

/** Gets the type of a shadow */
#define ETK_SHADOW_TYPE       (etk_shadow_type_get())
/** Casts the object to an Etk_Shadow */
#define ETK_SHADOW(obj)       (ETK_OBJECT_CAST((obj), ETK_SHADOW_TYPE, Etk_Shadow))
/** Checks if the object is an Etk_Shadow */
#define ETK_IS_SHADOW(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_SHADOW_TYPE))


/** @brief The different types of shadows */
typedef enum
{
   ETK_SHADOW_NONE,             /* TODOC */
   ETK_SHADOW_INSIDE,           /* TODOC */
   ETK_SHADOW_OUTSIDE           /* TODOC */
} Etk_Shadow_Type;

/** @brief The different edges where a shadow can be */
typedef enum
{
   ETK_SHADOW_NO_EDGE = 0,              /* TODOC */
   ETK_SHADOW_LEFT = 1 << 0,            /* TODOC */
   ETK_SHADOW_RIGHT = 1 << 1,           /* TODOC */
   ETK_SHADOW_TOP = 1 << 2,             /* TODOC */
   ETK_SHADOW_BOTTOM = 1 << 3,          /* TODOC */
   ETK_SHADOW_LEFT_RIGHT = ETK_SHADOW_LEFT | ETK_SHADOW_RIGHT,          /* TODOC */
   ETK_SHADOW_TOP_BOTTOM = ETK_SHADOW_TOP | ETK_SHADOW_BOTTOM,          /* TODOC */
   ETK_SHADOW_ALL = ETK_SHADOW_LEFT_RIGHT | ETK_SHADOW_TOP_BOTTOM       /* TODOC */
} Etk_Shadow_Edges;


/**
 * @brief @widget A container that can contain one child, and that can cast a shadow or have a border
 * @structinfo
 */
struct Etk_Shadow
{
   /* private: */
   /* Inherit from Etk_Bin */
   Etk_Bin bin;

   Etk_Shadow_Type type;
   Etk_Shadow_Edges edges;
   Etk_Color color;
   int offset_x;
   int offset_y;
   int radius;

   int border_width;
   Etk_Color border_color;

   Evas_Object *shadow_objs[4];
   Evas_Object *border_objs[4];
   Evas_Object *clip;

   Etk_Bool shadow_need_recalc:1;
   Etk_Bool border_need_recalc:1;
};


Etk_Type   *etk_shadow_type_get(void);
Etk_Widget *etk_shadow_new(void);

void        etk_shadow_shadow_set(Etk_Shadow *shadow, Etk_Shadow_Type type, Etk_Shadow_Edges edges, int radius, int offset_x, int offset_y, int opacity);
void        etk_shadow_shadow_get(Etk_Shadow *shadow, Etk_Shadow_Type *type, Etk_Shadow_Edges *edges, int *radius, int *offset_x, int *offset_y, int *opacity);
void        etk_shadow_shadow_color_set(Etk_Shadow *shadow, int r, int g, int b);
void        etk_shadow_shadow_color_get(Etk_Shadow *shadow, int *r, int *g, int *b);

void        etk_shadow_border_set(Etk_Shadow *shadow, int border_width);
int         etk_shadow_border_get(Etk_Shadow *shadow);
void        etk_shadow_border_color_set(Etk_Shadow *shadow, int r, int g, int b, int a);
void        etk_shadow_border_color_get(Etk_Shadow *shadow, int *r, int *g, int *b, int *a);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
