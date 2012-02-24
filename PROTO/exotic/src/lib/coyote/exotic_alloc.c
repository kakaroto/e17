#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h>

#include <Exotic.h>

#if 0
EAPI void *
_malloc_r(struct _reent *r __UNUSED__, size_t size)
{
   return malloc(size);
}

EAPI void *
_calloc_r(struct _reent *r __UNUSED__, size_t nmemb, size_t size)
{
   return calloc(nmemb, size);
}

EAPI void *
_realloc_r(struct _reent *r __UNUSED__, void *data, size_t size)
{
   return realloc(data, size);
}

EAPI void
_free_r(struct _reent *r __UNUSED__, void *data)
{
   free(data);
}
#endif
