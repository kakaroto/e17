#include "e_connman_private.h"
#include <stdlib.h>
#include <string.h>

static E_DBus_Signal_Handler *cb_name_owner_changed = NULL;
static DBusPendingCall *pending_get_name_owner = NULL;
static unsigned int init_count = 0;
static char *unique_name = NULL;

static const char bus_name[] = "net.connman";

E_DBus_Connection *e_connman_conn = NULL;

EAPI int E_CONNMAN_EVENT_MANAGER_IN = 0;
EAPI int E_CONNMAN_EVENT_MANAGER_OUT = 0;
EAPI int E_CONNMAN_EVENT_ELEMENT_ADD = 0;
EAPI int E_CONNMAN_EVENT_ELEMENT_DEL = 0;
EAPI int E_CONNMAN_EVENT_ELEMENT_UPDATED = 0;

const char *e_connman_iface_manager = NULL;
const char *e_connman_iface_profile = NULL;
const char *e_connman_iface_service = NULL;
const char *e_connman_iface_connection = NULL;
const char *e_connman_iface_technology = NULL;

const char *e_connman_prop_available = NULL;
const char *e_connman_prop_connections = NULL;
const char *e_connman_prop_default = NULL;
const char *e_connman_prop_ipv4 = NULL;
const char *e_connman_prop_ipv4_configuration = NULL;
const char *e_connman_prop_ethernet = NULL;
const char *e_connman_prop_method = NULL;
const char *e_connman_prop_address = NULL;
const char *e_connman_prop_gateway = NULL;
const char *e_connman_prop_netmask = NULL;
const char *e_connman_prop_mtu = NULL;
const char *e_connman_prop_name = NULL;
const char *e_connman_prop_offline_mode = NULL;
const char *e_connman_prop_policy = NULL;
const char *e_connman_prop_priority = NULL;
const char *e_connman_prop_profiles = NULL;
const char *e_connman_prop_profile_active = NULL;
const char *e_connman_prop_services = NULL;
const char *e_connman_prop_technologies = NULL;
const char *e_connman_prop_remember = NULL;
const char *e_connman_prop_state = NULL;
const char *e_connman_prop_strength = NULL;
const char *e_connman_prop_type = NULL;
const char *e_connman_prop_error = NULL;
const char *e_connman_prop_mode = NULL;
const char *e_connman_prop_security = NULL;
const char *e_connman_prop_passphrase = NULL;
const char *e_connman_prop_passphrase_required = NULL;
const char *e_connman_prop_login_required = NULL;
const char *e_connman_prop_favorite = NULL;
const char *e_connman_prop_immutable = NULL;
const char *e_connman_prop_auto_connect = NULL;
const char *e_connman_prop_setup_required = NULL;
const char *e_connman_prop_apn = NULL;
const char *e_connman_prop_mcc = NULL;
const char *e_connman_prop_mnc = NULL;
const char *e_connman_prop_roaming = NULL;
const char *e_connman_prop_technology_default = NULL;
const char *e_connman_prop_technologies_available = NULL;
const char *e_connman_prop_technologies_enabled = NULL;
const char *e_connman_prop_technologies_connected = NULL;

int _e_dbus_connman_log_dom = -1;

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
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_connman_manager_sync_elements(void)
{
   E_Connman_Element *manager;

   if (!unique_name)
      return EINA_FALSE;

   manager = e_connman_element_register(manager_path, e_connman_iface_manager);
   if (manager)
      e_connman_element_properties_sync(manager);
   else
      return EINA_FALSE;

   DBG("sync_manager: %s (%s)", unique_name, bus_name);

   return EINA_TRUE;
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
   DBG("enter connman at %s (old was %s)", uid, unique_name);
   if (unique_name && strcmp(unique_name, uid) == 0)
     {
        DBG("same unique_name for connman, ignore.");
        return;
     }

   if (unique_name)
      _e_connman_system_name_owner_exit();

   unique_name = strdup(uid);

   ecore_event_add(E_CONNMAN_EVENT_MANAGER_IN, NULL, NULL, NULL);
   e_connman_manager_sync_elements();
}

static void
_e_connman_system_name_owner_changed(void *data __UNUSED__, DBusMessage *msg)
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
        _e_connman_system_name_owner_enter(to);
     }
   else if (from[0] != '\0' && to[0] == '\0')
     {
        DBG("exit connman at %s", from);
        if (strcmp(unique_name, from) != 0)
           DBG("%s was not the known name %s, ignored.", from, unique_name);
        else
           _e_connman_system_name_owner_exit();
     }
   else
     {
        DBG("unknow change from %s to %s", from, to);
     }
}

