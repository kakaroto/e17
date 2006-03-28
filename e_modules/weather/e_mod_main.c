#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"

static Weather      *_weather_new                    (void);
static void          _weather_free                   (Weather *w);
static void          _weather_config_menu_new        (Weather *w);
static Weather_Face *_weather_face_init              (Weather *w, E_Container *con);
static void          _weather_face_menu_new          (Weather_Face *wf);
static void          _weather_face_cb_menu_configure (void *data, E_Menu *mn, E_Menu_Item *mi);
static void          _weather_face_cb_menu_edit      (void *data, E_Menu *mn, E_Menu_Item *mi);
static void          _weather_face_disable           (Weather_Face *wf);
static void          _weather_face_free              (Weather_Face *wf);
static void          _weather_face_cb_mouse_down     (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void          _weather_face_cb_gmc_change     (void *data, E_Gadman_Client *gmc, E_Gadman_Change change);
static int           _weather_face_cb_check          (void *data);
static int           _weather_server_add             (void *data, int type, void *event);
static int           _weather_server_del             (void *data, int type, void *event);
static int           _weather_server_data            (void *data, int type, void *event);
static int           _weather_parse                  (Weather_Face * wf);
static void          _weather_face_menu_cb_face_new  (void *data, E_Menu *m, E_Menu_Item *mi);
static void          _weather_face_menu_cb_face_del  (void *data, E_Menu *m, E_Menu_Item *mi);
static void          _weather_display_set            (Weather_Face * wf, int ok);

static int weather_count;
static E_Config_DD *conf_edd;
static E_Config_DD *conf_face_edd;

EAPI E_Module_Api e_modapi = 
{
   E_MODULE_API_VERSION,
     "Weather"
};

EAPI void *
e_modapi_init(E_Module *m) 
{
   Weather *w;
   
   w = _weather_new();
   m->config_menu = w->config_menu;
   return w;
}

EAPI int
e_modapi_shutdown(E_Module *m) 
{
   Weather *w;

   /* Set up module's message catalogue */
   bindtextdomain(PACKAGE, LOCALEDIR);
   bind_textdomain_codeset(PACKAGE, "UTF-8");
   
   w = m->data;
   if (!w)
     return 0;
   
   if (m->config_menu) 
     {
	e_menu_deactivate(m->config_menu);
	e_object_del(E_OBJECT(m->config_menu));
	m->config_menu = NULL;
     }
   if (w->cfd) 
     {
	e_object_del(E_OBJECT(w->cfd));
	w->cfd = NULL;
     }
   _weather_free(w);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m) 
{
   Weather *w;
   
   w = m->data;
   if (!w)
     return 0;
   e_config_domain_save("module.weather", conf_edd, w->conf);
}

EAPI int
e_modapi_info(E_Module *m) 
{
   m->icon_file = strdup(PACKAGE_DATA_DIR"/module_icon.png");
   return 1;
}

EAPI int
e_modapi_about(E_Module *m) 
{
   e_module_dialog_show(D_("Enlightenment Weather Module"),
			D_("A weather forecast module for Enlightenment"));
   return 1;
}

EAPI int
e_modapi_config(E_Module *m) 
{
   Weather *w;
   Evas_List *l;
   E_Container *con;
   
   w = m->data;
   if (!w)
     return 0;
   if (!w->faces)
     return 0;
   
   for (l = w->faces; l; l = l->next) 
     {
	Weather_Face *wf;
	
	wf = l->data;
	if (!wf)
	  continue;
	
	con = e_container_current_get(e_manager_current_get());
	if (wf->con == con) 
	  {
	     _configure_weather_module(wf);
	     break;
	  }
     }
   return 1;
}

static Weather *
_weather_new(void) 
{
   Weather *w;
   Evas_List *fl;
   E_Container *con;
   char *env;
   
   weather_count = 0;
   
   w = E_NEW(Weather, 1);
   if (!w)
     return NULL;
   
   conf_face_edd = E_CONFIG_DD_NEW("Weather_Config_Face", Config_Face);
   #undef T
   #undef D
   #define T Config_Face
   #define D conf_face_edd
   E_CONFIG_VAL(D, T, enabled, UCHAR);
   E_CONFIG_VAL(D, T, poll_time, DOUBLE);
   E_CONFIG_VAL(D, T, display, INT);
   E_CONFIG_VAL(D, T, degrees, INT);
   E_CONFIG_VAL(D, T, con, INT);   
   E_CONFIG_VAL(D, T, host, STR);
   E_CONFIG_VAL(D, T, code, STR);
   
   conf_edd = E_CONFIG_DD_NEW("Weather_Config", Config);
   #undef T
   #undef D
   #define T Config
   #define D conf_edd
   E_CONFIG_LIST(D, T, faces, conf_face_edd);
   
   w->conf = e_config_domain_load("module.weather", conf_edd);
   if (!w->conf) 
     w->conf = E_NEW(Config, 1);
   
   _weather_config_menu_new(w);
   
   fl = w->conf->faces;
   if (!fl) 
     {
	Weather_Face *wf;
	
	con = e_container_current_get(e_manager_current_get());
	wf = _weather_face_init(w, con);
	if (wf) 
	  {
	     wf->conf = E_NEW(Config_Face, 1);
	     wf->conf->enabled = 1;
	     wf->conf->poll_time = 1800.0;
	     wf->conf->display = DETAILED_DISPLAY;
	     wf->conf->degrees = DEGREES_C;
	     wf->conf->con = con->num;
	     wf->conf->host = evas_stringshare_add("www.rssweather.com");
	     wf->conf->code = evas_stringshare_add("ENKR");
	     E_CONFIG_LIMIT(wf->conf->poll_time, 900.0, 3600.0);
	     
	     w->conf->faces = evas_list_append(w->conf->faces, wf->conf);
	     e_config_save_queue();

	     wf->degrees = 'C';
	     edje_object_signal_emit(wf->weather_obj, "set_style", "detailed");
	     wf->loc_set = 0;
	     wf->check_timer = ecore_timer_add((double)wf->conf->poll_time, _weather_face_cb_check, wf->weather);

	     if (!wf->conf->enabled)
	       _weather_face_disable(wf);
	  }
     }
   else 
     {
	for (; fl; fl = fl->next) 
	  {
	     Weather_Face *wf;
	     Config_Face *conf;
	     
	     conf = fl->data;
	     con = e_util_container_number_get(conf->con);
	     if (!con)
	       continue;
	     wf = _weather_face_init(w, con);
	     if (!wf)
	       continue;
	     wf->conf = conf;
	     E_CONFIG_LIMIT(wf->conf->poll_time, 900.0, 3600.0);

	     switch (wf->conf->degrees) 
	       {
		case 0:
		  wf->degrees = 'F';
		  break;
		case 1:
		  wf->degrees = 'C';
		  break;
	       }
	     if (wf->conf->display == DETAILED_DISPLAY)
	       edje_object_signal_emit(wf->weather_obj, "set_style", "detailed");
	     else
	       edje_object_signal_emit(wf->weather_obj, "set_style", "simple");
	     
	     wf->loc_set = 0;
	     wf->check_timer = ecore_timer_add((double)wf->conf->poll_time, _weather_face_cb_check, wf->weather);

	     if (!wf->conf->enabled)
	       _weather_face_disable(wf);
	  }
     }

   env = getenv("http_proxy");
   if (!env)
     env = getenv("HTTP_PROXY");
   
   if ((env) && (!strncmp(env, "http://", 7))) 
     {
	char *host = NULL;
	char *p;
	int port = 0;
	
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
	     w->proxy.host = strdup(host);
	     w->proxy.port = port;
	  }	
     }
   w->add_handler = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD, _weather_server_add, w);
   w->del_handler = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL, _weather_server_del, w);
   w->data_handler = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA, _weather_server_data, w);

   _weather_face_cb_check(w);
   return w;
}

