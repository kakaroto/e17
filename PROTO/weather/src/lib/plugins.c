// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#include "EWeather_Plugins.h"
#include "../../config.h"

Eina_List *eweather_plugins_list_get()
{
   Eina_List *l2, *l2_next;
   Eina_List *l = ecore_file_ls(PLUGINSDIR);
   char *s;

   EINA_LIST_FOREACH_SAFE(l, l2, l2_next, s)
     {
	char *strip_ext = ecore_file_strip_ext(s);
	const char *ext = s + strlen(strip_ext);
	free(strip_ext);
	if(strcmp(".so", ext))
	{
	    l = eina_list_remove(l, s);
	    free(s);
	 }
     }
   return l;
}

void eweather_plugin_load(EWeather *eweather)
{
   char buf[1024];

   snprintf(buf, sizeof(buf), PLUGINSDIR"/%s", eweather->plugin.plugin_name);
   eweather->plugin.module = eina_module_new(buf);
   eina_module_load(eweather->plugin.module);
   eweather->plugin.plugin = eina_module_symbol_get(eweather->plugin.module, "_plugin_class");

   if(eweather->plugin.plugin)
     eweather->plugin.plugin->init(eweather, "FRXX3409");
}

void eweather_plugin_shutdown(EWeather *eweather)
{
   if(eweather->plugin.plugin)
     eweather->plugin.plugin->shutdown(eweather);
   eina_module_free(eweather->plugin.module);
}


void eweather_plugin_update(EWeather *eweather)
{
   if(eweather->func.update_cb)
     eweather->func.update_cb(eweather->func.data, eweather);
}

