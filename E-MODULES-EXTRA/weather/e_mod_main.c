#include <e.h>
#include "e_mod_main.h"

/* Gadcon Function Protos */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient);
static char *_gc_label(E_Gadcon_Client_Class *client_class);
static Evas_Object *_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas);
static const char *_gc_id_new(E_Gadcon_Client_Class *client_class);

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *weather_config = NULL;

/* Define Gadcon Class */
static const E_Gadcon_Client_Class _gadcon_class = {
  GADCON_CLIENT_CLASS_VERSION,
  "weather", {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL},
  E_GADCON_CLIENT_STYLE_PLAIN
};

/* Module specifics */
typedef struct _Instance Instance;
typedef struct _Weather Weather;

struct _Instance
{
   E_Gadcon_Client *gcc;
   Evas_Object *weather_obj;
   Weather *weather;
   Ecore_Timer *check_timer;
   Ecore_Con_Server *server;
   Ecore_Event_Handler *add_handler;
   Ecore_Event_Handler *del_handler;
   Ecore_Event_Handler *data_handler;
   Config_Item *ci;

   char *buffer, *location;
   int bufsize, cursize;
   int temp, loc_set;
   char degrees;
   char conditions[256];
   char icon[256];
};

struct _Weather
{
   Instance *inst;
   Evas_Object *weather_obj;
   Evas_Object *icon_obj;
};

struct 
{
   const char *host;
   int port;
} proxy = {
   NULL, 0
};

/* Module Function Protos */
static void _weather_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _weather_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi);
static void _weather_menu_cb_post(void *data, E_Menu *m);
static int _weather_cb_check(void *data);
static Config_Item *_weather_config_item_get(const char *id);
static Weather *_weather_new(Evas *evas);
static void _weather_free(Weather *w);
static void _weather_get_proxy(void);
static int _weather_server_add(void *data, int type, void *event);
static int _weather_server_del(void *data, int type, void *event);
static int _weather_server_data(void *data, int type, void *event);
static int _weather_parse(void *data);
static void _weather_display_set(Instance *inst, int ok);

/* Gadcon Functions */
static E_Gadcon_Client *
_gc_init (E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Evas_Object *o;
   E_Gadcon_Client *gcc;
   Weather *w;
   Instance *inst;

   inst = E_NEW(Instance, 1);
   inst->ci = _weather_config_item_get(id);

   w = _weather_new(gc->evas);
   w->inst = inst;
   inst->weather = w;

   o = w->weather_obj;
   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = inst;
   inst->gcc = gcc;
   inst->weather_obj = o;

   if (!inst->add_handler)
     inst->add_handler =
     ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD,
                             _weather_server_add, inst);
   if (!inst->del_handler)
     inst->del_handler =
     ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL,
                             _weather_server_del, inst);
   if (!inst->data_handler)
     inst->data_handler =
     ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA,
                             _weather_server_data, inst);

   evas_object_event_callback_add(w->weather_obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  _weather_cb_mouse_down, inst);
   weather_config->instances =
     eina_list_append(weather_config->instances, inst);

   if (inst->ci->display == 0)
     edje_object_signal_emit(inst->weather->weather_obj, "set_style",
                             "simple");
   else
     edje_object_signal_emit(inst->weather->weather_obj, "set_style",
                             "detailed");

   _weather_cb_check(inst);

   inst->check_timer =
     ecore_timer_add(inst->ci->poll_time, _weather_cb_check, inst);
   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst;
   Weather *w;

   inst = gcc->data;
   w = inst->weather;

   if (inst->check_timer) ecore_timer_del(inst->check_timer);
   if (inst->add_handler) ecore_event_handler_del(inst->add_handler);
   if (inst->data_handler) ecore_event_handler_del(inst->data_handler);
   if (inst->del_handler) ecore_event_handler_del(inst->del_handler);
   if (inst->server) ecore_con_server_del(inst->server);

   inst->server = NULL;
   weather_config->instances =
     eina_list_remove(weather_config->instances, inst);

   evas_object_event_callback_del(w->weather_obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  _weather_cb_mouse_down);

   _weather_free(w);
   E_FREE(inst);
}

static void
_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient)
{
   Instance *inst;

   inst = gcc->data;
   e_gadcon_client_aspect_set (gcc, 16, 16);
   e_gadcon_client_min_size_set (gcc, 16, 16);
}

static char *
_gc_label(E_Gadcon_Client_Class *client_class)
{
   return D_("Weather");
}

