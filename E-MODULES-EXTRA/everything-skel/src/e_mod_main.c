#include "e.h"
#include "e_mod_main.h"
#include "evry_api.h"

/* #undef DBG
 * #define DBG(...) ERR(__VA_ARGS__) */

/* #define QUERY_ITEM(_q, _it) Query_Item *_q = (Query_Item *) _it; */
/* typedef struct _Query_Item Query_Item; */

typedef struct _Plugin Plugin;

struct _Plugin
{
  Evry_Plugin base;
  const char *input;

};

/* struct _Query_Item
 * {
 *   Evry_Item base;
 * 
 *   const char *query;
 *   const char *match;
 * }; */

static const Evry_API *evry = NULL;
static Evry_Module *evry_module = NULL;
static Eina_List *plugins = NULL;

/* static E_DBus_Connection *conn = NULL;
 * static int dbus_active = 0; */

/* static Ecore_Event_Handler *action_handler = NULL; */

/* static DBusPendingCall *pending_get_name_owner = NULL;
 * static E_DBus_Signal_Handler *cb_name_owner_changed = NULL;
 * static const char bus_name[] = "org.freedesktop.Tracker1";
 * static const char tracker_path[] = "/org/freedesktop/Tracker1/Resources";
 * static const char tracker_interface[] = "org.freedesktop.Tracker1.Resources";
 * static const char fdo_bus_name[] = "org.freedesktop.DBus";
 * static const char fdo_interface[] = "org.freedesktop.DBus";
 * static const char fdo_path[] = "/org/freedesktop/DBus"; */

static const char _module_icon[] = "find";

static Evas_Object *_icon_get(Evry_Item *item, Evas *e);

/* static Evry_Type TRACKER_MUSIC;
 * static Evry_Type TRACKER_QUERY;
 * static Evry_Type FILE_LIST;
 * static Evry_Type MPRIS_TRACK; */


static Evry_Plugin *
_browse(Evry_Plugin *plugin, const Evry_Item *item)
{
   Plugin *p = NULL;

   /* if (!CHECK_TYPE(item, TRACKER_QUERY) &&
    *     !CHECK_TYPE(item, TRACKER_MUSIC))
    *   return NULL;
    * 
    * GET_PLUGIN(parent, plugin); */

   /* QUERY_ITEM(it, item);
    * 
    * if (plugin != item->plugin)
    *   return NULL;
    * 
    * p = E_NEW(Plugin, 1);
    * p->base = *plugin;
    * p->base.items = NULL;
    * 
    * p->filter_result = parent->filter_result;
    * p->parent = EINA_TRUE;
    * p->query = it->query;
    * 
    * if (it->match)
    *   p->match = eina_stringshare_add(it->match); */

   return EVRY_PLUGIN(p);
}

static Evry_Plugin *
_begin(Evry_Plugin *plugin, const Evry_Item *item)
{
   Plugin *p;

   GET_PLUGIN(parent, plugin);

   p = E_NEW(Plugin, 1);
   p->base = *plugin;
   p->base.items = NULL;

   /* p->filter_result = parent->filter_result;
    * p->parent = EINA_TRUE;
    * p->query = parent->query;
    * 
    * if (parent->match)
    *   p->match = eina_stringshare_ref(parent->match); */

   return EVRY_PLUGIN(p);
}

static void
_finish(Evry_Plugin *plugin)
{
   GET_PLUGIN(p, plugin);

   EVRY_PLUGIN_ITEMS_CLEAR(p);

   /* EINA_LIST_FREE(p->files, file)
    *   evry->item_free(EVRY_ITEM(file)); */

   IF_RELEASE(p->input);
   
   /* if (p->pnd)
    *   dbus_pending_call_cancel(p->pnd);
    * p->pnd = NULL; */

   E_FREE(p);
}


static int
_fetch(Evry_Plugin *plugin, const char *input)
{
   GET_PLUGIN(p, plugin);


   return !!(p->base.items);
}

/* static void
 * _name_owner_changed(void *data __UNUSED__, DBusMessage *msg)
 * {
 *    DBusError err;
 *    const char *name, *from, *to;
 * 
 *    dbus_error_init(&err);
 *    if (!dbus_message_get_args(msg, &err,
 * 			      DBUS_TYPE_STRING, &name,
 * 			      DBUS_TYPE_STRING, &from,
 * 			      DBUS_TYPE_STRING, &to,
 * 			      DBUS_TYPE_INVALID))
 *      {
 * 	ERR("could not get NameOwnerChanged arguments: %s: %s",
 * 	    err.name, err.message);
 * 	dbus_error_free(&err);
 * 	return;
 *      }
 * 
 *    if (strcmp(name, bus_name) != 0)
 *      return;
 * 
 *    DBG("NameOwnerChanged from=[%s] to=[%s]", from, to);
 * 
 *    if (to[0] == '\0')
 *      dbus_active = EINA_FALSE;
 *    else
 *      dbus_active = EINA_TRUE;
 * } */

