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

/* #undef DBG
 * #define DBG(...) ERR(__VA_ARGS__) */

typedef struct _Plugin Plugin;
typedef int (*Handler_Func) (void *data, int type, void *event);
typedef struct _Module_Config Module_Config;
typedef struct _Json_Data Json_Data;

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
  const char *translate;

  E_Config_Dialog *cfd;
  E_Module *module;
  char *theme;
};

struct _Json_Data
{
  Json_Data *parent;
  Json_Data *cur;

  int type;
  const char *key;
  const char *value;
  Eina_List *values;
  Eina_List *list;

  int flat;
};

static Module_Config *_conf;
static char _config_path[] =  "extensions/" PACKAGE;
static char _config_domain[] = "module." PACKAGE;
static E_Config_DD *_conf_edd = NULL;

static Eina_List *plugins = NULL;
static Eina_List *actions = NULL;

static char _trigger_google[] = "g ";
static char _trigger_wiki[] = "w ";
static char _trigger_gtranslate[] = "t ";

static char _request_goolge[] =
  "http://www.google.com/complete/search?hl=%s&output=text&q=%s";
static char _request_wiki[]   =
  "http://%s.wikipedia.org/w/api.php?action=opensearch&search=%s";
static char _request_gtranslate[] =
  "http://ajax.googleapis.com/ajax/services/language/translate?v=1.0&langpair=%s&q=%s";
static const char _imgur_key[] =
  "1606e11f5c2ccd9b7440f1ffd80b17de";

