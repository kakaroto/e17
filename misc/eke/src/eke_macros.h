#ifndef EKE_MACRO
#define EKE_MACRO

#include <stdlib.h>

#undef NEW
#define NEW(type, num) calloc(num, sizeof(type));

#undef FREE
#define FREE(dat) \
{ \
    free(dat); dat = NULL; \
}

#undef IF_FREE
#define IF_FREE(dat) \
{ \
    if (dat) FREE(dat); \
}

#endif

