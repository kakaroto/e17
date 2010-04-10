/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

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

/* #include <stdio.h>
 * #include <limits.h>
 * #include "e.h" */

#include "Evry.h"
#include "e_mod_main.h"
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

typedef struct buddyInfo_s buddyInfo;
struct buddyInfo_s
{
  //the buddy's nickname is kept in 'label'-variable of the corresponding evry_item
  int buddyListNumber; //pidgin's internal IDs for buddies
  int accountNr; //pidgin's internal account ID the buddy is associated to
  const char* networkID; //the network's ID for the buddy. e.g. UIN in AOL's ICQ
  int iconReference; //pidgin's internal reference for a icon
  const char* iconPath; //real path to the icon on the filesystem
  int ready;
};
const static int DEFAULT_CONVERSATION_TYPE = 1;

static void
_shutdown(void);

static void
cb_itemFree(Evry_Item *item);
static void
getBuddyList();
static void
getBuddyInfo(Evry_Item* item, const char *method, void (*cb) (void *data, DBusMessage *reply, DBusError *error));
static void
getBuddyIconReference(Evry_Item* item);
static void
getBuddyIconPath(Evry_Item* item);
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
_item_add(Evry_Item* item);
static void
_update_list();


static Evry_Plugin *plug = NULL;
static Eina_List *buddyEveryItems = NULL;
static E_DBus_Connection *conn = NULL;
static int active = 0;
static const char *_input = NULL;
static Evry_Action *act = NULL;
static Evry_Action *act2 = NULL;
static Evry_Action *act3 = NULL;

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
getBuddyInfo(Evry_Item* item, const char *method,
	     void (*cb) (void *data, DBusMessage *reply, DBusError *error))
{
  buddyInfo* info = item->data;
  DBusMessage *msg;

  if (!(msg = dbus_message_new_method_call(DBUS_PIDGIN_BUS_NAME,
					   DBUS_PIDGIN_PATH,
					   DBUS_PIDGIN_INTERFACE,
					   method)))
    return;
      
  dbus_message_append_args(msg,
			   DBUS_TYPE_INT32, &(info->buddyListNumber),
			   DBUS_TYPE_INVALID);
  
  e_dbus_message_send(conn, msg, cb, -1, item);
  dbus_message_unref(msg);
}

static void
getBuddyIconReference(Evry_Item* item)
{
  //get associated icon reference in order to get its path
  buddyInfo* info = item->data;
  DBusMessage *msg;

  if (!(msg = dbus_message_new_method_call(DBUS_PIDGIN_BUS_NAME,
					   DBUS_PIDGIN_PATH,
					   DBUS_PIDGIN_INTERFACE,
					   PURPLE_GET_BUDDYICONREF)))
    return;
	
  dbus_message_append_args(msg,
			   DBUS_TYPE_INT32, &(info->accountNr),
			   DBUS_TYPE_STRING,&(info->networkID),
			   DBUS_TYPE_INVALID);
  e_dbus_message_send(conn, msg, cb_buddyIconReference, -1, item);
  dbus_message_unref(msg);
}

