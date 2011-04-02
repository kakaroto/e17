#include "EWeather_Plugins.h"

#include <Ecore_Con.h>
#include <Ecore.h>
#include <stdio.h>

typedef struct Instance Instance;

static void _init(EWeather *eweather);
static void _shutdown(EWeather *eweather);
static void _poll_time_updated(EWeather *eweather);
static void _code_updated(EWeather *eweather);
static Eina_Bool _server_add(void *data, int type, void *event);
static Eina_Bool _server_del(void *data, int type, void *event);
static Eina_Bool _server_data(void *data, int type, void *event);
static Eina_Bool _weather_cb_check(void *data);
static EWeather_Type _weather_type_get(int id);
static Eina_Bool _server_data(void *data, int type, void *event);
static int _parse(Instance* inst);


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

   const char *host;
};

EAPI EWeather_Plugin _plugin_class =
{
   "Yahoo", 
   "http://weather.yahoo.com/",
   PACKAGE_DATA_DIR"/yahoo_logo.jpg",
   _init,
   _shutdown,
   _poll_time_updated,
   _code_updated
};

static void _init(EWeather *eweather)
{
   Instance *inst = calloc(1, sizeof(Instance));
   eweather->plugin.data = inst;
   inst->weather = eweather;
   inst->host = eina_stringshare_add("weather.yahooapis.com");

   ecore_con_init();
   
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

   if (inst->host) eina_stringshare_del(inst->host);

   if (inst->buffer) free(inst->buffer);

   if (inst->check_timer) ecore_timer_del(inst->check_timer);
   if (inst->add_handler) ecore_event_handler_del(inst->add_handler);
   if (inst->data_handler) ecore_event_handler_del(inst->data_handler);
   if (inst->del_handler) ecore_event_handler_del(inst->del_handler);

   if (inst->server) ecore_con_server_del(inst->server);

   ecore_con_shutdown();
   free(inst);
}

static void _poll_time_updated(EWeather *eweather)
{
   Instance *inst = eweather->plugin.data;

   if(inst->check_timer)
     ecore_timer_del(inst->check_timer);

   inst->check_timer =
      ecore_timer_add(0, _weather_cb_check, inst);
}

static void _code_updated(EWeather *eweather)
{
   Instance *inst = eweather->plugin.data;

   if(inst->check_timer)
     ecore_timer_del(inst->check_timer);

   inst->check_timer =
      ecore_timer_add(0, _weather_cb_check, inst);
}

   static Eina_Bool
_weather_cb_check(void *data)
{
   Instance *inst;

   if (!(inst = data)) return EINA_FALSE;
   if (inst->server) ecore_con_server_del(inst->server);
   inst->server = NULL;

   if (inst->weather->proxy.port != 0)
     inst->server =
	ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM, inst->weather->proxy.host,
	      inst->weather->proxy.port, inst);
   else
     inst->server =
	ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM, inst->host, 80, inst);

   if (!inst->server) return EINA_FALSE;

   ecore_timer_interval_set(inst->check_timer, inst->weather->poll_time);
   return EINA_TRUE;
}

   static Eina_Bool
_server_add(void *data, int type, void *event)
{
   Instance *inst;
   Ecore_Con_Event_Server_Add *ev;
   char buf[1024];


   if (!(inst = data)) return EINA_TRUE;
   if(!inst->weather->code) return EINA_FALSE;

   ev = event;
   if ((!inst->server) || (inst->server != ev->server)) return EINA_TRUE;

   snprintf(buf, sizeof(buf), "GET http://%s/forecastrss?w=%s HTTP/1.1\r\nHost: %s\r\n\r\n",
	 inst->host, inst->weather->code, inst->host);
   ecore_con_server_send(inst->server, buf, strlen (buf));
   return EINA_FALSE;
}

   static Eina_Bool
_server_del(void *data, int type, void *event)
{
   Instance *inst;
   Ecore_Con_Event_Server_Del *ev;
   int ret;

   inst = data;
   ev = event;
   if ((!inst->server) || (inst->server != ev->server)) return EINA_TRUE;

   ecore_con_server_del(inst->server);
   inst->server = NULL;

   ret = _parse(inst);

   inst->bufsize = 0;
   inst->cursize = 0;

   if(inst->buffer) free(inst->buffer);
   inst->buffer = NULL;
   return EINA_FALSE;
}

   static Eina_Bool
