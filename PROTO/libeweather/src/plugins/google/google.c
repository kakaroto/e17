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
static Eina_Bool _server_error(void *data, int type, void *event);
static Eina_Bool _server_data(void *data, int type, void *event);
static Eina_Bool _weather_cb_check(void *data);
static EWeather_Type _weather_type_get(const char* id);
static Eina_Bool _server_data(void *data, int type, void *event);
static int _parse(Instance* inst);


struct _Id_Type
{
   const char * id;
   EWeather_Type type;
};
 
static struct _Id_Type _tab[] =
{
     {"Clear", EWEATHER_TYPE_SUNNY},
     {"Chance of Rain", EWEATHER_TYPE_RAIN},
     {"Chance of Snow", EWEATHER_TYPE_SNOW},
     {"Chance of Storm", EWEATHER_TYPE_ISOLATED_THUNDERSTORMS},
     {"Chance of Snow", EWEATHER_TYPE_SNOW},
     {"Cloudy", EWEATHER_TYPE_CLOUDY},
     {"Fog", EWEATHER_TYPE_FOGGY},
     {"Isolated Thunderstorms", EWEATHER_TYPE_ISOLATED_THUNDERSTORMS},
     {"Mostly Cloudy", EWEATHER_TYPE_MOSTLY_CLOUDY_DAY},
     {"Mostly Sunny", EWEATHER_TYPE_PARTLY_CLOUDY_DAY},
     {"Overcast", EWEATHER_TYPE_CLOUDY},
     {"Partly Cloudy", EWEATHER_TYPE_PARTLY_CLOUDY_DAY},
     {"Partly Sunny", EWEATHER_TYPE_PARTLY_CLOUDY_DAY},
     {"Rain", EWEATHER_TYPE_RAIN},
     {"Showers", EWEATHER_TYPE_RAIN},
     {"Sunny", EWEATHER_TYPE_SUNNY},
     {"Scattered Showers", EWEATHER_TYPE_RAIN},
     {"Scattered Thunderstorms", EWEATHER_TYPE_SCATTERED_THUNDERSTORMS},
     {"Snow Showers", EWEATHER_TYPE_RAIN_SNOW},
     {"Thunderstorms", EWEATHER_TYPE_THUNDERSTORMS},
     {"Windy", EWEATHER_TYPE_WINDY},
     {"Snow", EWEATHER_TYPE_SNOW},
//     {"/ig/images/weather/rain_snow.gif", EWEATHER_TYPE_RAIN_SNOW},
     {"Sleet", EWEATHER_TYPE_RAIN_SNOW}, /* not sure */
     {"Mist", EWEATHER_TYPE_FOGGY}, /* not sure */
     {"Icy", EWEATHER_TYPE_SNOW}, /* not sure */
     {"Dust", EWEATHER_TYPE_FOGGY}, /* not sure */
     {"Smoke", EWEATHER_TYPE_FOGGY}, /* not sure */
     {"Haze", EWEATHER_TYPE_FOGGY}, /* not sure */
     {"Flurries", EWEATHER_TYPE_SNOW}, /* not sure */
     {"", EWEATHER_TYPE_UNKNOWN}
};

struct Instance
{
   EWeather *weather;

   Ecore_Con_Server *server;
   Ecore_Event_Handler *add_handler;
   Ecore_Event_Handler *del_handler;
   Ecore_Event_Handler *err_handler;
   Ecore_Event_Handler *data_handler;
   Ecore_Timer *check_timer;

   char *buffer, *location;
   int bufsize, cursize;

   const char *host;
};

