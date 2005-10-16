/** @file etk_utils.h */
#ifndef _ETK_UTILS_H_
#define _ETK_UTILS_H_

#include <stdio.h>

/* Gettext */
#include <libintl.h>
#define _(String) gettext (String)
#define gettext_noop(String) String
#define N_(String) gettext_noop (String)

/** @brief Gets the max of the two values */
#define ETK_MAX(v1, v2)    (((v1) > (v2)) ? (v1) : (v2)) 
/** @brief Gets the min of the two values */
#define ETK_MIN(v1, v2)    (((v1) < (v2)) ? (v1) : (v2)) 
/** @brief Clamps the value against the boudaries */
#define ETK_CLAMP(value, left, right)     ((value) < (left) ? (left) : ((value) > (right) ? (right) : (value)))
/** @brief Rounds the float value to the nearest integer */
#define ETK_ROUND(a)       ((a < 0.0) ? (int)(floor(a - 0.5)) : (int)(floor(a + 0.5)))

/* TODO: make it portable ?? */
/** @brief Displays a warning in the output console */
#define ETK_WARNING(format, ...)    fprintf(stderr, "Etk Warning: %s, %d: %s: " format "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

void etk_accumulator_bool_or(void *return_value, const void *value_to_accum, void *data);
void etk_accumulator_bool_and(void *return_value, const void *value_to_accum, void *data);

#endif
