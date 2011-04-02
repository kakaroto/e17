#include "EWeather_Plugins.h"

EWeather *eweather_new()
{
   EWeather *e = calloc(1, sizeof(EWeather));
   e->poll_time = 30*60;
   return e;
}

void eweather_free(EWeather *eweather)
{
   EWeather_Data *e_data;
   if(eweather->plugin.array)
     {
	eina_module_list_unload(eweather->plugin.array);
	eina_module_list_free(eweather->plugin.array);
     }

   EINA_LIST_FREE(eweather->data, e_data)
      free(e_data);
   free(eweather);
}


void eweather_plugin_set(EWeather *eweather, Eina_Module *module)
{
    if(eweather->plugin.module)
    {
      eweather_plugin_shutdown(eweather);
    }

    eweather->plugin.module = module;
    if(module)
      eweather_plugin_load(eweather);
}

void eweather_plugin_byname_set(EWeather *eweather, const char *name)
{
   Eina_Array *array;
   Eina_Module *m;
   int i;
   Eina_Array_Iterator it;
   
   array = eweather_plugins_list_get(eweather);
   EINA_ARRAY_ITER_NEXT(array, i, m, it)
     {
	EWeather_Plugin *plugin = eina_module_symbol_get(m, "_plugin_class");
	if(plugin && !strcmp(name, plugin->name))
	  {
	     eweather_plugin_set(eweather, m);
	     break ;
	  }
     }
}

void eweather_poll_time_set(EWeather *eweather, int poll_time)
{
   eweather->poll_time = poll_time;

   if(eweather->plugin.plugin && eweather->plugin.plugin->poll_time_updated)
	eweather->plugin.plugin->poll_time_updated(eweather);
}

void eweather_code_set(EWeather *eweather, const char *code)
{
   if(eweather->code)
     eina_stringshare_del(eweather->code);
   eweather->code = eina_stringshare_add(code);

   if(eweather->plugin.plugin && eweather->plugin.plugin->code_updated)
	eweather->plugin.plugin->code_updated(eweather);
}

void eweather_temp_type_set(EWeather *eweather, EWeather_Temp type)
{
   eweather->temp_type = type;
}

EWeather_Temp eweather_temp_type_get(EWeather *eweather)
{
   return eweather->temp_type;
}

EWeather_Type eweather_data_type_get(EWeather_Data *eweather_data)
{
   return eweather_data->type;
}

double eweather_data_temp_get(EWeather_Data *eweather_data)
{
   return eweather_data->temp;
}

double eweather_data_temp_min_get(EWeather_Data *eweather_data)
{
   return eweather_data->temp_min;
}


double eweather_data_temp_max_get(EWeather_Data *eweather_data)
{
   return eweather_data->temp_max;
}

const char *eweather_data_city_get(EWeather_Data *eweather_data)
{
   return eweather_data->city;
}


const char *eweather_data_region_get(EWeather_Data *eweather_data)
{
   return eweather_data->region;
}

const char *eweather_data_country_get(EWeather_Data *eweather_data)
{
   return eweather_data->country;
}

const char *eweather_data_date_get(EWeather_Data *eweather_data)
{
   return eweather_data->date;
}

EWeather_Data *eweather_data_current_get(EWeather *eweather)
{
   if(!eweather->data)
	eweather->data = eina_list_append(eweather->data, calloc(1, sizeof(EWeather_Data)));
   return eina_list_data_get(eweather->data);
}

EWeather_Data *eweather_data_get(EWeather *eweather, int num)
{
   while(eina_list_count(eweather->data) <= num)
     {
	EWeather_Data *e_data =  calloc(1, sizeof(EWeather_Data));
	eweather->data = eina_list_append(eweather->data, e_data);
     }
   return eina_list_nth(eweather->data, num);
}

unsigned int eweather_data_count(EWeather *eweather)
{
   return eina_list_count(eweather->data);
}

void eweather_callbacks_set(EWeather *eweather, Update_Cb update_cb, void *data)
{
   eweather->func.data = data;
   eweather->func.update_cb = update_cb;
}

double eweather_utils_celcius_get(double farenheit)
{
   return (farenheit - 32.) * 5./9.;
}

