#ifndef  EWEATHER_PLUGINS_H
#define  EWEATHER_PLUGINS_H

#include <EWeather.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EFL_EWEATHER_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_EINA_BUILD */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

typedef struct EWeather_Plugin EWeather_Plugin;
typedef void (*Plugin_Init) (EWeather *eweather);
typedef void (*Plugin_Shutdown) (EWeather *eweather);
typedef void (*Plugin_Poll_Time_Updated) (EWeather *eweather);
typedef void (*Plugin_Code_Updated) (EWeather *eweather);


struct EWeather_Data
{
   EWeather_Type type;
   double temp;
   double temp_min;
   double temp_max;
   char city[256];
   char region[256];
   char country[256];
   char date[256];
};


struct EWeather
{
   struct
     {
	Eina_Array *array;
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
   EWeather_Temp temp_type;
   const char *code;

   Eina_List *data;
};

struct EWeather_Plugin
{
   const char *name;
   const char *url;
   const char *logo;
   Plugin_Init init;
   Plugin_Shutdown shutdown;
   Plugin_Poll_Time_Updated poll_time_updated;
   Plugin_Code_Updated code_updated;
};

void eweather_plugin_load(EWeather *eweather);
EAPI void eweather_plugin_update(EWeather *eweather);
void eweather_plugin_shutdown(EWeather *eweather);

#endif
