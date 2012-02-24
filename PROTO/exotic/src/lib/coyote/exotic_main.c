#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string.h>

#include "exotic_private.h"

#include <Exotic.h>

EAPI void
exotic_external_set(const char *name, void *value)
{
   if (strcasecmp(name, "filesystem") == 0)
     exotic_filesystem_set(value);
}

