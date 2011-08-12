#include "e.h"
#include "e_mod_main.h"
#include "evry_api.h"

typedef struct _Plugin Plugin;
typedef struct _Item Item;

/* before getting into coding replace SKEL with something appropriate in:
 *
 *   configure.ac
 *   module.desktop.in
 *   e_module.spec.in
 *   README
 */

struct _Plugin
{
  Evry_Plugin base;

  /* add stuff that every plugin instance needs */

  /* for example keep a list of all items the plugin provides. */
  Eina_List *items;
};

/* use a custom item type. Evry provides type for TEXT, FILES and APPS
 * if you want to provide those you dont need this here. */
struct _Item
{
  Evry_Item *base;
};

/* pointer to evry functions */
static const Evry_API *evry = NULL;

/* module to be registered in evry */
static Evry_Module *evry_module = NULL;

static Eina_List *plugins = NULL;
static Eina_List *actions = NULL;

/* cast Evry_Item to Item.
 * - likewise GET_PLUGIN is defined in evry_api.h
 * - EVRY_ITEM casts Item to Evry_Item
 * - EVRY_PLUGIN casts Plugin to Evry_Plugin
 */
#define GET_MY_ITEM(_it, _item) Item *_it = (Item*)_item;
#define ITEM(_item) (Item*)_item;

static void
_cb_item_free(Evry_Item *item)
{
   GET_ITEM(it, item);

   E_FREE(it);
}

static Evas_Object *
_cb_icon_get(Evry_Item *item, Evas *e)
{
   Evas_Object *o;
   
   GET_MY_ITEM(it, item);

   o = e_icon_add(e); 
   e_icon_preload_set(o, 1);
   /* e_icon_file_set(o, FILE);  */
   
   return o;
}

static Item *
_item_add(Plugin *p, const char *label)
{
   Item *it;

   /* create a new Evry_Item of type 'Item' */
   it = EVRY_ITEM_NEW(Item, p, label, _cb_icon_get, _cb_item_free);

   /* append item to plugins' list */
   p->items = eina_list_append(p->items, it);

   return it;
}

static Evry_Plugin *
_cb_plugin_new(Evry_Plugin *plugin, const Evry_Item *it __UNUSED__)
{
   Plugin *p;

   /* create a new plugin instance */
   EVRY_PLUGIN_INSTANCE(p, plugin);

   return EVRY_PLUGIN(p);
}

static void
_cb_plugin_free(Evry_Plugin *plugin)
{
   GET_PLUGIN(p, plugin);

   EVRY_PLUGIN_ITEMS_CLEAR(p);

   E_FREE(p);
}

static int
_cb_plugin_fetch(Evry_Plugin *plugin, const char *input)
{
   Item *it;

   GET_PLUGIN(p, plugin);

   if (!input) return 0;

   /* clear the list that evry uses to show candidates */
   EVRY_PLUGIN_ITEMS_CLEAR(p);

   it = _item_add(p, input);

   /* add item to the list that evry uses to show candidates */
   EVRY_PLUGIN_ITEM_APPEND(p, it);

   /* or to add all items which evry_item->label matches input use this */
   // EVRY_PLUGIN_ITEMS_ADD(p, p->items, input, filter_detail = 0, set_usage = 0)

   /* return 1 when plugin provides items */
   return 1;
}

static int
_cb_module_init(const Evry_API *_api)
{
   Evry_Plugin *plugin;
   
   evry = _api;

   if (!evry->api_version_check(EVRY_API_VERSION))
     return EINA_FALSE;

   /* this creates the base plugin to be registered with evry.
    * evry calls _cb_plugin_new to get an instance that will be
    * queried with _cb_plugin_fetch and freed with _cb_plugin_free
    * when evry cleans up the state to which the plugin instance
    * belongs */
   plugin = EVRY_PLUGIN_BASE("SKELETON Plugin", "fdo_icon_name", EVRY_TYPE_TEXT,
			     _cb_plugin_new, _cb_plugin_free, _cb_plugin_fetch);

   if (evry->plugin_register(plugin, EVRY_PLUGIN_SUBJECT, 100))
     {
	/* if evry->plugin register returns 1 the plugin was
	   registered the first time and you can set some initial defaults */

	/* Plugin_Config *pc = _plug->config;
	 * pc->view_mode = VIEW_MODE_LIST;
	 * pc->aggregate = EINA_FALSE;
	 * pc->trigger = eina_stringshare_add(TRIGGER);
	 * pc->trigger_only = EINA_TRUE;
	 * pc->min_query = 4; */
     }
   plugins = eina_list_append(plugins, plugin);
   
   return EINA_TRUE;
}

static void
_cb_module_shutdown(void)
{
   Evry_Plugin *plugin;
   Evry_Action *act;
   
   EINA_LIST_FREE(plugins, plugin)
     EVRY_PLUGIN_FREE(plugin);

   EINA_LIST_FREE(actions, act)
     EVRY_ACTION_FREE(act);
}

/***************************************************************************/

/* e17 module stuff */

EAPI E_Module_Api e_modapi =
{
   E_MODULE_API_VERSION,
   "everything-SKEL"
};

EAPI void *
e_modapi_init(E_Module *m)
{
   EVRY_MODULE_NEW(evry_module, evry, _cb_module_init, _cb_module_shutdown);

   e_module_delayed_set(m, 1);

   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m __UNUSED__)
{
   EVRY_MODULE_FREE(evry_module);

   return 1;
}

EAPI int
e_modapi_save(E_Module *m __UNUSED__)
{
   return 1;
}