static void
getBuddyIconPath(Evry_Item* item)
{
  //get associated icon's entire path in order to display it
  buddyInfo* info = item->data;
  DBusMessage *msg;  

  if (!(msg = dbus_message_new_method_call(DBUS_PIDGIN_BUS_NAME,
					   DBUS_PIDGIN_PATH,
					   DBUS_PIDGIN_INTERFACE,
					   PURPLE_GET_BUDDYICONPATH)))
    return;
	
  dbus_message_append_args(msg,
			   DBUS_TYPE_INT32, &(info->iconReference),
			   DBUS_TYPE_INVALID);
  e_dbus_message_send(conn, msg, cb_buddyIconPath, -1, item);
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
  Evry_Item* item = NULL;
  buddyInfo *bi;

  if (!active || !check_msg(data, reply, error)) return;
  
  dbus_message_iter_init(reply, &itr);
  dbus_message_iter_recurse(&itr, &arr);

  do
    {
      //get and initialize new memory
      if (!(item = evry_item_new(NULL, plug, NULL, cb_itemFree)) ||
	  !(bi = E_NEW(buddyInfo, 1)))
	break;
      
      dbus_message_iter_get_basic(&arr, (dbus_int32_t*) &(bi->buddyListNumber));
      item->data = bi;
      bi->iconReference = -1;

      //get associated account number in order to open chat windows
      getBuddyInfo(item, PURPLE_GET_BUDDYACCOUNT, cb_buddyAccount);
      //get the network's ID for our buddy in order to open a chatwindow
      getBuddyInfo(item, PURPLE_GET_NETWORKID, cb_networkID);
      //get buddy's alias to show in 'everything'
      getBuddyInfo(item, PURPLE_GET_BUDDYALIAS, cb_buddyAlias);

      buddyEveryItems = eina_list_append(buddyEveryItems, item);
	  
      dbus_message_iter_next(&arr);

    }
  while (dbus_message_iter_has_next(&arr));
}

static void
cb_buddyAccount(void *data, DBusMessage *reply, DBusError *error)
{
  if (!active || !check_msg(data, reply, error)) return;
  
  buddyInfo* info = EVRY_ITEM(data)->data;

  dbus_message_get_args(reply, error,
			DBUS_TYPE_INT32, (dbus_int32_t*) &(info->accountNr),
			DBUS_TYPE_INVALID);
  _item_add(data);
}

static void
cb_networkID(void *data, DBusMessage *reply, DBusError *error)
{
  if (!active || !check_msg(data, reply, error)) return;

  buddyInfo* info = EVRY_ITEM(data)->data;
  const char* tmpString = NULL;

  dbus_message_get_args(reply, error,
			DBUS_TYPE_STRING, &tmpString,
			DBUS_TYPE_INVALID);

  info->networkID = eina_stringshare_add(tmpString);

  _item_add(data);
}

static void
cb_buddyAlias(void *data, DBusMessage *reply, DBusError *error)
{
  if (!active || !check_msg(data, reply, error)) return;

  Evry_Item* item = EVRY_ITEM(data);
  const char* tmpString = NULL;

  dbus_message_get_args(reply, error,
			DBUS_TYPE_STRING, &tmpString,
			DBUS_TYPE_INVALID);

  item->label = eina_stringshare_add(tmpString);

  _item_add(item);
}

static void
cb_buddyIconReference(void *data, DBusMessage *reply, DBusError *error)
{
  if (!active || !check_msg(data, reply, error)) return;
  
  buddyInfo* info = EVRY_ITEM(data)->data;
  
  if (dbus_message_get_args(reply, error,
			    DBUS_TYPE_INT32, &(info->iconReference),
			    DBUS_TYPE_INVALID))
    {
      if (info->iconReference > 0)
	getBuddyIconPath(data);
      else
	_item_add(data);
    }
}

static void
cb_buddyIconPath(void *data, DBusMessage *reply, DBusError *error)
{
  if (!active || !check_msg(data, reply, error)) return;

  buddyInfo* info = EVRY_ITEM(data)->data;
  const char* tmpString = NULL;

  dbus_message_get_args(reply, error,
			DBUS_TYPE_STRING, &tmpString,
			DBUS_TYPE_INVALID);

  info->iconPath = eina_stringshare_add(tmpString);
  
  _item_add(data);
}

static void
_item_add(Evry_Item* item)
{
  buddyInfo* info = ((buddyInfo*) item->data);
  if ((!item->label || item->label[0] == '\0') ||
      (!info->networkID || info->networkID[0] == '\0') ||
      (info->accountNr == 0))
    return;

  if (info->iconReference < 0)
    {
      getBuddyIconReference(item);
      info->ready = 1;
    }
  else
    {
      _update_list();
      evry_plugin_async_update(plug, EVRY_ASYNC_UPDATE_ADD);
    }
}

static Evry_Plugin *
_begin(Evry_Plugin *p, const Evry_Item *item)
{
  return p;
}

