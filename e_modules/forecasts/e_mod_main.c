/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include <e.h>
#include "e_mod_main.h"

#define FORECASTS 2
#define KM_TO_MI 1.609344
#define MB_TO_IN 68.946497518

#define DEBUG(f, a) printf("[forecasts] "f"\n", a);


/* Gadcon Function Protos */
static E_Gadcon_Client *_gc_init(E_Gadcon * gc, const char *name,
				  const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client * gcc);
static void _gc_orient(E_Gadcon_Client * gcc);
static char *_gc_label(void);
static Evas_Object *_gc_icon(Evas * evas);

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *forecasts_config = NULL;

/* Define Gadcon Class */
static const E_Gadcon_Client_Class _gadcon_class = {
  GADCON_CLIENT_CLASS_VERSION,
  "forecasts", {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon},
  E_GADCON_CLIENT_STYLE_PLAIN
};

/* Module specifics */
typedef struct _Instance Instance;
typedef struct _Forecasts Forecasts;

struct _Instance
{
   E_Gadcon_Client *gcc;
   Evas_Object *forecasts_obj;
   Forecasts *forecasts;
   Ecore_Timer *check_timer;
   Ecore_Con_Server *server;
   Ecore_Event_Handler *add_handler;
   Ecore_Event_Handler *del_handler;
   Ecore_Event_Handler *data_handler;

   struct {
	int temp, code;
	char update[52];
	char desc[256];
   } condition;

   struct {
	char temp, distance[3], pressure[3], speed[4];
   } units;

   struct {
	struct {
	     int chill, direction, speed;
	} wind;

	struct {
	     int humidity, rising;
	     float pressure, visibility;
	} atmosphere;

	struct {
	     char sunrise[9], sunset[9];
	} astronomy;
   } details;

   struct
     {
	char day[4];
	char date[12];
	int low, high, code;
	char desc[256];
     } forecast[FORECASTS];

   char *buffer, *location;
   const char *area;
   int bufsize, cursize;

   Popup *popup;
};

struct _Forecasts
{
   Instance *inst;
   Evas_Object *forecasts_obj;
   Evas_Object *icon_obj;
};

struct
{
   const char *host;
   int port;
} proxy;

/* Module Function Protos */
static void _forecasts_cb_mouse_down(void *data, Evas * e, Evas_Object * obj,
				    void *event_info);
static void _forecasts_menu_cb_configure(void *data, E_Menu * m,
					E_Menu_Item * mi);
static void _forecasts_menu_cb_post(void *data, E_Menu * m);
static int _forecasts_cb_check(void *data);
static Config_Item *_forecasts_config_item_get(const char *id);
static Forecasts *_forecasts_new(Evas * evas);
static void _forecasts_free(Forecasts * w);
static void _forecasts_get_proxy(void);
static int _forecasts_server_add(void *data, int type, void *event);
static int _forecasts_server_del(void *data, int type, void *event);
static int _forecasts_server_data(void *data, int type, void *event);
static int _forecasts_parse(void *data);
static void _forecasts_converter(Instance *inst);
static void _forecasts_convert_degrees(int *value, int dir);
static void _forecasts_convert_distances(int *value, int dir);
static void _forecasts_convert_distances_float(float *value, int dir);
static void _forecasts_convert_pressures(float *value, int dir);
static void _forecasts_display_set(Instance * inst, int ok);
static void _forecasts_popup_content_create(Instance *inst, Evas *evas);
static void _forecasts_popup_content_fill(Instance *inst);
static void _cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info);
static Evas_Object * _forecasts_popup_icon_create(Popup *popup, int code);

