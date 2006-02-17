#ifndef EWL_MACROS_H
#define EWL_MACROS_H

/**
 * @defgroup Ewl_Macros Ewl_Macros: Useful Macros Used Internally and Available Externally
 * Defines a variety of utility macros.
 *
 * @{
 */

#undef NEW
/**
 * @def NEW(type, num)
 * Allocates memory of @a num elements of sizeof(@a type).
 */
#define NEW(type, num) calloc(num, sizeof(type));

#undef REALLOC
/**
 * @def REALLOC(dat, type, num)
 * Reallocates memory pointed to by @a dat to @a num elements of sizeof(@a
 * type).
 */
#define REALLOC(dat, type, num) \
{ \
	if (dat) \
	  { \
		dat = realloc(dat, sizeof(type) * num); \
	  } \
}

#undef FREE
/**
 * @def FREE(dat)
 * Free the data pointed to by @a dat and it to NULL.
 */
#define FREE(dat) \
{ \
	free(dat); dat = NULL; \
}


#undef IF_FREE
/**
 * @def IF_FREE(dat)
 * If @a dat is non-NULL, free @a dat and assign it to NULL.
 */
#define IF_FREE(dat) \
{ \
	if (dat) FREE(dat); \
}

#undef ZERO
/**
 * @def ZERO(ptr, type, num)
 * Set the first @a num elements of sizeof(@a type) pointed to by @a ptr to
 * zero.
 */
#define ZERO(ptr, type, num) ptr = memset(ptr, 0, sizeof(type) * (num))

#ifndef MIN
/**
 * @def MIN(x, y)
 * Select the minimum of two values
 */
#define MIN(x, y) (((x) > (y)) ? (y) : (x))
#endif

#ifndef MAX
/**
 * @def MAX(x, y)
 * Select the maximum of two values
 */
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

/**
 * @}
 */

#endif

