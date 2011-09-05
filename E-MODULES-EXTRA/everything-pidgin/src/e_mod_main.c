/*
 *  C Implementation: evry_plug_pidgin
 *
 * Description:
 *
 *
 * Author: Leif Middelschulte <leif.middelschulte@gmail.com>, (C) 2010
 *
 * Copyright: See GPL2
 *
 */

#include "e.h"
#include "e_mod_main.h"
#include "evry_api.h"

//For debugging purposes
static int _evry_plugin_source_pidgin_log_dom = -1;

#undef ERR
#undef DBG

#define ERR(...) EINA_LOG_DOM_ERR(_evry_plugin_source_pidgin_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_evry_plugin_source_pidgin_log_dom, __VA_ARGS__)

#define DBUS_PIDGIN_BUS_NAME "im.pidgin.purple.PurpleService"
#define DBUS_PIDGIN_INTERFACE "im.pidgin.purple.PurpleInterface"
#define DBUS_PIDGIN_PATH "/im/pidgin/purple/PurpleObject"
#define PURPLE_GET_BUDDYLIST "PurpleBlistGetBuddies"
#define PURPLE_GET_NETWORKID "PurpleBuddyGetName"
#define PURPLE_GET_BUDDYACCOUNT "PurpleBuddyGetAccount"
#define PURPLE_GET_BUDDYALIAS "PurpleBuddyGetAlias"
#define PURPLE_GET_BUDDYICONREF "PurpleBuddyIconsFind"
#define PURPLE_GET_BUDDYICONPATH "PurpleBuddyIconGetFullPath"

typedef struct _Buddy_Info Buddy_Info;
typedef struct _Plugin Plugin;

struct _Plugin
{
  Evry_Plugin base;
};

struct _Buddy_Info
{
  Evry_Item base;

  //the buddy's nickname is kept in 'label'-variable of the corresponding evry_item
  int buddyListNumber; //pidgin's internal IDs for buddies
  int accountNr; //pidgin's internal account ID the buddy is associated to
  const char* networkID; //the network's ID for the buddy. e.g. UIN in AOL's ICQ
  int iconReference; //pidgin's internal reference for a icon
  const char *file;
  const char *message;

  int ready;
};

#define BUDDYINFO(_it)((Buddy_Info *)_it)
#define GET_BUDDYINFO(_bi, _it) Buddy_Info * _bi = (Buddy_Info *)_it

static void
cb_itemFree(Evry_Item *item);
static void
getBuddyList();
static void
getBuddyInfo(Buddy_Info *bi, const char *method, void (*cb) (void *data, DBusMessage *reply, DBusError *error));
static void
getBuddyIconReference(Buddy_Info *bi);
static void
getBuddyIconPath(Buddy_Info *bi);
static void
cb_buddyAccount(void *data, DBusMessage *reply, DBusError *error);
static void
cb_networkID(void *data, DBusMessage *reply, DBusError *error);
static void
cb_buddyList(void *data, DBusMessage *reply, DBusError *error);
static void
cb_buddyAlias(void *data, DBusMessage *reply, DBusError *error);
static void
cb_buddyIconReference(void *data, DBusMessage *reply, DBusError *error);
static void
cb_buddyIconPath(void *data, DBusMessage *reply, DBusError *error);
static void
_item_add(Buddy_Info *bi);
static void
_update_list(int async);

static const Evry_API *evry = NULL;
static Evry_Module *evry_module = NULL;
static Eina_List *buddyEveryItems = NULL;
static E_DBus_Connection *conn = NULL;
static int active = 0;
static const char *_input = NULL;
static Evry_Action *act = NULL;
static Evry_Action *act2 = NULL;
static Evry_Action *act3 = NULL;
static const int DEFAULT_CONVERSATION_TYPE = 1;
static Evry_Type PIDGIN_CONTACT;
static const char *buddy_icon_default = NULL;

static Evry_Plugin *plugin_base = NULL;
static Plugin *plug = NULL;

static void
getBuddyList()
{
   DBG("getting a fresh buddyList!");
   DBusMessage *msg;

   if (!(msg = dbus_message_new_method_call(DBUS_PIDGIN_BUS_NAME,
					    DBUS_PIDGIN_PATH,
					    DBUS_PIDGIN_INTERFACE,
					    PURPLE_GET_BUDDYLIST)))
     {
	DBG("Couldn't call pidgin's method via dbus!\n");
     }

   dbus_message_append_args(msg, DBUS_TYPE_INVALID);
   e_dbus_message_send(conn, msg, cb_buddyList, -1, NULL);
   dbus_message_unref(msg);

}

