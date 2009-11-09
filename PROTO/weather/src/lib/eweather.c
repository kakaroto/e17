// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

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
   if(eweather->plugin.module)
     {
	eweather_plugin_shutdown(eweather);
	eina_stringshare_del(eweather->plugin.plugin_name);
     }

   EINA_LIST_FREE(eweather->data, e_data)
      free(e_data);
   free(eweather);
}


void eweather_plugin_set(EWeather *eweather, const char *plugin)
{
    if(eweather->plugin.module)
    {
      eweather_plugin_shutdown(eweather);
      eina_stringshare_del(eweather->plugin.plugin_name);
    }

    eweather->plugin.plugin_name = eina_stringshare_add(plugin);
    eweather_plugin_load(eweather);
}

EWeather_Type eweather_data_type_get(EWeather_Data *eweather_data)
{
   return eweather_data->type;
}

int eweather_data_temp_get(EWeather_Data *eweather_data)
{
   return eweather_data->temp;
}

int eweather_data_temp_min_get(EWeather_Data *eweather_data)
{
   return eweather_data->temp_min;
}


int eweather_data_temp_max_get(EWeather_Data *eweather_data)
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


