#include "e_bluez_private.h"
#include <stdlib.h>
#include <string.h>

static E_DBus_Signal_Handler *cb_name_owner_changed = NULL;
static DBusPendingCall *pending_get_name_owner = NULL;
static unsigned int init_count = 0;
static char *unique_name = NULL;

static const char bus_name[] = "org.bluez";

E_DBus_Connection *e_bluez_conn = NULL;

EAPI int E_BLUEZ_EVENT_MANAGER_IN = 0;
EAPI int E_BLUEZ_EVENT_MANAGER_OUT = 0;
EAPI int E_BLUEZ_EVENT_ELEMENT_ADD = 0;
EAPI int E_BLUEZ_EVENT_ELEMENT_DEL = 0;
EAPI int E_BLUEZ_EVENT_ELEMENT_UPDATED = 0;
EAPI int E_BLUEZ_EVENT_DEVICE_FOUND = 0;

const char *e_bluez_iface_manager = NULL;
const char *e_bluez_iface_adapter = NULL;
const char *e_bluez_iface_device = NULL;
const char *e_bluez_prop_address = NULL;
const char *e_bluez_prop_name = NULL;
const char *e_bluez_prop_alias = NULL;
const char *e_bluez_prop_class = NULL;
const char *e_bluez_prop_icon = NULL;
const char *e_bluez_prop_paired = NULL;
const char *e_bluez_prop_trusted = NULL;
const char *e_bluez_prop_connected = NULL;
const char *e_bluez_prop_uuids = NULL;
const char *e_bluez_prop_powered = NULL;
const char *e_bluez_prop_discoverable = NULL;
const char *e_bluez_prop_pairable = NULL;
const char *e_bluez_prop_discoverabletimeout = NULL;
const char *e_bluez_prop_pairabletimeout = NULL;
const char *e_bluez_prop_discovering = NULL;
const char *e_bluez_prop_devices = NULL;

int _e_dbus_bluez_log_dom = -1;

const char *
e_bluez_system_bus_name_get(void)
{
   return unique_name ? unique_name : bus_name;
}

/***********************************************************************
* Manager
***********************************************************************/

/*
 * FIXME: Do we really need to call Manager.GetProperties()?
 */