static Json_Data  *_json_parse(const char *string, Eina_Bool flat, int len);
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

  EVRY_PLUGIN_ITEMS_FREE(p);

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
_gtranslate_data_cb(void *data, int ev_type, void *event)
{
  Ecore_Con_Event_Url_Data *ev = event;
  Plugin *p = data;
  Json_Data *d, *rsp;
  const char *val, *msg;
  Eina_List *l;
  Evry_Item *it;
  int len;

  if (data != ecore_con_url_data_get(ev->url_con))
    return 1;

  EVRY_PLUGIN_ITEMS_FREE(p);

  msg = (const char *) ev->data;
  len = ecore_con_url_received_bytes_get(ev->url_con);

  fprintf(stdout, "parse %*s\n", len, msg);

  rsp = _json_parse(msg, 1, len);

  d = _json_data_find(rsp, "translatedText");
  if (d)
    {
      printf("string %s\n", d->value);
      it = EVRY_ITEM_NEW(Evry_Item, p, d->value, NULL, NULL);
      it->context = eina_stringshare_ref(EVRY_PLUGIN(p)->name);
      it->fuzzy_match = -1;
      EVRY_PLUGIN_ITEM_APPEND(p, it);
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
  const char *val;
  Eina_List *l, *ll;
  Evry_Item *it;
  int len;

  if (data != ecore_con_url_data_get(ev->url_con))
    return 1;

  EVRY_PLUGIN_ITEMS_FREE(p);

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

  evry_plugin_async_update (EVRY_PLUGIN(p), EVRY_ASYNC_UPDATE_ADD);

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

static Evry_Plugin *
_begin_gtranslate(Evry_Plugin *plugin, const Evry_Item *it)
{
  GET_PLUGIN(p, plugin);

  _begin(plugin, it);

  ecore_con_url_additional_header_add(p->con_url, "Host", "ajax.googleapis.com");
  /* ecore_con_url_additional_header_add(p->con_url, "Accept", "application/json"); */
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

  if (!strcmp(p->base.name, N_("Translate")))
    snprintf(buf, sizeof(buf), p->request, _conf->translate, query);
  else
    snprintf(buf, sizeof(buf), p->request, _conf->lang, query);
  /* printf("send request %s\n", buf); */

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

  return 1;
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
  Evry_Action *act;
  char *header;

  act = ecore_con_url_data_get(ev->url_con);

  if (data != act)
    return;

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
  Evry_Action *act;
  E_Notification *n;
  int len;

  act = ecore_con_url_data_get(ev->url_con);

  if (data != act)
    return;

  GET_FILE(file, act->it1.item);

  len = ecore_con_url_received_bytes_get(ev->url_con);
  fprintf(stdout, "parse %*s\n", len, (char *)ev->data);

  rsp = _json_parse((char *)ev->data, 1, len);
  d = _json_data_find(rsp, "imgur_page");

  if (d)
    {
      len = strlen(d->value);
      ecore_x_selection_primary_set(ecore_x_window_root_first_get(), d->value, len);
      ecore_x_selection_clipboard_set(ecore_x_window_root_first_get(), d->value, len);
      n = e_notification_full_new("Everything", 0, "image", "Image Sent",
				  "Link copied to clipboard", -1);
    }
  else
    {
      n = e_notification_full_new("Everything", 0, "image", "Something went wrong :(",
				  file->path, -1);
    }

  e_notification_send(n, NULL, NULL);
  e_notification_unref(n);

  _json_data_free(rsp);

  EVRY_ITEM_FREE(file);
  EVRY_ITEM_FREE(act);

  return 0;
}

static int
_con_url_progress(void *data, int ev_type, void *event)
{
  Ecore_Con_Event_Url_Progress *ev = event;

  return 1;
}

static int
_action_upload_check(Evry_Action *act, const Evry_Item *it)
{
  return (EVRY_FILE(it)->mime && !(strncmp(EVRY_FILE(it)->mime, "image/", 6)));
}

static int
_action_upload(Evry_Action *act)
{
  struct stat info;
  struct curl_httppost* post = NULL;
  struct curl_httppost* last = NULL;
  E_Notification *n;
  Ecore_Con_Url *con_url;
  int ret;

  GET_FILE(file, act->it1.item);
  if (!evry_file_path_get(file))
    return 0;

  con_url = ecore_con_url_new("http://imgur.com/api/upload.json");

  con_complete = ecore_event_handler_add
    (ECORE_CON_EVENT_URL_COMPLETE, _con_complete, act);

  con_data = ecore_event_handler_add
    (ECORE_CON_EVENT_URL_DATA, _con_data, act);

  evry_item_ref(EVRY_ITEM(act));
  evry_item_ref(EVRY_ITEM(file));
  ecore_con_url_data_set(con_url, act);

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
  Evry_Action *act;
  Plugin *plug;

  if (!evry_api_version_check(EVRY_API_VERSION))
    return EINA_FALSE;

#define PLUGIN_NEW(_name, _icon, _begin, _cleaup, _fetch, _complete, _request, _data_cb, _trigger) { \
    p = EVRY_PLUGIN_NEW(Plugin, _name, _icon, EVRY_TYPE_TEXT, _begin, _cleanup, _fetch, NULL); \
    p->config_path = _config_path;				\
    plugins = eina_list_append(plugins, p);			\
    p->complete = _complete;					\
    GET_PLUGIN(plug, p);					\
    plug->request = _request;					\
    plug->data_cb = _data_cb;					\
    if (evry_plugin_register(p, EVRY_PLUGIN_SUBJECT, 10)) {	\
      Plugin_Config *pc = p->config;				\
      pc->view_mode = VIEW_MODE_LIST;				\
      pc->aggregate = EINA_FALSE;				\
      pc->top_level = EINA_FALSE;				\
      pc->view_mode = VIEW_MODE_DETAIL;				\
      pc->min_query = 3;					\
      pc->trigger_only = EINA_TRUE;				\
      pc->trigger = eina_stringshare_add(_trigger); }}		\

  PLUGIN_NEW(N_("Google"), "text-html",
	     _begin, _cleanup, _fetch, &_complete,
	     _request_goolge, _google_data_cb,
	     _trigger_google);

  PLUGIN_NEW(N_("Wikipedia"), "text-html",
	     _begin, _cleanup, _fetch, &_complete,
	     _request_wiki, _wikipedia_data_cb,
	     _trigger_wiki);

  PLUGIN_NEW(N_("Translate"), "text-html",
	     _begin_gtranslate, _cleanup, _fetch, NULL,
	     _request_gtranslate, _gtranslate_data_cb,
	     _trigger_gtranslate);


#define ACTION_NEW(_name, _type, _icon, _action, _check, _method)	\
  act = EVRY_ACTION_NEW(_name, _type, 0, _icon, _action, _check);	\
  EVRY_ITEM_DATA_INT_SET(act, _method);					\
  EVRY_ITEM(act)->icon_get = &_icon_get;				\
  evry_action_register(act, 1);						\
  actions = eina_list_append(actions, act);				\

  ACTION_NEW(N_("Google for it"), EVRY_TYPE_TEXT, "google",
	     _action, NULL, ACT_GOOGLE);

  ACTION_NEW(N_("Wikipedia Page"), EVRY_TYPE_TEXT, "wikipedia",
	     _action, NULL, ACT_WIKIPEDIA);

  ACTION_NEW(N_("Feeling Lucky"), EVRY_TYPE_TEXT, "feeling-lucky",
	     _action, NULL, ACT_FEELING_LUCKY);

  ACTION_NEW(N_("Upload Image"), EVRY_TYPE_FILE, "go-next",
	     _action_upload, _action_upload_check, ACT_UPLOAD_IMGUR);
  act->remember_context = EINA_TRUE;

  return EINA_TRUE;
}

static void
_plugins_shutdown(void)
{
  Evry_Plugin *p;
  Evry_Action *act;

  EINA_LIST_FREE(plugins, p)
    EVRY_PLUGIN_FREE(p);

  EINA_LIST_FREE(actions, act)
    evry_action_free(act);
}

/***************************************************************************/

struct _E_Config_Dialog_Data
{
  char *browser;
  char *lang;
  char *translate;
};

static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
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

  ow = e_widget_label_add(evas, _("Translate"));
  e_widget_framelist_object_append(of, ow);
  ow = e_widget_entry_add(evas, &cfdata->translate, NULL, NULL, NULL);
  e_widget_framelist_object_append(of, ow);

  e_widget_list_object_append(o, of, 1, 1, 0.5);
  return o;
}

