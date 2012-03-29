#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string.h>
#include <stdlib.h>

#include <Exotic.h>

EAPI char *
exotic_realpath(const char *path, char *resolved_path)
{
   /* No symbolic path nor anything meaningfull on this OS */
   return strcpy(resolved_path, path);
}

#undef realloc

EAPI void *
exotic_realloc(void *in, size_t size)
{
   if (!in)
     {
        if (!size) return NULL;
        return malloc(size);
     }
   if (!size)
     {
        free(in);
        return NULL;
     }

   return realloc(in, size);
}

#undef free

EAPI void
exotic_free(void *in)
{
   if (in) free(in);
}
