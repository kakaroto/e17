/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

#include "e.h"
#include "e_mod_main.h"
#include "evry_api.h"
// TODO - show error when input not parseable

static int  _cb_data(void *data, int type, void *event);
static int  _cb_error(void *data, int type, void *event);
static int  _cb_del(void *data, int type, void *event);

static const Evry_API *evry = NULL;
static Evry_Module *evry_module = NULL;
static Evry_Plugin *_plug;
static Ecore_Event_Handler *action_handler = NULL;

static Ecore_Exe *exe = NULL;
static Eina_List *history = NULL;
static Eina_List *handlers = NULL;
static int error = 0;

static char _module_icon[] = "accessories-calculator";

static Evry_Plugin *
_begin(Evry_Plugin *p, const Evry_Item *item __UNUSED__)
{
   Evry_Item *it;

   if (history)
     {
	const char *result;

	EINA_LIST_FREE(history, result)
	  {
	     it = EVRY_ITEM_NEW(Evry_Item, p, result, NULL, NULL);
	     it->context = eina_stringshare_ref(p->name);
	     p->items = eina_list_prepend(p->items, it);
	     eina_stringshare_del(result);
	  }
     }

   it = EVRY_ITEM_NEW(Evry_Item, p, "0", NULL, NULL);
   it->context = eina_stringshare_ref(p->name);
   p->items = eina_list_prepend(p->items, it);

   return p;
}

static int
_run_bc(Evry_Plugin *p)
{
   handlers = eina_list_append
     (handlers, ecore_event_handler_add
      (ECORE_EXE_EVENT_DATA, _cb_data, p));
   handlers = eina_list_append
     (handlers, ecore_event_handler_add
      (ECORE_EXE_EVENT_ERROR, _cb_error, p));
   handlers = eina_list_append
     (handlers, ecore_event_handler_add
      (ECORE_EXE_EVENT_DEL, _cb_del, p));

   exe = ecore_exe_pipe_run("bc -l",
			    ECORE_EXE_PIPE_READ |
			    ECORE_EXE_PIPE_READ_LINE_BUFFERED |
			    ECORE_EXE_PIPE_WRITE |
			    ECORE_EXE_PIPE_ERROR |
			    ECORE_EXE_PIPE_ERROR_LINE_BUFFERED,
			    NULL);
   return !!exe;
}


static void
_cleanup(Evry_Plugin *p)
{
   Ecore_Event_Handler *h;
   Evry_Item *it;
   int items = 10;

   if (p->items)
     {
	evry->item_free(p->items->data);
	p->items = eina_list_remove_list(p->items, p->items);
     }

   EINA_LIST_FREE(p->items, it)
     {
	if (items-- > 0)
	  history = eina_list_prepend(history, eina_stringshare_add(it->label));

	evry->item_free(it);
     }

   EINA_LIST_FREE(handlers, h)
     ecore_event_handler_del(h);

   if (exe)
     {
	ecore_exe_quit(exe);
	ecore_exe_free(exe);
	exe = NULL;
     }
}

static int
_cb_action_performed(void *data, int type, void *event)
{
   Eina_List *l;
   Evry_Item *it, *it2, *it_old;
   Evry_Event_Action_Performed *ev = event;
   Evry_Plugin *p = _plug;

   if (!ev->it1 || !(ev->it1->plugin == p))
     return 1;

   if (!p->items)
     return 1;
   
   /* remove duplicates */
   if (p->items->next)
     {
	it = p->items->data;

	EINA_LIST_FOREACH(p->items->next, l, it2)
	  {
	     if (!strcmp(it->label, it2->label))
	       {
		  p->items = eina_list_promote_list(p->items, l);
		  evry->item_changed(it, 0, 1);
		  EVRY_PLUGIN_UPDATE(p, EVRY_UPDATE_ADD);
		  return 1;
	       }
	  }
     }

   it_old = p->items->data;
   it_old->selected = EINA_FALSE;

   it2 = EVRY_ITEM_NEW(Evry_Item, p, it_old->label, NULL, NULL);
   it2->context = eina_stringshare_ref(p->name);
   p->items = eina_list_prepend(p->items, it2);
   evry->item_changed(it2, 0, 1);
   EVRY_PLUGIN_UPDATE(p, EVRY_UPDATE_ADD);

   return 1;
}

