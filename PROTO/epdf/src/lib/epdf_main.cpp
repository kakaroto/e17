#ifdef HAVE_CONFIG_H
# include <config.h>
#endif


#include <GlobalParams.h>

#include "Epdf.h"

static int _epdf_main_count = 0;

int
epdf_init (void)
{
  if (_epdf_main_count)
    goto beach;

  if (!(globalParams = new GlobalParams(NULL)))
    return 0;

  if (!eina_list_init())
    {
      delete (globalParams);
      return 0;
    }

 beach:
  return ++_epdf_main_count;
}

int
epdf_shutdown()
{
  if (_epdf_main_count != 1)
    goto beach;

  delete globalParams;
  eina_list_shutdown();

 beach:
  return --_epdf_main_count;
}

const char *
epdf_poppler_version_get (void)
{
  return POPPLER_VERSION;
}