/* Gadcon Functions */
static E_Gadcon_Client *
_gc_init(E_Gadcon * gc, const char *name, const char *id, const char *style)
{
   Evas_Object *o;
   E_Gadcon_Client *gcc;
   Forecasts *w;
   Instance *inst;
   Config_Item *ci;
   char buf[4096];
   int pw, ph;
   Popup *popup;

   inst = E_NEW(Instance, 1);

   ci = _forecasts_config_item_get(id);
   inst->area = evas_stringshare_add(ci->code);

   w = _forecasts_new(gc->evas);
   w->inst = inst;
   inst->forecasts = w;

   o = w->forecasts_obj;
   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = inst;
   inst->gcc = gcc;
   inst->forecasts_obj = o;
   evas_object_event_callback_add(inst->forecasts_obj, EVAS_CALLBACK_MOUSE_IN,
				   _cb_mouse_in, inst);
   evas_object_event_callback_add(inst->forecasts_obj, EVAS_CALLBACK_MOUSE_OUT,
				   _cb_mouse_out, inst);

   popup = E_NEW(Popup, 1);
   inst->popup = popup;
   popup->win = e_popup_new(e_zone_current_get(e_container_current_get(e_manager_current_get())), 0, 0, 0, 0);
   e_popup_layer_set(popup->win, 999);
   o = edje_object_add(popup->win->evas);
   snprintf(buf, sizeof(buf), "%s/forecasts.edj",
	 e_module_dir_get(forecasts_config->module));
   if (!e_theme_edje_object_set(o, "base/theme/modules/forecasts",
	    "modules/forecasts/popup"))
     edje_object_file_set(o, buf, "modules/forecasts/popup");
   evas_object_show(o);
   popup->o_bg = o;
   _forecasts_popup_content_create(inst, popup->win->evas);
   e_widget_min_size_get(popup->o_list, &pw, &ph);
   edje_extern_object_min_size_set(popup->o_list, pw, ph);
   edje_object_part_swallow(o, "e.swallow.content", popup->o_list);
   edje_object_size_min_calc(popup->o_bg, &popup->w, &popup->h);
   evas_object_move(popup->o_bg, 0, 0);
   evas_object_resize(popup->o_bg, popup->w, popup->h);

   if (!ci->show_text)
     edje_object_signal_emit(inst->forecasts_obj, "e,state,description,hide", "e");
   else
     edje_object_signal_emit(inst->forecasts_obj, "e,state,description,show", "e");
   
   if (!inst->add_handler)
     inst->add_handler =
	ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD,
	      _forecasts_server_add, inst);
   if (!inst->del_handler)
     inst->del_handler =
	ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL,
	      _forecasts_server_del, inst);
   if (!inst->data_handler)
     inst->data_handler =
	ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA,
	      _forecasts_server_data, inst);

   evas_object_event_callback_add(w->forecasts_obj, EVAS_CALLBACK_MOUSE_DOWN,
	 _forecasts_cb_mouse_down, inst);
   forecasts_config->instances =
      evas_list_append(forecasts_config->instances, inst);

   _forecasts_cb_check(inst);
   inst->check_timer =
      ecore_timer_add((double) ci->poll_time, _forecasts_cb_check, inst);
   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client * gcc)
{
   Instance *inst;
   Forecasts *w;

   inst = gcc->data;
   w = inst->forecasts;

   if (inst->check_timer)
     ecore_timer_del(inst->check_timer);
   if (inst->add_handler)
     ecore_event_handler_del(inst->add_handler);
   if (inst->data_handler)
     ecore_event_handler_del(inst->data_handler);
   if (inst->del_handler)
     ecore_event_handler_del(inst->del_handler);
   if (inst->server)
     ecore_con_server_del(inst->server);
   if (inst->area)
     evas_stringshare_del(inst->area);

   inst->server = NULL;
   forecasts_config->instances =
      evas_list_remove(forecasts_config->instances, inst);

   evas_object_event_callback_del(w->forecasts_obj, EVAS_CALLBACK_MOUSE_DOWN,
	 _forecasts_cb_mouse_down);

   _forecasts_free(w);
   E_FREE(inst);
}

static void
_gc_orient(E_Gadcon_Client * gcc)
{
   Instance *inst;

   inst = gcc->data;
   e_gadcon_client_aspect_set(gcc, 16, 16);
   e_gadcon_client_min_size_set(gcc, 16, 16);
}

static char *
_gc_label(void)
{
   return D_("Forecasts");
}

static Evas_Object *
_gc_icon(Evas * evas)
{
   Evas_Object *o;
   char buf[4096];

   o = edje_object_add(evas);
   snprintf(buf, sizeof(buf), "%s/module.edj",
	 e_module_dir_get(forecasts_config->module));
   edje_object_file_set(o, buf, "icon");
   return o;
}

