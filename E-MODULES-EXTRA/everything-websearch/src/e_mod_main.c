/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

#include <Evry.h>
#include <curl/curl.h>
#include <E_Notify.h>

#include "e_mod_main.h"
#include "json.h"

#define ACT_GOOGLE		1
#define ACT_FEELING_LUCKY	2
#define ACT_WIKIPEDIA		3
#define ACT_UPLOAD_IMGUR	4

typedef struct _Plugin Plugin;
typedef int (*Handler_Func) (void *data, int type, void *event);
typedef struct _Module_Config Module_Config;
struct _Plugin
{
  Evry_Plugin base;

  Ecore_Con_Url *con_url;
  Ecore_Timer *timer;
  Ecore_Event_Handler *handler;

  const char *input;
  const char *server_address;
  const char *request;

  int (*fetch) (void *data);
  int (*data_cb) (void *data, int ev_type, void *event);
};

struct _Module_Config
{
  int version;

  const char *lang;
  const char *browser;

  E_Config_Dialog *cfd;
  E_Module *module;
  char *theme;
};

static Module_Config *_conf;
static char _config_path[] =  "extensions/" PACKAGE;
static char _config_domain[] = "module." PACKAGE;
static E_Config_DD *_conf_edd = NULL;

static Plugin *_plug1 = NULL;
static Plugin *_plug2 = NULL;
static Evry_Action *_act1 = NULL;
static Evry_Action *_act2 = NULL;
static Evry_Action *_act3 = NULL;
static Evry_Action *_act4 = NULL;

static char _trigger_google[] = "g ";
static char _trigger_wiki[] = "w ";

static char _header[] =
  "User-Agent: Wget/1.12 (linux-gnu)\n"
  "Accept: */*\n"
  "Connection: Keep-Alive\n\n";

static char _request_goolge[] =
  "http://www.google.com/complete/search?hl=%s&output=text&q=%s";
static char _request_wiki[]   =
  "http://%s.wikipedia.org/w/api.php?action=opensearch&search=%s";
static char _address_google[] = "www.google.com";
static char _address_wiki[]   = "www.wikipedia.org";
static const char *_id_none;

static const char _imgur_key[] = "1606e11f5c2ccd9b7440f1ffd80b17de";

typedef struct _Json_Data Json_Data;

struct _Json_Data
{
  int flat;

  Json_Data *parent;
  Json_Data *cur;

  int type;

  const char *key;
  const char *value;
  Eina_List *values;

  Eina_List *list;
};
static Json_Data  *_json_parse(char *string, Eina_Bool flat, int len);
static Json_Data  *_json_data_find(const Json_Data *d, const char *key);
static void        _json_data_free(Json_Data *d);


/***************************************************************************/

static int
_wikipedia_data_cb(void *data, int ev_type, void *event)
{
  Ecore_Con_Event_Url_Data *ev = event;
  Plugin *p = data;
  Json_Data *d, *rsp;
  const char *val;
  Eina_List *l;
  Evry_Item *it;
  int len;

  if (data != ecore_con_url_data_get(ev->url_con))
    return 1;

  len = ecore_con_url_received_bytes_get(ev->url_con);
  rsp = _json_parse((char *)ev->data, 0, len);

  if (rsp && rsp->list &&
      (d = rsp->list->data) &&
      (d->type == JSON_ARRAY_BEGIN) &&
      (d = d->list->data) &&
      (d->type == JSON_ARRAY_BEGIN))
    {
      EINA_LIST_FOREACH(d->values, l, val)
	{
	  it = EVRY_ITEM_NEW(Evry_Item, p, val, NULL, NULL);
	  it->context = eina_stringshare_ref(EVRY_PLUGIN(p)->name);
	  it->fuzzy_match = -1;
	  EVRY_PLUGIN_ITEM_APPEND(p, it);
	}
    }

  _json_data_free(rsp);

  evry_plugin_async_update (EVRY_PLUGIN(p), EVRY_ASYNC_UPDATE_ADD);

  return 1;
}

