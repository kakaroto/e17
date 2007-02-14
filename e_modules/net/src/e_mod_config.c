#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "e_mod_net.h"

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
   ci->app = evas_stringshare_add("");
   ci->limit = 0;
   ci->show_text = 1;
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

EAPI void 
_config_updated(const char *id) 
{
   Evas_List *l;
   Config_Item *ci;
   
   if (!cfg) return;
   ci = _config_item_get(id);
   for (l = cfg->instances; l; l = l->next) 
     {
	Instance *inst;
	
	inst = l->data;
	if (!inst) continue;
	if (!inst->gcc->id) continue;
	if (strcmp(inst->gcc->id, id)) continue;
	if (!inst->timer)
	  inst->timer = ecore_timer_add(0.5, _cb_poll, inst);
	if (!ci->show_text)
	  edje_object_signal_emit(inst->o_net, "e,state,text,hide", "");
	else
	  edje_object_signal_emit(inst->o_net, "e,state,text,show", "");
	  
	break;
     }
}