static void
_forecasts_cb_mouse_down(void *data, Evas * e, Evas_Object * obj,
			void *event_info)
{
   Instance *inst;
   Evas_Event_Mouse_Down *ev;

   inst = data;
   ev = event_info;
   if ((ev->button == 3) && (!forecasts_config->menu))
     {
	E_Menu *mn;
	E_Menu_Item *mi;
	int x, y, w, h;

	mn = e_menu_new();
	e_menu_post_deactivate_callback_set(mn, _forecasts_menu_cb_post, inst);
	forecasts_config->menu = mn;

	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, D_("Configuration"));
	e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
	e_menu_item_callback_set(mi, _forecasts_menu_cb_configure, inst);

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
_forecasts_menu_cb_post(void *data, E_Menu * m)
{
   if (!forecasts_config->menu)
     return;
   e_object_del(E_OBJECT(forecasts_config->menu));
   forecasts_config->menu = NULL;
}

static void
_forecasts_menu_cb_configure(void *data, E_Menu * m, E_Menu_Item * mi)
{
   Instance *inst;
   Config_Item *ci;

   inst = data;
   ci = _forecasts_config_item_get(inst->gcc->id);
   _config_forecasts_module(ci);
}

static Config_Item *
_forecasts_config_item_get(const char *id)
{
   Evas_List *l;
   Config_Item *ci;

   for (l = forecasts_config->items; l; l = l->next)
     {
	ci = l->data;
	if (!ci->id)
	  continue;
	if (!strcmp(ci->id, id))
	  return ci;
     }

   ci = E_NEW(Config_Item, 1);
   ci->id = evas_stringshare_add(id);
   ci->poll_time = 900.0;
   ci->degrees = DEGREES_C;
   ci->host = evas_stringshare_add("xml.weather.yahoo.com");
   ci->code = evas_stringshare_add("BUXX0005");
   ci->show_text = 1;

   forecasts_config->items = evas_list_append(forecasts_config->items, ci);
   return ci;
}

/* Gadman Module Setup */
EAPI E_Module_Api e_modapi = {
     E_MODULE_API_VERSION,
     "Forecasts"
};

EAPI void *
e_modapi_init(E_Module * m)
{
   bindtextdomain(PACKAGE, LOCALEDIR);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   conf_item_edd = E_CONFIG_DD_NEW("Forecasts_Config_Item", Config_Item);
#undef T
#undef D
#define T Config_Item
#define D conf_item_edd
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, poll_time, DOUBLE);
   E_CONFIG_VAL(D, T, degrees, INT);
   E_CONFIG_VAL(D, T, host, STR);
   E_CONFIG_VAL(D, T, code, STR);
   E_CONFIG_VAL(D, T, show_text, INT);

   conf_edd = E_CONFIG_DD_NEW("Forecasts_Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
   E_CONFIG_LIST(D, T, items, conf_item_edd);

   forecasts_config = e_config_domain_load("module.forecasts", conf_edd);
   if (!forecasts_config)
     {
	Config_Item *ci;

	forecasts_config = E_NEW(Config, 1);

	ci = E_NEW(Config_Item, 1);
	ci->poll_time = 900.0;
	ci->degrees = DEGREES_C;
	ci->host = evas_stringshare_add("xml.weather.yahoo.com");
	ci->code = evas_stringshare_add("BUXX0005");
	ci->id = evas_stringshare_add("0");
	ci->show_text = 1;

	forecasts_config->items = evas_list_append(forecasts_config->items, ci);
     }
   _forecasts_get_proxy();

   forecasts_config->module = m;
   e_gadcon_provider_register(&_gadcon_class);
   return m;
}

EAPI int
e_modapi_shutdown(E_Module * m)
{
   forecasts_config->module = NULL;
   e_gadcon_provider_unregister(&_gadcon_class);

   if (forecasts_config->config_dialog)
     e_object_del(E_OBJECT(forecasts_config->config_dialog));
   if (forecasts_config->menu)
     {
	e_menu_post_deactivate_callback_set(forecasts_config->menu, NULL, NULL);
	e_object_del(E_OBJECT(forecasts_config->menu));
	forecasts_config->menu = NULL;
     }

   while (forecasts_config->items)
     {
	Config_Item *ci;

	ci = forecasts_config->items->data;
	if (ci->id)
	  evas_stringshare_del(ci->id);
	if (ci->host)
	  evas_stringshare_del(ci->host);
	if (ci->code)
	  evas_stringshare_del(ci->code);
	forecasts_config->items =
	   evas_list_remove_list(forecasts_config->items, forecasts_config->items);
	free(ci);
	ci = NULL;
     }

   E_FREE(forecasts_config);
   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);
   return 1;
}

EAPI int
e_modapi_save(E_Module * m)
{
   Evas_List *l;

   for (l = forecasts_config->instances; l; l = l->next)
     {
	Instance *inst;
	Config_Item *ci;

	inst = l->data;
	ci = _forecasts_config_item_get(inst->gcc->id);

	if (ci->id)
	  evas_stringshare_del(ci->id);
	ci->id = evas_stringshare_add(inst->gcc->id);
     }

   e_config_domain_save("module.forecasts", conf_edd, forecasts_config);
   return 1;
}

EAPI int
e_modapi_about(E_Module * m)
{
   e_module_dialog_show(m, D_("Enlightenment Forecasts Module"),
	 D_("A forecasts forecast module for Enlightenment"));
   return 1;
}

static Forecasts *
_forecasts_new(Evas * evas)
{
   Forecasts *w;
   const char *file;
   char buf[4096];

   w = E_NEW(Forecasts, 1);

   w->forecasts_obj = edje_object_add(evas);

   snprintf(buf, sizeof(buf), "%s/forecasts.edj",
	 e_module_dir_get(forecasts_config->module));
   if (!e_theme_edje_object_set(w->forecasts_obj, "base/theme/modules/forecasts",
	    "modules/forecasts/main"))
     edje_object_file_set(w->forecasts_obj, buf, "modules/forecasts/main");
   evas_object_show(w->forecasts_obj);

   w->icon_obj = edje_object_add(evas);
   if (!e_theme_edje_object_set(w->icon_obj, "base/theme/modules/forecasts/icons",
	    "modules/forecasts/icons/3200"))
     edje_object_file_set(w->icon_obj, buf, "modules/forecasts/icons/3200");
   edje_object_part_swallow(w->forecasts_obj, "icon", w->icon_obj);

   return w;
}

static void
_forecasts_free(Forecasts * w)
{
   evas_object_del(w->forecasts_obj);
   evas_object_del(w->icon_obj);
   free(w);
   w = NULL;
}

static void
_forecasts_get_proxy(void)
{
   char env[128];
   char *host = NULL;
   char *p;
   int port = 0;

   snprintf(env, sizeof(env), "%s", getenv("http_proxy"));
   if (!env[0])
     snprintf(env, sizeof(env), "%s", getenv("HTTP_PROXY"));
   if (strncmp(env, "http://", 7)) return;

   host = strchr(env, ':');
   host += 3;
   p = strchr(host, ':');
   if (p)
     {
	*p = 0;
	p++;
	if (sscanf(p, "%d", &port) != 1)
	  port = 0;
     }
   if ((host) && (port))
     {
	proxy.host = evas_stringshare_add(host);
	proxy.port = port;
     }
}