static int
_google_data_cb(void *data, int ev_type, void *event)
{
  Ecore_Con_Event_Url_Data *ev = event;
  Plugin *p = data;
  Json_Data *d, *d2, *rsp = NULL;
  int len;
  const char *val;
  Eina_List *l, *ll;
  Evry_Item *it;

  if (data != ecore_con_url_data_get(ev->url_con))
    return 1;

  len = ecore_con_url_received_bytes_get(ev->url_con);
  /* FUCK, cant google give this as json instead of some weird
     javascript shit ?! - strip parentheses */
  char *beg, *msg;
  beg = msg = ev->data;
  msg = strchr(msg, '(');
  if (msg) msg++;
  len = len - (msg - beg) - 1;

  /* fprintf(stdout, "parse %*s\n", len, msg); */
  if (!msg)
    return 1;

  rsp = _json_parse(msg, 0, len);

  if (rsp && rsp->list &&
      (d = rsp->list->data) &&
      (d->type == JSON_ARRAY_BEGIN) &&
      (d = d->list->data) &&
      (d->type == JSON_ARRAY_BEGIN))
    {
      EINA_LIST_FOREACH(d->list, l, d2)
	{
	  ll = d2->values;

	  if (!ll->data || !ll->next->data)
	    continue;

	  val = ll->data;
	  it = EVRY_ITEM_NEW(Evry_Item, p, val, NULL, NULL);
	  it->context = eina_stringshare_ref(EVRY_PLUGIN(p)->name);
	  val = ll->next->data;
	  EVRY_ITEM_DETAIL_SET(it, val);
	  it->fuzzy_match = -1;
	  EVRY_PLUGIN_ITEM_APPEND(p, it);
	}
    }

  _json_data_free(rsp);

  return 1;
}

static Evry_Plugin *
_begin(Evry_Plugin *plugin, const Evry_Item *it)
{
  GET_PLUGIN(p, plugin);

  p->con_url = ecore_con_url_new(NULL);

  p->handler = ecore_event_handler_add
    (ECORE_CON_EVENT_URL_DATA, p->data_cb, p);

  ecore_con_url_data_set(p->con_url, p);
  return plugin;
}

static void
_cleanup(Evry_Plugin *plugin)
{
  GET_PLUGIN(p, plugin);

  ecore_con_url_destroy(p->con_url);
  p->con_url = NULL;

  ecore_event_handler_del(p->handler);
  p->handler = NULL;

  EVRY_PLUGIN_ITEMS_FREE(p);
}

static int
_send_request(void *data)
{
  Plugin *p = data;
  char buf[1024];
  char *query;

  query = evry_util_url_escape(p->input, 0);

  snprintf(buf, sizeof(buf), p->request, _conf->lang, query);

  printf("send request\n", buf);

  ecore_con_url_url_set(p->con_url, buf);
  ecore_con_url_send(p->con_url, NULL, 0, NULL);

  free(query);

  p->timer = NULL;

  return 0;
}

static int
_fetch(Evry_Plugin *plugin, const char *input)
{
  GET_PLUGIN(p, plugin);

  if (p->input)
    eina_stringshare_del(p->input);
  p->input = NULL;

  if (p->timer)
    ecore_timer_del(p->timer);
  p->timer = NULL;

  if (input && strlen(input) >= plugin->config->min_query)
    {
      p->input = eina_stringshare_add(input);
      p->timer = ecore_timer_add(0.1, _send_request, p);
    }
  else
    {
      EVRY_PLUGIN_ITEMS_FREE(p);
    }

  return 0;
}

/***************************************************************************/

