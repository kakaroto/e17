/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

#include <Evry.h>
#include "e_mod_main.h"

#define ACT_GOOGLE		1
#define ACT_FEELING_LUCKY	2
#define ACT_WIKIPEDIA		3

typedef struct _Plugin Plugin;
typedef int (*Handler_Func) (void *data, int type, void *event);
typedef struct _Module_Config Module_Config;
struct _Plugin
{
  Evry_Plugin base;

  Ecore_Con_Server *svr;
  Ecore_Event_Handler *handler;
  Ecore_Timer *timer;

  const char *input;
  const char *server_address;
  const char *request;

  int (*fetch) (void *data);

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

static char _trigger_google[] = "g ";
static char _trigger_wiki[] = "w ";

static char _header[] =
  "User-Agent: Wget/1.12 (linux-gnu)\n"
  "Accept: */*\n"
  "Connection: Keep-Alive\n\n";

static char _request_goolge[] = "GET http://www.google.com/complete/search?hl=%s&output=text&q=\"%s\n%s";
static char _request_wiki[]   = "GET http://%s.wikipedia.org/w/api.php?action=opensearch&search=%s HTTP/1.0\n%s";
static char _address_google[] = "www.google.com";
static char _address_wiki[]   = "www.wikipedia.org";
static const char *_id_none;

int
_server_data(void *data, int ev_type, Ecore_Con_Event_Server_Data *ev)
{
  Plugin *p = data;
  char *result = (char *)ev->data;
  Evry_Item *it;
  char *list;

  if (ev->server != p->svr) return 1;

  EVRY_PLUGIN_ITEMS_FREE(p);

  it = EVRY_ITEM_NEW(Evry_Item, p, p->input, NULL, NULL);
  it->context = eina_stringshare_ref(EVRY_PLUGIN(p)->name);
  it->id = eina_stringshare_ref(_id_none);
  EVRY_PLUGIN_ITEM_APPEND(p, it);

  if ((list = strstr(result, "[[\"")))
    {
      list += 3;

      char **items = eina_str_split(list, "\"],[\"", 0);
      char **i;

      for(i = items; *i; i++)
	{
	  char **item= eina_str_split(*i, "\",\"", 2);
	  it = EVRY_ITEM_NEW(Evry_Item, p, *item, NULL, NULL);
	  it->detail = eina_stringshare_add(*(item + 1));
	  it->context = eina_stringshare_ref(EVRY_PLUGIN(p)->name);
	  it->fuzzy_match = -1;
	  EVRY_PLUGIN_ITEM_APPEND(p, it);
	  free(*item);
	  free(item);
	}

      free(*items);
      free(items);

      evry_plugin_async_update (EVRY_PLUGIN(p), EVRY_ASYNC_UPDATE_ADD);
    }
  else if ((list = strstr(result, ",[\"")))
    {
      list += 3;

      char **items = eina_str_split(list, "\"", 0);
      char **i;
      for(i = items; *i; i++)
	{
	  if (**i == ',' || **i == ']') continue;
	  it = EVRY_ITEM_NEW(Evry_Item, p, *i, NULL, NULL);
	  it->detail = eina_stringshare_add("Wikipedia");
	  it->context = eina_stringshare_ref(EVRY_PLUGIN(p)->name);
	  it->fuzzy_match = -1;
	  EVRY_PLUGIN_ITEM_APPEND(p, it);
	}

      free(*items);
      free(items);

    }

  evry_plugin_async_update (EVRY_PLUGIN(p), EVRY_ASYNC_UPDATE_ADD);

  return 1;
}

static Evry_Plugin *
_begin(Evry_Plugin *plugin, const Evry_Item *it)
{
  GET_PLUGIN(p, plugin);

  p->handler = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA,
				       (Handler_Func)_server_data, p);
  return plugin;
}

static void
_cleanup(Evry_Plugin *plugin)
{
  GET_PLUGIN(p, plugin);

  if (p->svr) ecore_con_server_del(p->svr);
  p->svr = NULL;

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

  if (p->svr) ecore_con_server_del(p->svr);
  p->svr = ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM,
				    p->server_address, 80, NULL);

  if (p->svr)
    {
      query = evry_util_url_escape(p->input, 0);

      snprintf(buf, sizeof(buf), p->request,
  	       _conf->lang, query, _header);

      ecore_con_server_send(p->svr, buf, strlen(buf));
    }

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

  if (input && strlen(input) > 2)
    {
      p->input = eina_stringshare_add(input);
      p->timer = ecore_timer_add(0.3, _send_request, p);
    }

  return 0;
}

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

Evas_Object *
_icon_get(Evry_Item *it, Evas *e)
{
  Evas_Object *o = e_icon_add(e);
  if (e_icon_file_edje_set(o, _conf->theme, it->icon))
    return o;

  evas_object_del(o);

  return NULL;
}

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

  p = EVRY_PLUGIN_NEW(Plugin, N_("Google"), "text-html", EVRY_TYPE_TEXT,
		      _begin, _cleanup, _fetch, NULL);

  p->trigger = _trigger_google;
  p->complete = &_complete;
  p->config_path = _config_path;
  _plug1 = (Plugin *) p;
  _plug1->server_address = _address_google;
  _plug1->request = _request_goolge;
  evry_plugin_register(p, EVRY_PLUGIN_SUBJECT, 10);

  p = EVRY_PLUGIN_NEW(Plugin, N_("Wikipedia"), "text-html", EVRY_TYPE_TEXT,
		  _begin, _cleanup, _fetch, NULL);
  p->trigger = _trigger_wiki;
  p->complete = &_complete;
  p->config_path = _config_path;
  _plug2 = (Plugin *) p;
  _plug2->server_address = _address_wiki;
  _plug2->request = _request_wiki;
  evry_plugin_register(p, EVRY_PLUGIN_SUBJECT, 9);

  _act1 = EVRY_ACTION_NEW(N_("Google for it"), EVRY_TYPE_TEXT,0, "go-next", _action, NULL);
  EVRY_ITEM_DATA_INT_SET(_act1, ACT_GOOGLE);
  EVRY_ITEM_ICON_SET(_act1, "google");
  EVRY_ITEM(_act1)->icon_get = &_icon_get;
  evry_action_register(_act1, 1);

  _act2 = EVRY_ACTION_NEW(N_("Wikipedia Page"), EVRY_TYPE_TEXT, 0, "go-next", _action, NULL);
  EVRY_ITEM_DATA_INT_SET(_act2, ACT_WIKIPEDIA);
  EVRY_ITEM_ICON_SET(_act2, "google");
  EVRY_ITEM(_act2)->icon_get = &_icon_get;
  evry_action_register(_act2, 1);

  _act3 = EVRY_ACTION_NEW(N_("Feeling Lucky"), EVRY_TYPE_TEXT, 0, "go-next", _action, NULL);
  EVRY_ITEM_DATA_INT_SET(_act3, ACT_FEELING_LUCKY);
  EVRY_ITEM_ICON_SET(_act3, "feeling-lucky");
  EVRY_ITEM(_act3)->icon_get = &_icon_get;
  evry_action_register(_act3, 1);

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

  return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
  if (e_datastore_get("everything_loaded"))
    _plugins_shutdown();

  _conf_shutdown();

  ecore_con_shutdown();

  eina_stringshare_del(_id_none);

  return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
  e_config_domain_save(_config_domain, _conf_edd, _conf);
  return 1;
}

/***************************************************************************/