static void
_cleanup(Evry_Plugin *p)
{
  /* free instances */
  active = 0;
  Evry_Item *it;
  
  EINA_LIST_FREE(buddyEveryItems, it)
    evry_item_free(it);

  EVRY_PLUGIN_ITEMS_CLEAR(p);  
}

Evas_Object *
_icon_get(Evry_Plugin *p, const Evry_Item *it, Evas *e)
{
  if (((buddyInfo*) it->data)->iconPath != NULL)
    {
      Evas_Object* obj = evas_object_image_filled_add(e);
      evas_object_image_file_set(obj, ((buddyInfo*) it->data)->iconPath, NULL);
      return obj;
    }
  else
    {
      return NULL;
    }
}

static void
cb_itemFree(Evry_Item *item)
{
  buddyInfo* info;
  info = ((buddyInfo*) item->data);

  eina_stringshare_del(info->networkID);
  eina_stringshare_del(info->iconPath);
  E_FREE(info);
  E_FREE(item);
}
  
static void
_update_list()
{
  Evry_Item *it;
  Eina_List *l;

  EVRY_PLUGIN_ITEMS_CLEAR(plug);
  
  EINA_LIST_FOREACH(buddyEveryItems, l, it)
    {
      buddyInfo* info;
      info = (buddyInfo*) it->data;

      if (!info->ready || info->iconReference < 0)
	continue;

      if (_input && !(it->fuzzy_match = evry_fuzzy_match(it->label, _input)))
	continue;

      EVRY_PLUGIN_ITEM_APPEND(plug, it);
    }
  
  plug->items = evry_fuzzy_match_sort(plug->items);
}


static int
_fetch(Evry_Plugin *p, const char *input)
{
  _input = input;

  if (!active)
    {
      getBuddyList();
      active = 1;
      return 0;
    }

  _update_list();

  return 1;
}

static void
cb_sendFile(void *data, DBusMessage *reply, DBusError *error)
{
  DBusMessage *msg;
  int connection;
  Evry_Action *act = data;
  buddyInfo* info = act->item1->data;
  ITEM_FILE(file, act->item2);

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
			   DBUS_TYPE_INT32, &(connection),
			   DBUS_TYPE_STRING, &(info->networkID),
			   DBUS_TYPE_STRING, &(file->path),			    
			   DBUS_TYPE_INVALID);

  e_dbus_message_send(conn, msg, NULL, -1, NULL);
  dbus_message_unref(msg);

 end:
  evry_item_free((Evry_Item *)act->item1);
  evry_item_free((Evry_Item *)act->item2);
}

static int
_action_send(Evry_Action *act)
{
  buddyInfo* info = act->item1->data;
  DBusMessage *msg;

  if (!(msg = dbus_message_new_method_call(DBUS_PIDGIN_BUS_NAME,
					   DBUS_PIDGIN_PATH,
					   DBUS_PIDGIN_INTERFACE,
					   "PurpleAccountGetConnection")))
    return 0;
      
  dbus_message_append_args(msg,
			   DBUS_TYPE_INT32, &(info->accountNr),
			   DBUS_TYPE_INVALID);
  
  e_dbus_message_send(conn, msg, cb_sendFile, -1, act);
  dbus_message_unref(msg);

  /* when action returns and everything hides items will be freed,
     but we need to wait for the connection.. */
  evry_item_ref((Evry_Item *)act->item1);
  evry_item_ref((Evry_Item *)act->item2);
   
  return EVRY_ACTION_FINISHED;
}


static void
cb_sendMessage(void *data, DBusMessage *reply, DBusError *error)
{
  DBusMessage *msg;
  int imData;
  Evry_Action *act = data;
  buddyInfo* info = act->item1->data;

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
			   DBUS_TYPE_STRING, &(act->item2->label),
			   DBUS_TYPE_INVALID);

  e_dbus_message_send(conn, msg, NULL, -1, NULL);
  dbus_message_unref(msg);

 end:
  evry_item_free((Evry_Item *)act->item1);
  evry_item_free((Evry_Item *)act->item2);
}