_server_data(void *data, int type, void *event)
{
   Instance *inst;
   Ecore_Con_Event_Server_Data *ev;

   inst = data;
   ev = event;

   if ((!inst->server) || (inst->server != ev->server)) return EINA_TRUE;
   while ((inst->cursize + ev->size) >= inst->bufsize)
     {
	inst->bufsize += 4096;
	inst->buffer = realloc(inst->buffer, inst->bufsize);
     }

   memcpy(inst->buffer + inst->cursize, ev->data, ev->size);
   inst->cursize += ev->size;
   inst->buffer[inst->cursize] = 0;
   _parse(inst);
   return EINA_FALSE;
}

   static int
_parse(Instance *inst)
{
   char *needle;
   char location[1024];
   char day[1024];
   char date[1024];
   EWeather_Data *e_data = eweather_data_current_get(inst->weather);
   EWeather_Data *e_data_current;
   int code;

   location[0] = 0;

   if (!inst->buffer) return 0;

   //printf("%s\n", inst->buffer);
   needle = strstr(inst->buffer, "<yweather:location");
   if (!needle) goto error;

   needle = strstr(needle, "city=\"");
   if (!needle) goto error;
   needle+=6;
   sscanf(needle, "%[^\"]\"", e_data->city);

   needle = strstr(needle, "region=\"");
   if (!needle) goto error;
   needle+=8;
   sscanf(needle, "%[^\"]\"", e_data->region);

   needle = strstr(needle, "country=\"");
   if (!needle) goto error;
   needle+=9;
   sscanf(needle, "%[^\"]\"", e_data->country);

   needle = strstr(needle, "<pubDate>");
   if (!needle) goto error;
   needle += 9; 
   sscanf(needle, "%[^<]<", e_data->date);


   needle = strstr(needle, "<yweather:condition");
   if (!needle) goto error;

   needle = strstr(needle, "code=\"");
   if (!needle) goto error;
   needle+=6;
   sscanf(needle, "%d\"", &code);

   e_data->type = _weather_type_get(code);

   needle = strstr(needle, "temp=\"");
   if (!needle) goto error;
   needle+=6;
   sscanf(needle, "%lf\"", &(e_data->temp));


   needle = strstr(needle, "<b>Forecast:</b><BR />");
   if (!needle) goto error;

   needle = strstr(needle, "High: ");
   if (!needle) goto error;
   needle+=6;
   sscanf(needle, "%lf ", &(e_data->temp_max));

   needle = strstr(needle, "Low: ");
   if (!needle) goto error;
   needle+=5;
   sscanf(needle, "%lf ", &(e_data->temp_min));

   e_data_current = e_data;

   /*printf("CITY %s\n", e_data->city);
   printf("REGION %s\n", e_data->region);
   printf("COUNTRY %s\n", e_data->country);
   printf("TYPE %d\n", e_data->type);
   printf("TEMP %d\n", e_data->temp);
   printf("TEMP MIN %d\n", e_data->temp_min);
   printf("TEMP MAX %d\n", e_data->temp_max);
   */

   //tomorrow
   e_data = eweather_data_get(inst->weather, 1);

   needle = strstr(needle, "<yweather:forecast day=\"");
   if (!needle) goto error;
   needle+=24;

   needle = strstr(needle, "<yweather:forecast day=\"");
   if (!needle) goto error;
   needle+=24;
   sscanf(needle, "%[^\"]\"", day);
 
   needle = strstr(needle, "date=\"");
   if (!needle) goto error;
   needle+=6;
   sscanf(needle, "%[^\"]\"", date);

   snprintf(e_data->date, 256, "%s %s", day, date);

   needle = strstr(needle, "low=\"");
   if (!needle) goto error;
   needle+=5;
   sscanf(needle, "%lf\"", &(e_data->temp_min));

   needle = strstr(needle, "high=\"");
   if (!needle) goto error;
   needle+=6;
   sscanf(needle, "%lf\"", &(e_data->temp_max));

   e_data->temp = ( e_data->temp_min + e_data->temp_max ) / 2;

   needle = strstr(needle, "code=\"");
   if (!needle) goto error;
   needle+=6;
   sscanf(needle, "%d\"", &code);
   e_data->type = _weather_type_get(code);

   strcpy(e_data->country, e_data_current->country);
   strcpy(e_data->region, e_data_current->region);
   strcpy(e_data->city, e_data_current->city);


   eweather_plugin_update(inst->weather);
   return 1;
error:
   //printf ("ERROR: Couldn't parse info\n");
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