static int
_action(Evry_Action *act)
{
  Evry_Item_App *app = E_NEW(Evry_Item_App, 1);
  Evry_Item_File *file = E_NEW(Evry_Item_File, 1);
  char buf[1024];
  Eina_List *l;
  E_Border *bd;

  app->desktop = efreet_util_desktop_exec_find(_conf->browser);

  if (!app->desktop)
    app->file = "xdg-open";

  char *tmp = evry_util_url_escape(act->it1.item->label, 0);

  if (EVRY_ITEM_DATA_INT_GET(act) == ACT_GOOGLE)
    {
      snprintf(buf, sizeof(buf), "http://www.google.com/search?hl=%s&q=%s",
	       _conf->lang, tmp);
    }
  else if (EVRY_ITEM_DATA_INT_GET(act) == ACT_WIKIPEDIA)
    {
      snprintf(buf, sizeof(buf), "http://%s.wikipedia.org/wiki/%s",
	       _conf->lang, tmp);
    }
  else if (EVRY_ITEM_DATA_INT_GET(act) == ACT_FEELING_LUCKY)
    {
      snprintf(buf, sizeof(buf), "http://www.google.com/search?hl=%s&q=%s&btnI=745",
	       _conf->lang, tmp);
    }
  E_FREE(tmp);

  file->path = buf;

  evry_util_exec_app(EVRY_ITEM(app), EVRY_ITEM(file));

  if (app->desktop)
    {
      EINA_LIST_FOREACH(e_border_client_list(), l, bd)
	{
	  if (bd->desktop && bd->desktop == app->desktop)
	    {
	      e_desk_show(bd->desk);
	      e_border_raise(bd);
	      break;
	    }
	}
      efreet_desktop_free(app->desktop);
    }

  E_FREE(file);
  E_FREE(app);
}

/***************************************************************************/

static Ecore_Event_Handler *con_complete;
static Ecore_Event_Handler *con_data;
static Ecore_Event_Handler *con_progress;

static int
_con_complete(void *data, int ev_type, void *event)
{
  Ecore_Con_Event_Url_Complete *ev = event;
  const Eina_List *l, *ll;

  if (data != _act4)
    return;

  printf("completed\n");

  char *reply = ecore_con_url_data_get(ev->url_con);
  char *header;

  l = ecore_con_url_response_headers_get(ev->url_con);

  EINA_LIST_FOREACH(l, ll, header)
    printf("%s", header);

  ecore_event_handler_del(con_complete);
  ecore_event_handler_del(con_data);

  ecore_con_url_destroy(ev->url_con);

  return 0;
}

static int
_con_data(void *data, int ev_type, void *event)
{
  Ecore_Con_Event_Url_Data *ev = event;
  Json_Data *d, *rsp;
  Evry_Item_File *file;
  int len, cb = 0;

  if (data != _act4)
    return;

  len = ecore_con_url_received_bytes_get(ev->url_con);
  file = ecore_con_url_data_get(ev->url_con);

  rsp = _json_parse((char *)ev->data, 1, len);

  d = _json_data_find(rsp, "imgur_page");
  if (d)
    {
      printf("copied to clipboard\n");

      len = strlen(d->value);
      ecore_x_selection_primary_set(ecore_x_window_root_first_get(), d->value, len);
      ecore_x_selection_clipboard_set(ecore_x_window_root_first_get(), d->value, len);
      cb = 1;
    }
  _json_data_free(rsp);

  E_Notification *n;
  if (cb)
    n = e_notification_full_new("Everything", 0, "image", "Image Sent", "Link copied to clipboard", -1);
  else
    n = e_notification_full_new("Everything", 0, "image", "Image Sent", file->path, -1);

  e_notification_send(n, NULL, NULL);
  e_notification_unref(n);

  EVRY_ITEM_FREE(file);

  return 0;
}

static int
_con_url_progress(void *data, int ev_type, void *event)
{
  Ecore_Con_Event_Url_Progress *ev = event;

  return 1;
}

static int
_action_upload_check(Evry_Action *act, const Evry_Item *item)
{
  GET_FILE(file, item);

  if (!file->mime || strncmp(file->mime, "image/", 6))
    return 0;

  return 1;
}

