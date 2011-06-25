#include "EMap.h"
#include "emap_private.h"

int LOG_DOMAIN;

static int count = 0;

int
emap_init()
{
   if (count++ > 1) return count;

   ecore_init();
   ecore_file_init();

   LOG_DOMAIN = eina_log_domain_register("EMap", "\033[34;1m");

   return count;
}
int
emap_shutdown()
{
   if(count-- > 1) return count;

   eina_log_domain_unregister(LOG_DOMAIN);

   ecore_file_shutdown();
   ecore_shutdown();

   return count;
}

