#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string.h>
#include <stdlib.h>

#include <Exotic.h>

EAPI char *
exotic_realpath(const char *path, char *resolved_path)
{
   char *real = lrealpath(path);

   if (real)
     {
        if (resolved_path)
          {
             memcpy(resolved_path, real, PATH_MAX);
             free(real);
             return resolved_path;
          }
        else
          {
             return real;
          }
     }

   return NULL;
}

