// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#ifndef  EWEATHER_PLUGINS_INC
#define  EWEATHER_PLUGINS_INC

#include "EWeather.h"
#include <Ecore_File.h>
#include <string.h>
#include <stdio.h>

typedef struct EWeather_Plugin EWeather_Plugin;
typedef void (*Plugin_Init) (EWeather *eweather, const char *code);
typedef void (*Plugin_Shutdown) (EWeather *eweather);

struct EWeather_Data
{
   EWeather_Type type;
   int temp;
   int temp_min;
   int temp_max;
   char city[256];
   char region[256];
   char country[256];
};


struct EWeather
{
   struct
     {
	const char *plugin_name;
	EWeather_Plugin *plugin;
	Eina_Module *module;
	void *data;
     } plugin;

   struct {
	Update_Cb update_cb;
	void *data;
   } func;

   struct
     {
	const char *host;
	int port;
     } proxy;
   int poll_time;

   Eina_List *data;
};

struct EWeather_Plugin
{
   Plugin_Init init;
   Plugin_Shutdown shutdown;
};

void eweather_plugin_load(EWeather *eweather);
EAPI void eweather_plugin_update(EWeather *eweather);

#endif