static Evas_Object *
_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas)
{
   Evas_Object *o;
   char buf[4096];

   o = edje_object_add(evas);
   snprintf(buf, sizeof (buf), "%s/e-module-weather.edj",
            e_module_dir_get (weather_config->module));
   edje_object_file_set(o, buf, "icon");
   return o;
}

static const char *
_gc_id_new(E_Gadcon_Client_Class *client_class)
{
   Config_Item *ci;

   ci = _weather_config_item_get (NULL);
   return ci->id;
}

static void
_weather_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst;
   Evas_Event_Mouse_Down *ev;

   inst = data;
   ev = event_info;
   if ((ev->button == 3) && (!weather_config->menu))
     {
        E_Menu *mn;
        E_Menu_Item *mi;
        int x, y, w, h;

        mn = e_menu_new();
        e_menu_post_deactivate_callback_set(mn, _weather_menu_cb_post, inst);
        weather_config->menu = mn;

        mi = e_menu_item_new(mn);
        e_menu_item_label_set(mi, D_("Configuration"));
        e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
        e_menu_item_callback_set(mi, _weather_menu_cb_configure, inst);

        mi = e_menu_item_new(mn);
        e_menu_item_separator_set(mi, 1);

        e_gadcon_client_util_menu_items_append(inst->gcc, mn, 0);
        e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &x, &y, &w, &h);
        e_menu_activate_mouse(mn,
                              e_util_zone_current_get(e_manager_current_get
                                                      ()), x + ev->output.x,
                              y + ev->output.y, 1, 1,
                              E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
        evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button,
                                 EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }
}

static void
_weather_menu_cb_post(void *data, E_Menu *m)
{
   if (!weather_config->menu) return;
   e_object_del(E_OBJECT(weather_config->menu));
   weather_config->menu = NULL;
}

static void
_weather_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Instance *inst;

   inst = data;
   _config_weather_module(inst->ci);
}

static Config_Item *
_weather_config_item_get(const char *id)
{
   Eina_List *l;
   Config_Item *ci;
   char buf[128];

   if (!id)
     {
	int  num = 0;

	/* Create id */
	if (weather_config->items)
	  {
	     const char *p;

	     ci = eina_list_last(weather_config->items)->data;
	     p = strrchr(ci->id, '.');
	     if (p) num = atoi(p + 1) + 1;
	  }
	snprintf(buf, sizeof (buf), "%s.%d", _gadcon_class.name, num);
	id = buf;
     }
   else
     {
	for (l = weather_config->items; l; l = l->next)
	  {
	     ci = l->data;
	     if (!ci->id) continue;
	     if (!strcmp(ci->id, id)) return ci;
	  }
     }

   ci = E_NEW(Config_Item, 1);
   ci->id = eina_stringshare_add(id);
   ci->poll_time = 900.0;
   ci->display = 0;
   ci->degrees = DEGREES_F;
   ci->host = eina_stringshare_add("www.rssweather.com");
   ci->code = eina_stringshare_add("KJFK");

   weather_config->items = eina_list_append(weather_config->items, ci);
   return ci;
}

/* Gadman Module Setup */
EAPI E_Module_Api e_modapi = { E_MODULE_API_VERSION, "Weather" };

EAPI void *
e_modapi_init(E_Module *m)
{
   char buf[4096];

   snprintf(buf, sizeof (buf), "%s/locale", e_module_dir_get(m));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   conf_item_edd = E_CONFIG_DD_NEW("Weather_Config_Item", Config_Item);
#undef T
#undef D
#define T Config_Item
#define D conf_item_edd
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, poll_time, DOUBLE);
   E_CONFIG_VAL(D, T, display, INT);
   E_CONFIG_VAL(D, T, degrees, INT);
   E_CONFIG_VAL(D, T, host, STR);
   E_CONFIG_VAL(D, T, code, STR);

   conf_edd = E_CONFIG_DD_NEW("Weather_Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
   E_CONFIG_LIST(D, T, items, conf_item_edd);

   weather_config = e_config_domain_load("module.weather", conf_edd);
   if (!weather_config)
     {
        Config_Item *ci;

        weather_config = E_NEW(Config, 1);
        ci = E_NEW(Config_Item, 1);
        ci->poll_time = 900.0;
        ci->display = 0;
        ci->degrees = DEGREES_F;
        ci->host = eina_stringshare_add("www.rssweather.com");
        ci->code = eina_stringshare_add("KJFK");
        ci->id = eina_stringshare_add("0");
        weather_config->items = eina_list_append(weather_config->items, ci);
     }
   _weather_get_proxy();

   weather_config->module = m;
   e_gadcon_provider_register(&_gadcon_class);
   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   weather_config->module = NULL;
   e_gadcon_provider_unregister(&_gadcon_class);

   if (weather_config->config_dialog)
     e_object_del(E_OBJECT(weather_config->config_dialog));
   if (weather_config->menu)
     {
        e_menu_post_deactivate_callback_set(weather_config->menu, NULL, NULL);
        e_object_del(E_OBJECT(weather_config->menu));
        weather_config->menu = NULL;
     }

   while (weather_config->items)
     {
        Config_Item *ci;

        ci = weather_config->items->data;
        if (ci->id) eina_stringshare_del(ci->id);
        if (ci->host) eina_stringshare_del(ci->host);
        if (ci->code) eina_stringshare_del(ci->code);
        weather_config->items =
          eina_list_remove_list(weather_config->items, weather_config->items);
        E_FREE(ci);
     }

   E_FREE(weather_config);
   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   e_config_domain_save("module.weather", conf_edd, weather_config);
   return 1;
}

