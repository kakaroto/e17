#ifndef ENGRAVE_MACROS_H
#define ENGRAVE_MACROS_H

/**
 * @file engrave_macros.h Macros used in the code and available to programs.
 * @brief Contains a set of macros that are used in Engrave and available to programs.
 */

/**
 * @defgroup Engrave_Macros Macros: Macros used internally and available externally
 *
 * @{
 */

#undef NEW
/**
* @def NEW(type, num)
* Allocates memory of @a num elements of sizeof(@a type).
*/
#define NEW(type, num) calloc(num, sizeof(type))

#undef FREE
/**
 * @def FREE(val)
 * Free the data pointed to by @a val and set to NULL
 */
#define FREE(val) \
{ \
  free(val); val = NULL; \
}

#undef IF_FREE
/**
 * @def IF_FREE(val)
 * If @a val is non-NULL, free @a val and assign to NULL
 */
#define IF_FREE(val) \
{ \
  if (val) FREE(val) \
}

/**
 * @}
 */

#endif

