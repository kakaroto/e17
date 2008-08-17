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

/** @file etk_utils.h */
#ifndef _ETK_UTILS_H_
#define _ETK_UTILS_H_

#include <libintl.h>
#include <math.h>
#include <stdio.h>

#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Utils Etk_Utils
 * @brief A set of utility functions and macros
 * @{
 */

/** Finds a translation of "string" according to the locale settings (use gettext) */
#define _(string)          gettext(string)

/** Gets the max of the two values */
#define ETK_MAX(v1, v2)    (((v1) > (v2)) ? (v1) : (v2))
/** Gets the min of the two values */
#define ETK_MIN(v1, v2)    (((v1) < (v2)) ? (v1) : (v2))
/** Clamps the value against the boudaries */
#define ETK_CLAMP(value, left, right)     ((value) < (left) ? (left) : ((value) > (right) ? (right) : (value)))
/** Rounds the float value to the nearest integer */
#define ETK_ROUND(a)       ((a < 0.0) ? (int)(floor(a - 0.5)) : (int)(floor(a + 0.5)))
/** Tests if the position (x, y) is inside the rectangle starting at (xx, yy) and of size (ww, hh) */
#define ETK_INSIDE(x, y, xx, yy, ww, hh) \
   (((x) < ((xx) + (ww))) && ((y) < ((yy) + (hh))) && ((x) >= (xx)) && ((y) >= (yy)))

/** Displays a warning in the output console */
#define ETK_WARNING(format, ...) \
   fprintf(stderr, "[Etk-Warning] (%s:%d - %s()):\n" format "\n\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)


Etk_Bool etk_callback_set_null(void *data);

unsigned int etk_current_time_get();

/** @} */

#ifdef __cplusplus
}
#endif

#endif