static Weather *
_weather_new(Evas *evas)
{
   Weather *w;
   char buf[4096];

   w = E_NEW(Weather, 1);
   w->weather_obj = edje_object_add(evas);

   snprintf(buf, sizeof (buf), "%s/weather.edj",
            e_module_dir_get(weather_config->module));
   if (!e_theme_edje_object_set
       (w->weather_obj, "base/theme/modules/weather", "modules/weather/main"))
     edje_object_file_set(w->weather_obj, buf, "modules/weather/main");
   evas_object_show(w->weather_obj);
   edje_object_part_text_set(w->weather_obj, "location", "");

   w->icon_obj = e_icon_add(evas);
   snprintf(buf, sizeof (buf), "%s/images/unknown.png",
            e_module_dir_get(weather_config->module));
   e_icon_file_set(w->icon_obj, buf);
   edje_object_part_swallow(w->weather_obj, "icon", w->icon_obj);

   return w;
}

static void
_weather_free(Weather *w)
{
   evas_object_del(w->weather_obj);
   evas_object_del(w->icon_obj);
   E_FREE(w);
}

static void
_weather_get_proxy(void)
{
   char *env, *p, *host = NULL;
   int port = 0;

   env = getenv("http_proxy");
   if ((!env) || (!*env)) env = getenv("HTTP_PROXY");
   if ((!env) || (!*env)) return;
   if (strncmp(env, "http://", 7)) return;

   env = strdup(env);
   host = strchr(env, ':');
   host += 3;
   p = strchr(host, ':');
   if (p)
     {
        *p = 0;
        p++;
        if (sscanf(p, "%d", &port) != 1) port = 0;
    }
  // FIXME: A proxy doesn't necessarily need a port
   if ((host) && (port))
     {
        if (proxy.host) eina_stringshare_del(proxy.host);
        proxy.host = eina_stringshare_add(host);
        proxy.port = port;
     }
   free(env);
}

static int
_weather_cb_check(void *data)
{
   Instance *inst;

   if (!(inst = data)) return 0;
   if (inst->server) ecore_con_server_del(inst->server);
   inst->server = NULL;

   if (proxy.port != 0)
     inst->server =
     ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM, proxy.host,
                              proxy.port, inst);
   else
     inst->server =
     ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM, inst->ci->host, 80, inst);

   if (!inst->server) return 0;

   return 1;
}

static int
_weather_server_add(void *data, int type, void *event)
{
   Instance *inst;
   Ecore_Con_Event_Server_Add *ev;
   char buf[1024], icao[1024];

   if (!(inst = data)) return 1;

   ev = event;
   if ((!inst->server) || (inst->server != ev->server)) return 1;

   snprintf(icao, sizeof(icao), "/icao/%s/rss.php", inst->ci->code);
   snprintf(buf, sizeof(buf), "GET http://%s%s HTTP/1.1\r\nHost: %s\r\n\r\n",
            inst->ci->host, icao, inst->ci->host);
   ecore_con_server_send(inst->server, buf, strlen (buf));
   return 0;
}

static int
_weather_server_del(void *data, int type, void *event)
{
   Instance *inst;
   Ecore_Con_Event_Server_Del *ev;
   int ret;

   inst = data;
   ev = event;
   if ((!inst->server) || (inst->server != ev->server)) return 1;

   ecore_con_server_del(inst->server);
   inst->server = NULL;

   ret = _weather_parse(inst);
   _weather_convert_degrees(inst);
   _weather_display_set(inst, ret);

   inst->bufsize = 0;
   inst->cursize = 0;
   E_FREE(inst->buffer);
   return 0;
}