/**
 * Synchronize elements with server.
 *
 * This will call Manager.GetProperties() on server, retrieve properties
 * and some element paths and then request their properties.
 *
 * This call will add events E_BLUEZ_EVENT_ELEMENT_ADD and
 * E_BLUEZ_EVENT_ELEMENT_UPDATED to the main loop.
 *
 * This will not remove stale elements.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_bluez_manager_sync_elements(void)
{
   E_Bluez_Element *manager;

   if (!unique_name)
      return EINA_FALSE;

   manager = e_bluez_element_register(manager_path, e_bluez_iface_manager);
   if (manager)
      e_bluez_element_properties_sync(manager);
   else
      return EINA_FALSE;

   DBG("sync_manager: %s (%s)", unique_name, bus_name);

   return EINA_TRUE;
}

static void
_e_bluez_system_name_owner_exit(void)
{
   e_bluez_manager_clear_elements();
   ecore_event_add(E_BLUEZ_EVENT_MANAGER_OUT, NULL, NULL, NULL);

   free(unique_name);
   unique_name = NULL;
}

static void
_e_bluez_system_name_owner_enter(const char *uid)
{
   DBG("enter bluez at %s (old was %s)", uid, unique_name);
   if (unique_name && strcmp(unique_name, uid) == 0)
     {
        DBG("same unique_name for bluez, ignore.");
        return;
     }

   if (unique_name)
      _e_bluez_system_name_owner_exit();

   unique_name = strdup(uid);

   ecore_event_add(E_BLUEZ_EVENT_MANAGER_IN, NULL, NULL, NULL);
   e_bluez_manager_sync_elements();
}

static void
_e_bluez_system_name_owner_changed(void *data __UNUSED__, DBusMessage *msg)
{
   DBusError err;
   const char *name, *from, *to;

   dbus_error_init(&err);
   if (!dbus_message_get_args(msg, &err,
                              DBUS_TYPE_STRING, &name,
                              DBUS_TYPE_STRING, &from,
                              DBUS_TYPE_STRING, &to,
                              DBUS_TYPE_INVALID))
     {
        ERR("could not get NameOwnerChanged arguments: %s: %s",
            err.name, err.message);
        dbus_error_free(&err);
        return;
     }

   if (strcmp(name, bus_name) != 0)
      return;

   DBG("NameOwnerChanged from=[%s] to=[%s]", from, to);

   if (from[0] == '\0' && to[0] != '\0')
     {
        _e_bluez_system_name_owner_enter(to);
     }
   else if (from[0] != '\0' && to[0] == '\0')
     {
        DBG("exit bluez at %s", from);
        if (strcmp(unique_name, from) != 0)
           DBG("%s was not the known name %s, ignored.", from, unique_name);
        else
           _e_bluez_system_name_owner_exit();
     }
   else
     {
        DBG("unknow change from %s to %s", from, to);
     }
}

static void
_e_bluez_get_name_owner(void *data __UNUSED__, DBusMessage *msg, DBusError *err)
{
   DBusMessageIter itr;
   int t;
   const char *uid;

   pending_get_name_owner = NULL;

   if (!_dbus_callback_check_and_init(msg, &itr, err))
      return;

   t = dbus_message_iter_get_arg_type(&itr);
   if (!_dbus_iter_type_check(t, DBUS_TYPE_STRING))
      return;

   dbus_message_iter_get_basic(&itr, &uid);
   if (!uid)
     {
        ERR("no name owner!");
        return;
     }

   _e_bluez_system_name_owner_enter(uid);
   return;
}

/**
 * Initialize E BlueZ (E_Bluez) system.
 *
 * This will connect and watch org.bluez.Manager and Element
 * events and translate to Ecore main loop events, also provide a
 * proxy for method invocation on server.
 *
 * Interesting events are:
 *   - E_BLUEZ_EVENT_MANAGER_IN: issued when bluez is avaiable.
 *   - E_BLUEZ_EVENT_MANAGER_OUT: issued when bluez connection is lost.
 *   - E_BLUEZ_EVENT_ELEMENT_ADD: element was added.
 *   - E_BLUEZ_EVENT_ELEMENT_DEL: element was deleted.
 *   - E_BLUEZ_EVENT_ELEMENT_UPDATED: element was updated (properties
 *     or state changed).
 *   - E_BLUEZ_EVENT_DEVICE_FOUND: a device was found, raised after calling
 *     Adapter.StartDiscorvery()
 *
 * Manager IN/OUT events do not provide any event information, just
 * tells you that system is usable or not. After manager is out, all
 * elements will be removed, so after this event do not use the system anymore.
 *
 * Element events will give you an element object. After DEL event callback
 * returns, that element will not be valid anymore.
 */