/* static void
 * _get_name_owner(void *data __UNUSED__, DBusMessage *msg, DBusError *err)
 * {
 *    DBusMessageIter itr;
 *    int t;
 *    const char *uid;
 * 
 *    pending_get_name_owner = NULL;
 * 
 *    if (dbus_error_is_set(err))
 *      {
 * 	ERR("request name error: %s", err->message);
 * 	return;
 *      }
 * 
 *    if (!dbus_message_iter_init(msg, &itr))
 *      return;
 * 
 *    t = dbus_message_iter_get_arg_type(&itr);
 *    if (t != DBUS_TYPE_STRING)
 *      return;
 * 
 *    dbus_message_iter_get_basic(&itr, &uid);
 * 
 *    if (uid)
 *      dbus_active = EINA_TRUE;
 * 
 *    return;
 * } */

static Evas_Object *
_icon_get(Evry_Item *item, Evas *e)
{

   /* Evas_Object *o = e_icon_add(e);
    * e_icon_file_set(o, thumb_buf);
    * if (o) return o;
    * 
    * if (item->browseable)
    *   return evry->icon_theme_get("folder", e);
    * else
    *   return evry->icon_mime_get(it->mime, e); */

   return NULL;
}


/* static int
 * _cb_action_performed(void *data, int type, void *event)
 * {
 *    Evry_Event_Action_Performed *ev = event;
 *    const char *url;
 * 
 *    if (!dbus_active)
 *      return 1;
 * 
 *    if (CHECK_TYPE(ev->it1, EVRY_TYPE_FILE) &&
 *        (ev->action && !strcmp(ev->action, "Move to Trash")) &&
 *        (url = evry->file_url_get(EVRY_FILE(ev->it1))))
 *      {
 * 	printf("file deleted %s\n", url);
 * 	_send_query(query_remove_file, url, NULL, 1,
 * 		    NULL, NULL);
 *      }
 *    
 *    return 1;
 * } */

static int
_plugins_init(const Evry_API *_api)
{
   Evry_Plugin *p;

   if (evry_module->active)
     return EINA_TRUE;

   evry = _api;

   if (!evry->api_version_check(EVRY_API_VERSION))
     return EINA_FALSE;

   /* conn = e_dbus_bus_get(DBUS_BUS_SESSION);
    * 
    * if (!conn)
    *   return EINA_FALSE;
    * 
    * cb_name_owner_changed = e_dbus_signal_handler_add
    *   (conn, fdo_bus_name, fdo_path, fdo_interface, "NameOwnerChanged",
    *    _name_owner_changed, NULL);
    * 
    * pending_get_name_owner = e_dbus_get_name_owner
    *   (conn, bus_name, _get_name_owner, NULL); */


   /* TRACKER_QUERY = evry->type_register("TRACKER_QUERY");
    * TRACKER_MUSIC = evry->type_register("TRACKER_MUSIC");
    * FILE_LIST     = evry->type_register("FILE_LIST");
    * MPRIS_TRACK   = evry->type_register("MPRIS_TRACK"); */

   /* action_handler = ecore_event_handler_add(EVRY_EVENT_ACTION_PERFORMED,
    * 					    _cb_action_performed, NULL); */

   /* #define FILE_PLUGIN_NEW(_name, _plug_type, _icon, _type, _begin, _finish, _fetch, _query) { \
    *      p = EVRY_PLUGIN_NEW(Plugin, _name, _icon, _type, _begin, _finish, _fetch, NULL); \
    *      GET_PLUGIN(p1, p);							\
    *      p1->query = _query;						\
    *      if (evry->plugin_register(p, _plug_type, _prio++)) {		\
    * 	p->config->min_query = 4;					\
    * 	p->config->top_level = 0; }					\
    *      plugins = eina_list_append(plugins, p); }
    * 
    * 
    *    FILE_PLUGIN_NEW(N_("Tracker"), EVRY_PLUGIN_SUBJECT, "find", EVRY_TYPE_FILE,
    * 		   _begin, _finish, _fetch, query_files);
    * 
    *    FILE_PLUGIN_NEW(N_("Tracker"), EVRY_PLUGIN_OBJECT, "find", EVRY_TYPE_FILE,
    * 		   _begin, _finish, _fetch, query_files); */



   return EINA_TRUE;
}

static void
_plugins_shutdown(void)
{
   Plugin *p;

   if (!evry_module->active) return;

   /* if (conn)
    *   {
    *     e_dbus_signal_handler_del(conn, cb_name_owner_changed);
    *     e_dbus_connection_close(conn);
    *     conn = NULL;
    *   } */

   /* ecore_event_handler_del(action_handler);
    * action_handler = NULL; */

   EINA_LIST_FREE(plugins, p)
     EVRY_PLUGIN_FREE(p);

   evry_module->active = EINA_FALSE;
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

   /* Location of message catalogs for localization */
   snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

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