static void
getBuddyInfo(Buddy_Info* bi, const char *method,
	     void (*cb) (void *data, DBusMessage *reply, DBusError *error))
{
   DBusMessage *msg;

   if (!(msg = dbus_message_new_method_call(DBUS_PIDGIN_BUS_NAME,
					    DBUS_PIDGIN_PATH,
					    DBUS_PIDGIN_INTERFACE,
					    method)))
     return;

   dbus_message_append_args(msg,
			    DBUS_TYPE_INT32, &(bi->buddyListNumber),
			    DBUS_TYPE_INVALID);

   e_dbus_message_send(conn, msg, cb, -1, bi);
   dbus_message_unref(msg);
}

static void
getBuddyIconReference(Buddy_Info *bi)
{
   //get associated icon reference in order to get its path

   DBusMessage *msg;

   if (!(msg = dbus_message_new_method_call(DBUS_PIDGIN_BUS_NAME,
					    DBUS_PIDGIN_PATH,
					    DBUS_PIDGIN_INTERFACE,
					    PURPLE_GET_BUDDYICONREF)))
     return;

   dbus_message_append_args(msg,
			    DBUS_TYPE_INT32, &(bi->accountNr),
			    DBUS_TYPE_STRING,&(bi->networkID),
			    DBUS_TYPE_INVALID);
   e_dbus_message_send(conn, msg, cb_buddyIconReference, -1, bi);
   dbus_message_unref(msg);
}

static void
getBuddyIconPath(Buddy_Info *bi)
{
   //get associated icon's entire path in order to display it
   DBusMessage *msg;

   if (!(msg = dbus_message_new_method_call(DBUS_PIDGIN_BUS_NAME,
					    DBUS_PIDGIN_PATH,
					    DBUS_PIDGIN_INTERFACE,
					    PURPLE_GET_BUDDYICONPATH)))
     return;

   dbus_message_append_args(msg,
			    DBUS_TYPE_INT32, &(bi->iconReference),
			    DBUS_TYPE_INVALID);
   e_dbus_message_send(conn, msg, cb_buddyIconPath, -1, bi);
   dbus_message_unref(msg);
}

static int
check_msg(void *data, DBusMessage *reply, DBusError *error)
{
   if (dbus_error_is_set(error))
     {
	DBG("Error: %s - %s\n", error->name, error->message);
	return 0;
     }
   return (dbus_message_get_type(reply) == DBUS_MESSAGE_TYPE_METHOD_RETURN);
}

static void
cb_buddyList(void *data, DBusMessage *reply, DBusError *error)
{
   DBusMessageIter itr, arr;
   Buddy_Info *bi;

   if (!active || !check_msg(data, reply, error)) return;

   dbus_message_iter_init(reply, &itr);
   dbus_message_iter_recurse(&itr, &arr);

   do
     {
	bi = EVRY_ITEM_NEW(Buddy_Info, plug, NULL, NULL, cb_itemFree);
	if (!bi) continue;

	dbus_message_iter_get_basic(&arr, (dbus_int32_t*) &(bi->buddyListNumber));
	bi->iconReference = -1;

	//get associated account number in order to open chat windows
	getBuddyInfo(bi, PURPLE_GET_BUDDYACCOUNT, cb_buddyAccount);
	//get the network's ID for our buddy in order to open a chatwindow
	getBuddyInfo(bi, PURPLE_GET_NETWORKID, cb_networkID);
	//get buddy's alias to show in 'everything'
	getBuddyInfo(bi, PURPLE_GET_BUDDYALIAS, cb_buddyAlias);

	buddyEveryItems = eina_list_append(buddyEveryItems, bi);

	dbus_message_iter_next(&arr);

     }
   while (dbus_message_iter_has_next(&arr));
}

static void
cb_buddyAccount(void *data, DBusMessage *reply, DBusError *error)
{
   if (!active || !check_msg(data, reply, error)) return;

   GET_BUDDYINFO(bi, data);

   dbus_message_get_args(reply, error,
			 DBUS_TYPE_INT32, (dbus_int32_t*) &(bi->accountNr),
			 DBUS_TYPE_INVALID);
   _item_add(bi);
}