unsigned int
e_bluez_system_init(E_DBus_Connection *edbus_conn)
{
   init_count++;

   if (init_count > 1)
      return init_count;

   _e_dbus_bluez_log_dom = eina_log_domain_register
         ("e_dbus_bluez", EINA_LOG_DEFAULT_COLOR);

   if(_e_dbus_bluez_log_dom < 0)
     {
        EINA_LOG_ERR
           ("impossible to create a log domain for edbus_bluez module");
        return -1;
     }

   if (E_BLUEZ_EVENT_MANAGER_IN == 0)
      E_BLUEZ_EVENT_MANAGER_IN = ecore_event_type_new();

   if (E_BLUEZ_EVENT_MANAGER_OUT == 0)
      E_BLUEZ_EVENT_MANAGER_OUT = ecore_event_type_new();

   if (E_BLUEZ_EVENT_ELEMENT_ADD == 0)
      E_BLUEZ_EVENT_ELEMENT_ADD = ecore_event_type_new();

   if (E_BLUEZ_EVENT_ELEMENT_DEL == 0)
      E_BLUEZ_EVENT_ELEMENT_DEL = ecore_event_type_new();

   if (E_BLUEZ_EVENT_ELEMENT_UPDATED == 0)
      E_BLUEZ_EVENT_ELEMENT_UPDATED = ecore_event_type_new();

   if (E_BLUEZ_EVENT_DEVICE_FOUND == 0)
      E_BLUEZ_EVENT_DEVICE_FOUND = ecore_event_type_new();

   if (!e_bluez_iface_manager)
      e_bluez_iface_manager = eina_stringshare_add("org.bluez.Manager");

   if (!e_bluez_iface_adapter)
      e_bluez_iface_adapter = eina_stringshare_add("org.bluez.Adapter");

   if (!e_bluez_iface_device)
      e_bluez_iface_device = eina_stringshare_add("org.bluez.Device");

   if (!e_bluez_prop_address)
      e_bluez_prop_address = eina_stringshare_add("Address");

   if (!e_bluez_prop_name)
      e_bluez_prop_name = eina_stringshare_add("Name");

   if (!e_bluez_prop_alias)
      e_bluez_prop_alias = eina_stringshare_add("Alias");

   if (!e_bluez_prop_class)
      e_bluez_prop_class = eina_stringshare_add("Class");

   if (!e_bluez_prop_icon)
      e_bluez_prop_icon = eina_stringshare_add("Icon");

   if (!e_bluez_prop_paired)
      e_bluez_prop_paired = eina_stringshare_add("Paired");

   if (!e_bluez_prop_trusted)
      e_bluez_prop_trusted = eina_stringshare_add("Trusted");

   if (!e_bluez_prop_connected)
      e_bluez_prop_connected = eina_stringshare_add("Connected");

   if (!e_bluez_prop_uuids)
      e_bluez_prop_uuids = eina_stringshare_add("UUIDs");

   if (!e_bluez_prop_powered)
      e_bluez_prop_powered = eina_stringshare_add("Powered");

   if (!e_bluez_prop_discoverable)
      e_bluez_prop_discoverable = eina_stringshare_add("Discoverable");

   if (!e_bluez_prop_pairable)
      e_bluez_prop_pairable = eina_stringshare_add("Pairable");

   if (!e_bluez_prop_discoverabletimeout)
      e_bluez_prop_discoverabletimeout = eina_stringshare_add("DiscoverableTimeout");

   if (!e_bluez_prop_pairabletimeout)
      e_bluez_prop_pairabletimeout = eina_stringshare_add("PairableTimeout");

   if (!e_bluez_prop_discovering)
      e_bluez_prop_discovering = eina_stringshare_add("Discovering");

   if (!e_bluez_prop_devices)
      e_bluez_prop_devices = eina_stringshare_add("Devices");

   e_bluez_conn = edbus_conn;
   cb_name_owner_changed = e_dbus_signal_handler_add
         (e_bluez_conn, E_DBUS_FDO_BUS, E_DBUS_FDO_PATH, E_DBUS_FDO_INTERFACE, "NameOwnerChanged",
         _e_bluez_system_name_owner_changed, NULL);

   if (pending_get_name_owner)
      dbus_pending_call_cancel(pending_get_name_owner);

   pending_get_name_owner = e_dbus_get_name_owner
         (e_bluez_conn, bus_name, _e_bluez_get_name_owner, NULL);

   e_bluez_elements_init();

   return init_count;
}

static inline void
_stringshare_del(const char **str)
{
   if (!*str)
      return;

   eina_stringshare_del(*str);
   *str = NULL;
}

/**
 * Shutdown bluez system.
 *
 * When count drops to 0 resources will be released and no calls should be
 * made anymore.
 */
unsigned int
e_bluez_system_shutdown(void)
{
   if (init_count == 0)
     {
        ERR("bluez system already shut down.");
        return 0;
     }

   init_count--;
   if (init_count > 0)
      return init_count;

   _stringshare_del(&e_bluez_iface_manager);
   _stringshare_del(&e_bluez_iface_adapter);
   _stringshare_del(&e_bluez_iface_device);
   _stringshare_del(&e_bluez_prop_address);
   _stringshare_del(&e_bluez_prop_name);
   _stringshare_del(&e_bluez_prop_alias);
   _stringshare_del(&e_bluez_prop_class);
   _stringshare_del(&e_bluez_prop_icon);
   _stringshare_del(&e_bluez_prop_paired);
   _stringshare_del(&e_bluez_prop_trusted);
   _stringshare_del(&e_bluez_prop_connected);
   _stringshare_del(&e_bluez_prop_uuids);
   _stringshare_del(&e_bluez_prop_powered);
   _stringshare_del(&e_bluez_prop_discoverable);
   _stringshare_del(&e_bluez_prop_pairable);
   _stringshare_del(&e_bluez_prop_discoverabletimeout);
   _stringshare_del(&e_bluez_prop_pairabletimeout);
   _stringshare_del(&e_bluez_prop_discovering);
   _stringshare_del(&e_bluez_prop_devices);

   if (pending_get_name_owner)
     {
        dbus_pending_call_cancel(pending_get_name_owner);
        pending_get_name_owner = NULL;
     }

   if (cb_name_owner_changed)
     {
        e_dbus_signal_handler_del(e_bluez_conn, cb_name_owner_changed);
        cb_name_owner_changed = NULL;
     }

   if (unique_name)
      _e_bluez_system_name_owner_exit();

   e_bluez_elements_shutdown();
   eina_log_domain_unregister(_e_dbus_bluez_log_dom);
   e_bluez_conn = NULL;

   return init_count;
}