static void
cb_getImData(void *data, DBusMessage *reply, DBusError *error)
{
  DBusMessage *msg;
  int conversation;
  Evry_Action *act = data;

  if (!check_msg(data, reply, error)) goto end;

  if (!act->item2)
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

  e_dbus_message_send(conn, msg, cb_sendMessage, -1, act);
  dbus_message_unref(msg);

  return;
   
 end:
  evry_item_free((Evry_Item *)act->item1);
  if (act->item2)
    evry_item_ref((Evry_Item *)act->item2);
}

static int
_action_chat(Evry_Action *act)
{
  DBusMessage *msg;
  buddyInfo* info = act->item1->data;

  if (!(msg = dbus_message_new_method_call(DBUS_PIDGIN_BUS_NAME,
					   DBUS_PIDGIN_PATH,
					   DBUS_PIDGIN_INTERFACE,
					   "PurpleConversationNew")))
    {
      DBG("Couldn't call pidgin's method via dbus!\n");
      return EVRY_ACTION_OTHER;
    }

  dbus_message_append_args(msg,
			   DBUS_TYPE_INT32, &DEFAULT_CONVERSATION_TYPE,
			   DBUS_TYPE_INT32, &(info->accountNr),
			   DBUS_TYPE_STRING,&(info->networkID),
			   DBUS_TYPE_INVALID);

  e_dbus_message_send(conn, msg, cb_getImData, -1, act);
   
  dbus_message_unref(msg);

  /* when action returns and everything hides items will be freed,
     but we need to wait for the connection.. */
  evry_item_ref((Evry_Item *)act->item1);
  if (act->item2)
    evry_item_ref((Evry_Item *)act->item2);

  return EVRY_ACTION_FINISHED;
}


static Eina_Bool
module_init(void)
{
  if (!evry_api_version_check(EVRY_API_VERSION))
    return EINA_FALSE;

  if (!(conn = e_dbus_bus_get(DBUS_BUS_SESSION)))
    {
      EINA_LOG_CRIT("could not connect to dbus' session bus");
      return EINA_FALSE;
    }

  if (_evry_plugin_source_pidgin_log_dom < 0)
    {
      _evry_plugin_source_pidgin_log_dom =
	eina_log_domain_register("evry plugin source pidgin", NULL);

      if (_evry_plugin_source_pidgin_log_dom < 0)
	{
	  EINA_LOG_CRIT( "could not register log domain 'evry plugin source pidgin'");
	  return EINA_FALSE;
	}
    }

  plug = evry_plugin_new(NULL, "Pidgin", type_subject, NULL, "PIDGINCONTACT",
			 1, NULL, NULL, _begin, _cleanup, _fetch, NULL, _icon_get, NULL, NULL);

  evry_plugin_register(plug, 1);
  
  act = evry_action_new("Chat", "PIDGINCONTACT", NULL, NULL, "go-next", //icon
			_action_chat, NULL, NULL, NULL, NULL);
  
  act2 = evry_action_new("Send File", "PIDGINCONTACT", "FILE", NULL, NULL, //icon
			 _action_send, NULL, NULL, NULL, NULL);

  act3 = evry_action_new("Write Message", "PIDGINCONTACT", "TEXT", NULL, "go-next", //icon
			 _action_chat, NULL, NULL, NULL, NULL);

  evry_action_register(act, 0);
  evry_action_register(act2, 1);
  evry_action_register(act3, 1);

  return EINA_TRUE;
}

static void
module_shutdown(void)
{
  eina_log_domain_unregister(_evry_plugin_source_pidgin_log_dom);
  _evry_plugin_source_pidgin_log_dom = -1;

  EVRY_PLUGIN_FREE(plug);

  evry_action_free(act);
  evry_action_free(act2);
  evry_action_free(act3);
}

/***************************************************************************/
/**/
/* actual module specifics */

static E_Module *_module = NULL;
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
  _module = m;

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

  _module = NULL;
   
  return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
  return 1;
}

/**/
/***************************************************************************/