static void *
_create_data(E_Config_Dialog *cfd)
{
  E_Config_Dialog_Data *cfdata = NULL;
  cfdata = E_NEW(E_Config_Dialog_Data, 1);

#define CP(_name) cfdata->_name = strdup(_conf->_name);
#define C(_name) cfdata->_name = _conf->_name;
  CP(browser);
  CP(lang);
  CP(translate);
#undef CP
#undef C
  return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
  E_FREE(cfdata->browser);
  E_FREE(cfdata->lang);
  E_FREE(cfdata->translate);
  _conf->cfd = NULL;
  E_FREE(cfdata);
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
  CP(translate);
#undef CP
#undef C

  e_config_domain_save(_config_domain, _conf_edd, _conf);
  e_config_save_queue();
  return 1;
}

static void
_conf_new(void)
{
  if (!_conf)
    {
      _conf = E_NEW(Module_Config, 1);
      _conf->version = (MOD_CONFIG_FILE_EPOCH << 16);
    }

#define IFMODCFG(v) if ((_conf->version & 0xffff) < v) {
#define IFMODCFGEND }

  /* setup defaults */
  IFMODCFG(0x008d);
  _conf->browser = eina_stringshare_add("firefox");
  _conf->lang = eina_stringshare_add("en");
  IFMODCFGEND;

  IFMODCFG(0x009d);
  _conf->translate = eina_stringshare_add("en|de");
  IFMODCFGEND;

  _conf->version = MOD_CONFIG_FILE_VERSION;

  e_config_save_queue();
}

static void
_conf_free(void)
{
  if (!_conf) return;
  eina_stringshare_del(_conf->browser);
  eina_stringshare_del(_conf->lang);
  eina_stringshare_del(_conf->translate);
  free(_conf->theme);
  E_FREE(_conf);
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
  E_CONFIG_VAL(D, T, translate, STR);
#undef T
#undef D
  _conf = e_config_domain_load(_config_domain, _conf_edd);

  if (_conf && !evry_util_module_config_check
      (_("Everything Websearch"), _conf->version, MOD_CONFIG_FILE_EPOCH, MOD_CONFIG_FILE_VERSION))
    {
      _conf_free();
    }

  _conf_new();

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
  ecore_con_url_init();
  _conf_init(m);

  if (!_plugins_init())
    {
      _conf_shutdown();
      return NULL;
    }

  e_module_delayed_set(m, 1);

  return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{

  if (e_datastore_get("everything_loaded"))
    _plugins_shutdown();

  _conf_shutdown();
  e_notification_shutdown();
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
      if (d->flat)
	{
	  if (d->cur->parent) d->cur = d->cur->parent;
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

  printf("found %p\n", d);

  eina_stringshare_del(k);

  return d;
}

static void
_json_data_free(Json_Data *jd)
{
  Json_Data *d;
  const char *val;
  if (!jd) return;

  if (jd->list) DBG("-------------------");

  EINA_LIST_FREE(jd->list, d)
    {
      DBG("%s : %s", d->key, d->value);
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
_json_parse(const char *string, Eina_Bool flat, int len)
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
