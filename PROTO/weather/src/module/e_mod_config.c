#include <e.h>
#include "e_mod_config.h"
#include "e_mod_main.h"
#include "e_mod_gadcon.h"

/* public functions */
EAPI Config_Item *
_weather_config_item_get(const char *id) 
{
   Config_Item *ci;

   if (!id)
     {
        char buf[128];
	int  num = 0;

	/* Create id */
	if (weather_cfg->items)
	  {
	     const char *p;

	     ci = eina_list_last(weather_cfg->items)->data;
	     p = strrchr(ci->id, '.');
	     if (p) num = atoi(p + 1) + 1;
	  }
	snprintf(buf, sizeof(buf), "%s.%d", _gc_name(), num);
	id = buf;
     }
   else
     {
        Eina_List *l;

	for (l = weather_cfg->items; l; l = l->next) 
	  {
	     ci = l->data;
	     if (!ci->id) continue;
	     if (!strcmp(ci->id, id)) return ci;
	  }
     }
   ci = E_NEW(Config_Item, 1);
   ci->id = eina_stringshare_add(id);
   ci->degrees = 0;
   ci->location = NULL;
   ci->plugin = NULL;
   ci->poll_time = 60.0;
   weather_cfg->items = eina_list_append(weather_cfg->items, ci);
   return ci;
}

EAPI void 
_weather_config_updated(Config_Item *ci) 
{

}

EAPI void 
_weather_config_new(void) 
{
   char buf[PATH_MAX];

   weather_cfg = E_NEW(Config, 1);
   weather_cfg->version = (MOD_CONFIG_FILE_EPOCH << 16);

#define IFMODCFG(v) \
   if ((weather_cfg->version & 0xffff) < v) {
#define IFMODCFGEND }

   IFMODCFG(0x008d);
   _weather_config_item_get(NULL);
   IFMODCFGEND;

   weather_cfg->version = MOD_CONFIG_FILE_VERSION;

   /* set any config limits here with E_CONFIG_LIMIT */

   e_config_save_queue();
}

EAPI void 
_weather_config_free(void) 
{
   E_FREE(weather_cfg);
}
