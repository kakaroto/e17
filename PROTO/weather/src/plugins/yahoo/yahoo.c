// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#include "EWeather_Plugins.h"

#include <Ecore_Con.h>
#include <Ecore.h>
#include <stdio.h>

typedef struct Instance Instance;

static void _init(EWeather *eweather, const char *code);
static void _shutdown(EWeather *eweather);
static int _server_add(void *data, int type, void *event);
static int _server_del(void *data, int type, void *event);
static int _server_data(void *data, int type, void *event);
static int _weather_cb_check(void *data);
static EWeather_Type _weather_type_get(int id);
static int _server_data(void *data, int type, void *event);
static int _parse(void *data);


struct _Id_Type
{
   int id;
   EWeather_Type type;
};
 
static struct _Id_Type _tab[] =
{
     {0, EWEATHER_TYPE_ISOLATED_THUNDERSTORMS}, //tornado
     {1, EWEATHER_TYPE_SCATTERED_THUNDERSTORMS}, //tropical storm
     {2, EWEATHER_TYPE_SCATTERED_THUNDERSTORMS}, //hurricane
     {3, EWEATHER_TYPE_SCATTERED_THUNDERSTORMS}, //severe thunderstorms
     {4, EWEATHER_TYPE_THUNDERSTORMS}, //thunderstorms
     {5, EWEATHER_TYPE_RAIN_SNOW}, //mixed rain and snow
     {6, EWEATHER_TYPE_RAIN}, //mixed rain and sleet
     {7, EWEATHER_TYPE_RAIN_SNOW}, //mixed snow and sleet
     {8, EWEATHER_TYPE_RAIN}, //freezing drizzle
     {9, EWEATHER_TYPE_RAIN}, //drizzle
     {10, EWEATHER_TYPE_RAIN}, //freezing rain
     {11, EWEATHER_TYPE_RAIN}, //showers
     {12, EWEATHER_TYPE_RAIN}, //showers
     {13, EWEATHER_TYPE_SNOW}, //snow flurries
     {14, EWEATHER_TYPE_SNOW}, //light snow showers
     {15, EWEATHER_TYPE_SNOW}, //blowing snow
     {16, EWEATHER_TYPE_SNOW}, //snow
     {17, EWEATHER_TYPE_RAIN}, //hail
     {18, EWEATHER_TYPE_SNOW}, //sleet
     {19, EWEATHER_TYPE_FOGGY}, //dust
     {20, EWEATHER_TYPE_FOGGY}, //foggy
     {21, EWEATHER_TYPE_FOGGY}, //haze
     {22, EWEATHER_TYPE_FOGGY}, //smoky
     {23, EWEATHER_TYPE_WINDY}, //blustery
     {24, EWEATHER_TYPE_WINDY}, //windy
     {25, EWEATHER_TYPE_WINDY}, //cold
     {26, EWEATHER_TYPE_CLOUDY}, //cloudy
     {27, EWEATHER_TYPE_MOSTLY_CLOUDY_NIGHT}, //mostly cloudy (night)
     {28, EWEATHER_TYPE_MOSTLY_CLOUDY_DAY}, //mostly cloudy (day)
     {29, EWEATHER_TYPE_PARTLY_CLOUDY_NIGHT}, //partly cloudy (night)
     {30, EWEATHER_TYPE_PARTLY_CLOUDY_DAY}, //partly cloudy (day)
     {31, EWEATHER_TYPE_CLEAR_NIGHT}, //clear (night)
     {32, EWEATHER_TYPE_SUNNY}, //sunny
     {33, EWEATHER_TYPE_CLEAR_NIGHT}, //fair (night)
     {34, EWEATHER_TYPE_SUNNY}, //fair (day)
     {35, EWEATHER_TYPE_RAIN_SNOW}, //mixed rain and hail
     {36, EWEATHER_TYPE_SUNNY}, //hot
     {37, EWEATHER_TYPE_ISOLATED_THUNDERSTORMS}, //isolated thunderstorms
     {38, EWEATHER_TYPE_SCATTERED_THUNDERSTORMS}, //scattered thunderstorms
     {39, EWEATHER_TYPE_SCATTERED_THUNDERSTORMS}, //scattered thunderstorms
     {40, EWEATHER_TYPE_RAIN}, // scattered showers
     {41, EWEATHER_TYPE_HEAVY_SNOW}, //heavy snow
     {42, EWEATHER_TYPE_RAIN_SNOW}, //scattered snow showers
     {43, EWEATHER_TYPE_HEAVY_SNOW}, //heavy snow
     {44, EWEATHER_TYPE_CLOUDY}, //partly cloudy
     {45, EWEATHER_TYPE_RAIN}, // thundershowers
     {46, EWEATHER_TYPE_HEAVY_SNOW}, //snow showers
     {47, EWEATHER_TYPE_ISOLATED_THUNDERSTORMS}, //isolated thundershowers
     {3200, EWEATHER_TYPE_UNKNOWN}
};

struct Instance
{
   EWeather *weather;

   Ecore_Con_Server *server;
   Ecore_Event_Handler *add_handler;
   Ecore_Event_Handler *del_handler;
   Ecore_Event_Handler *data_handler;
   Ecore_Timer *check_timer;

   char *buffer, *location;
   int bufsize, cursize;

   const char *code;
   const char *host;
};

EAPI EWeather_Plugin _plugin_class =
{
   _init,
   _shutdown
};