static void 
_weather_free(Weather *w) 
{
   Evas_List *l;
   
   E_CONFIG_DD_FREE(conf_edd);
   E_CONFIG_DD_FREE(conf_face_edd);

   ecore_event_handler_del(w->add_handler);
   ecore_event_handler_del(w->del_handler);
   ecore_event_handler_del(w->data_handler);
   
   while (w->faces)
     _weather_face_free(w->faces->data);

   evas_list_free(w->conf->faces);
   evas_list_free(w->faces);
   e_object_del(E_OBJECT(w->config_menu));
   E_FREE(w->conf);
   E_FREE(w);
}

static void 
_weather_config_menu_new(Weather *w) 
{
   E_Menu *mn;
   
   mn = e_menu_new();
   w->config_menu = mn;
}

static Weather_Face *
_weather_face_init(Weather *w, E_Container *con) 
{
   Weather_Face *wf;
   Evas_Object *o;
   
   wf = E_NEW(Weather_Face, 1);
   if (!wf)
     return NULL;
   
   wf->con = con;
   e_object_ref(E_OBJECT(con));
   wf->evas = con->bg_evas;
   wf->weather = w;
   w->faces = evas_list_append(w->faces, wf);

   evas_event_freeze(wf->evas);
   
   o = edje_object_add(wf->evas);
   wf->weather_obj = o;
   if (!e_theme_edje_object_set(o, "base/theme/modules/weather", "modules/weather/main"))
     edje_object_file_set(o, PACKAGE_DATA_DIR"/weather.edj", "modules/weather/main");
   evas_object_show(o);
   edje_object_part_text_set(o, "location", "");

   wf->icon_obj = e_icon_add(wf->evas);
   e_icon_file_set(wf->icon_obj, PACKAGE_DATA_DIR"/images/unknown.png");
   edje_object_part_swallow(wf->weather_obj, "icon", wf->icon_obj);
   
   o = evas_object_rectangle_add(wf->evas);
   wf->event_obj = o;
   evas_object_layer_set(o, 2);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, _weather_face_cb_mouse_down, wf);
   evas_object_show(o);

   wf->gmc = e_gadman_client_new(con->gadman);
   e_gadman_client_domain_set(wf->gmc, "module.weather", weather_count++);
   e_gadman_client_policy_set(wf->gmc, E_GADMAN_POLICY_ANYWHERE | E_GADMAN_POLICY_HMOVE | 
			      E_GADMAN_POLICY_VMOVE | E_GADMAN_POLICY_HSIZE | E_GADMAN_POLICY_VSIZE);
   e_gadman_client_auto_size_set(wf->gmc, 200, 40);
   e_gadman_client_align_set(wf->gmc, 1.0, 1.0);
   e_gadman_client_resize(wf->gmc, 200, 40);
   e_gadman_client_change_func_set(wf->gmc, _weather_face_cb_gmc_change, wf);
   e_gadman_client_load(wf->gmc);
   
   evas_event_thaw(wf->evas);
   
   return wf;
}

