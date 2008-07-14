/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_MACROS_H
#define EWL_MACROS_H

/**
 * @addtogroup Ewl_Macros Ewl_Macros: Useful Macros Used Internally and Available Externally
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

#undef IF_FREE_LIST
/**
 * @def IF_FREE_LIST(list)
 * If @a list is non-NULL, free @a list and set it NULL.
 */
#define IF_FREE_LIST(list) \
{ \
        if (list) { \
                ecore_list_destroy(list); \
                list = NULL; \
        } \
}

#undef IF_FREE_HASH
/**
 * @def IF_FREE_HASH(list)
 * If @a hash is non-NULL, free @a hash and set it NULL.
 */
#define IF_FREE_HASH(hash) \
{ \
        if (hash) { \
                ecore_hash_destroy(hash); \
                hash = NULL; \
        } \
}

#undef IF_RELEASE
/**
 * @def IF_RELEASE(item)
 * If @a item is non-NULL, ecore_string_release @a item and set it NULL.
 */
#define IF_RELEASE(item) \
{ \
        if (item) { \
                ecore_string_release(item); \
                item = NULL; \
        } \
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
 * @def PTRINT_TO_INT(x)
 * Does a cast from a pointer to the type int, if debug is enabled at 
 * compile-time it will add a runtime check if any information get lost.
 */
#ifdef EWL_ENABLE_DEBUG
# define PTRINT_TO_INT(x) ewl_cast_pointer_to_integer(x)
#else
# define PTRINT_TO_INT(x) ((int)((long int)(x)))
#endif

/**
 * @def PRTINT_TO_INT(x)
 * Does a cast from a pointer to the type int, if debug is enabled at 
 * compile-time it will add a runtime check if any information get lost.
 */
#define INT_TO_PTRINT(x) ((void *)((long int)(x)))

/**
 * @}
 */

#endif

