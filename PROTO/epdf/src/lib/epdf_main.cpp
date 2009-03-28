#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "Epdf.h"

static int _epdf_main_count = 0;

int
epdf_init (void)
{
  if (_epdf_main_count)
    goto beach;

  eina_list_init();

 beach:
  return ++_epdf_main_count;
}

int
epdf_shutdown()
{
  if (_epdf_main_count != 1)
    goto beach;

  eina_list_shutdown();

 beach:
  return --_epdf_main_count;
}

const char *
epdf_poppler_version_get (void)
{
  return POPPLER_VERSION;
}
