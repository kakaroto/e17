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
#ifndef ETCH_PRIVATE_H_
#define ETCH_PRIVATE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <stdarg.h>
#include <math.h> 
#include <sys/time.h>

#include <assert.h>

#include "Eina.h"

/**
 * 
 */
struct _Etch
{
	Eina_Inlist *animations; /** List of objects */
	unsigned long frame; /** Current frame */
	unsigned int fps; /** Number of frames per second */
	double curr; /** Current time in seconds */
	double start; /** Time where an animation starts in seconds */
	double end; /** Time where an animation ends in seconds */	
};

/**
 * Specific data needed for cubic bezier animations
 */
typedef struct _Etch_Animation_Cubic
{
	Etch_Data cp1; /** First control point */
	Etch_Data cp2; /** Second control point */
} Etch_Animation_Cubic;

/**
 * Specific data needed for quadratic bezier animations
 */
typedef struct _Etch_Animation_Quadratic
{
	Etch_Data cp; /** Control point */ 
} Etch_Animation_Quadratic;

/**
 * An animation mark is a defined state on the timeline of an animation. It sets
 * that a given time a property should have the specified value.
 */
struct _Etch_Animation_Keyframe
{
	EINA_INLIST; /** A keyframe is always a list */
	Etch_Animation *animation; /** reference to the animation */
	Etch_Data value; /** the property value for this mark */
	double time; /** the time where the keyframe is, already transformed to seconds */
	Etch_Animation_Type type; /** type of interpolation between this mark and the next */
	union {
		Etch_Animation_Quadratic q;
		Etch_Animation_Cubic c;
	} data; /** interpolation specific data */ 
};

/**
 * Many objects can use the same animation.
 */
struct _Etch_Animation
{
	EINA_INLIST; /** An animation is a list */
	Eina_Inlist *keys; /** list of keyframes */
	/* TODO if the marks are already ordered do we need to have the start
	 * and end time duplicated here? */
	double start; /** initial time already transformed to seconds */
	double end; /** end time already transformed to seconds */
	double m; /** last interpolator value in the range [0,1] */
	Etch_Data curr;
	Etch_Data_Type dtype; /** animations only animates data types, no properties */
	Etch_Animation_Callback cb; /** function to call when a value has been set */
	void *data; /** user provided data */
};

static inline double etch_timeval_to_double(struct timeval *t)
{
	return (double)t->tv_sec + (((double)t->tv_usec) / 1000000);
}

void etch_animation_animate(Etch_Animation *a, double curr);
Etch_Animation * etch_animation_new(Etch_Data_Type dtype, Etch_Animation_Callback cb, void *data);

#endif /*ETCH_PRIVATE_H_*/
