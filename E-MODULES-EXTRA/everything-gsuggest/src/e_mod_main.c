/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

#include <Evry.h>
#include "e_mod_main.h"

typedef struct _Plugin Plugin;
typedef int (*Handler_Func) (void *data, int type, void *event);

struct _Plugin
{
  Evry_Plugin base;

  Ecore_Con_Server *svr;
  Ecore_Event_Handler *handler;
  Ecore_Timer *timer;
  
  const char *input;  
};

static Plugin *_plug = NULL;
static Evry_Action *_act = NULL;
static const char _trigger[] = "go ";

int
_server_data (void *data, int ev_type, Ecore_Con_Event_Server_Data *ev)
{
  Plugin *p = data;
  char *result = (char *)ev->data;
  Evry_Item *it;

  EVRY_PLUGIN_ITEMS_FREE(p);
  
  if (!strncmp(result, "HTTP/1.0 200 OK", 15))
    {
      result += 2;
      result = strstr(result, "[[\"");
      if (result) 
	{
	  result +=3;

	  char **items = eina_str_split(result, "\"],[\"", 0);
	  char **i;
	  
	  for(i = items; *i; i++)
	    {
	      char **item= eina_str_split(*i, "\",\"", 2);	      
	      /* printf("%s %s\n", *item, *(item + 1)); */
	      it = evry_item_new(NULL, EVRY_PLUGIN(p), *item, NULL);
	      it->detail = eina_stringshare_add(*(item + 1));
	      it->fuzzy_match = -1;
	      EVRY_PLUGIN_ITEM_APPEND(p, it);
	      free(*item);
	      free(item);
	    }
	  
	  free(*items);
	  free(items);

	  evry_plugin_async_update (EVRY_PLUGIN(p), EVRY_ASYNC_UPDATE_ADD);
	}
    }

  if (p->svr) ecore_con_server_del(p->svr);
  p->svr = NULL;
  
  return 1;
}

static Evry_Plugin *
_begin(Evry_Plugin *plugin, const Evry_Item *it)
{
  PLUGIN(p, plugin);
  
  p->handler = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA,
				       (Handler_Func)_server_data, p);
  return plugin;
}

static void
_cleanup(Evry_Plugin *plugin)
{
  PLUGIN(p, plugin);

  if (p->svr) ecore_con_server_del(p->svr);
  p->svr = NULL;
  
  ecore_event_handler_del(p->handler);
  p->handler = NULL;
  
  EVRY_PLUGIN_ITEMS_FREE(p);
}

static int
_timer_cb(void *data)
{
  Plugin *p = data;
  
  p->svr = ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM,"www.google.com", 80, NULL);
  
  if (p->svr)
    {
      char buf[1024];
      char *query;
      
      query = evry_util_url_escape(p->input, 0); 

      snprintf(buf, sizeof(buf),
	       "GET http://www.google.com/complete/search?output=text&q=\"%s\n",
	       query);
      /* printf("send: %s\n", buf); */
      free(query);
  
      ecore_con_server_send(p->svr, buf, strlen(buf));
    }

  p->timer = NULL;
  
  return 0;
}

static int
_fetch(Evry_Plugin *plugin, const char *input)
{
  PLUGIN(p, plugin);

  if (p->input)
    eina_stringshare_del(p->input);
  p->input = NULL;

  if (p->timer)
    ecore_timer_del(p->timer);
  p->timer = NULL;
  
  if (input && strlen(input) > 2)
    {        
      p->input = eina_stringshare_add(input);  
      p->timer = ecore_timer_add(0.2, _timer_cb, p);
    }
  
  return 0;
}

static Evas_Object *
_icon_get(Evry_Plugin *p __UNUSED__, const Evry_Item *it, Evas *e)
{
  return NULL;
}

static int
_action(Evry_Action *act)
{
  Evry_Item_App *app = E_NEW(Evry_Item_App, 1);
  Evry_Item_File *file = E_NEW(Evry_Item_File, 1);

  app->desktop = efreet_util_desktop_exec_find("chromium-browser");
  
  if (app->desktop)
    {
      char buf[1024];
      Eina_List *l;
      E_Border *bd;
      
      snprintf(buf, sizeof(buf),
	       "http://www.google.de/search?q=%s",
	       act->item1->label);
      file->path = buf;
      evry_util_exec_app(EVRY_ITEM(app), EVRY_ITEM(file));

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
  free(app);
  free(file);
}

static Eina_Bool
module_init(void)
{
  if (!evry_api_version_check(EVRY_API_VERSION))
    return EINA_FALSE;
  
  if (!ecore_con_init())
    return EINA_FALSE;

  _plug = E_NEW(Plugin, 1);
  EVRY_PLUGIN_NEW(_plug, "GSuggest", type_subject, "", "TEXT",
		  _begin, _cleanup, _fetch, NULL, NULL);

  evry_plugin_register(EVRY_PLUGIN(_plug), 10);
  EVRY_PLUGIN(_plug)->trigger = _trigger;
  
  _act = EVRY_ACTION_NEW("Google for it", "TEXT", NULL, NULL, _action, NULL);
  evry_action_register(_act, 1);
  
  return EINA_TRUE;
}

static void
module_shutdown(void)
{
  EVRY_PLUGIN_FREE(_plug);
  
  ecore_con_shutdown();
}

/***************************************************************************/
/**/
/* actual module specifics */

static E_Module *module = NULL;
static Eina_Bool _active = EINA_FALSE;

/***************************************************************************/
/**/
/* module setup */
EAPI E_Module_Api e_modapi = 
  {
    E_MODULE_API_VERSION,
    PACKAGE
  };

EAPI void *
e_modapi_init(E_Module *m)
{
  module = m;

  if (e_datastore_get("everything_loaded"))
    _active = module_init();
   
  e_module_delayed_set(m, 1); 

  return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
  if (_active && e_datastore_get("everything_loaded"))
    module_shutdown();

  module = NULL;
   
  return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
  return 1;
}

/**/
/***************************************************************************/

