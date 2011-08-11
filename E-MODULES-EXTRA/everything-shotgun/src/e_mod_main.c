#include "e.h"
#include "e_mod_main.h"

typedef struct _Plugin Plugin;

struct _Plugin
{
  Evry_Plugin base;
  Eina_List *contacts;
  const char *input;
  Eina_Bool active : 1;
  Eet_File *images;
};

const Evry_API *evry = NULL;
Evry_Type SHOTGUN_CONTACT;
Evry_Type SHOTGUN_MESSAGE;
Eina_List *messages = NULL;
char *theme_file = NULL;
int SHOTGUN_EVENT_MESSAGE_ADD;

static Evry_Module *evry_module = NULL;
static Eina_List *plugins = NULL;
static Eina_List *actions = NULL;
static E_DBus_Connection *conn = NULL;
static const char DBUS_SHOTGUN_BUS_NAME[] = "org.shotgun";
static const char DBUS_SHOTGUN_LIST[]     = "org.shotgun.list";
static const char DBUS_SHOTGUN_CONTACT[]  = "org.shotgun.contact";
static const char DBUS_SHOTGUN_CORE[]     = "org.shotgun.core";
static const char DBUS_SHOTGUN_PATH[]     = "/org/shotgun/remote";
static E_DBus_Signal_Handler *_dbus_signal_new_msg = NULL;
static E_DBus_Signal_Handler *_dbus_signal_new_msg_self = NULL;

static void _add_message(int self, const char *contact, const char *message);


static void
_item_free(Evry_Item *it)
{
   GET_CONTACT(c, it);

   IF_RELEASE(c->id);
   IF_RELEASE(c->icon);

   if (c->o_icon)
     evas_object_del(c->o_icon);
   
   E_FREE(c);
}

static Evry_Plugin *
_inst_new(Evry_Plugin *plugin, const Evry_Item *it)
{
   Plugin *p;
   char buf[4096];

   EVRY_PLUGIN_INSTANCE(p, plugin);

   eet_init();
   snprintf(buf, sizeof(buf), "%s/.config/shotgun/shotgun.eet", e_user_homedir_get());
   p->images = eet_open(buf, EET_FILE_MODE_READ);
   if (!p->images) ERR("Could not open image cache file!");

   return EVRY_PLUGIN(p);
}

static void
_inst_free(Evry_Plugin *plugin)
{
   Evry_Item *it;

   GET_PLUGIN(p, plugin);

   EVRY_PLUGIN_ITEMS_CLEAR(p);
   IF_RELEASE(p->input);
   EINA_LIST_FREE(p->contacts, it)
     _item_free(it);

   if (p->images) eet_close(p->images);
   eet_shutdown();

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
}

static Evas_Object *
_icon_get(Evry_Item *it, Evas *e)
{
   Evas_Object *o = NULL;
   
   GET_CONTACT(c, it);
   GET_PLUGIN(p, it->plugin);

   if (c->icon)
     {
	size_t size;
	unsigned char *img;

	img = eet_read(p->images, c->icon, (int*)&size);
	if (img)
	  {
	     o = evas_object_image_filled_add(e);
	     evas_object_size_hint_aspect_set(o, EVAS_ASPECT_CONTROL_BOTH, 1, 1);
	     evas_object_image_memfile_set(o, img, size, NULL, NULL);

	     if (evas_object_image_load_error_get(o) != EVAS_LOAD_ERROR_NONE)
	       {
		  evas_object_del(o);
		  o = NULL;
		  IF_RELEASE(c->icon);
	       }
	     free(img);
	  }
     }

   if(!o)
     {
	o = edje_object_add(e);
	edje_object_file_set(o, theme_file, "contact_icon");
     }
   return o;
}

static void
_dbus_cb_icon_get(void *data, DBusMessage *reply, DBusError *error)
{
   DBusMessageIter item, array;
   char *icon;

   GET_CONTACT(c, data);
   GET_PLUGIN(p, c->base.plugin);

   if (!p->active)
     return;

   if (!_check_msg(data, reply, error))
     return;

   dbus_message_get_args(reply, error,
			 DBUS_TYPE_STRING, &(icon),
			 DBUS_TYPE_INVALID);
   if (icon)
     {
	c->icon = eina_stringshare_add(icon);
	evry->item_changed(EVRY_ITEM(c), 1, 0);
     }
}

static void
_item_new(Plugin *p, char *id)
{
   Contact *c;
   DBusMessage *msg;

   c = EVRY_ITEM_NEW(Contact, p, id, _icon_get, _item_free);
   c->id = eina_stringshare_add(id);

   p->contacts = eina_list_append(p->contacts, c);

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

   EVRY_PLUGIN_ITEMS_CLEAR(p);
   EVRY_PLUGIN_ITEMS_ADD(p, p->contacts, p->input, 0, 0);

   EVRY_PLUGIN_UPDATE(p, EVRY_UPDATE_ADD);
}

