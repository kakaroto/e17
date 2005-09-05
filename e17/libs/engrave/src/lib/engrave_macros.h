#ifndef ENGRAVE_MACROS_H
#define ENGRAVE_MACROS_H

#undef NEW
#define NEW(type, num) calloc(num, sizeof(type))

#undef FREE
#define FREE(val) \
{ \
  free(val); val = NULL; \
}

#undef IF_FREE
#define IF_FREE(val) \
{ \
  if (val) FREE(val) \
  val = NULL; \
}

#endif