EAPI EWeather_Plugin _plugin_class =
{
   "Google", 
   "http://www.google.fr/",
   PACKAGE_DATA_DIR"/google_logo.jpg",
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
   inst->host = eina_stringshare_add("www.google.com");

   printf("INIT %d\n",ecore_con_init());

   inst->add_handler =
      ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD,
	    _server_add, inst);
   inst->del_handler =
      ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL,
	    _server_del, inst);
   inst->err_handler =
      ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ERROR,
	    _server_error, inst);
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
   if (inst->err_handler) ecore_event_handler_del(inst->err_handler);
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

   printf("CHECK\n");
   if (!(inst = data)) return EINA_FALSE;
   if (inst->server) ecore_con_server_del(inst->server);
   inst->server = NULL;

   if (inst->weather->proxy.port != 0)
     inst->server =
	ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM, inst->weather->proxy.host,
	      inst->weather->proxy.port, inst);
   else
     {
     inst->server =
	ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM, inst->host, 80, inst);

   printf("CHECX %p\n", inst->server);
     }

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
   char *s;
   int i;

   printf("SERVER ADD\n");
   if (!(inst = data)) return EINA_TRUE;
   if(!inst->weather->code) return EINA_FALSE;

   ev = event;
   if ((!inst->server) || (inst->server != ev->server)) return EINA_TRUE;

   s = strdup(inst->weather->code);
   for(i=0; i<strlen(s); i++)
     if(s[i] == ' ')
       s[i] = '+';

   snprintf(buf, sizeof(buf), "GET http://%s/ig/api?weather=%s HTTP/1.1\r\nHost: %s\r\n\r\n",
	 inst->host, s, inst->host);

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
_server_error(void *data, int type, void *event)
{
   Instance *inst;
   Ecore_Con_Event_Server_Error *ev;

   inst = data;
   ev = event;
   if ((!inst->server) || (inst->server != ev->server)) return EINA_TRUE;

   fprintf(stderr, "Connection error to %s : %s\n",
           ecore_con_server_name_get(inst->server) ,ev->error);

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
   char code[1024];
   int i;

   location[0] = 0;

   if (!inst->buffer) return 0;

   //printf("%s\n", inst->buffer);

   needle = strstr(inst->buffer, "<city data=\"");
   if (!needle) goto error;
   needle+=12;
   sscanf(needle, "%[^\"]\"", e_data->city);

   needle = strstr(needle, "<current_date_time data=\"");
   if (!needle) goto error;
   needle+=25;
   sscanf(needle, "%[^+]+", date);

   needle = strstr(needle, "<condition data=\"");
   if (!needle) goto error;
   needle += 17;
   sscanf(needle, "%[^\"]\"", code);

   e_data->type = _weather_type_get(code);

   needle = strstr(needle, "<temp_f data=\"");
   if (!needle) goto error;
   needle+=14;
   sscanf(needle, "%lf\"", &(e_data->temp));

   needle = strstr(needle, "<day_of_week data=\"");
   if (!needle) goto error;
   needle += 19; 
   sscanf(needle, "%[^\"]\"", day);

   snprintf(e_data->date, 256, "%s %s", day, date);


   needle = strstr(needle, "<low data=\"");
   if (!needle) goto error;
   needle+=11;
   sscanf(needle, "%lf\"", &(e_data->temp_min));

   needle = strstr(needle, "<high data=\"");
   if (!needle) goto error;
   needle+=12;
   sscanf(needle, "%lf\"", &(e_data->temp_max));

   e_data_current = e_data;
   
   for(i=1; i<4; i++)
     {
	e_data = eweather_data_get(inst->weather, i);

	needle = strstr(needle, "<day_of_week data=\"");
	if (!needle) goto error;
	needle+= 19;
	sscanf(needle, "%[^\"]\"", e_data->date);

	needle = strstr(needle, "<low data=\"");
	if (!needle) goto error;
	needle+=11;
	sscanf(needle, "%lf\"", &(e_data->temp_min));

	needle = strstr(needle, "<high data=\"");
	if (!needle) goto error;
	needle+=12;
	sscanf(needle, "%lf\"", &(e_data->temp_max));

	e_data->temp = ( e_data->temp_min + e_data->temp_max ) / 2;

	needle = strstr(needle, "<condition data=\"");
	if (!needle) goto error;
	needle += 17;
	sscanf(needle, "%[^\"]\"", code);

	e_data->type = _weather_type_get(code);

	strcpy(e_data->country, e_data_current->country);
	strcpy(e_data->region, e_data_current->region);
	strcpy(e_data->city, e_data_current->city);
     }

   eweather_plugin_update(inst->weather);
   return 1;
error:
   //printf ("ERROR: Couldn't parse info\n");
   return 0;
}




static EWeather_Type _weather_type_get(const char *id)
{
   int i;
   for (i = 0; i < sizeof (_tab) / sizeof (struct _Id_Type); ++i)
     if ( !strcmp(_tab[i].id, id))
       {
	  return _tab[i].type;
       }

   return EWEATHER_TYPE_UNKNOWN;
}

