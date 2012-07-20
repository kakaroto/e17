#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Eina.h>

#include "Vigrid.h"

static int _vigrid_count = 0;

EAPI int
vigrid_init(void)
{
   if (_vigrid_count++ != 0) return _vigrid_count;

   eina_init();

   return _vigrid_count;
}

EAPI int
vigrid_shutdown(void)
{
   if (--_vigrid_count != 0) return _vigrid_count;

   eina_shutdown();

   return _vigrid_count;
}