static int
_action_upload(Evry_Action *act)
{
  struct stat info;
  struct curl_httppost* post = NULL;
  struct curl_httppost* last = NULL;
  E_Notification *n;
  int ret;

  GET_FILE(file, act->it1.item);
  if (!evry_file_path_get(file))
    return 0;

  Ecore_Con_Url *con_url = ecore_con_url_new("http://imgur.com/api/upload.json");
  con_complete = ecore_event_handler_add
    (ECORE_CON_EVENT_URL_COMPLETE, _con_complete, act);

  con_data = ecore_event_handler_add
    (ECORE_CON_EVENT_URL_DATA, _con_data, act);

  evry_item_ref(EVRY_ITEM(file));
  ecore_con_url_data_set(con_url, file);

  /* con_url_progress = ecore_event_handler_add
   *   (ECORE_CON_EVENT_URL_PROGRESS, _con_url_progress, act); */

  ret = curl_formadd(&post, &last,
		     CURLFORM_COPYNAME, "key",
		     CURLFORM_COPYCONTENTS, _imgur_key,
		     CURLFORM_END);

  ret = curl_formadd(&post, &last,
  		     CURLFORM_COPYNAME, "image",
  		     CURLFORM_FILE, file->path,
  		     CURLFORM_END);

  ecore_con_url_http_post_send(con_url, post);

  n = e_notification_full_new("Everything", 0, "image", "Send Image", file->path, -1);
  e_notification_send(n, NULL, NULL);
  e_notification_unref(n);
  return 0;
}

Evas_Object *
_icon_get(Evry_Item *it, Evas *e)
{
  Evas_Object *o = e_icon_add(e);
  if (e_icon_file_edje_set(o, _conf->theme, it->icon))
    return o;

  evas_object_del(o);

  return NULL;
}

/***************************************************************************/

static int
_complete(Evry_Plugin *p, const Evry_Item *item, char **input)
{
  char buf[128];
  snprintf(buf, sizeof(buf), "%s ", item->label);

  *input = strdup(buf);

  return EVRY_COMPLETE_INPUT;
}

static Eina_Bool
_plugins_init(void)
{
  Evry_Plugin *p;

  if (!evry_api_version_check(EVRY_API_VERSION))
    return EINA_FALSE;

  p = EVRY_PLUGIN_NEW(Plugin, N_("Google"),
		      "text-html", EVRY_TYPE_TEXT,
		      _begin, _cleanup, _fetch, NULL);

  p->complete = &_complete;
  p->config_path = _config_path;
  _plug1 = (Plugin *) p;
  _plug1->server_address = _address_google;
  _plug1->request = _request_goolge;
  _plug1->data_cb = &_google_data_cb;
  if (evry_plugin_register(p, EVRY_PLUGIN_SUBJECT, 10))
    {
      Plugin_Config *pc = p->config;
      pc->view_mode = VIEW_MODE_LIST;
      pc->aggregate = EINA_FALSE;
      pc->top_level = EINA_FALSE;
      pc->view_mode = VIEW_MODE_DETAIL;
      pc->min_query = 3;
      pc->trigger = eina_stringshare_add(_trigger_google);
    }

  p = EVRY_PLUGIN_NEW(Plugin, N_("Wikipedia"),
		      "text-html", EVRY_TYPE_TEXT,
		      _begin, _cleanup, _fetch, NULL);
  p->complete = &_complete;
  p->config_path = _config_path;
  _plug2 = (Plugin *) p;
  _plug2->server_address = _address_wiki;
  _plug2->request = _request_wiki;
  _plug2->data_cb = &_wikipedia_data_cb;
  if (evry_plugin_register(p, EVRY_PLUGIN_SUBJECT, 9))
    {
      Plugin_Config *pc = p->config;
      pc->view_mode = VIEW_MODE_LIST;
      pc->aggregate = EINA_FALSE;
      pc->top_level = EINA_FALSE;
      pc->view_mode = VIEW_MODE_DETAIL;
      pc->min_query = 3;
      pc->trigger = eina_stringshare_add(_trigger_wiki);
    }

  _act1 = EVRY_ACTION_NEW(N_("Google for it"),
			  EVRY_TYPE_TEXT, 0,
			  NULL, _action, NULL);
  EVRY_ITEM_DATA_INT_SET(_act1, ACT_GOOGLE);
  EVRY_ITEM_ICON_SET(_act1, "google");
  EVRY_ITEM(_act1)->icon_get = &_icon_get;
  evry_action_register(_act1, 1);

  _act2 = EVRY_ACTION_NEW(N_("Wikipedia Page"),
			  EVRY_TYPE_TEXT, 0,
			  NULL, _action, NULL);
  EVRY_ITEM_DATA_INT_SET(_act2, ACT_WIKIPEDIA);
  EVRY_ITEM_ICON_SET(_act2, "google");
  EVRY_ITEM(_act2)->icon_get = &_icon_get;
  evry_action_register(_act2, 1);

  _act3 = EVRY_ACTION_NEW(N_("Feeling Lucky"),
			  EVRY_TYPE_TEXT, 0,
			  NULL, _action, NULL);
  EVRY_ITEM_DATA_INT_SET(_act3, ACT_FEELING_LUCKY);
  EVRY_ITEM_ICON_SET(_act3, "feeling-lucky");
  EVRY_ITEM(_act3)->icon_get = &_icon_get;
  evry_action_register(_act3, 1);

  _act4 = EVRY_ACTION_NEW(N_("Upload Image"), EVRY_TYPE_FILE, 0, "go-next",
  			  _action_upload, _action_upload_check);
  _act4->remember_context = EINA_TRUE;
  EVRY_ITEM_DATA_INT_SET(_act4, ACT_UPLOAD_IMGUR);
  evry_action_register(_act4, 1);

  return EINA_TRUE;
}

