#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "Epdf.h"


static int _epdf_main_count = 0;

int
epdf_init (void)
{
   if (_epdf_main_count)
     goto beach;

   if (!eina_init())
     return 0;

 beach:
   return ++_epdf_main_count;
}

int
epdf_shutdown()
{
   if (_epdf_main_count != 1)
     goto beach;

   eina_shutdown();

 beach:
   return --_epdf_main_count;
}

const char *
epdf_backend_version_get (void)
{
   return "0.5";
}

Eina_Bool epdf_fonts_antialias_get(void)
{
   return EINA_TRUE;
}

void epdf_fonts_antialias_set(Eina_Bool on)
{
}

Eina_Bool epdf_lines_antialias_get(void)
{
   return EINA_TRUE;
}

void epdf_lines_antialias_set(Eina_Bool on)
{
}
