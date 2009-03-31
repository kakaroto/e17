#include "e_connman_private.h"
#include <stdlib.h>
#include <string.h>

static E_DBus_Signal_Handler *cb_name_owner_changed = NULL;
static DBusPendingCall *pending_get_name_owner = NULL;
static unsigned int init_count = 0;
static char *unique_name = NULL;

static const char bus_name[] = "org.moblin.connman";
static const char fdo_bus_name[] = "org.freedesktop.DBus";
static const char fdo_interface[] = "org.freedesktop.DBus";
static const char fdo_path[] = "/org/freedesktop/DBus";

E_DBus_Connection *e_connman_conn = NULL;

EAPI int E_CONNMAN_EVENT_MANAGER_IN = 0;
EAPI int E_CONNMAN_EVENT_MANAGER_OUT = 0;
EAPI int E_CONNMAN_EVENT_ELEMENT_ADD = 0;
EAPI int E_CONNMAN_EVENT_ELEMENT_DEL = 0;
EAPI int E_CONNMAN_EVENT_ELEMENT_UPDATED = 0;

const char *e_connman_iface_manager = NULL;
const char *e_connman_iface_network = NULL;
const char *e_connman_iface_profile = NULL;
const char *e_connman_iface_service = NULL;
const char *e_connman_iface_device = NULL;
const char *e_connman_iface_connection = NULL;

const char *e_connman_prop_available = NULL;
const char *e_connman_prop_connected = NULL;
const char *e_connman_prop_connections = NULL;
const char *e_connman_prop_default = NULL;
const char *e_connman_prop_device = NULL;
const char *e_connman_prop_devices = NULL;
const char *e_connman_prop_interface = NULL;
const char *e_connman_prop_ipv4_address = NULL;
const char *e_connman_prop_ipv4_method = NULL;
const char *e_connman_prop_name = NULL;
const char *e_connman_prop_network = NULL;
const char *e_connman_prop_networks = NULL;
const char *e_connman_prop_offline_mode = NULL;
const char *e_connman_prop_policy = NULL;
const char *e_connman_prop_powered = NULL;
const char *e_connman_prop_priority = NULL;
const char *e_connman_prop_profiles = NULL;
const char *e_connman_prop_remember = NULL;
const char *e_connman_prop_scan_interval = NULL;
const char *e_connman_prop_scanning = NULL;
const char *e_connman_prop_state = NULL;
const char *e_connman_prop_strengh = NULL;
const char *e_connman_prop_type = NULL;
const char *e_connman_prop_wifi_mode = NULL;
const char *e_connman_prop_wifi_passphrase = NULL;
const char *e_connman_prop_wifi_security = NULL;
const char *e_connman_prop_wifi_ssid = NULL;

const char *
e_connman_system_bus_name_get(void)
{
   return unique_name ? unique_name : bus_name;
}


/***********************************************************************
 * Manager
 ***********************************************************************/

/**
 * Synchronize elements with server.
 *
 * This will call Manager.GetProperties() on server, retrieve properties
 * and some element paths and then request their properties.
 *
 * This call will add events E_CONNMAN_EVENT_ELEMENT_ADD and
 * E_CONNMAN_EVENT_ELEMENT_UPDATED to the main loop.
 *
 * This will not remove stale elements.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_manager_sync_elements(void)
{
   E_Connman_Element *manager;

   if (!unique_name)
     return FALSE;
   manager = e_connman_element_register(manager_path, e_connman_iface_manager);
   if (manager)
     e_connman_element_properties_sync(manager);
   else
     return FALSE;

   DBG("sync_manager: %s (%s)\n", unique_name, bus_name);

   return TRUE;
}

static void
_e_connman_system_name_owner_exit(void)
{
   e_connman_manager_clear_elements();
   ecore_event_add(E_CONNMAN_EVENT_MANAGER_OUT, NULL, NULL, NULL);

   free(unique_name);
   unique_name = NULL;
}

static void
_e_connman_system_name_owner_enter(const char *uid)
{
   DBG("enter connman at %s (old was %s)\n", uid, unique_name);
   if (unique_name && strcmp(unique_name, uid) == 0)
     {
	DBG("same unique_name for connman, ignore.\n");
	return;
     }

   if (unique_name)
     _e_connman_system_name_owner_exit();

   unique_name = strdup(uid);

   ecore_event_add(E_CONNMAN_EVENT_MANAGER_IN, NULL, NULL, NULL);
   e_connman_manager_sync_elements();
}

static void
_e_connman_system_name_owner_changed(void *data, DBusMessage *msg)
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
	ERR("could not get NameOwnerChanged arguments: %s: %s\n",
	    err.name, err.message);
	dbus_error_free(&err);
	return;
     }

   if (strcmp(name, bus_name) != 0)
     return;

   DBG("NameOwnerChanged from=[%s] to=[%s]\n", from, to);

   if (from[0] == '\0' && to[0] != '\0')
     _e_connman_system_name_owner_enter(to);
   else if (from[0] != '\0' && to[0] == '\0')
     {
	DBG("exit connman at %s\n", from);
	if (strcmp(unique_name, from) != 0)
	  DBG("%s was not the known name %s, ignored.\n", from, unique_name);
	else
	  _e_connman_system_name_owner_exit();
     }
   else
     DBG("unknow change from %s to %s\n", from, to);
}

static void
_e_connman_get_name_owner(void *data, DBusMessage *msg, DBusError *err)
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
	ERR("no name owner!\n");
	return;
     }

   _e_connman_system_name_owner_enter(uid);
   return;
}

/**
 * Initialize E Connection Manager (E_Connman) system.
 *
 * This will connect and watch org.moblin.connman.Manager and Element
 * events and translate to Ecore main loop events, also provide a
 * proxy for method invocation on server.
 *
 * Interesting events are:
 *   - E_CONNMAN_EVENT_MANAGER_IN: issued when connman is avaiable.
 *   - E_CONNMAN_EVENT_MANAGER_OUT: issued when connman connection is lost.
 *   - E_CONNMAN_EVENT_ELEMENT_ADD: element was added.
 *   - E_CONNMAN_EVENT_ELEMENT_DEL: element was deleted.
 *   - E_CONNMAN_EVENT_ELEMENT_UPDATED: element was updated (properties
 *     or state changed).
 *
 * Manager IN/OUT events do not provide any event information, just
 * tells you that system is usable or not. After manager is out, all
 * elements will be removed, so after this event do not use the system anymore.
 *
 * Element events will give you an element object. After DEL event callback
 * returns, that element will not be valid anymore.
 */