static int
_fetch(Evry_Plugin *p, const char *input)
{
   char buf[1024];

   if (!input) return 0;

   if (!exe && !_run_bc(p)) return 0;

   if (!strncmp(input, "scale=", 6))
     snprintf(buf, 1024, "%s\n", input);
   else
     snprintf(buf, 1024, "scale=3;%s\n", input);

   ecore_exe_send(exe, buf, strlen(buf));

   /* XXX after error we get no response for first input ?! - send a
      second time...*/
   if (error)
     {
	ecore_exe_send(exe, buf, strlen(buf));
	error = 0;
     }

   return 1;
}

static int
_cb_data(void *data, int type __UNUSED__, void *event)
{
   Ecore_Exe_Event_Data *ev = event;
   Evry_Plugin *p = data;
   Evry_Item *it;

   if (ev->exe != exe) return 1;

   if (ev->lines)
     {
	it = p->items->data;
	eina_stringshare_del(it->label);
	it->label = eina_stringshare_add(ev->lines->line);

	if (it) evry->item_changed(it, 0, 0);
     }

   return 1;
}

static int
_cb_error(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Exe_Event_Data *ev = event;

   if (ev->exe != exe)
     return 1;

   error = 1;

   return 1;
}

static int
_cb_del(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Exe_Event_Del *e = event;

   if (e->exe != exe)
     return 1;

   exe = NULL;
   return 1;
}

static int
_plugins_init(const Evry_API *_api)
{
   if (evry_module->active)
     return EINA_TRUE;

   evry = _api;

   if (!evry->api_version_check(EVRY_API_VERSION))
     return EINA_FALSE;

   action_handler = evry->event_handler_add(EVRY_EVENT_ACTION_PERFORMED,
					    _cb_action_performed, NULL);

   _plug = EVRY_PLUGIN_NEW(Evry_Plugin, N_("Calculator"),
			_module_icon,
			EVRY_TYPE_TEXT,
			_begin, _cleanup, _fetch, NULL);

   _plug->history     = EINA_FALSE;
   _plug->async_fetch = EINA_TRUE;

   if (evry->plugin_register(_plug, EVRY_PLUGIN_SUBJECT, 0))
     {
	Plugin_Config *pc = _plug->config;
	pc->view_mode = VIEW_MODE_LIST;
	pc->trigger = eina_stringshare_add("=");
	/* pc->trigger_only = EINA_TRUE; */
	pc->aggregate = EINA_FALSE;
	pc->top_level = EINA_FALSE;
	pc->min_query = 3;
     }

   return EINA_TRUE;
}

static void
_plugins_shutdown(void)
{
   if (!evry_module->active) return;

   ecore_event_handler_del(action_handler);
   action_handler = NULL;

   EVRY_PLUGIN_FREE(_plug);

   evry_module->active = EINA_FALSE;
}

/***************************************************************************/

EAPI E_Module_Api e_modapi =
{
   E_MODULE_API_VERSION,
   "everything-calc"
};

EAPI void *
e_modapi_init(E_Module *m)
{
   evry_module = E_NEW(Evry_Module, 1);
   evry_module->init     = &_plugins_init;
   evry_module->shutdown = &_plugins_shutdown;
   EVRY_MODULE_REGISTER(evry_module);

   if ((evry = e_datastore_get("everything_loaded")))
     evry_module->active = _plugins_init(evry);

   e_module_delayed_set(m, 1);

   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   _plugins_shutdown();

   EVRY_MODULE_UNREGISTER(evry_module);
   E_FREE(evry_module);

   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   return 1;
}

/***************************************************************************/