static void
_e_connman_get_name_owner(void *data __UNUSED__, DBusMessage *msg, DBusError *err)
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

   _e_connman_system_name_owner_enter(uid);
   return;
}

/**
 * Initialize E Connection Manager (E_Connman) system.
 *
 * This will connect and watch net.connman.Manager and Element
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

   _e_dbus_connman_log_dom = eina_log_domain_register
         ("e_dbus_connman", EINA_LOG_DEFAULT_COLOR);

   if (_e_dbus_connman_log_dom < 0)
     {
        EINA_LOG_ERR
           ("impossible to create a log domain for edbus_connman module");
        return -1;
     }

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

   if (!e_connman_iface_manager)
      e_connman_iface_manager = eina_stringshare_add("net.connman.Manager");

   if (!e_connman_iface_profile)
      e_connman_iface_profile = eina_stringshare_add("net.connman.Profile");

   if (!e_connman_iface_service)
      e_connman_iface_service = eina_stringshare_add("net.connman.Service");

   if (!e_connman_iface_connection)
      e_connman_iface_connection = eina_stringshare_add("net.connman.Connection");

   if (!e_connman_iface_technology)
      e_connman_iface_technology = eina_stringshare_add("net.connman.Technology");

   if (!e_connman_prop_available)
      e_connman_prop_available = eina_stringshare_add("Available");

   if (!e_connman_prop_connections)
      e_connman_prop_connections = eina_stringshare_add("Connections");

   if (!e_connman_prop_default)
      e_connman_prop_default = eina_stringshare_add("Default");

   if (!e_connman_prop_ipv4)
      e_connman_prop_ipv4 = eina_stringshare_add("IPv4");

   if (!e_connman_prop_ipv4_configuration)
      e_connman_prop_ipv4_configuration = eina_stringshare_add("IPv4.Configuration");

   if (!e_connman_prop_ethernet)
      e_connman_prop_ethernet = eina_stringshare_add("Ethernet");

   if (!e_connman_prop_method)
      e_connman_prop_method = eina_stringshare_add("Method");

   if (!e_connman_prop_address)
      e_connman_prop_address = eina_stringshare_add("Address");

   if (!e_connman_prop_gateway)
      e_connman_prop_gateway = eina_stringshare_add("Gateway");

   if (!e_connman_prop_netmask)
      e_connman_prop_netmask = eina_stringshare_add("Netmask");

   if (!e_connman_prop_mtu)
      e_connman_prop_mtu = eina_stringshare_add("MTU");

   if (!e_connman_prop_name)
      e_connman_prop_name = eina_stringshare_add("Name");

   if (!e_connman_prop_offline_mode)
      e_connman_prop_offline_mode = eina_stringshare_add("OfflineMode");

   if (!e_connman_prop_policy)
      e_connman_prop_policy = eina_stringshare_add("Policy");

   if (!e_connman_prop_priority)
      e_connman_prop_priority = eina_stringshare_add("Priority");

   if (!e_connman_prop_profiles)
      e_connman_prop_profiles = eina_stringshare_add("Profiles");

   if (!e_connman_prop_profile_active)
      e_connman_prop_profile_active = eina_stringshare_add("ActiveProfile");

   if (!e_connman_prop_services)
      e_connman_prop_services = eina_stringshare_add("Services");

   if (!e_connman_prop_technologies)
      e_connman_prop_technologies = eina_stringshare_add("Technologies");

   if (!e_connman_prop_remember)
      e_connman_prop_remember = eina_stringshare_add("Remember");

   if (!e_connman_prop_state)
      e_connman_prop_state = eina_stringshare_add("State");

   if (!e_connman_prop_strength)
      e_connman_prop_strength = eina_stringshare_add("Strength");

   if (!e_connman_prop_type)
      e_connman_prop_type = eina_stringshare_add("Type");

   if (!e_connman_prop_error)
      e_connman_prop_error = eina_stringshare_add("Error");

   if (!e_connman_prop_mode)
      e_connman_prop_mode = eina_stringshare_add("Mode");

   if (!e_connman_prop_security)
      e_connman_prop_security = eina_stringshare_add("Security");

   if (!e_connman_prop_passphrase)
      e_connman_prop_passphrase = eina_stringshare_add("Passphrase");

   if (!e_connman_prop_passphrase_required)
      e_connman_prop_passphrase_required = eina_stringshare_add("PassphraseRequired");

   if (!e_connman_prop_login_required)
      e_connman_prop_login_required = eina_stringshare_add("LoginRequired");

   if (!e_connman_prop_favorite)
      e_connman_prop_favorite = eina_stringshare_add("Favorite");

   if (!e_connman_prop_immutable)
      e_connman_prop_immutable = eina_stringshare_add("Immutable");

   if (!e_connman_prop_auto_connect)
      e_connman_prop_auto_connect = eina_stringshare_add("AutoConnect");

   if (!e_connman_prop_setup_required)
      e_connman_prop_setup_required = eina_stringshare_add("SetupRequired");

   if (!e_connman_prop_apn)
      e_connman_prop_apn = eina_stringshare_add("APN");

   if (!e_connman_prop_mcc)
      e_connman_prop_mcc = eina_stringshare_add("MCC");

   if (!e_connman_prop_mnc)
      e_connman_prop_mnc = eina_stringshare_add("MNC");

   if (!e_connman_prop_roaming)
      e_connman_prop_roaming = eina_stringshare_add("Roaming");

   if (!e_connman_prop_technology_default)
      e_connman_prop_technology_default = eina_stringshare_add("DefaultTechnology");

   if (!e_connman_prop_technologies_available)
      e_connman_prop_technologies_available = eina_stringshare_add("AvailableTechnologies");

   if (!e_connman_prop_technologies_enabled)
      e_connman_prop_technologies_enabled = eina_stringshare_add("EnabledTechnologies");

   if (!e_connman_prop_technologies_connected)
      e_connman_prop_technologies_connected = eina_stringshare_add("ConnectedTechnologies");

   e_connman_conn = edbus_conn;
   cb_name_owner_changed = e_dbus_signal_handler_add
         (e_connman_conn, E_DBUS_FDO_BUS, E_DBUS_FDO_PATH, E_DBUS_FDO_INTERFACE, "NameOwnerChanged",
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
        ERR("connman system already shut down.");
        return 0;
     }

   init_count--;
   if (init_count > 0)
      return init_count;

   _stringshare_del(&e_connman_iface_manager);
   _stringshare_del(&e_connman_iface_profile);
   _stringshare_del(&e_connman_iface_service);
   _stringshare_del(&e_connman_iface_connection);
   _stringshare_del(&e_connman_iface_technology);

   _stringshare_del(&e_connman_prop_available);
   _stringshare_del(&e_connman_prop_connections);
   _stringshare_del(&e_connman_prop_default);
   _stringshare_del(&e_connman_prop_ipv4);
   _stringshare_del(&e_connman_prop_ipv4_configuration);
   _stringshare_del(&e_connman_prop_ethernet);
   _stringshare_del(&e_connman_prop_method);
   _stringshare_del(&e_connman_prop_address);
   _stringshare_del(&e_connman_prop_gateway);
   _stringshare_del(&e_connman_prop_netmask);
   _stringshare_del(&e_connman_prop_mtu);
   _stringshare_del(&e_connman_prop_name);
   _stringshare_del(&e_connman_prop_offline_mode);
   _stringshare_del(&e_connman_prop_policy);
   _stringshare_del(&e_connman_prop_priority);
   _stringshare_del(&e_connman_prop_profiles);
   _stringshare_del(&e_connman_prop_profile_active);
   _stringshare_del(&e_connman_prop_services);
   _stringshare_del(&e_connman_prop_technologies);
   _stringshare_del(&e_connman_prop_remember);
   _stringshare_del(&e_connman_prop_state);
   _stringshare_del(&e_connman_prop_strength);
   _stringshare_del(&e_connman_prop_type);
   _stringshare_del(&e_connman_prop_error);
   _stringshare_del(&e_connman_prop_mode);
   _stringshare_del(&e_connman_prop_security);
   _stringshare_del(&e_connman_prop_passphrase);
   _stringshare_del(&e_connman_prop_passphrase_required);
   _stringshare_del(&e_connman_prop_login_required);
   _stringshare_del(&e_connman_prop_favorite);
   _stringshare_del(&e_connman_prop_immutable);
   _stringshare_del(&e_connman_prop_auto_connect);
   _stringshare_del(&e_connman_prop_setup_required);
   _stringshare_del(&e_connman_prop_apn);
   _stringshare_del(&e_connman_prop_mcc);
   _stringshare_del(&e_connman_prop_mnc);
   _stringshare_del(&e_connman_prop_roaming);
   _stringshare_del(&e_connman_prop_technology_default);
   _stringshare_del(&e_connman_prop_technologies_available);
   _stringshare_del(&e_connman_prop_technologies_enabled);
   _stringshare_del(&e_connman_prop_technologies_connected);

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
   eina_log_domain_unregister(_e_dbus_connman_log_dom);
   e_connman_conn = NULL;

   return init_count;
}