unsigned int
e_connman_system_init(E_DBus_Connection *edbus_conn)
{
   init_count++;

   if (init_count > 1)
     return init_count;

   if (!eina_init())
     fputs("ERROR: Error during the initialization of eina.\n", stderr);

   if (E_CONNMAN_EVENT_MANAGER_IN == 0)
     E_CONNMAN_EVENT_MANAGER_IN = ecore_event_type_new();
   if (E_CONNMAN_EVENT_MANAGER_OUT == 0)
     E_CONNMAN_EVENT_MANAGER_OUT = ecore_event_type_new();
   if (E_CONNMAN_EVENT_ELEMENT_ADD == 0)
     E_CONNMAN_EVENT_ELEMENT_ADD = ecore_event_type_new();
   if (E_CONNMAN_EVENT_ELEMENT_DEL == 0)
     E_CONNMAN_EVENT_ELEMENT_DEL = ecore_event_type_new();
   if (E_CONNMAN_EVENT_ELEMENT_UPDATED == 0)
     E_CONNMAN_EVENT_ELEMENT_UPDATED = ecore_event_type_new();

   if (e_connman_iface_manager == NULL)
     e_connman_iface_manager = eina_stringshare_add("org.moblin.connman.Manager");
   if (e_connman_iface_network == NULL)
     e_connman_iface_network = eina_stringshare_add("org.moblin.connman.Network");
   if (e_connman_iface_profile == NULL)
     e_connman_iface_profile = eina_stringshare_add("org.moblin.connman.Profile");
   if (e_connman_iface_service == NULL)
     e_connman_iface_service = eina_stringshare_add("org.moblin.connman.Service");
   if (e_connman_iface_device == NULL)
     e_connman_iface_device = eina_stringshare_add("org.moblin.connman.Device");
   if (e_connman_iface_connection == NULL)
     e_connman_iface_connection = eina_stringshare_add("org.moblin.connman.Connection");

   if (e_connman_prop_available == NULL)
     e_connman_prop_available = eina_stringshare_add("Available");
   if (e_connman_prop_connected == NULL)
     e_connman_prop_connected = eina_stringshare_add("Connected");
   if (e_connman_prop_connections == NULL)
     e_connman_prop_connections = eina_stringshare_add("Connections");
   if (e_connman_prop_default == NULL)
     e_connman_prop_default = eina_stringshare_add("Default");
   if (e_connman_prop_device == NULL)
     e_connman_prop_device = eina_stringshare_add("Device");
   if (e_connman_prop_devices == NULL)
     e_connman_prop_devices = eina_stringshare_add("Devices");
   if (e_connman_prop_interface == NULL)
     e_connman_prop_interface = eina_stringshare_add("Interface");
   if (e_connman_prop_ipv4_address == NULL)
     e_connman_prop_ipv4_address = eina_stringshare_add("IPv4.Address");
   if (e_connman_prop_ipv4_method == NULL)
     e_connman_prop_ipv4_method = eina_stringshare_add("IPv4.Method");
   if (e_connman_prop_name == NULL)
     e_connman_prop_name = eina_stringshare_add("Name");
   if (e_connman_prop_network == NULL)
     e_connman_prop_network = eina_stringshare_add("Network");
   if (e_connman_prop_networks == NULL)
     e_connman_prop_networks = eina_stringshare_add("Networks");
   if (e_connman_prop_offline_mode == NULL)
     e_connman_prop_offline_mode = eina_stringshare_add("OfflineMode");
   if (e_connman_prop_policy == NULL)
     e_connman_prop_policy = eina_stringshare_add("Policy");
   if (e_connman_prop_powered == NULL)
     e_connman_prop_powered = eina_stringshare_add("Powered");
   if (e_connman_prop_priority == NULL)
     e_connman_prop_priority = eina_stringshare_add("Priority");
   if (e_connman_prop_profiles == NULL)
     e_connman_prop_profiles = eina_stringshare_add("Profiles");
   if (e_connman_prop_remember == NULL)
     e_connman_prop_remember = eina_stringshare_add("Remember");
   if (e_connman_prop_scan_interval == NULL)
     e_connman_prop_scan_interval = eina_stringshare_add("ScanInterval");
   if (e_connman_prop_scanning == NULL)
     e_connman_prop_scanning = eina_stringshare_add("Scanning");
   if (e_connman_prop_state == NULL)
     e_connman_prop_state = eina_stringshare_add("State");
   if (e_connman_prop_strengh == NULL)
     e_connman_prop_strengh = eina_stringshare_add("Strength");
   if (e_connman_prop_type == NULL)
     e_connman_prop_type = eina_stringshare_add("Type");
   if (e_connman_prop_wifi_mode == NULL)
     e_connman_prop_wifi_mode = eina_stringshare_add("WiFi.Mode");
   if (e_connman_prop_wifi_passphrase == NULL)
     e_connman_prop_wifi_passphrase = eina_stringshare_add("WiFi.Passphrase");
   if (e_connman_prop_wifi_security == NULL)
     e_connman_prop_wifi_security = eina_stringshare_add("WiFi.Security");
   if (e_connman_prop_wifi_ssid == NULL)
     e_connman_prop_wifi_ssid = eina_stringshare_add("WiFi.SSID");

   e_connman_conn = edbus_conn;
   cb_name_owner_changed = e_dbus_signal_handler_add
     (e_connman_conn, fdo_bus_name, fdo_path, fdo_interface, "NameOwnerChanged",
      _e_connman_system_name_owner_changed, NULL);

   if (pending_get_name_owner)
     dbus_pending_call_cancel(pending_get_name_owner);

   pending_get_name_owner = e_dbus_get_name_owner
     (e_connman_conn, bus_name, _e_connman_get_name_owner, NULL);

   e_connman_elements_init();

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
 * Shutdown connman system.
 *
 * When count drops to 0 resources will be released and no calls should be
 * made anymore.
 */
unsigned int
e_connman_system_shutdown(void)
{
   if (init_count == 0)
     {
	fputs("ERROR: connman system already shutdown.\n", stderr);
	return 0;
     }
   init_count--;
   if (init_count > 0)
     return init_count;

   _stringshare_del(&e_connman_iface_manager);
   _stringshare_del(&e_connman_iface_network);
   _stringshare_del(&e_connman_iface_profile);
   _stringshare_del(&e_connman_iface_service);
   _stringshare_del(&e_connman_iface_device);
   _stringshare_del(&e_connman_iface_connection);

   _stringshare_del(&e_connman_prop_available);
   _stringshare_del(&e_connman_prop_connected);
   _stringshare_del(&e_connman_prop_connections);
   _stringshare_del(&e_connman_prop_default);
   _stringshare_del(&e_connman_prop_device);
   _stringshare_del(&e_connman_prop_devices);
   _stringshare_del(&e_connman_prop_interface);
   _stringshare_del(&e_connman_prop_ipv4_address);
   _stringshare_del(&e_connman_prop_ipv4_method);
   _stringshare_del(&e_connman_prop_name);
   _stringshare_del(&e_connman_prop_network);
   _stringshare_del(&e_connman_prop_networks);
   _stringshare_del(&e_connman_prop_offline_mode);
   _stringshare_del(&e_connman_prop_policy);
   _stringshare_del(&e_connman_prop_powered);
   _stringshare_del(&e_connman_prop_priority);
   _stringshare_del(&e_connman_prop_profiles);
   _stringshare_del(&e_connman_prop_remember);
   _stringshare_del(&e_connman_prop_scan_interval);
   _stringshare_del(&e_connman_prop_scanning);
   _stringshare_del(&e_connman_prop_state);
   _stringshare_del(&e_connman_prop_strengh);
   _stringshare_del(&e_connman_prop_type);
   _stringshare_del(&e_connman_prop_wifi_mode);
   _stringshare_del(&e_connman_prop_wifi_passphrase);
   _stringshare_del(&e_connman_prop_wifi_security);
   _stringshare_del(&e_connman_prop_wifi_ssid);

   if (pending_get_name_owner)
     {
	dbus_pending_call_cancel(pending_get_name_owner);
	pending_get_name_owner = NULL;
     }

   if (cb_name_owner_changed)
     {
	e_dbus_signal_handler_del(e_connman_conn, cb_name_owner_changed);
	cb_name_owner_changed = NULL;
     }

   if (unique_name)
     _e_connman_system_name_owner_exit();

   e_connman_elements_shutdown();
   eina_shutdown();
   e_connman_conn = NULL;

   return init_count;
}
