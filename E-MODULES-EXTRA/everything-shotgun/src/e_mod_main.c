#include "e.h"
#include "e_mod_main.h"
#include "evry_api.h"

typedef struct _Plugin Plugin;
typedef struct _Contact Contact;

struct _Plugin
{
  Evry_Plugin base;
  Eina_List *contacts;

  Eina_Bool active : 1;
};

struct _Contact
{
  Evry_Item base;

  const char *id;
  const char *icon;
  Eina_Bool *has_icon;
};

static const Evry_API *evry = NULL;
static Evry_Module *evry_module = NULL;
static Eina_List *plugins = NULL;
static Eina_List *actions = NULL;
static E_DBus_Connection *conn = NULL;
static const char DBUS_SHOTGUN_BUS_NAME[] = "org.shotgun";
static const char DBUS_SHOTGUN_LIST[] = "org.shotgun.list";
static const char DBUS_SHOTGUN_CONTACT[] = "org.shotgun.contact";
static const char DBUS_SHOTGUN_PATH[] = "/org/shotgun/remote";

static Evry_Type SHOTGUN_CONTACT;

#define CONTACT(_it)((Contact *)_it)
#define GET_CONTACT(_c, _it) Contact *_c = (Contact *)_it

static void
_item_free(Evry_Item *it)
{ 
   GET_CONTACT(c, it);

   IF_RELEASE(c->id);
   IF_RELEASE(c->icon);
   
   E_FREE(c);
}

static Evas_Object *
_icon_get(Evry_Item *it, Evas *e)
{
   return NULL;
}

static Evry_Plugin *
_inst_new(Evry_Plugin *plugin, const Evry_Item *it)
{
   Plugin *p;
   
   EVRY_PLUGIN_INSTANCE(p, plugin);
   
   return EVRY_PLUGIN(p);
}

static void
_inst_free(Evry_Plugin *plugin)
{
   Evry_Item *it;
   
   GET_PLUGIN(p, plugin);
   EVRY_PLUGIN_ITEMS_CLEAR(p);

   EINA_LIST_FREE(p->contacts, it)
     _item_free(it); 
   
   E_FREE(p);
}

static int
_check_msg(void *data, DBusMessage *reply, DBusError *error)
{
   if (dbus_error_is_set(error))
     {
	DBG("Error: %s - %s\n", error->name, error->message);
	return 0;
     }
   return (dbus_message_get_type(reply) == DBUS_MESSAGE_TYPE_METHOD_RETURN);
}

static void
_dbus_cb_chat_reply(void *data, DBusMessage *reply, DBusError *error)
{
   DBusMessageIter item, array;
   Contact *c;
      
   if (!_check_msg(data, reply, error))
     return;

   printf("message sent\n");

}

static void
_dbus_cb_icon_get(void *data, DBusMessage *reply, DBusError *error)
{
   DBusMessageIter item, array;
   char *id;

   GET_CONTACT(c, data);
   GET_PLUGIN(p, c->base.plugin);
   
   if (!p->active)
     return;
   
   if (!_check_msg(data, reply, error))
     return;

   dbus_message_get_args(reply, error,
			 DBUS_TYPE_STRING, &(c->icon),
			 DBUS_TYPE_INVALID);

   if (c->icon)
     evry->item_changed(EVRY_ITEM(c), 1, 0);

   printf("got icon %s\n", c->icon);

}

static void
_item_new(Plugin *p, char *id)
{
   Contact *c;
   DBusMessage *msg;
   
   c = EVRY_ITEM_NEW(Contact, p, id, NULL, _item_free);
   c->id = eina_stringshare_add(id);
	    
   p->contacts = eina_list_append(p->contacts, c); 

   EVRY_PLUGIN_ITEM_APPEND(p, c);
   
   if (!(msg = dbus_message_new_method_call(DBUS_SHOTGUN_BUS_NAME,
					    DBUS_SHOTGUN_PATH,
					    DBUS_SHOTGUN_CONTACT,
					    "icon")))
     {
	DBG("dbus!\n");
	return;
     }

   dbus_message_append_args(msg,
			    DBUS_TYPE_STRING,&(c->id),
			    DBUS_TYPE_INVALID);

   e_dbus_message_send(conn, msg, _dbus_cb_icon_get, -1, c);

   dbus_message_unref(msg);
}