static void
_plugins_shutdown(void)
{
  EVRY_PLUGIN_FREE(_plug1);
  EVRY_PLUGIN_FREE(_plug2);

  evry_action_free(_act1);
  evry_action_free(_act2);
  evry_action_free(_act3);
  evry_action_free(_act4);
}

/***************************************************************************/

struct _E_Config_Dialog_Data
{
  char *browser;
  char *lang;
};

static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _fill_data(E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

static E_Config_Dialog *
_conf_dialog(E_Container *con, const char *params)
{
  E_Config_Dialog *cfd = NULL;
  E_Config_Dialog_View *v = NULL;
  char buf[4096];

  if (e_config_dialog_find(_config_path, _config_path))
    return NULL;

  v = E_NEW(E_Config_Dialog_View, 1);
  if (!v) return NULL;

  v->create_cfdata = _create_data;
  v->free_cfdata = _free_data;
  v->basic.create_widgets = _basic_create;
  v->basic.apply_cfdata = _basic_apply;

  snprintf(buf, sizeof(buf), "%s/e-module.edj", _conf->module->dir);

  cfd = e_config_dialog_new(con, _("Everything Websearch"),
			    _config_path, _config_path, buf, 0, v, NULL);

  _conf->cfd = cfd;
  return cfd;
}

static Evas_Object *
_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
  Evas_Object *o = NULL, *of = NULL, *ow = NULL;

  o = e_widget_list_add(evas, 0, 0);

  of = e_widget_framelist_add(evas, _("General"), 0);
  e_widget_framelist_content_align_set(of, 0.0, 0.0);

  ow = e_widget_label_add(evas, _("Browser"));
  e_widget_framelist_object_append(of, ow);
  ow = e_widget_entry_add(evas, &cfdata->browser, NULL, NULL, NULL);
  e_widget_framelist_object_append(of, ow);

  ow = e_widget_label_add(evas, _("Language"));
  e_widget_framelist_object_append(of, ow);
  ow = e_widget_entry_add(evas, &cfdata->lang, NULL, NULL, NULL);
  e_widget_framelist_object_append(of, ow);

  e_widget_list_object_append(o, of, 1, 1, 0.5);
  return o;
}

static void *
_create_data(E_Config_Dialog *cfd)
{
  E_Config_Dialog_Data *cfdata = NULL;

  cfdata = E_NEW(E_Config_Dialog_Data, 1);
  _fill_data(cfdata);
  return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
  E_FREE(cfdata->browser);
  E_FREE(cfdata->lang);
  _conf->cfd = NULL;
  E_FREE(cfdata);
}