static int
_forecasts_cb_check(void *data)
{
   Instance *inst;
   Config_Item *ci;

   inst = data;
   ci = _forecasts_config_item_get(inst->gcc->id);

   if (inst->server)
     {
	ecore_con_server_del(inst->server);
	inst->server = NULL;
     }
   if (proxy.port != 0)
     inst->server =
	ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM,
	      proxy.host, proxy.port, inst);
   else
     inst->server =
	ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM, ci->host, 80, inst);

   return 1;
}

static int
_forecasts_server_add(void *data, int type, void *event)
{
   Instance *inst;
   Config_Item *ci;
   Ecore_Con_Event_Server_Add *ev;
   char buf[1024];
   char forecast[1024];
   char degrees;

   inst = data;
   if (!inst)
     return 1;

   ci = _forecasts_config_item_get(inst->gcc->id);
   ev = event;
   if ((!inst->server) || (inst->server != ev->server))
     return 1;

   if (ci->degrees == DEGREES_F)
     degrees = 'f';
   else
     degrees = 'c';

   snprintf(forecast, sizeof(forecast), "/forecastrss?p=%s&u=%c", ci->code, degrees);
   snprintf(buf, sizeof(buf), "GET http://%s%s HTTP/1.1\r\nHost: %s\r\n\r\n",
	 ci->host, forecast, ci->host);
   DEBUG("Server: %s", buf);
   ecore_con_server_send(inst->server, buf, strlen(buf));
   return 0;
}

static int
_forecasts_server_del(void *data, int type, void *event)
{
   Instance *inst;
   Ecore_Con_Event_Server_Del *ev;
   int ret;

   inst = data;
   ev = event;
   if ((!inst->server) || (inst->server != ev->server))
     return 1;

   ecore_con_server_del(inst->server);
   inst->server = NULL;

   ret = _forecasts_parse(inst);
   _forecasts_converter(inst);
   _forecasts_display_set(inst, ret);

   inst->bufsize = 0;
   inst->cursize = 0;
   free(inst->buffer);
   inst->buffer = NULL;

   return 0;
}