static void 
_weather_face_menu_new(Weather_Face *wf) 
{
   E_Menu *mn, *sn;
   E_Menu_Item *mi, *si;

   mn = e_menu_new();
   wf->menu = mn;

   sn = e_menu_new();   

   si = e_menu_item_new(sn);
   e_menu_item_label_set(si, _("Add A Face"));
   e_menu_item_callback_set(si, _weather_face_menu_cb_face_new, wf->weather);
   
   if (evas_list_count(wf->weather->conf->faces) > 1) 
     {
	si = e_menu_item_new(sn);
	e_menu_item_label_set(si, _("Remove This Face"));
	e_menu_item_callback_set(si, _weather_face_menu_cb_face_del, wf);
     }
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Faces"));
   e_menu_item_icon_file_set(mi, PACKAGE_DATA_DIR"/module_icon.png");
   e_menu_item_submenu_set(mi, sn);
   
   mi = e_menu_item_new(mn);
   e_menu_item_separator_set(mi, 1);
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Configuration"));
   e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
   e_menu_item_callback_set(mi, _weather_face_cb_menu_configure, wf);

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Edit Mode"));
   e_util_menu_item_edje_icon_set(mi, "enlightenment/gadgets");
   e_menu_item_callback_set(mi, _weather_face_cb_menu_edit, wf);
}

