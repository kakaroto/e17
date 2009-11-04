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
   weather_cfg->items = eina_list_append(weather_cfg->items, ci);
   return ci;
}

EAPI void 
_weather_config_updated(Config_Item *ci) 
{

}
