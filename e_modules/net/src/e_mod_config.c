#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"

EAPI Config_Item *
_config_item_get(const char *id) 
{
   Evas_List *l;
   Config_Item *ci;
   
   for (l = cfg->items; l; l = l->next) 
     {
	ci = l->data;
	if (!ci->id) continue;
	if (!strcmp(ci->id, id)) return ci;
     }
   ci = E_NEW(Config_Item, 1);
   ci->id = evas_stringshare_add(id);
   ci->device = evas_stringshare_add("eth0");
   ci->poll_time = 1.0;
   cfg->items = evas_list_append(cfg->items, ci);
   return ci;
}

EAPI Ecore_List *
_config_devices_get(void) 
{
   Ecore_List *devs = NULL;
   FILE *f;
   char buf[256];
   char dev[64];
   unsigned long dummy;
   
   f = fopen("/proc/net/dev", "r");
   if (!f) return NULL;

   devs = ecore_list_new();
   ecore_list_set_free_cb(devs, free);
   while (fgets(buf, 256, f)) 
     {
	int i = 0;
	
	for (; buf[i] != 0; i++)
	  if (buf[i] == ':') buf[i] = ' ';

	if (sscanf (buf, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu "
		    "%lu %lu %lu %lu\n", dev, &dummy, &dummy, &dummy,
		    &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
		    &dummy, &dummy, &dummy, &dummy, &dummy, &dummy) < 17)
	  continue;
	ecore_list_append(devs, strdup(dev));
     }
   fclose(f);
   if (devs) ecore_list_goto_first(devs);
   return devs;
}