static void 
_weather_face_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi) 
{
   Weather_Face *wf;
   
   wf = data;
   if (!wf)
     return;
   
   _configure_weather_module(wf);
}

static void 
_weather_face_cb_menu_edit(void *data, E_Menu *mn, E_Menu_Item *mi) 
{
   Weather_Face *wf;
   
   wf = data;
   if (!wf)
     return;
   e_gadman_mode_set(wf->gmc->gadman, E_GADMAN_MODE_EDIT);
}

static void 
_weather_face_disable(Weather_Face *wf) 
{
   wf->conf->enabled = 0;
   e_config_save_queue();
   evas_object_hide(wf->weather_obj);
   evas_object_hide(wf->icon_obj);   
   evas_object_hide(wf->event_obj);
}

static void 
_weather_face_free(Weather_Face *wf) 
{
   if (wf->check_timer)
     ecore_timer_del(wf->check_timer);
   
   e_object_unref(E_OBJECT(wf->con));
   
   if (wf->menu)
     e_object_del(E_OBJECT(wf->menu));
   if (wf->event_obj)
     evas_object_del(wf->event_obj);
   if (wf->icon_obj)
     evas_object_del(wf->icon_obj);
   if (wf->weather_obj)
     evas_object_del(wf->weather_obj);
   
   if (wf->gmc)
     {
	e_gadman_client_save(wf->gmc);
	e_object_del(E_OBJECT(wf->gmc));
     }
   wf->weather->faces = evas_list_remove(wf->weather->faces, wf);
   
   if (wf->conf->host)
     evas_stringshare_del(wf->conf->host);
   if (wf->conf->code)
     evas_stringshare_del(wf->conf->code);
   
   E_FREE(wf->conf);
   E_FREE(wf);
   weather_count--;
}

static void 
_weather_face_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info) 
{
   Weather_Face *wf;
   Evas_Event_Mouse_Down *ev;
   
   wf = data;
   ev = event_info;
   if (ev->button == 3) 
     {
	_weather_face_menu_new(wf);
	e_menu_activate_mouse(wf->menu, e_zone_current_get(wf->con),
			      ev->output.x, ev->output.y, 1, 1,
			      E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
	e_util_container_fake_mouse_up_all_later(wf->con);
     }
}

static void 
_weather_face_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change) 
{
   Weather_Face *wf;
   Evas_Coord x, y, w, h;
   
   wf = data;
   if (!wf)
     return;
   
   switch (change) 
     {
      case E_GADMAN_CHANGE_MOVE_RESIZE:
	e_gadman_client_geometry_get(wf->gmc, &x, &y, &w, &h);
	evas_object_move(wf->weather_obj, x, y);
	evas_object_move(wf->event_obj, x, y);
	evas_object_resize(wf->weather_obj, w, h);
	evas_object_resize(wf->event_obj, w, h);
	break;
      case E_GADMAN_CHANGE_RAISE:
	evas_object_raise(wf->weather_obj);
	evas_object_raise(wf->event_obj);
	break;
      default:
	break;
     }
}

static int 
_weather_face_cb_check(void *data) 
{
   Weather *w;
   Evas_List *l;
   
   w = data;
   for (l = w->faces; l; l = l->next) 
     {
	Weather_Face *wf;
	
	wf = l->data;
	if (wf->server)
	  continue;
	if (wf->weather->proxy.port) 
	  {
	     wf->server = ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM,
						   wf->weather->proxy.host,
						   wf->weather->proxy.port, wf);
	  }
	else 
	  {
	     wf->server = ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM,
						   wf->conf->host, 80, wf);
	  }	
     }
   return 1;
}