static int
_weather_server_data(void *data, int type, void *event)
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
_weather_parse(void *data)
{
   Instance *inst;
   char *needle, *ext;
   char location[256];

   location[0] = 0;

   if (!(inst = data)) return 0;
   if (inst->buffer == NULL) return 0;

   needle = strstr(inst->buffer, "<title");
   if (!needle) goto error;

   needle = strstr(needle, ">");
   sscanf(needle, ">%[^<]<", location);
   E_FREE(inst->location);
   if (location[0])
     {
        char *tmp;

        tmp = strdup(location);
        if (strstr(tmp, ",")) inst->location = strdup(strtok(tmp, ","));
        free(tmp);
     }

   needle = strstr(inst->buffer, "<content:encoded>");
   if (!needle) goto error;

   needle = strstr(needle, "<img");
   if (!needle) goto error;
   needle = strstr(needle, "id=");
   if (!needle) goto error;

   sscanf(needle, "id=\"%[^\"]\"", inst->icon);
   ext = strstr(inst->icon, ".");
   if (!strcmp(ext, ".gif")) strcpy(ext, ".png");

   needle = strstr(needle, "class=\"sky\"");
   if (!needle) goto error;
   needle = strstr(needle, ">");
   if (!needle) goto error;

   sscanf(needle, ">%[^<]<", inst->conditions);

   needle = strstr(needle, "class=\"temp\"");
   if (!needle) goto error;
   needle = strstr(needle, ">");
   if (!needle) goto error;
   sscanf(needle, ">%d", &inst->temp);
   needle = strstr(needle, "<");
   if (!needle) goto error;
   needle--;
   inst->degrees = needle[0];

  return 1;
error:
  printf ("ERROR: Couldn't parse info from rssweather.com\n");
  return 0;
}

void
_weather_convert_degrees(void *data)
{
   Instance *inst;

   inst = data;
   if ((inst->degrees == 'F') && (inst->ci->degrees == DEGREES_C))
     {
        inst->temp = (inst->temp - 32) * 5.0 / 9.0;
        inst->degrees = 'C';
     }
   if ((inst->degrees == 'C') && (inst->ci->degrees == DEGREES_F))
     {
        inst->temp = (inst->temp * 9.0 / 5.0) + 32;
        inst->degrees = 'F';
     }
}

static void
_weather_display_set(Instance *inst, int ok)
{
   char buf[4096], m[4096];

   if (!inst) return;
   snprintf(m, sizeof (m), "%s", e_module_dir_get(weather_config->module));
//   if (!ok) return;

   snprintf(buf, sizeof (buf), "%s/images/%s", m, inst->icon);
   e_icon_file_set(inst->weather->icon_obj, buf);
   edje_object_part_swallow(inst->weather->weather_obj, "icon",
                            inst->weather->icon_obj);
   edje_object_part_text_set(inst->weather->weather_obj, "location",
                             inst->location);
   snprintf(buf, sizeof (buf), "%d°%c", inst->temp, inst->degrees);
   edje_object_part_text_set(inst->weather->weather_obj, "temp", buf);
   edje_object_part_text_set(inst->weather->weather_obj, "conditions",
			     inst->conditions);
}

void
_weather_config_updated(Config_Item *ci)
{
   Eina_List *l;
   char buf[4096];

   if (!weather_config) return;
   for (l = weather_config->instances; l; l = l->next)
     {
        Instance *inst;

        inst = l->data;
        if (inst->ci != ci) continue;
        if (ci->display == 0)
          edje_object_signal_emit(inst->weather->weather_obj, "set_style",
                                  "simple");
        else if (ci->display == 1)
          edje_object_signal_emit(inst->weather->weather_obj, "set_style",
                                  "detailed");

        _weather_convert_degrees(inst);

        snprintf(buf, sizeof (buf), "%d°%c", inst->temp, inst->degrees);
        edje_object_part_text_set(inst->weather->weather_obj, "temp", buf);

        _weather_cb_check(inst);
        if (!inst->check_timer)
          inst->check_timer = ecore_timer_add(ci->poll_time, 
                                              _weather_cb_check, inst);
        else
          ecore_timer_interval_set(inst->check_timer, ci->poll_time);
     }
}
