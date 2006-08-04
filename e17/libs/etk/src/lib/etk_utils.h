/** @file etk_utils.h */
#ifndef _ETK_UTILS_H_
#define _ETK_UTILS_H_

#include <stdio.h>
#include <libintl.h>

/* Gettext */
#define _(string) gettext(string)

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
   fprintf(stderr, "Etk Warning: %s, %d: %s: " format "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

void etk_accumulator_bool_or(void *return_value, const void *value_to_accum, void *data);
void etk_accumulator_bool_and(void *return_value, const void *value_to_accum, void *data);
void etk_accumulator_stopping_bool_or(void *return_value, const void *value_to_accum, void *data);
void etk_accumulator_stopping_bool_and(void *return_value, const void *value_to_accum, void *data);

void etk_callback_set_null(void *data);

#endif
