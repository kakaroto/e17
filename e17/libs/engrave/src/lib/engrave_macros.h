#ifndef ENGRAVE_MACROS_H
#define ENGRAVE_MACROS_H

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

#endif

