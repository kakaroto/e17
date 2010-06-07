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

  if (!(globalParams = new GlobalParams()))
    return 0;

  if (!eina_init())
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
  eina_shutdown();

 beach:
  return --_epdf_main_count;
}

const char *
epdf_backend_version_get (void)
{
  return POPPLER_VERSION;
}

Eina_Bool epdf_fonts_antialias_get (void)
{
  return globalParams->getAntialias();
}

void epdf_fonts_antialias_set (Eina_Bool on)
{
  /* Nice API */
  globalParams->setAntialias((char*)(on ? "yes" : "no"));
}

Eina_Bool epdf_lines_antialias_get (void)
{
    return globalParams->getVectorAntialias();
}

void epdf_lines_antialias_set (Eina_Bool on)
{
    globalParams->setVectorAntialias((char*)(on ? "yes" : "no"));
}