static int 
_weather_server_add(void *data, int type, void *event) 
{
   Weather *w;
   Weather_Face *wf;
   Ecore_Con_Event_Server_Add *ev;
   Evas_List *l;
   char buf[1024];
   char icao[1024];
   
   w = data;
   ev = event;
   for (l = w->faces; l; l = l->next) 
     {
	wf = l->data;
	if (wf->server == ev->server)
	  break;
     }
   
   if ((!wf) || (ev->server != wf->server))
     return 1;
   
   snprintf(icao, sizeof(icao), "/icao/%s/rss.php", wf->conf->code);   
   snprintf(buf, sizeof(buf), "GET http://%s%s HTTP/1.1\r\nHost: %s\r\n\r\n",
	    wf->conf->host, icao, wf->conf->host);
   ecore_con_server_send(wf->server, buf, strlen(buf));
   return 0;
}

static int 
_weather_server_del(void *data, int type, void *event) 
{
   Weather *w;
   Weather_Face *wf;
   Ecore_Con_Event_Server_Del *ev;
   Evas_List *l;
   char buf[1024];
   
   w = data;
   ev = event;
   for (l = w->faces; l; l = l->next) 
     {
	wf = l->data;
	if (wf->server == ev->server)
	  break;
     }
   
   if ((!wf) || (ev->server != wf->server))
     return 1;

   ecore_con_server_del(wf->server);
   wf->server = NULL;
   
   /* Parse, convert, display */
   int ret;   
   ret = _weather_parse(wf);
   _weather_convert_degrees(wf);
   _weather_display_set(wf, ret);
   
   wf->bufsize = 0;
   wf->cursize = 0;
   free(wf->buffer);
   wf->buffer = NULL;
   return 0;
}

static int 
_weather_server_data(void *data, int type, void *event) 
{
   Weather *w;
   Weather_Face *wf;
   Ecore_Con_Event_Server_Data *ev;
   Evas_List *l;
   char buf[1024];
   
   w = data;
   ev = event;
   for (l = w->faces; l; l = l->next) 
     {
	wf = l->data;
	if (wf->server == ev->server)
	  break;
     }
   
   if ((!wf) || (ev->server != wf->server))
     return 1;

   while ((wf->cursize + ev->size) >= wf->bufsize) 
     {
	wf->bufsize += 4096;
	wf->buffer = realloc(wf->buffer, wf->bufsize);
     }
   
   memcpy(wf->buffer + wf->cursize, ev->data, ev->size);
   wf->cursize += ev->size;
   wf->buffer[wf->cursize] = 0;
   return 0;
}

static int
_weather_parse(Weather_Face *wf)
{
   char *needle, *ext;
   char location[256];
   
   needle = strstr(wf->buffer, "<title");
   if (!needle)
     goto error;
   
   needle = strstr(needle, ">");
   sscanf(needle, ">%[^<]<", location);
   if (location) 
     {
	char *tmp = strdup(location);
	if (strstr(tmp, ",")) 
	  {
	     tmp = strtok(tmp, ",");
	     wf->location = strdup(tmp);
	     wf->loc_set = 1;
	  }
     }
      
   needle = strstr(wf->buffer, "<content:encoded>");
   if (!needle)
     goto error;

   /* Get the icon */
   needle = strstr(needle, "<img");
   if (!needle)
     goto error;
   needle = strstr(needle, "id=");
   if (!needle)
     goto error;
   sscanf(needle, "id=\"%[^\"]\"", wf->icon);
   ext = strstr(wf->icon, ".");
   if (!strcmp(ext, ".gif"))
     strcpy(ext, ".png");

   /* Get the conditions */
   needle = strstr(needle, "class=\"sky\"");
   if (!needle)
     goto error;
   needle = strstr(needle, ">");
   if (!needle)
     goto error;
   sscanf(needle, ">%[^<]<", wf->conditions);
   
   /* Get the temp */
   needle = strstr(needle, "class=\"temp\"");
   if (!needle)
     goto error;
   needle = strstr(needle, ">");
   if (!needle)
     goto error;
   sscanf(needle, ">%d", &wf->temp);
   needle = strstr(needle, "<");
   if (!needle)
     goto error;
   needle--;
   wf->degrees = needle[0];
   
   return 1;
 error:
   printf("ERROR: Couldn't parse info from rssweather.com\n");
   return 0;
}