static void
_fill_data(E_Config_Dialog_Data *cfdata)
{
#define CP(_name) cfdata->_name = strdup(_conf->_name);
#define C(_name) cfdata->_name = _conf->_name;
  CP(browser);
  CP(lang);
#undef CP
#undef C
}

static int
_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
#define CP(_name)						\
  if (_conf->_name)						\
    eina_stringshare_del(_conf->_name);				\
  _conf->_name = eina_stringshare_add(cfdata->_name);
#define C(_name) _conf->_name = cfdata->_name;
  CP(browser);
  CP(lang);
#undef CP
#undef C

  e_config_domain_save(_config_domain, _conf_edd, _conf);
  e_config_save_queue();
  return 1;
}

static void
_conf_new(void)
{
  _conf = E_NEW(Module_Config, 1);
  _conf->version = (MOD_CONFIG_FILE_EPOCH << 16);

#define IFMODCFG(v) if ((_conf->version & 0xffff) < v) {
#define IFMODCFGEND }

  /* setup defaults */
  IFMODCFG(0x008d);
  _conf->browser = eina_stringshare_add("firefox");
  _conf->lang = eina_stringshare_add("en");
  IFMODCFGEND;

  _conf->version = MOD_CONFIG_FILE_VERSION;

  e_config_save_queue();
}

static void
_conf_free(void)
{
  if (_conf)
    {
      eina_stringshare_del(_conf->browser);
      eina_stringshare_del(_conf->lang);

      free(_conf->theme);

      E_FREE(_conf);
    }
}

static void
_conf_init(E_Module *m)
{
  char buf[4096];

  snprintf(buf, sizeof(buf), "%s/e-module.edj", m->dir);

  e_configure_registry_category_add("extensions", 80, _("Extensions"),
				    NULL, "preferences-extensions");

  e_configure_registry_item_add(_config_path, 110, _("Everything Websearch"),
				NULL, buf, _conf_dialog);

  _conf_edd = E_CONFIG_DD_NEW("Module_Config", Module_Config);

#undef T
#undef D
#define T Module_Config
#define D _conf_edd
  E_CONFIG_VAL(D, T, version, INT);
  E_CONFIG_VAL(D, T, browser, STR);
  E_CONFIG_VAL(D, T, lang, STR);
#undef T
#undef D

  _conf = e_config_domain_load(_config_domain, _conf_edd);

  if (_conf && !evry_util_module_config_check(_("Everything Websearch"), _conf->version,
					      MOD_CONFIG_FILE_EPOCH, MOD_CONFIG_FILE_VERSION))
    _conf_free();

  if (!_conf) _conf_new();

  _conf->module = m;
  _conf->theme = strdup(buf);
}

static void
_conf_shutdown(void)
{
  _conf_free();

  E_CONFIG_DD_FREE(_conf_edd);
}

/***************************************************************************/

EAPI E_Module_Api e_modapi =
  {
    E_MODULE_API_VERSION,
    PACKAGE
  };

