#include "EWeather_Plugins.h"
#include "../../config.h"

Eina_Array *eweather_plugins_list_get(EWeather *eweather)
{
   if(!eweather->plugin.array)
     {
	eweather->plugin.array = eina_module_list_get(NULL, PLUGINSDIR, EINA_FALSE, NULL, NULL);
	if(eweather->plugin.array)
	  eina_module_list_load(eweather->plugin.array);
     }

   return eweather->plugin.array;
}

void eweather_plugin_load(EWeather *eweather)
{
   eweather->plugin.plugin = eina_module_symbol_get(eweather->plugin.module, "_plugin_class");

   if(eweather->plugin.plugin)
     eweather->plugin.plugin->init(eweather);
}

void eweather_plugin_shutdown(EWeather *eweather)
{
   EWeather_Data *e_data;

   EINA_LIST_FREE(eweather->data, e_data)
      free(e_data);	

   if(eweather->plugin.plugin)
     eweather->plugin.plugin->shutdown(eweather);
}


void eweather_plugin_update(EWeather *eweather)
{
   if(eweather->func.update_cb)
     eweather->func.update_cb(eweather->func.data, eweather);
}

Eina_Module *eweather_plugin_search(EWeather *eweather, const char *name)
{
   Eina_Array *array;
   unsigned int i;
   Eina_Array_Iterator it;
   Eina_Module *m;

   array = eweather_plugins_list_get(eweather);
   EINA_ARRAY_ITER_NEXT(array, i, m, it)
     {
	EWeather_Plugin *plugin = eina_module_symbol_get(m, "_plugin_class");
	if(plugin && !strcmp(name, plugin->name))
	  return m;
     }

   return NULL;
}

int eweather_plugin_id_search(EWeather *eweather, const char *name)
{
   Eina_Array *array;
   unsigned int i;
   Eina_Array_Iterator it;
   Eina_Module *m;

   array = eweather_plugins_list_get(eweather);
   EINA_ARRAY_ITER_NEXT(array, i, m, it)
     {
	EWeather_Plugin *plugin = eina_module_symbol_get(m, "_plugin_class");
	if(plugin && !strcmp(name, plugin->name))
	  return i;
     }

   return -1;
}

const char *eweather_plugin_name_get(EWeather *eweather, int i)
{
   Eina_Array *array;
   Eina_Module *m;

   array = eweather_plugins_list_get(eweather);

   m = eina_array_data_get(array, i);
   if(!m) return NULL;

   EWeather_Plugin *plugin = eina_module_symbol_get(m, "_plugin_class");
   if(!plugin) return NULL;

   return plugin->name;
}

const char *eweather_plugin_logo_get(EWeather *eweather, int i)
{
   Eina_Array *array;
   Eina_Module *m;

   array = eweather_plugins_list_get(eweather);

   m = eina_array_data_get(array, i);
   if(!m) return NULL;

   EWeather_Plugin *plugin = eina_module_symbol_get(m, "_plugin_class");
   if(!plugin) return NULL;

   return plugin->logo;
}

const char *eweather_plugin_url_get(EWeather *eweather, int i)
{
   Eina_Array *array;
   Eina_Module *m;

   array = eweather_plugins_list_get(eweather);

   m = eina_array_data_get(array, i);
   if(!m) return NULL;

   EWeather_Plugin *plugin = eina_module_symbol_get(m, "_plugin_class");
   if(!plugin) return NULL;

   return plugin->url;
}

const char *eweather_plugin_current_name_get(EWeather *eweather)
{
   if(!eweather->plugin.plugin)
     return NULL;

   return eweather->plugin.plugin->name;
}

const char *eweather_plugin_current_logo_get(EWeather *eweather)
{
   if(!eweather->plugin.plugin)
     return NULL;

   return eweather->plugin.plugin->logo;
}

const char *eweather_plugin_current_url_get(EWeather *eweather)
{
   if(!eweather->plugin.plugin)
     return NULL;

   return eweather->plugin.plugin->url;
}