static void
cb_networkID(void *data, DBusMessage *reply, DBusError *error)
{
   const char* tmpString = NULL;

   if (!active || !check_msg(data, reply, error)) return;

   GET_BUDDYINFO(bi, data);

   dbus_message_get_args(reply, error,
			 DBUS_TYPE_STRING, &tmpString,
			 DBUS_TYPE_INVALID);

   bi->networkID = eina_stringshare_add(tmpString);

   _item_add(bi);
}

static void
cb_buddyAlias(void *data, DBusMessage *reply, DBusError *error)
{
   const char* tmpString = NULL;

   if (!active || !check_msg(data, reply, error)) return;

   GET_BUDDYINFO(bi, data);

   dbus_message_get_args(reply, error,
			 DBUS_TYPE_STRING, &tmpString,
			 DBUS_TYPE_INVALID);

   EVRY_ITEM_LABEL_SET(bi, tmpString);

   _item_add(bi);
}

static void
cb_buddyIconReference(void *data, DBusMessage *reply, DBusError *error)
{
   if (!active || !check_msg(data, reply, error)) return;

   GET_BUDDYINFO(bi, data);

   if (dbus_message_get_args(reply, error,
			     DBUS_TYPE_INT32, &(bi->iconReference),
			     DBUS_TYPE_INVALID))
     {
	if (bi->iconReference > 0)
	  getBuddyIconPath(bi);
	else
	  _item_add(bi);
     }
}

static void
cb_buddyIconPath(void *data, DBusMessage *reply, DBusError *error)
{
   const char* tmpString = NULL;

   if (!active || !check_msg(data, reply, error)) return;

   GET_BUDDYINFO(bi, data);

   dbus_message_get_args(reply, error,
			 DBUS_TYPE_STRING, &tmpString,
			 DBUS_TYPE_INVALID);

   /* printf("icon %s %s\n", EVRY_ITEM(bi)->label, tmpString); */

   EVRY_ITEM_ICON_SET(bi, tmpString);

   _item_add(bi);
}

static void
_item_add(Buddy_Info *bi)
{
   if ((!EVRY_ITEM(bi)->label ||
	EVRY_ITEM(bi)->label[0] == '\0') ||
       (!bi->networkID || bi->networkID[0] == '\0') ||
       (bi->accountNr == 0))
     return;

   if (bi->iconReference < 0)
     {
	getBuddyIconReference(bi);
	return;
     }

   if (!EVRY_ITEM(bi)->icon)
     EVRY_ITEM(bi)->icon = eina_stringshare_ref(buddy_icon_default);

   bi->ready = 1;

   _update_list(1);
}

static Evry_Plugin *
_begin(Evry_Plugin *plugin, const Evry_Item *item __UNUSED__)
{
   if (plug) return NULL;
   
   EVRY_PLUGIN_INSTANCE(plug, plugin);
   
   return EVRY_PLUGIN(plug);
}

static void
_cleanup(Evry_Plugin *plugin __UNUSED__)
{
   Buddy_Info *bi;

   active = 0;

   EVRY_PLUGIN_ITEMS_CLEAR(plug);

   EINA_LIST_FREE(buddyEveryItems, bi)
     EVRY_ITEM_FREE(bi);

   E_FREE(plug);
   plug = NULL;
}

static void
cb_itemFree(Evry_Item *it)
{
   GET_BUDDYINFO(bi, it);

   IF_RELEASE(bi->networkID);
   IF_RELEASE(bi->file);
   IF_RELEASE(bi->message);

   E_FREE(bi);
}

static void
_update_list(int async)
{
   Eina_List *l;
   Buddy_Info *bi;

   EVRY_PLUGIN_ITEMS_CLEAR(plug);

   EINA_LIST_FOREACH(buddyEveryItems, l, bi)
     if (!bi->ready) break;

   if (l) return;

   if (!_input)
     {
	EINA_LIST_FOREACH(buddyEveryItems, l, bi)
	  EVRY_PLUGIN_ITEM_APPEND(plug, bi);
     }
   else
     {
	EVRY_PLUGIN_ITEMS_ADD(plug, buddyEveryItems, _input, 0, 0);
     }

   if (async)
     EVRY_PLUGIN_UPDATE(plug, EVRY_UPDATE_ADD);
}