EAPI void *
e_modapi_init(E_Module *m)
{
  char buf[4096];

  snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
  bindtextdomain(PACKAGE, buf);
  bind_textdomain_codeset(PACKAGE, "UTF-8");

  e_notification_init();

  if (!ecore_con_init())
    return NULL;

  _conf_init(m);

  if (!_plugins_init())
    {
      _conf_shutdown();
      return NULL;
    }

  _id_none = eina_stringshare_add("");

  e_module_delayed_set(m, 1);

  ecore_con_url_init();

  return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{

  if (e_datastore_get("everything_loaded"))
    _plugins_shutdown();

  _conf_shutdown();

  ecore_con_shutdown();

  e_notification_shutdown();

  eina_stringshare_del(_id_none);

  ecore_con_url_shutdown();
  return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
  e_config_domain_save(_config_domain, _conf_edd, _conf);
  return 1;
}

/***************************************************************************/

static int
_parse_callback(void *userdata, int type, const char *data, uint32_t length)
{
  Json_Data *d = userdata;
  Json_Data *d2;

  switch (type)
    {
    case JSON_OBJECT_BEGIN:
      DBG("object beg %p\n", d->cur);
      if (d->flat && d->cur->parent)
	{
	  d->cur = d->cur->parent;
	  break;
	}
      d2 = calloc(1, sizeof(Json_Data));
      d2->parent = d->cur;
      d2->type = type;
      d->cur->list =  eina_list_append(d->cur->list, d2);
      d->cur = d2;
      break;

    case JSON_ARRAY_BEGIN:
      DBG("array beg %p\n", d->cur);
      d2 = calloc(1, sizeof(Json_Data));
      d2->parent = d->cur;
      d2->type = type;
      d->cur->list =  eina_list_append(d->cur->list, d2);
      d->cur = d2;
      break;

    case JSON_OBJECT_END:
      DBG("object end %p\n", d->cur);
      if (d->flat) break;
      d->cur = d->cur->parent;
      break;

    case JSON_ARRAY_END:
      DBG("array end %p\n", d->cur);
      d->cur = d->cur->parent;
      break;

    case JSON_KEY:
      DBG("key %*s\n", length, data);
      d2 = calloc(1, sizeof(Json_Data));
      if (d2->key) eina_stringshare_del(d2->key);
      d2->key = eina_stringshare_add_length(data, length);
      d2->parent = d->cur;
      d->cur->list =  eina_list_append(d->cur->list, d2);
      d->cur = d2;
      break;

    case JSON_STRING:
    case JSON_INT:
    case JSON_FLOAT:
      if (d->cur->type == JSON_ARRAY_BEGIN)
	{
	  DBG("- %*s\n", length, data);
	  d->cur->values = eina_list_append
	    (d->cur->values, eina_stringshare_add_length(data, length));
	}
      else
	{
	  DBG("val  %s: %*s\n", d->cur->key, length, data);
	  d->cur->type = type;
	  if (d->cur->value) eina_stringshare_del(d->cur->value);
	  d->cur->value = eina_stringshare_add_length(data, length);
	  d->cur = d->cur->parent;
	}

      break;

    case JSON_NULL:
    case JSON_TRUE:
    case JSON_FALSE:
      DBG("constant\n");
      d->cur = d->cur->parent;
      break;
    }

  return 0;
}

static Json_Data *
_json_data_find(const Json_Data *jd, const char *key)
{
  Json_Data *d = NULL;
  Eina_List *l;
  const char *k;

  if (!jd) return NULL;

  k = eina_stringshare_add(key);

  EINA_LIST_FOREACH(jd->list, l, d)
    if (d->key == k)
      break;

  eina_stringshare_del(k);

  return d;
}

static void
_json_data_free(Json_Data *jd)
{
  Json_Data *d;
  const char *val;

  if (!jd) return;

  EINA_LIST_FREE(jd->list, d)
    {
      if (d->key) eina_stringshare_del(d->key);
      if (d->value) eina_stringshare_del(d->value);
      EINA_LIST_FREE(d->values, val)
	{
	  eina_stringshare_del(val);
	}

      _json_data_free(d);
    }
  E_FREE(jd);
}

static Json_Data *
_json_parse(char *string, Eina_Bool flat, int len)
{
  Eina_Hash *h;
  struct json_parser parser;
  int i, ret;

  if (!len)
    len = strlen(string);

  if (!string)
    return NULL;

  Json_Data *d = E_NEW(Json_Data, 1);
  d->flat = flat;
  d->cur = d;

  if (json_parser_init(&parser, NULL, _parse_callback, d))
    {
      ERR("something wrong happened during init");
      E_FREE(d);
      return NULL;
    }

  for (i = 0; i < len; i += 1)
    {
      if ((ret = json_parser_string(&parser, string + i, 1, NULL)))
	{
	  ERR("%d\n", ret);
	  break;
	}
    }

  json_parser_free(&parser);
  if (ret)
    {
      _json_data_free(d);

      return NULL;
    }

  return d;
}