void
_weather_convert_degrees(Weather_Face *wf)
{
   /* Check if degrees is in C or F */
   if ((wf->degrees == 'F') && (wf->conf->degrees == DEGREES_C))
     {
        wf->temp = (wf->temp - 32) * 5.0 / 9.0;
        wf->degrees = 'C';
     }
   if ((wf->degrees == 'C') && (wf->conf->degrees == DEGREES_F))
     {
        wf->temp = (wf->temp * 9.0 / 5.0) + 32;
        wf->degrees = 'F';
     }
}

static void 
_weather_face_menu_cb_face_new(void *data, E_Menu *m, E_Menu_Item *mi) 
{
   Weather *w;
   Weather_Face *wf;
   E_Container *con;
   
   w = data;
   if (!w)
     return;

   con = e_container_current_get(e_manager_current_get());
   wf = _weather_face_init(w, con);
   if (!wf)
     return;

   wf->conf = E_NEW(Config_Face, 1);
   wf->conf->enabled = 1;
   wf->conf->poll_time = 1800.0;
   wf->conf->display = DETAILED_DISPLAY;
   wf->conf->degrees = DEGREES_C;
   wf->conf->con = con->num;
   wf->conf->host = evas_stringshare_add("www.rssweather.com");
   wf->conf->code = evas_stringshare_add("ENKR");
   E_CONFIG_LIMIT(wf->conf->poll_time, 900.0, 3600.0);

   w->conf->faces = evas_list_append(w->conf->faces, wf->conf);   
   e_config_save_queue();

   wf->degrees = 'C';
   wf->loc_set = 0;
   wf->check_timer = ecore_timer_add((double)wf->conf->poll_time, _weather_face_cb_check, wf->weather);

   if (!wf->conf->enabled)
     _weather_face_disable(wf);
   
   _weather_face_cb_check(wf->weather);
}

static void 
_weather_face_menu_cb_face_del(void *data, E_Menu *m, E_Menu_Item *mi) 
{
   Weather *w;
   Weather_Face *wf;
   
   wf = data;
   if (!wf)
     return;
   
   w = wf->weather;
   if (!w)
     return;
   
   w->conf->faces = evas_list_remove(w->conf->faces, wf->conf);
   e_config_save_queue();
   _weather_face_free(wf);
}

static void
_weather_display_set(Weather_Face *wf, int ok)
{
   char buf[4096];

   if (!wf)
     return;
   
   if (wf->conf->display == DETAILED_DISPLAY)
     edje_object_signal_emit(wf->weather_obj, "set_style", "detailed");
   else
     edje_object_signal_emit(wf->weather_obj, "set_style", "simple");
   
   if (!ok)
     {
	snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/images/unknown.png");
	e_icon_file_set(wf->icon_obj, buf);
	edje_object_part_swallow(wf->weather_obj, "icon", wf->icon_obj);
	edje_object_part_text_set(wf->weather_obj, "location", "");
        edje_object_part_text_set(wf->weather_obj, "temp", "");
        edje_object_part_text_set(wf->weather_obj, "conditions", "");
     }
   else 
     {
	snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/images/%s", wf->icon);
	e_icon_file_set(wf->icon_obj, buf);
	edje_object_part_swallow(wf->weather_obj, "icon", wf->icon_obj);
	edje_object_part_text_set(wf->weather_obj, "location", wf->location);	
        snprintf(buf, sizeof(buf), "%d°%c",wf->temp, wf->degrees);
        edje_object_part_text_set(wf->weather_obj, "temp", buf);
        edje_object_part_text_set(wf->weather_obj, "conditions", wf->conditions);
     }
}
