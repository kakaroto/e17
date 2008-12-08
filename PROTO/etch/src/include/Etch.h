/* ETCH - Timeline Based Animation Library
 * Copyright (C) 2007-2008 Jorge Luis Zapata, Hisham Mardam-Bey
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
#ifndef ETCH_H_
#define ETCH_H_

#include "Eina.h"

/**
 * @mainpage Etch
 * @section intro Introduction
 * Etch is a library that ... 
 * 
 * @file
 * @brief Etch API
 * @defgroup Etch_Group API
 * @{
 * 
 * @todo add the ability to make an object's property relative to other
 * TODO remove the object's concept, just animations, the data should not be stored
 * TODO on the object but on the animation itself
 * TODO etch_animation_add(Etch *e, Etch_Data_Type dt, callback)
 * TODO etch_animation_clone(Etch_Animation *a, Etch_Animation *clone)
 * TODO etch_timer_get(Etch *e, unsigned long *secs, unsigned long *msecs);
 * TODO remove every double and convert it into secs/usecs
 */

/**
 * @defgroup Etch_Core_Group Core
 * @{
 */
typedef struct _Etch Etch; /**< Etch Opaque Handler */
EAPI Etch * etch_new(void);
EAPI void etch_delete(Etch *e);
EAPI void etch_timer_fps_set(Etch *e, unsigned int fps);
EAPI unsigned int etch_timer_fps_get(Etch *e);
EAPI void etch_timer_tick(Etch *e);
EAPI int etch_timer_has_end(Etch *e);
EAPI void etch_timer_goto(Etch *e, unsigned long frame);

/**
 * Data types for a property
 */
typedef enum _Etch_Data_Type
{
	ETCH_UINT32, /**< Unsigned integer of 32 bits */
	ETCH_INT32, /**< Signed integer of 32 bits */
	ETCH_FLOAT, /**< Single precision float */
	ETCH_DOUBLE, /**< Double precision float */
	ETCH_ARGB, /**< Color (Alpha, Red, Green, Blue) of 32 bits */
	ETCH_DATATYPES,
} Etch_Data_Type;

/**
 * Container of every property data type supported
 */
typedef struct _Etch_Data
{
	Etch_Data_Type type;
	union {
		uint32_t u32;
		int32_t i32;
		float f;
		double d;
		unsigned int argb;
	} data;
} Etch_Data;

/** 
 * @}
 * @defgroup Etch_Animations_Group Animations
 * @{
 */
typedef struct _Etch_Animation Etch_Animation; /**< Animation Opaque Handler */
typedef struct _Etch_Animation_Keyframe Etch_Animation_Keyframe; /**< Animation Keyframe Opaque Handler */

typedef enum _Etch_Animation_Type
{
	ETCH_ANIMATION_LINEAR,
	ETCH_ANIMATION_COSIN,
	ETCH_ANIMATION_QUADRATIC,
	ETCH_ANIMATION_CUBIC,
	ETCH_ANIMATION_TYPES
} Etch_Animation_Type;

/** Callback function used when a property value changes */
typedef void (*Etch_Animation_Callback)(const Etch_Data *curr, const Etch_Data *prev, void *data);

EAPI Etch_Animation * etch_animation_add(Etch *e, Etch_Data_Type dtype,
		Etch_Animation_Callback cb, void *data);
EAPI void etch_animation_delete(Etch_Animation *a);
EAPI Etch_Animation_Keyframe * etch_animation_keyframe_add(Etch_Animation *a);
EAPI void etch_animation_keyframe_del(Etch_Animation *a, Etch_Animation_Keyframe *m);
EAPI void etch_animation_keyframe_type_set(Etch_Animation_Keyframe *m, Etch_Animation_Type t);
EAPI Etch_Animation_Type etch_animation_keyframe_type_get(Etch_Animation_Keyframe *m);
EAPI void etch_animation_keyframe_time_set(Etch_Animation_Keyframe *m, unsigned long secs, unsigned long usecs);
EAPI void etch_animation_keyframe_value_set(Etch_Animation_Keyframe *m, ...);
EAPI void etch_animation_keyframe_value_get(Etch_Animation_Keyframe *m, ...);
/** 
 * @}
 * @}
 */
#endif /*ETCH_H_*/