static void _init(EWeather *eweather, const char *code)
{
   Instance *inst = calloc(1, sizeof(Instance));
   eweather->plugin.data = inst;
   inst->weather = eweather;
   inst->code = eina_stringshare_add(code);
   inst->host = eina_stringshare_add("weather.yahooapis.com");

   inst->add_handler =
      ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD,
	    _server_add, inst);
   inst->del_handler =
      ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL,
	    _server_del, inst);
   inst->data_handler =
      ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA,
	    _server_data, inst);

   inst->check_timer =
      ecore_timer_add(0, _weather_cb_check, inst);
}

static void _shutdown(EWeather *eweather)
{
   Instance *inst = eweather->plugin.data;
   if (inst->code) eina_stringshare_del(inst->code);
   if (inst->host) eina_stringshare_del(inst->host);

   if (inst->buffer) free(inst->buffer);

   if (inst->check_timer) ecore_timer_del(inst->check_timer);
   if (inst->add_handler) ecore_event_handler_del(inst->add_handler);
   if (inst->data_handler) ecore_event_handler_del(inst->data_handler);
   if (inst->del_handler) ecore_event_handler_del(inst->del_handler);
   if (inst->server) ecore_con_server_del(inst->server);

   free(inst);
}

   static int
_weather_cb_check(void *data)
{
   Instance *inst;

   if (!(inst = data)) return 0;
   if (inst->server) ecore_con_server_del(inst->server);
   inst->server = NULL;

   if (inst->weather->proxy.port != 0)
     inst->server =
	ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM, inst->weather->proxy.host,
	      inst->weather->proxy.port, inst);
   else
     inst->server =
	ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM, inst->host, 80, inst);

   if (!inst->server) return 0;

   ecore_timer_interval_set(inst->check_timer, inst->weather->poll_time);
   return 1;
}

   static int
_server_add(void *data, int type, void *event)
{
   Instance *inst;
   Ecore_Con_Event_Server_Add *ev;
   char buf[1024], icao[1024];

   if (!(inst = data)) return 1;

   ev = event;
   if ((!inst->server) || (inst->server != ev->server)) return 1;

   snprintf(icao, sizeof(icao), "/icao/%s/rss.php", inst->code);
   snprintf(buf, sizeof(buf), "GET http://%s/forecastrss?p=%s HTTP/1.1\r\nHost: %s\r\n\r\n",
	 inst->host, inst->code, inst->host);
   ecore_con_server_send(inst->server, buf, strlen (buf));
   return 0;
}

   static int
_server_del(void *data, int type, void *event)
{
   Instance *inst;
   Ecore_Con_Event_Server_Del *ev;
   int ret;

   inst = data;
   ev = event;
   if ((!inst->server) || (inst->server != ev->server)) return 1;

   ecore_con_server_del(inst->server);
   inst->server = NULL;

   ret = _parse(inst);

   inst->bufsize = 0;
   inst->cursize = 0;

   if(inst->buffer) free(inst->buffer);
   inst->buffer = NULL;
   return 0;
}

   static int
_server_data(void *data, int type, void *event)
{
   Instance *inst;
   Ecore_Con_Event_Server_Data *ev;

   inst = data;
   ev = event;

   if ((!inst->server) || (inst->server != ev->server)) return 1;
   while ((inst->cursize + ev->size) >= inst->bufsize)
     {
	inst->bufsize += 4096;
	inst->buffer = realloc(inst->buffer, inst->bufsize);
     }

   memcpy(inst->buffer + inst->cursize, ev->data, ev->size);
   inst->cursize += ev->size;
   inst->buffer[inst->cursize] = 0;
   return 0;
}

   static int
_parse(void *data)
{
   Instance *inst;
   char *needle, *ext;
   char location[1024];
   EWeather_Data *e_data = eweather_data_current_get(inst->weather);
   int code;

   location[0] = 0;

   if (!(inst = data)) return 0;
   if (inst->buffer == NULL) return 0;

   //printf("%s\n", inst->buffer);

   needle = strstr(inst->buffer, "<yweather:location");
   if (!needle) goto error;

   needle = strstr(needle, "city=\"");
   needle+=6;
   sscanf(needle, "%[^\"]\"", e_data->city);

   needle = strstr(needle, "region=\"");
   needle+=8;
   sscanf(needle, "%[^\"]\"", e_data->region);

   needle = strstr(needle, "country=\"");
   needle+=9;
   sscanf(needle, "%[^\"]\"", e_data->country);


   needle = strstr(needle, "<yweather:condition");

   needle = strstr(needle, "code=\"");
   needle+=6;
   sscanf(needle, "%d\"", &code);

   e_data->type = _weather_type_get(code);

   needle = strstr(needle, "temp=\"");
   needle+=6;
   sscanf(needle, "%d\"", &(e_data->temp));


   needle = strstr(needle, "<b>Forecast:</b><BR />");

   needle = strstr(needle, "High: ");
   needle+=6;
   sscanf(needle, "%d ", &(e_data->temp_max));

   needle = strstr(needle, "Low: ");
   needle+=5;
   sscanf(needle, "%d ", &(e_data->temp_min));

   printf("CITY %s\n", e_data->city);
   printf("REGION %s\n", e_data->region);
   printf("COUNTRY %s\n", e_data->country);
   printf("TYPE %d\n", e_data->type);
   printf("TEMP %d\n", e_data->temp);
   printf("TEMP MIN %d\n", e_data->temp_min);
   printf("TEMP MAX %d\n", e_data->temp_max);

   eweather_plugin_update(inst->weather);
   return 1;
error:
   printf ("ERROR: Couldn't parse info\n");
   return 0;
}




static EWeather_Type _weather_type_get(int id)
{
   int i;
   for (i = 0; i < sizeof (_tab) / sizeof (struct _Id_Type); ++i)
     if (_tab[i].id == id)
       {
	  return _tab[i].type;
       }

   return EWEATHER_TYPE_UNKNOWN;
}