static int
_fetch(Evry_Plugin *plugin __UNUSED__, const char *input)
{
   IF_RELEASE(_input);
   if (input)
     _input = eina_stringshare_add(input);

   if (!active)
     {
	getBuddyList();
	active = 1;
	return 0;
     }

   _update_list(0);

   return 1;
}

static void
cb_sendFile(void *data, DBusMessage *reply, DBusError *error)
{
   DBusMessage *msg;
   int connection;
   GET_BUDDYINFO(bi, data);

   if (!check_msg(data, reply, error)) goto end;

   dbus_message_get_args(reply, error,
			 DBUS_TYPE_INT32, (dbus_int32_t*) &(connection),
			 DBUS_TYPE_INVALID);


   if (!(msg = dbus_message_new_method_call(DBUS_PIDGIN_BUS_NAME,
					    DBUS_PIDGIN_PATH,
					    DBUS_PIDGIN_INTERFACE,
					    "ServSendFile")))
     goto end;

   dbus_message_append_args(msg,
			    DBUS_TYPE_INT32,  &(connection),
			    DBUS_TYPE_STRING, &(bi->networkID),
			    DBUS_TYPE_STRING, &(bi->file),
			    DBUS_TYPE_INVALID);

   e_dbus_message_send(conn, msg, NULL, -1, NULL);
   dbus_message_unref(msg);

 end:
   EVRY_ITEM_FREE(bi);
}

static int
_action_send(Evry_Action *act)
{
   DBusMessage *msg;
   const char *path;

   GET_BUDDYINFO(bi, act->it1.item);

   if (!(msg = dbus_message_new_method_call(DBUS_PIDGIN_BUS_NAME,
					    DBUS_PIDGIN_PATH,
					    DBUS_PIDGIN_INTERFACE,
					    "PurpleAccountGetConnection")))
     return 0;

   IF_RELEASE(bi->message);
   if (act->it2.item)
     {
	GET_FILE(file, act->it2.item);

	if ((path = evry->file_path_get(file)))
	  bi->file = eina_stringshare_ref(path);
     }

   /* when action returns and everything hides items will be freed,
      but we need to wait for the connection.. */
   EVRY_ITEM_REF(bi);

   dbus_message_append_args(msg,
			    DBUS_TYPE_INT32, &(bi->accountNr),
			    DBUS_TYPE_INVALID);

   e_dbus_message_send(conn, msg, cb_sendFile, -1, bi);
   dbus_message_unref(msg);

   return EVRY_ACTION_FINISHED;
}


static void
cb_sendMessage(void *data, DBusMessage *reply, DBusError *error)
{
   DBusMessage *msg;
   int imData;
   GET_BUDDYINFO(bi, data);

   if (!check_msg(data, reply, error)) goto end;

   dbus_message_get_args(reply, error,
			 DBUS_TYPE_INT32, (dbus_int32_t*) &(imData),
			 DBUS_TYPE_INVALID);


   if (!(msg = dbus_message_new_method_call(DBUS_PIDGIN_BUS_NAME,
					    DBUS_PIDGIN_PATH,
					    DBUS_PIDGIN_INTERFACE,
					    "PurpleConvImSend")))
     goto end;

   dbus_message_append_args(msg,
			    DBUS_TYPE_INT32,  &(imData),
			    DBUS_TYPE_STRING, &(bi->message),
			    DBUS_TYPE_INVALID);

   e_dbus_message_send(conn, msg, NULL, -1, NULL);
   dbus_message_unref(msg);

 end:
   EVRY_ITEM_FREE(bi);
}

static void
cb_getImData(void *data, DBusMessage *reply, DBusError *error)
{
   DBusMessage *msg;
   int conversation;
   GET_BUDDYINFO(bi, data);

   if (!check_msg(data, reply, error)) goto end;

   if (!bi->message)
     goto end;

   dbus_message_get_args(reply, error,
			 DBUS_TYPE_INT32, (dbus_int32_t*) &(conversation),
			 DBUS_TYPE_INVALID);


   if (!(msg = dbus_message_new_method_call(DBUS_PIDGIN_BUS_NAME,
					    DBUS_PIDGIN_PATH,
					    DBUS_PIDGIN_INTERFACE,
					    "PurpleConversationGetImData")))
     goto end;

   dbus_message_append_args(msg,
			    DBUS_TYPE_INT32, &(conversation),
			    DBUS_TYPE_INVALID);

   e_dbus_message_send(conn, msg, cb_sendMessage, -1, bi);
   dbus_message_unref(msg);
   return;

 end:
   EVRY_ITEM_FREE(bi);
}