static void
_dbus_cb_list_get(void *data, DBusMessage *reply, DBusError *error)
{
   DBusMessageIter item, array;
   char *id;
   
   GET_PLUGIN(p, data);
   
   if (!p->active)
     return;
   
   if (!_check_msg(data, reply, error))
     return;

   dbus_message_iter_init(reply, &array);

   if (dbus_message_iter_get_arg_type(&array) == DBUS_TYPE_ARRAY)
     {
	dbus_message_iter_recurse(&array, &item);

	while (dbus_message_iter_get_arg_type(&item) == DBUS_TYPE_STRING)
	  {
	     dbus_message_iter_get_basic (&item, &id);
	     if (id && id[0])
	       _item_new(p, id);

	     dbus_message_iter_next(&item);
	  }
     }
   EVRY_PLUGIN_UPDATE(p, EVRY_UPDATE_ADD);
}

static int
_fetch(Evry_Plugin *plugin, const char *input)
{
   Evry_Item *it;
   DBusMessage *msg;
   
   GET_PLUGIN(p, plugin);

   if (!p->active)
     {
	if (!(msg = dbus_message_new_method_call(DBUS_SHOTGUN_BUS_NAME,
						 DBUS_SHOTGUN_PATH,
						 DBUS_SHOTGUN_LIST,
						 "get_all")))
	  {
	     printf("error fetch\n");
	     return 0;
	  }

	dbus_message_append_args(msg, DBUS_TYPE_INVALID);
	e_dbus_message_send(conn, msg, _dbus_cb_list_get, -1, plugin);
	dbus_message_unref(msg);

	p->active = EINA_TRUE;
     }
   
   return !!(p->base.items);
}

static int
_action_chat(Evry_Action *act)
{
   DBusMessage *msg;
   const char *message;
   int status = 0;
   
   GET_CONTACT(c, act->it1.item);

   if (!act->it2.item)
     return EVRY_ACTION_OTHER;

   if (!(msg = dbus_message_new_method_call(DBUS_SHOTGUN_BUS_NAME,
					    DBUS_SHOTGUN_PATH,
					    DBUS_SHOTGUN_CONTACT,
					    "send_echo")))
     {
	DBG("dbus!\n");
	return EVRY_ACTION_OTHER;
     }

   
   message = act->it2.item->label;
   printf("send  %s to %s\n", message, c->id);

   dbus_message_append_args(msg,
			    DBUS_TYPE_STRING,&(c->id),
			    DBUS_TYPE_STRING,&(message),
			    DBUS_TYPE_UINT32, &(status),
			    DBUS_TYPE_INVALID);

   e_dbus_message_send(conn, msg, _dbus_cb_chat_reply, -1, NULL);

   dbus_message_unref(msg);

   return EVRY_ACTION_FINISHED;
}

static int
_plugins_init(const Evry_API *_api)
{
   Evry_Plugin *plugin;
   Evry_Action *act;
   
   if (evry_module->active)
     return EINA_TRUE;

   evry = _api;
   
   if (!evry->api_version_check(EVRY_API_VERSION))
     return EINA_FALSE;

   if (!(conn = e_dbus_bus_get(DBUS_BUS_SESSION)))
     {
	ERR("could not connect to dbus' session bus");
	return EINA_FALSE;
     }

   SHOTGUN_CONTACT = evry->type_register("SHOTGUN_CONTACT");

   plugin = EVRY_PLUGIN_NEW(Evry_Plugin, N_("Shotgun"), "folder",
			     SHOTGUN_CONTACT,
			    _inst_new, _inst_free, _fetch, NULL);

   evry->plugin_register(plugin, EVRY_PLUGIN_SUBJECT, 1);

   plugins = eina_list_append(plugins, plugin);

   act = EVRY_ACTION_NEW(N_("Write Message"), SHOTGUN_CONTACT, EVRY_TYPE_TEXT, "go-next",
			  _action_chat, NULL);
   evry->action_register(act, 0);
   
   actions = eina_list_append(actions, act); 

   return EINA_TRUE;
}

static void
_plugins_shutdown(void)
{
  Plugin *p;
  Evry_Action *act;
  
  if (!evry_module->active) return;

  if (conn)
    e_dbus_connection_close(conn);

  EINA_LIST_FREE(plugins, p)
    EVRY_PLUGIN_FREE(p);

  EINA_LIST_FREE(actions, act)
    EVRY_ACTION_FREE(act);
  
  evry_module->active = EINA_FALSE;
}


/***************************************************************************/

/* module setup */
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

   EVRY_MODULE_NEW(evry_module, evry, _plugins_init, _plugins_shutdown);

   e_module_delayed_set(m, 1);

  return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   EVRY_MODULE_FREE(evry_module);
   
  return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
  return 1;
}