static int
_fetch(Evry_Plugin *plugin, const char *input)
{
   Evry_Item *it;
   DBusMessage *msg;

   GET_PLUGIN(p, plugin);

   IF_RELEASE(p->input);
   p->input = eina_stringshare_add(input);

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

   EVRY_PLUGIN_ITEMS_CLEAR(p);
   EVRY_PLUGIN_ITEMS_ADD(p, p->contacts, input, 0, 0);

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

   _add_message(1, c->id, message);

   return EVRY_ACTION_CLEAR;
}

static void
_add_message(int self, const char *contact, const char *message)
{
   Message *m;
   char *s;

   if (!contact || !message)
     return;

   m = E_NEW(Message, 1);
   if ((s = strchr(contact, '/')))
     m->contact = eina_stringshare_add_length(contact, s - contact);
   else
     m->contact = eina_stringshare_add(contact);

   m->msg = eina_stringshare_add(message);

   printf("%d got %s from %s\n", self, message, m->contact);

   m->self = self;
   messages = eina_list_append(messages, m);

   if (eina_list_count(messages) > MAX_HISTORY)
     {
	m = eina_list_data_get(messages);
	messages = eina_list_remove_list(messages, messages);
	eina_stringshare_del(m->contact);
	eina_stringshare_del(m->msg);
	E_FREE(m);
     }

   ecore_event_add(SHOTGUN_EVENT_MESSAGE_ADD, NULL, NULL, NULL);
}

static void
_dbus_cb_signal_new_msg(void *data, DBusMessage *msg)
{
   char *contact, *message;

   dbus_message_get_args(msg, NULL,
			 DBUS_TYPE_STRING, &(contact),
			 DBUS_TYPE_STRING, &(message),
			 DBUS_TYPE_INVALID);
   _add_message(0, contact, message);
}

static void
_dbus_cb_signal_new_msg_self(void *data, DBusMessage *msg)
{
   char *contact, *message;

   dbus_message_get_args(msg, NULL,
			 DBUS_TYPE_STRING, &(contact),
			 DBUS_TYPE_STRING, &(message),
			 DBUS_TYPE_INVALID);
   _add_message(1, contact, message);
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

   SHOTGUN_EVENT_MESSAGE_ADD = ecore_event_type_new();

   SHOTGUN_CONTACT = evry->type_register("SHOTGUN_CONTACT");
   SHOTGUN_MESSAGE = evry->type_register("SHOTGUN_MESSAGE");

   plugin = EVRY_PLUGIN_NEW(Evry_Plugin, N_("Shotgun"), "folder",
			    SHOTGUN_CONTACT,
			    _inst_new, _inst_free, _fetch, NULL);

   evry->plugin_register(plugin, EVRY_PLUGIN_SUBJECT, 1);

   plugins = eina_list_append(plugins, plugin);

   act = EVRY_ACTION_NEW(N_("Write Message"), SHOTGUN_CONTACT, SHOTGUN_MESSAGE,
			 "go-next", _action_chat, NULL);
   evry->action_register(act, 0);

   actions = eina_list_append(actions, act);

   _dbus_signal_new_msg = e_dbus_signal_handler_add
     (conn, DBUS_SHOTGUN_BUS_NAME, DBUS_SHOTGUN_PATH, DBUS_SHOTGUN_CORE,
      "new_msg", _dbus_cb_signal_new_msg, NULL);

   _dbus_signal_new_msg_self = e_dbus_signal_handler_add
     (conn, DBUS_SHOTGUN_BUS_NAME, DBUS_SHOTGUN_PATH, DBUS_SHOTGUN_CORE,
      "new_msg_self", _dbus_cb_signal_new_msg_self, NULL);

   evry_plug_msg_init();

   return EINA_TRUE;
}

static void
_plugins_shutdown(void)
{
   Plugin *p;
   Evry_Action *act;
   Message *m;

   if (!evry_module->active) return;

   if (conn)
     {
	e_dbus_connection_close(conn);
	e_dbus_signal_handler_del(conn, _dbus_signal_new_msg);
	e_dbus_signal_handler_del(conn, _dbus_signal_new_msg_self);
     }

   EINA_LIST_FREE(plugins, p)
     EVRY_PLUGIN_FREE(p);

   EINA_LIST_FREE(actions, act)
     EVRY_ACTION_FREE(act);

   evry_plug_msg_shutdown();

   EINA_LIST_FREE(messages, m)
     {
	eina_stringshare_del(m->contact);
	eina_stringshare_del(m->msg);
	E_FREE(m);
     }

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

   snprintf(buf, sizeof(buf), "%s/e-module.edj", e_module_dir_get(m));
   theme_file = strdup(buf);

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

   if (theme_file) free(theme_file);

   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   return 1;
}