static int
_forecasts_server_data(void *data, int type, void *event)
{
   Instance *inst;
   Ecore_Con_Event_Server_Data *ev;

   inst = data;
   ev = event;

   if ((!inst->server) || (inst->server != ev->server))
     return 1;
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
_forecasts_parse(void *data)
{
   Instance *inst;
   char *needle, *ext;
   char city[256];
   char region[256];
   char location[512];
   int  visibility;
   int  i;

   inst = data;
   if (!inst)
     return 0;
   if (inst->buffer == NULL)
     return 0;

   /* Location */
   needle = strstr(inst->buffer, "<yweather:location city=");
   if (!needle) goto error;
   needle = strstr(needle, "\"");
   sscanf(needle, "\"%255[^\"]\"", city);

   region[0] = '\0';
   needle = strstr(needle, "region=\"");
   needle = strstr(needle, "\"");
   sscanf(needle, "\"%255[^\"]\"", region);

   if (strlen(region))
     snprintf(location, 512, "%s, %s", city, region);
   else
     snprintf(location, 512, "%s", city);

   E_FREE(inst->location);
   inst->location = strdup(location);

   /* Units */
   needle = strstr(inst->buffer, "<yweather:units temperature=");
   if (!needle)
     goto error;
   needle = strstr(needle, "\"");
   sscanf(needle, "\"%c\"", &inst->units.temp);
   needle = strstr(needle, "distance=\"");
   if (!needle) goto error;
   needle = strstr(needle, "\"");
   sscanf(needle, "\"%2[^\"]\"", inst->units.distance);
   needle = strstr(needle, "pressure=\"");
   if (!needle) goto error;
   needle = strstr(needle, "\"");
   sscanf(needle, "\"%2[^\"]\"", inst->units.pressure);
   needle = strstr(needle, "speed=\"");
   if (!needle) goto error;
   needle = strstr(needle, "\"");
   sscanf(needle, "\"%3[^\"]\"", inst->units.speed);

   /* Current conditions */
   needle = strstr(inst->buffer, "<yweather:condition text=");
   if (!needle) goto error;
   needle = strstr(needle, "\"");
   sscanf(needle, "\"%255[^\"]\"", inst->condition.desc);
   needle = strstr(needle, "code=\"");
   if (!needle) goto error;
   needle = strstr(needle, "\"");
   sscanf(needle, "\"%d\"", &inst->condition.code);
   needle = strstr(needle, "temp=\"");
   if (!needle) goto error;
   needle = strstr(needle, "\"");
   sscanf(needle, "\"%d\"", &inst->condition.temp);
   needle = strstr(needle, "date=\"");
   if (!needle) goto error;
   needle = strstr(needle, "\"");
   sscanf(needle, "\"%51[^\"]\"", inst->condition.update);

   /* Details */
   /* Wind */
   needle = strstr(inst->buffer, "<yweather:wind chill=");
   if (!needle) goto error;
   needle = strstr(needle, "\"");
   sscanf(needle, "\"%d\"", &inst->details.wind.chill);
   needle = strstr(needle, "direction=\"");
   if (!needle) goto error;
   needle = strstr(needle, "\"");
   sscanf(needle, "\"%d\"", &inst->details.wind.direction);
   needle = strstr(needle, "speed=\"");
   if (!needle) goto error;
   needle = strstr(needle, "\"");
   sscanf(needle, "\"%d\"", &inst->details.wind.speed);

   /* Atmosphere */
   needle = strstr(inst->buffer, "<yweather:atmosphere humidity=");
   if (!needle) goto error;
   needle = strstr(needle, "\"");
   sscanf(needle, "\"%d\"", &inst->details.atmosphere.humidity);
   needle = strstr(needle, "visibility=\"");
   if (!needle) goto error;
   needle = strstr(needle, "\"");
   sscanf(needle, "\"%d\"", &visibility);
   inst->details.atmosphere.visibility = (float) visibility / 100;
   needle = strstr(needle, "pressure=\"");
   if (!needle) goto error;
   needle = strstr(needle, "\"");
   sscanf(needle, "\"%f\"", &inst->details.atmosphere.pressure);
   needle = strstr(needle, "rising=\"");
   if (!needle) goto error;
   needle = strstr(needle, "\"");
   sscanf(needle, "\"%d\"", &inst->details.atmosphere.rising);

   /* Astronomy */
   needle = strstr(inst->buffer, "<yweather:astronomy sunrise=");
   if (!needle) goto error;
   needle = strstr(needle, "\"");
   sscanf(needle, "\"%8[^\"]\"", inst->details.astronomy.sunrise);
   needle = strstr(needle, "sunset=\"");
   if (!needle) goto error;
   needle = strstr(needle, "\"");
   sscanf(needle, "\"%8[^\"]\"", inst->details.astronomy.sunset);

   /* Forecasts */
   for (i = 0; i < FORECASTS; i++) 
     {
	needle = strstr(needle, "<yweather:forecast day=");
	if (!needle) goto error;
	needle = strstr(needle, "\"");
	sscanf(needle, "\"%4[^\"]\"", inst->forecast[i].day);
	needle = strstr(needle, "date=\"");
	if (!needle) goto error;
	needle = strstr(needle, "\"");
	sscanf(needle, "\"%12[^\"]\"", inst->forecast[i].date);
	needle = strstr(needle, "low=\"");
	if (!needle) goto error;
	needle = strstr(needle, "\"");
	sscanf(needle, "\"%d\"", &inst->forecast[i].low);
	needle = strstr(needle, "high=\"");
	if (!needle) goto error;
	needle = strstr(needle, "\"");
	sscanf(needle, "\"%d\"", &inst->forecast[i].high);
	needle = strstr(needle, "text=\"");
	if (!needle) goto error;
	needle = strstr(needle, "\"");
	sscanf(needle, "\"%255[^\"]\"", inst->forecast[i].desc);
	needle = strstr(needle, "code=\"");
	if (!needle) goto error;
	needle = strstr(needle, "\"");
	sscanf(needle, "\"%d\"", &inst->forecast[i].code);
     }
   return 1;

error:
   printf("ERROR: Couldn't parse info from xml.weather.yahoo.com\n");
   return 0;
}

void
_forecasts_converter(Instance *inst)
{
   Config_Item *ci;
   int i, dir = -1;

   ci = _forecasts_config_item_get(inst->gcc->id);
   if ((inst->units.temp == 'F') && (ci->degrees == DEGREES_C))
     {
	dir = DEGREES_C;
	inst->units.temp = 'C';
	snprintf(inst->units.distance, 3, "km");
	snprintf(inst->units.pressure, 3, "mb");
	snprintf(inst->units.speed, 4, "kph");
     }
   else if ((inst->units.temp == 'C') && (ci->degrees == DEGREES_F))
     {
	dir = DEGREES_F;
	inst->units.temp = 'F';
	snprintf(inst->units.distance, 3, "mi");
	snprintf(inst->units.pressure, 3, "in");
	snprintf(inst->units.speed, 4, "mph");
     }
   if (dir == -1) return;

   _forecasts_convert_degrees(&inst->condition.temp, dir);

   _forecasts_convert_degrees(&inst->details.wind.chill, dir);
   _forecasts_convert_distances(&inst->details.wind.speed, dir);
   _forecasts_convert_distances_float(&inst->details.atmosphere.visibility, dir);
   _forecasts_convert_pressures(&inst->details.atmosphere.pressure, dir);
   for (i = 0; i < FORECASTS; i++)
     {
	_forecasts_convert_degrees(&inst->forecast[i].low, dir);
	_forecasts_convert_degrees(&inst->forecast[i].high, dir);
     }
}

static void
_forecasts_convert_degrees(int *value, int dir)
{
   if ((dir == DEGREES_C))
     *value = (*value - 32) * 5.0 / 9.0;
   else
     *value = (*value * 9.0 / 5.0) + 32;
}

static void
_forecasts_convert_distances(int *value, int dir)
{
   if ((dir == DEGREES_C))
     *value = (*value) * KM_TO_MI;
   else
     *value = (*value) / KM_TO_MI;
}

static void
_forecasts_convert_distances_float(float *value, int dir)
{
   if ((dir == DEGREES_C))
     *value = (*value) * KM_TO_MI;
   else
     *value = (*value) / KM_TO_MI;
}

static void
_forecasts_convert_pressures(float *value, int dir)
{
   if ((dir == DEGREES_C))
     *value = (*value) * MB_TO_IN;
   else
     *value = (*value) / MB_TO_IN;
}

static void
_forecasts_display_set(Instance * inst, int ok)
{
   char buf[4096];
   char m[4096];

   if (!inst)
     return;

   snprintf(m, sizeof(m), "%s/forecasts.edj",
	 e_module_dir_get(forecasts_config->module));
   snprintf(buf, sizeof(buf), "modules/forecasts/icons/%d", inst->condition.code);
   if (!e_theme_edje_object_set(inst->forecasts->icon_obj,
	    "base/theme/modules/forecasts/icons", buf))
     edje_object_file_set(inst->forecasts->icon_obj, m, buf);
   edje_object_part_swallow(inst->forecasts->forecasts_obj, "icon", inst->forecasts->icon_obj);
   snprintf(buf, sizeof(buf), "%d°%c", inst->condition.temp, inst->units.temp);
   edje_object_part_text_set(inst->forecasts->forecasts_obj, "e.text.temp", buf);
   edje_object_part_text_set(inst->forecasts->forecasts_obj, "e.text.description",
	 inst->condition.desc);
   _forecasts_popup_content_fill(inst);
}

void
_forecasts_config_updated(const char *id)
{
   Evas_List *l;
   Config_Item *ci;
   char buf[4096];

   if (!forecasts_config)
     return;
   ci = _forecasts_config_item_get(id);
   for (l = forecasts_config->instances; l; l = l->next)
     {
	Instance *inst;

	inst = l->data;
	if (!inst->gcc->id)
	  continue;
	if (!strcmp(inst->gcc->id, ci->id))
	  {
	     int area_changed = 0;

	     if (inst->area && strcmp(inst->area, ci->code))
	       area_changed = 1;

	     if (inst->area) evas_stringshare_del(inst->area);
	     inst->area = evas_stringshare_add(ci->code);
	     _forecasts_converter(inst);
	     _forecasts_popup_content_fill(inst);

	     snprintf(buf, sizeof(buf), "%d°%c", inst->condition.temp, inst->units.temp);
	     edje_object_part_text_set(inst->forecasts->forecasts_obj, "e.text.temp", buf);

	     if (!ci->show_text)
	       edje_object_signal_emit(inst->forecasts_obj, "e,state,description,hide", "e");
	     else
	       edje_object_signal_emit(inst->forecasts_obj, "e,state,description,show", "e");
   
	     if (area_changed)
	       _forecasts_cb_check(inst);
	     if (!inst->check_timer)
	       inst->check_timer =
		  ecore_timer_add((double) ci->poll_time, _forecasts_cb_check,
			inst);
	     else
	       ecore_timer_interval_set(inst->check_timer,
		     (double) ci->poll_time);

	     break;
	  }
     }
}

static void
_forecasts_popup_content_create(Instance *inst, Evas *evas)
{
  Evas_Object *o, *ol, *of, *ob;
  Evas_Coord minw, minh;
  int row = 0, i;
  int w, h;

  o = e_widget_list_add(evas, 0, 0);
  of = e_widget_frametable_add(evas, D_("No location"), 0);
  ob = e_widget_label_add(evas, D_("No description"));
  e_widget_frametable_object_append(of, ob, 0, row, 3, 1, 0, 0, 1, 0);
  inst->popup->current_desc = ob;
  e_widget_list_object_append(o, of, 1, 1, 0.5);
  inst->popup->o_ft[0] = of;

  ob = e_widget_label_add(evas, D_("Wind Chill"));
  e_widget_frametable_object_append(of, ob, 0, ++row, 1, 1, 1, 0, 0, 0);
  ob = e_widget_label_add(evas, "0°C");
  e_widget_frametable_object_append(of, ob, 1, row, 1, 1, 1, 0, 0, 0);
  inst->popup->wind_chill = ob;

  ob = e_widget_image_add_from_object(evas,
	_forecasts_popup_icon_create(inst->popup, 3200), 0, 0);
  e_widget_frametable_object_append(of, ob, 2, row, 1, 4, 1, 1, 1, 1);
  inst->popup->icon = ob;

  ob = e_widget_label_add(evas, D_("Wind Speed"));
  e_widget_frametable_object_append(of, ob, 0, ++row, 1, 1, 1, 0, 0, 0);
  ob = e_widget_label_add(evas, "0 kph");
  e_widget_frametable_object_append(of, ob, 1, row, 1, 1, 1, 0, 0, 0);
  inst->popup->wind_speed = ob;

  ob = e_widget_label_add(evas, D_("Humidity"));
  e_widget_frametable_object_append(of, ob, 0, ++row, 1, 1, 1, 0, 0, 0);
  ob = e_widget_label_add(evas, "0 %");
  e_widget_frametable_object_append(of, ob, 1, row, 1, 1, 1, 0, 0, 0);
  inst->popup->humidity = ob;

  ob = e_widget_label_add(evas, D_("Visibility"));
  e_widget_frametable_object_append(of, ob, 0, ++row, 1, 1, 1, 0, 0, 0);
  ob = e_widget_label_add(evas, "0.0 km");
  e_widget_frametable_object_append(of, ob, 1, row, 1, 1, 1, 0, 0, 0);
  inst->popup->visibility = ob;

  ob = e_widget_label_add(evas, D_("Pressure"));
  e_widget_frametable_object_append(of, ob, 0, ++row, 1, 1, 1, 0, 0, 0);
  ob = e_widget_label_add(evas, "0.0 mb");
  e_widget_frametable_object_append(of, ob, 1, row, 1, 1, 1, 0, 0, 0);
  inst->popup->pressure = ob;

  ob = e_widget_label_add(evas, D_("Steady"));
  e_widget_frametable_object_append(of, ob, 2, row, 1, 1, 1, 0, 1, 0);
  inst->popup->rising = ob;

  ob = e_widget_label_add(evas, D_("Sunrise"));
  e_widget_frametable_object_append(of, ob, 0, ++row, 1, 1, 1, 0, 0, 0);
  ob = e_widget_label_add(evas, "0:00 am");
  e_widget_frametable_object_append(of, ob, 1, row, 2, 1, 1, 0, 1, 0);
  inst->popup->sunrise = ob;

  ob = e_widget_label_add(evas, D_("Sunset"));
  e_widget_frametable_object_append(of, ob, 0, ++row, 1, 1, 1, 0, 0, 0);
  ob = e_widget_label_add(evas, "0:00 pm");
  e_widget_frametable_object_append(of, ob, 1, row, 2, 1, 1, 0, 1, 0);
  inst->popup->sunset = ob;

  e_widget_min_size_get(of, &w, &h);
  ol = e_widget_list_add(evas, 1, 1);
  e_widget_list_object_append(o, ol, 1, 1, 0.5);

  for (i = 0; i < FORECASTS; i++)
    {
       int row = 0;

       of = e_widget_frametable_add(evas, D_("No date"), 0);
       e_widget_list_object_append(ol, of, 1, 1, 0.5);
       inst->popup->o_ft[i+1] = of;

       ob = e_widget_label_add(evas, D_("No description"));
       e_widget_frametable_object_append(of, ob, 0, row, 3, 1, 0, 0, 1, 0);
       inst->popup->desc[i] = ob;

       ob = e_widget_label_add(evas, D_("High"));
       e_widget_frametable_object_append(of, ob, 0, ++row, 1, 1, 1, 0, 1, 0);
       ob = e_widget_label_add(evas, "0°C");
       e_widget_frametable_object_append(of, ob, 1, row, 1, 1, 1, 0, 1, 0);
       inst->popup->high[i] = ob;

       ob = e_widget_image_add_from_object(evas,
	     _forecasts_popup_icon_create(inst->popup, 3200), 0, 0);
       e_widget_frametable_object_append(of, ob, 2, row, 1, 2, 0, 0, 0, 0);
       inst->popup->f_icon[i] = ob;

       ob = e_widget_label_add(evas, D_("Low"));
       e_widget_frametable_object_append(of, ob, 0, ++row, 1, 1, 1, 0, 1, 0);
       ob = e_widget_label_add(evas, "0°C");
       e_widget_frametable_object_append(of, ob, 1, row, 1, 1, 1, 0, 1, 0);
       inst->popup->low[i] = ob;
       e_widget_min_size_get(of, &w, &h);
    }

  inst->popup->o_list = o;
}

static void
_forecasts_popup_content_fill(Instance *inst)
{
   int i, pw, ph;
   char buf[4096];
   char m[4096];
   Evas_Object *o;

   snprintf(buf, sizeof(buf), "%s", inst->location);
   e_widget_frametable_label_set(inst->popup->o_ft[0], buf);

   snprintf(buf, sizeof(buf), "%s", inst->condition.desc);
   e_widget_label_text_set(inst->popup->current_desc, buf);

   snprintf(buf, sizeof(buf), "%d°%c", inst->details.wind.chill, inst->units.temp);
   e_widget_label_text_set(inst->popup->wind_chill, buf);

   snprintf(buf, sizeof(buf), "%d %s", inst->details.wind.speed, inst->units.speed);
   e_widget_label_text_set(inst->popup->wind_speed, buf);

   snprintf(buf, sizeof(buf), "%d %%", inst->details.atmosphere.humidity);
   e_widget_label_text_set(inst->popup->humidity, buf);

   snprintf(buf, sizeof(buf), "%.2f %s", inst->details.atmosphere.visibility, inst->units.distance);
   e_widget_label_text_set(inst->popup->visibility, buf);

   snprintf(buf, sizeof(buf), "%.2f %s", inst->details.atmosphere.pressure, inst->units.pressure);
   e_widget_label_text_set(inst->popup->pressure, buf);

   if (inst->details.atmosphere.rising == 1)
     snprintf(buf, sizeof(buf), D_("Rising"));
   else if (inst->details.atmosphere.rising == 2)
     snprintf(buf, sizeof(buf), D_("Falling"));
   else
     snprintf(buf, sizeof(buf), D_("Steady"));
   e_widget_label_text_set(inst->popup->rising, buf);

   snprintf(buf, sizeof(buf), "%s", inst->details.astronomy.sunrise);
   e_widget_label_text_set(inst->popup->sunrise, buf);

   snprintf(buf, sizeof(buf), "%s", inst->details.astronomy.sunset);
   e_widget_label_text_set(inst->popup->sunset, buf);

   
   e_widget_image_object_set(inst->popup->icon,
	 _forecasts_popup_icon_create(inst->popup, inst->condition.code));

   e_widget_frametable_content_align_set(inst->popup->o_ft[0], 0.5, 0.5);

   for (i = 0; i < FORECASTS; i++)
     {
	e_widget_image_object_set(inst->popup->f_icon[i],
	      _forecasts_popup_icon_create(inst->popup, inst->forecast[i].code));

	snprintf(buf, sizeof(buf), "%d°%c", inst->forecast[i].high, inst->units.temp);
	e_widget_label_text_set(inst->popup->high[i], buf);

	snprintf(buf, sizeof(buf), "%d°%c", inst->forecast[i].low, inst->units.temp);
	e_widget_label_text_set(inst->popup->low[i], buf);

	snprintf(buf, sizeof(buf), "%s", inst->forecast[i].date);
	e_widget_frametable_label_set(inst->popup->o_ft[i+1], buf);

	snprintf(buf, sizeof(buf), "%s", inst->forecast[i].desc);
	e_widget_label_text_set(inst->popup->desc[i], buf);

	e_widget_frametable_content_align_set(inst->popup->o_ft[i+1], 0.5, 0.5);
     }

   e_widget_min_size_get(inst->popup->o_list, &pw, &ph);
   edje_extern_object_min_size_set(inst->popup->o_list, pw, ph);
   edje_object_size_min_calc(inst->popup->o_bg, &inst->popup->w, &inst->popup->h);
   evas_object_resize(inst->popup->o_bg, inst->popup->w, inst->popup->h);
}

static Evas_Object *
_forecasts_popup_icon_create(Popup *popup, int code)
{
   char buf[4096];
   char m[4096];
   Evas_Object *o;

   snprintf(m, sizeof(m), "%s/forecasts.edj",
	 e_module_dir_get(forecasts_config->module));
   o = edje_object_add(popup->win->evas);
   snprintf(buf, sizeof(buf), "modules/forecasts/icons/%d", code);
   if (!e_theme_edje_object_set(o, "base/theme/modules/forecasts/icons", buf))
     edje_object_file_set(o, m, buf);

   return o;
}

static void 
_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst;
   int ww, wh, pw, ph;
   Evas_Coord gx, gy, gw, gh, cx, cy, cw, ch, px, py;
   
   inst = data;
   if (!inst->popup) return;
   e_popup_show(inst->popup->win);
   evas_object_show(inst->popup->o_bg);
   e_widget_min_size_get(inst->popup->o_list, &pw, &ph);
   edje_extern_object_min_size_set(inst->popup->o_list, pw, ph);
   edje_object_size_min_calc(inst->popup->o_bg, &inst->popup->w, &inst->popup->h);
   /* FIXME: can't seem to get the correct size */
   inst->popup->w = 400;
   inst->popup->h = 400;
   evas_object_resize(inst->popup->o_bg, inst->popup->w, inst->popup->h);

   /* Popup positioning */
   evas_object_geometry_get(obj, &gx, &gy, &gw, &gh);
   e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &cx, &cy, &cw, &ch);
   switch (inst->gcc->gadcon->orient)
     {
      case E_GADCON_ORIENT_CORNER_RT:
      case E_GADCON_ORIENT_CORNER_RB:
      case E_GADCON_ORIENT_RIGHT:
	 px = gx - inst->popup->w;
	 py = gy;
	 if (py + inst->popup->h >= ch)
	   py = gy + gh - inst->popup->h;
	 break;
      case E_GADCON_ORIENT_LEFT:
      case E_GADCON_ORIENT_CORNER_LT:
      case E_GADCON_ORIENT_CORNER_LB:
	 px = gx + gw;
	 py = gy;
	 if (py + inst->popup->h >= ch)
	   py = gy + gh - inst->popup->h;
	 break;
	 break;
      case E_GADCON_ORIENT_TOP:
      case E_GADCON_ORIENT_CORNER_TL:
      case E_GADCON_ORIENT_CORNER_TR:
	 py = gy + gh;
	 px = gx;
	 if (px + inst->popup->w >= cw)
	   px = gx + gw - inst->popup->w;
	 break;
      case E_GADCON_ORIENT_BOTTOM:
      case E_GADCON_ORIENT_CORNER_BL:
      case E_GADCON_ORIENT_CORNER_BR:
	 py = gy - inst->popup->h;
	 px = gx;
	 if (px + inst->popup->w >= cw)
	   px = gx + gw - inst->popup->w;
	 break;
      default:
	 e_popup_move_resize(inst->popup->win, 50, 50, inst->popup->w, inst->popup->h);
	 return;
     }
   e_popup_move_resize(inst->popup->win, px, py, inst->popup->w, inst->popup->h);
}

static void 
_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst;
   
   inst = data;
   if (!inst->popup) return;
   e_popup_hide(inst->popup->win);
}