static int
_action_chat(Evry_Action *act)
{
   DBusMessage *msg;
   GET_BUDDYINFO(bi, act->it1.item);

   if (!(msg = dbus_message_new_method_call(DBUS_PIDGIN_BUS_NAME,
					    DBUS_PIDGIN_PATH,
					    DBUS_PIDGIN_INTERFACE,
					    "PurpleConversationNew")))
     {
	DBG("Couldn't call pidgin's method via dbus!\n");
	return EVRY_ACTION_OTHER;
     }

   /* when action returns and everything hides items will be freed,
      but we need to wait for the connection.. */
   IF_RELEASE(bi->message);
   if (act->it2.item)
     bi->message = eina_stringshare_ref(act->it2.item->label);

   EVRY_ITEM_REF(bi);

   dbus_message_append_args(msg,
			    DBUS_TYPE_INT32, &DEFAULT_CONVERSATION_TYPE,
			    DBUS_TYPE_INT32, &(bi->accountNr),
			    DBUS_TYPE_STRING,&(bi->networkID),
			    DBUS_TYPE_INVALID);

   e_dbus_message_send(conn, msg, cb_getImData, -1, bi);

   dbus_message_unref(msg);

   return EVRY_ACTION_FINISHED;
}

static int
_plugins_init(const Evry_API *_api)
{
   evry = _api;

   if (!evry->api_version_check(EVRY_API_VERSION))
     return EINA_FALSE;

   if (!(conn = e_dbus_bus_get(DBUS_BUS_SESSION)))
     {
	EINA_LOG_CRIT("could not connect to dbus' session bus");
	eina_log_domain_unregister(_evry_plugin_source_pidgin_log_dom);
	return EINA_FALSE;
     }

   PIDGIN_CONTACT = evry->type_register("PIDGIN_CONTACT");

   buddy_icon_default = eina_stringshare_add("emblem-people");

   plugin_base = EVRY_PLUGIN_BASE("Pidgin", NULL, PIDGIN_CONTACT,
				  _begin, _cleanup, _fetch);
   
   evry->plugin_register(plugin_base, EVRY_PLUGIN_SUBJECT, 1);

   act = EVRY_ACTION_NEW("Chat", PIDGIN_CONTACT, 0, "go-next",
			 _action_chat, NULL);

   act2 = EVRY_ACTION_NEW("Send File", PIDGIN_CONTACT, EVRY_TYPE_FILE, NULL,
			  _action_send, NULL);

   act3 = EVRY_ACTION_NEW("Write Message", PIDGIN_CONTACT, EVRY_TYPE_TEXT, "go-next",
			  _action_chat, NULL);

   evry->action_register(act, 0);
   evry->action_register(act2, 1);
   evry->action_register(act3, 1);

   return EINA_TRUE;
}

static void
_plugins_shutdown(void)
{
   if (conn)
     e_dbus_connection_close(conn);

   EVRY_PLUGIN_FREE(plugin_base);

   evry->action_free(act);
   evry->action_free(act2);
   evry->action_free(act3);

   IF_RELEASE(buddy_icon_default);
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

   if (_evry_plugin_source_pidgin_log_dom < 0)
     {
	_evry_plugin_source_pidgin_log_dom =
	  eina_log_domain_register("evry plugin source pidgin", NULL);

	if (_evry_plugin_source_pidgin_log_dom < 0)
	  {
	     EINA_LOG_CRIT( "could not register log domain 'evry plugin source pidgin'");
	     return NULL;
	  }
     }

   EVRY_MODULE_NEW(evry_module, evry, _plugins_init, _plugins_shutdown);

   e_module_delayed_set(m, 1);

   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   EVRY_MODULE_FREE(evry_module);
   
   eina_log_domain_unregister(_evry_plugin_source_pidgin_log_dom);
   _evry_plugin_source_pidgin_log_dom = -1;

   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   return 1;
}

/***************************************************************************/
