#include "E_Connman.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

static void
_method_success_check(void *data, DBusMessage *msg, DBusError *error)
{
   const char *name = data;

   if ((!error) || (!dbus_error_is_set(error)))
     {
	printf("SUCCESS: method %s() finished successfully.\n", name);
	return;
     }

   printf("FAILURE: method %s() finished with error: %s %s\n",
	  name, error->name, error->message);
   dbus_error_free(error);
}

static void
_elements_print(E_Connman_Element **elements, unsigned int count)
{
   unsigned int i;
   for (i = 0; i < count; i++)
     {
	printf("--- element %d:\n", i);
	e_connman_element_print(stdout, elements[i]);
     }
   free(elements);
   printf("END: all elements count = %u\n", count);
}

static void
_strings_print(const char **strings, unsigned int count)
{
   unsigned int i;
   for (i = 0; i < count; i++)
     printf("--- strings %d: \"%s\"\n", i, strings[i]);
   free(strings);
   printf("END: all strings count = %u\n", count);
}

static int
_on_element_add(void *data, int type, void *info)
{
   E_Connman_Element *element = info;
   printf(">>> %s\n", element->path);
   return 1;
}

static int
_on_element_del(void *data, int type, void *info)
{
   E_Connman_Element *element = info;
   printf("<<< %s\n", element->path);
   return 1;
}

static int
_on_element_updated(void *data, int type, void *info)
{
   E_Connman_Element *element = info;
   printf("!!! %s\n", element->path);
   e_connman_element_print(stderr, element);
   return 1;
}

static int
_on_cmd_quit(char *cmd, char *args)
{
   fputs("Bye!\n", stderr);
   ecore_main_loop_quit();
   return 0;
}

static int
_on_cmd_sync(char *cmd, char *args)
{
   e_connman_manager_sync_elements();
   return 1;
}

static char *
_tok(char *p)
{
   p = strchr(p, ' ');
   if (!p)
     return NULL;

   *p = '\0';
   p++;
   while (isspace(*p))
     p++;
   if (*p == '\0')
     return NULL;

   return p;
}

static int
_on_cmd_get_all(char *cmd, char *args)
{
   E_Connman_Element **elements;
   char *type;
   unsigned int count;
   bool ret;

   if (!args)
     type = NULL;
   else
     type = args;

   if (type)
     ret = e_connman_elements_get_all_type(type, &count, &elements);
   else
     ret = e_connman_elements_get_all(&count, &elements);

   if (!ret)
     fputs("ERROR: could not get elements\n", stderr);
   else
     {
	printf("BEG: all elements type=%s count = %d\n", type, count);
	_elements_print(elements, count);
     }

   return 1;
}

static E_Connman_Element *
_element_from_args(char *args, char **next_args)
{
   E_Connman_Element *element;

   if (!args)
     {
	fputs("ERROR: missing element path\n", stderr);
	*next_args = NULL;
	return NULL;
     }

   *next_args = _tok(args);
   element = e_connman_element_get(args);
   if (!element)
     fprintf(stderr, "ERROR: no element called \"%s\".\n", args);

   return element;
}

static int
_on_cmd_print(char *cmd, char *args)
{
   char *next_args;
   E_Connman_Element *element = _element_from_args(args, &next_args);
   if (element)
     e_connman_element_print(stdout, element);
   return 1;
}

static int
_on_cmd_get_properties(char *cmd, char *args)
{
   char *next_args;
   E_Connman_Element *element = _element_from_args(args, &next_args);
   if (element)
     e_connman_element_properties_sync(element);
   return 1;
}

static int
_on_cmd_property_set(char *cmd, char *args)
{
   char *next_args, *name, *p;
   E_Connman_Element *element = _element_from_args(args, &next_args);
   void *value;
   long vlong;
   unsigned short vu16;
   unsigned int vu32;
   int type;

   if (!element)
     return 1;

   if (!next_args)
     {
	fputs("ERROR: missing parameters name, type and value.\n", stderr);
	return 1;
     }

   name = next_args;
   p = _tok(name);
   if (!p)
     {
	fputs("ERROR: missing parameters type and value.\n", stderr);
	return 1;
     }

   next_args = _tok(p);
   if (!next_args)
     {
	fputs("ERROR: missing parameter value.\n", stderr);
	return 1;
     }

   type = p[0];
   switch (type)
     {
      case DBUS_TYPE_BOOLEAN:
	 vlong = !!atol(next_args);
	 value = (void *)vlong;
	 fprintf(stderr, "DBG: boolean is: %ld\n", vlong);
	 break;
      case DBUS_TYPE_UINT16:
	 vu16 = strtol(next_args, &p, 0);
	 if (p == next_args)
	   {
	      fprintf(stderr, "ERROR: invalid number \"%s\".\n", next_args);
	      return 1;
	   }
	 value = &vu16;
	 fprintf(stderr, "DBG: u16 is: %hu\n", vu16);
	 break;
      case DBUS_TYPE_UINT32:
	 vu32 = strtol(next_args, &p, 0);
	 if (p == next_args)
	   {
	      fprintf(stderr, "ERROR: invalid number \"%s\".\n", next_args);
	      return 1;
	   }
	 value = &vu32;
	 fprintf(stderr, "DBG: u16 is: %u\n", vu32);
	 break;
      case DBUS_TYPE_STRING:
      case DBUS_TYPE_OBJECT_PATH:
	 p = next_args + strlen(next_args);
	 if (p > next_args)
	   p--;
	 while (p > next_args && isspace(*p))
	   p--;
	 if (p <= next_args)
	   {
	      fprintf(stderr, "ERROR: invalid string \"%s\".\n", next_args);
	   }
	 p[1] = '\0';
	 value = next_args;
	 fprintf(stderr, "DBG: string is: \"%s\"\n", next_args);
	 break;
      default:
	 fprintf(stderr, "ERROR: don't know how to parse type '%c' (%d)\n",
		 type, type);
	 return 1;
     }

   fprintf(stderr, "set_property %s [%p] %s %c %p...\n",
	   args, element, name, type, value);
   if (!e_connman_element_property_set(element, name, type, value))
     {
	fputs("ERROR: error setting property.\n", stderr);
	return 1;
     }
   return 1;
}


/* Manager Commands */

static int
_on_cmd_manager_get(char *cmd, char *args)
{
   E_Connman_Element *element;
   element = e_connman_manager_get();
   e_connman_element_print(stderr, element);
   return 1;
}

static int
_on_cmd_manager_get_profiles(char *cmd, char *args)
{
   unsigned int count;
   E_Connman_Element **profiles;

   if (!e_connman_manager_profiles_get(&count, &profiles))
     {
	fputs("ERROR: can't get profiles\n", stderr);
	return 1;
     }
   printf("BEG: all manager profiles elements count = %d\n", count);
   _elements_print(profiles, count);
   return 1;
}

static int
_on_cmd_manager_get_devices(char *cmd, char *args)
{
   unsigned int count;
   E_Connman_Element **devices;

   if (!e_connman_manager_devices_get(&count, &devices))
     {
	fputs("ERROR: can't get devices\n", stderr);
	return 1;
     }
   printf("BEG: all manager devices elements count = %d\n", count);
   _elements_print(devices, count);
   return 1;
}

static int
_on_cmd_manager_get_services(char *cmd, char *args)
{
   unsigned int count;
   E_Connman_Element **services;

   if (!e_connman_manager_services_get(&count, &services))
     {
	fputs("ERROR: can't get services\n", stderr);
	return 1;
     }
   printf("BEG: all manager services elements count = %d\n", count);
   _elements_print(services, count);
   return 1;
}

static int
_on_cmd_manager_register_agent(char *cmd, char *args)
{
   char *path;

   if (!args)
     {
	fputs("ERROR: missing the object path\n", stderr);
	return 1;
     }

   path = args;
   if (e_connman_manager_agent_register
       (path, _method_success_check, "manager_register_agent"))
     printf(":::Registering agent %s...\n", path);
   else
     fprintf(stderr, "ERROR: can't register agent %s\n", path);

   return 1;
}

static int
_on_cmd_manager_unregister_agent(char *cmd, char *args)
{
   char *path;

   if (!args)
     {
	fputs("ERROR: missing the object path\n", stderr);
	return 1;
     }

   path = args;
   if (e_connman_manager_agent_unregister
       (path, _method_success_check, "manager_unregister_agent"))
     printf(":::Unregistering agent %s...\n", path);
   else
     fprintf(stderr, "ERROR: can't unregister agent %s\n", path);

   return 1;
}

static int
_on_cmd_manager_get_state(char *cmd, char *args)
{
   const char *state;
   if (e_connman_manager_state_get(&state))
     printf(":::Manager state = \"%s\"\n", state);
   else
     fputs("ERROR: can't get manager state\n", stderr);
   return 1;
}

static int
_on_cmd_manager_get_offline_mode(char *cmd, char *args)
{
   bool offline;
   if (e_connman_manager_offline_mode_get(&offline))
     printf(":::Manager Offline Mode = %hhu\n", offline);
   else
     fputs("ERROR: can't get manager offline mode\n", stderr);
   return 1;
}

static int
_on_cmd_manager_set_offline_mode(char *cmd, char *args)
{
   bool offline;
   if (!args)
     {
	fputs("ERROR: missing the offline mode value\n", stderr);
	return 1;
     }
   _tok(args);
   offline = !!atol(args);
   if (e_connman_manager_offline_mode_set
       (offline, _method_success_check, "manager_set_offline_mode"))

     printf(":::Manager Offline Mode set to %hhu\n", offline);
   else
     fputs("ERROR: can't set manager offline mode\n", stderr);
   return 1;
}

static int
_on_cmd_manager_request_scan(char *cmd, char *args)
{
   if (args)
     _tok(args);

   if (!args)
     args = "";

   if (e_connman_manager_request_scan
       (args, _method_success_check, "manager_request_scan"))
     printf(":::Manager Request Scan for %s\n", args[0] ? args : "<all>");
   else
     fputs("ERROR: can't request scan on manager\n", stderr);
   return 1;
}

static int
_on_cmd_manager_technology_enable(char *cmd, char *args)
{
   if (!args)
     {
	fputs("ERROR: missing the technology type\n", stderr);
	return 1;
     }
   _tok(args);

   if (e_connman_manager_technology_enable
       (args, _method_success_check, "manager_technology_enable"))
     printf(":::Manager Enable Technology %s\n", args);
   else
     fputs("ERROR: can't enable technology on manager\n", stderr);
   return 1;
}

static int
_on_cmd_manager_technology_disable(char *cmd, char *args)
{
   if (!args)
     {
	fputs("ERROR: missing the technology type\n", stderr);
	return 1;
     }
   _tok(args);

   if (e_connman_manager_technology_disable
       (args, _method_success_check, "manager_technology_disable"))
     printf(":::Manager Disable Technology %s\n", args);
   else
     fputs("ERROR: can't disable technology on manager\n", stderr);
   return 1;
}

static int
_on_cmd_manager_get_technologies_available(char *cmd, char *args)
{
   const char **strings;
   unsigned int count;

   if (!e_connman_manager_technologies_available_get(&count, &strings))
     fputs("ERROR: can't get available technologies\n", stderr);
   else
     {
	printf("BEG: available technologies count = %u\n", count);
	_strings_print(strings, count);
     }

   return 1;
}

static int
_on_cmd_manager_get_technologies_enabled(char *cmd, char *args)
{
   const char **strings;
   unsigned int count;

   if (!e_connman_manager_technologies_enabled_get(&count, &strings))
     fputs("ERROR: can't get enabled technologies\n", stderr);
   else
     {
	printf("BEG: enabled technologies count = %u\n", count);
	_strings_print(strings, count);
     }

   return 1;
}

static int
_on_cmd_manager_get_technologies_connected(char *cmd, char *args)
{
   const char **strings;
   unsigned int count;

   if (!e_connman_manager_technologies_connected_get(&count, &strings))
     fputs("ERROR: can't get connected technologies\n", stderr);
   else
     {
	printf("BEG: connected technologies count = %u\n", count);
	_strings_print(strings, count);
     }

   return 1;
}

static int
_on_cmd_manager_profile_remove(char *cmd, char *args)
{
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the profile path\n", stderr);
	return 1;
     }
   _tok(args);

   e = e_connman_profile_get(args);
   if (e_connman_manager_profile_remove
       (e, _method_success_check, "manager_profile_remove"))
     printf(":::Manager Remove Profile %s\n", args);
   else
     fputs("ERROR: can't remove profile from manager\n", stderr);
   return 1;
}

static int
_on_cmd_manager_profile_get_active(char *cmd, char *args)
{
   E_Connman_Element *e;

   if (!e_connman_manager_profile_active_get(&e))
     fputs("ERROR: can't active_get profile from manager\n", stderr);
   else
     e_connman_element_print(stderr, e);
   return 1;
}

static int
_on_cmd_manager_profile_set_active(char *cmd, char *args)
{
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the profile path\n", stderr);
	return 1;
     }
   _tok(args);

   e = e_connman_profile_get(args);
   if (e_connman_manager_profile_active_set
       (e, _method_success_check, "manager_profile_set_active"))
     printf(":::Manager Active Profile set to %s\n", args);
   else
     fputs("ERROR: can't set active profile\n", stderr);
   return 1;
}

/* Device Commands */
static int
_on_cmd_device_propose_scan(char *cmd, char *args)
{
   char *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the device path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_device_get(path);
   if (e_connman_device_propose_scan
       (e, _method_success_check, "device_propose_scan"))
     printf(":::Proposing scan %s...\n", path);
   else
     fputs("ERROR: can't propose scan\n", stderr);
   return 1;
}

static int
_on_cmd_device_get_address(char *cmd, char *args)
{
   const char *address, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the device path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_device_get(path);
   if (e_connman_device_address_get(e, &address))
     printf(":::Device %s Address = \"%s\"\n", path, address);
   else
     fputs("ERROR: can't get device address\n", stderr);
   return 1;
}

static int
_on_cmd_device_get_name(char *cmd, char *args)
{
   const char *name, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the device path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_device_get(path);
   if (e_connman_device_name_get(e, &name))
     printf(":::Device %s Name = \"%s\"\n", path, name);
   else
     fputs("ERROR: can't get device name\n", stderr);
   return 1;
}

static int
_on_cmd_device_get_type(char *cmd, char *args)
{
   const char *type, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the device path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_device_get(path);
   if (e_connman_device_type_get(e, &type))
     printf(":::Device %s Type = \"%s\"\n", path, type);
   else
     fputs("ERROR: can't get device type\n", stderr);
   return 1;
}

static int
_on_cmd_device_get_interface(char *cmd, char *args)
{
   const char *interface, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the device path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_device_get(path);
   if (e_connman_device_interface_get(e, &interface))
     printf(":::Device %s Interface = \"%s\"\n", path, interface);
   else
     fputs("ERROR: can't get device interface\n", stderr);
   return 1;
}

static int
_on_cmd_device_get_powered(char *cmd, char *args)
{
   char *path;
   bool powered;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the device path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_device_get(path);
   if (e_connman_device_powered_get(e, &powered))
     printf(":::Device %s Powered = %hhu\n", path, powered);
   else
     fputs("ERROR: can't get device powered\n", stderr);
   return 1;
}

static int
_on_cmd_device_set_powered(char *cmd, char *args)
{
   char *device_path, *next_args;
   bool powered;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the device path\n", stderr);
	return 1;
     }
   device_path = args;
   next_args = _tok(args);
   if (!next_args)
     {
	fputs("ERROR: missing the powered value\n", stderr);
	return 1;
     }
   powered = !!atol(next_args);

   e = e_connman_device_get(device_path);
   if (e_connman_device_powered_set
       (e, powered, _method_success_check, "device_set_powered"))
     printf(":::Device %s powered set to %hhu\n", device_path, powered);
   else
     fputs("ERROR: can't set device powered\n", stderr);
   return 1;
}

static int
_on_cmd_device_get_scan_interval(char *cmd, char *args)
{
   char *path;
   unsigned short scan_interval;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the device path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_device_get(path);
   if (e_connman_device_scan_interval_get(e, &scan_interval))
     printf(":::Device %s ScanInterval = %hu\n", path, scan_interval);
   else
     fputs("ERROR: can't get device scan interval\n", stderr);
   return 1;
}

static int
_on_cmd_device_set_scan_interval(char *cmd, char *args)
{
   char *device_path, *next_args, *p;
   unsigned short scan_interval;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the device path\n", stderr);
	return 1;
     }
   device_path = args;
   next_args = _tok(args);
   if (!next_args)
     {
	fputs("ERROR: missing the scan interval value\n", stderr);
	return 1;
     }
   scan_interval = strtol(next_args, &p, 0);
   if (p == next_args)
     {
	fprintf(stderr, "ERROR: invalid number \"%s\".\n", next_args);
	return 1;
     }

   e = e_connman_device_get(device_path);
   if (e_connman_device_scan_interval_set
       (e, scan_interval, _method_success_check, "device_set_scan_interval"))
     printf(":::Device %s scan interval set to %hu\n", device_path, scan_interval);
   else
     fputs("ERROR: can't set device scan interval\n", stderr);
   return 1;
}

static int
_on_cmd_device_get_scanning(char *cmd, char *args)
{
   char *path;
   bool scanning;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the device path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_device_get(path);
   if (e_connman_device_scanning_get(e, &scanning))
     printf(":::Device %s Scanning = %hhu\n", path, scanning);
   else
     fputs("ERROR: can't get device scanning\n", stderr);
   return 1;
}

static int
_on_cmd_device_get_networks(char *cmd, char *args)
{
   E_Connman_Element **networks;
   unsigned int count;
   char *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the device path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_device_get(path);
   if (!e_connman_device_networks_get(e, &count, &networks))
     {
	fputs("ERROR: can't get networks\n", stderr);
	return 1;
     }

   printf("BEG: all device network elements count = %d\n", count);
   _elements_print(networks, count);
   return 1;
}

/* Profile Commands */

static int
_on_cmd_profile_get_name(char *cmd, char *args)
{
   const char *name, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the profile path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_profile_get(path);
   if (e_connman_profile_name_get(e, &name))
     printf(":::Profile %s Name = \"%s\"\n", path, name);
   else
     fputs("ERROR: can't get profile name\n", stderr);
   return 1;
}

static int
_on_cmd_profile_set_name(char *cmd, char *args)
{
   char *path, *next_args;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the profile path\n", stderr);
	return 1;
     }
   path = args;
   next_args = _tok(args);
   if (!next_args)
     {
	fputs("ERROR: missing the offline mode value\n", stderr);
	return 1;
     }
   _tok(next_args);

   e = e_connman_profile_get(path);
   if (e_connman_profile_name_set
       (e, next_args, _method_success_check, "profile_set_name"))
     printf(":::Profile %s Name set to %s\n", path, next_args);
   else
     fputs("ERROR: can't set profile name\n", stderr);
   return 1;
}

static int
_on_cmd_profile_get_offline_mode(char *cmd, char *args)
{
   char *path;
   bool offline;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the profile path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_profile_get(path);
   if (e_connman_profile_offline_mode_get(e, &offline))
     printf(":::Profile  %s Offline Mode = %hhu\n", path, offline);
   else
     fputs("ERROR: can't get profile offline mode\n", stderr);
   return 1;
}

static int
_on_cmd_profile_set_offline_mode(char *cmd, char *args)
{
   char *path, *next_args;
   bool offline;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the profile path\n", stderr);
	return 1;
     }
   path = args;
   next_args = _tok(args);
   if (!next_args)
     {
	fputs("ERROR: missing the offline mode value\n", stderr);
	return 1;
     }
   _tok(next_args);
   offline = !!atol(next_args);

   e = e_connman_profile_get(path);
   if (e_connman_profile_offline_mode_set
       (e, offline, _method_success_check, "profile_set_offline_mode"))
     printf(":::Profile %s Offline Mode set to %hhu\n", path, offline);
   else
     fputs("ERROR: can't set profile offline mode\n", stderr);
   return 1;
}

static int
_on_cmd_profile_get_services(char *cmd, char *args)
{
   E_Connman_Element **services;
   E_Connman_Element *e;
   unsigned int count;
   char *path;

   if (!args)
     {
	fputs("ERROR: missing the profile path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_profile_get(path);
   if (!e_connman_profile_services_get(e, &count, &services))
     {
	fputs("ERROR: can't get services\n", stderr);
	return 1;
     }
   printf("BEG: all profile services count = %d\n", count);
   _elements_print(services, count);
   return 1;
}


/* Network Commands */

static int
_on_cmd_network_get_address(char *cmd, char *args)
{
   const char *address, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the network path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_network_get(path);
   if (e_connman_network_address_get(e, &address))
     printf(":::Network %s Address = \"%s\"\n", path, address);
   else
     fputs("ERROR: can't get network address\n", stderr);
   return 1;
}

static int
_on_cmd_network_get_name(char *cmd, char *args)
{
   const char *name, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the network path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_network_get(path);
   if (e_connman_network_name_get(e, &name))
     printf(":::Network %s Name = \"%s\"\n", path, name);
   else
     fputs("ERROR: can't get network name\n", stderr);
   return 1;
}

static int
_on_cmd_network_get_connected(char *cmd, char *args)
{
   char *path;
   bool connected;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the network path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_network_get(path);
   if (e_connman_network_connected_get(e, &connected))
     printf(":::Network %s Connected = %hhu\n", path, connected);
   else
     fputs("ERROR: can't get network connected\n", stderr);
   return 1;
}

static int
_on_cmd_network_get_strength(char *cmd, char *args)
{
   char *path;
   unsigned char strength;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the network path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_network_get(path);
   if (e_connman_network_strength_get(e, &strength))
     printf(":::Network %s Strength = %#02hhx (%d)\n", path, strength, strength);
   else
     fputs("ERROR: can't get network strength\n", stderr);
   return 1;
}

static int
_on_cmd_network_get_frequency(char *cmd, char *args)
{
   char *path;
   unsigned short frequency;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the network path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_network_get(path);
   if (e_connman_network_frequency_get(e, &frequency))
     printf(":::Network %s Frequency = %#04hx (%d)\n", path, frequency, frequency);
   else
     fputs("ERROR: can't get network frequency\n", stderr);
   return 1;
}

static int
_on_cmd_network_get_device(char *cmd, char *args)
{
   E_Connman_Element *e, *device;
   char *path;

   if (!args)
     {
	fputs("ERROR: missing the network path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_network_get(path);
   if (!e_connman_network_device_get(e, &device))
     fputs("ERROR: can't get network device\n", stderr);
   else
     e_connman_element_print(stderr, device);
   return 1;
}

static int
_on_cmd_network_get_wifi_ssid(char *cmd, char *args)
{
   unsigned char *bytes;
   char *path;
   unsigned int i, count;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the network path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_network_get(path);
   if (e_connman_network_wifi_ssid_get(e, &count, &bytes))
     {
	printf(":::Network %s Wifi SSID = ", path);
	for (i = 0; i < count; i++)
	  printf("%#02hhx (\"%c\"), ", bytes[i], bytes[i]);
	printf("\n");
     }
   else
     fputs("ERROR: can't get network wifi ssid\n", stderr);
   return 1;
}

static int
_on_cmd_network_get_wifi_mode(char *cmd, char *args)
{
   const char *wifi_mode, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the network path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_network_get(path);
   if (e_connman_network_wifi_mode_get(e, &wifi_mode))
     printf(":::Network %s Wifi Mode = \"%s\"\n", path, wifi_mode);
   else
     fputs("ERROR: can't get network wifi mode\n", stderr);
   return 1;
}

static int
_on_cmd_network_get_wifi_security(char *cmd, char *args)
{
   const char *wifi_security, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the network path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_network_get(path);
   if (e_connman_network_wifi_security_get(e, &wifi_security))
     printf(":::Network %s Wifi Security = \"%s\"\n", path, wifi_security);
   else
     fputs("ERROR: can't get network wifi security\n", stderr);
   return 1;
}

static int
_on_cmd_network_get_wifi_passphrase(char *cmd, char *args)
{
   const char *wifi_passphrase, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the network path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_network_get(path);
   if (e_connman_network_wifi_passphrase_get(e, &wifi_passphrase))
     printf(":::Network %s Wifi Passphrase = \"%s\"\n", path, wifi_passphrase);
   else
     fputs("ERROR: can't get network wifi passphrase\n", stderr);
   return 1;
}

static int
_on_cmd_network_get_wifi_channel(char *cmd, char *args)
{
   char *path;
   E_Connman_Element *e;
   unsigned short wifi_channel;

   if (!args)
     {
	fputs("ERROR: missing the network path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_network_get(path);
   if (e_connman_network_wifi_channel_get(e, &wifi_channel))
     printf(":::Network %s Wifi Channel = %#02hx (%d)\n", path, wifi_channel, wifi_channel);
   else
     fputs("ERROR: can't get network wifi channel\n", stderr);
   return 1;
}

static int
_on_cmd_network_get_wifi_eap(char *cmd, char *args)
{
   const char *wifi_eap, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the network path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_network_get(path);
   if (e_connman_network_wifi_eap_get(e, &wifi_eap))
     printf(":::Network %s Wifi EAP = \"%s\"\n", path, wifi_eap);
   else
     fputs("ERROR: can't get network wifi eap\n", stderr);
   return 1;
}

/* Services Commands */
static int
_on_cmd_service_connect(char *cmd, char *args)
{
   char *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_connect
       (e, _method_success_check, "service_connect"))
     printf(":::Connecting to Service %s...\n", path);
   else
     fputs("ERROR: can't connect to service\n", stderr);
   return 1;
}

static int
_on_cmd_service_disconnect(char *cmd, char *args)
{
   char *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_disconnect
       (e, _method_success_check, "service_disconnect"))
     printf(":::Disconnecting Service %s...\n", path);
   else
     fputs("ERROR: can't disconnect service\n", stderr);
   return 1;
}

static int
_on_cmd_service_remove(char *cmd, char *args)
{
   char *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_remove
       (e, _method_success_check, "service_remove"))
     printf(":::Removing Service %s...\n", path);
   else
     fputs("ERROR: can't remove service\n", stderr);
   return 1;
}

static int
_on_cmd_service_move_before(char *cmd, char *args)
{
   char *path, *service_path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   service_path = args;
   path = _tok(args);

   if (!path)
     {
	fputs("ERROR: missing the object service\n", stderr);
	return 1;
     }
   _tok(path);

   e = e_connman_service_get(service_path);
   if (e_connman_service_move_before
       (e, path, _method_success_check, "service_move_before"))
     printf(":::Moving before %s...\n", path);
   else
     fputs("ERROR: can't move before\n", stderr);
   return 1;
}

static int
_on_cmd_service_move_after(char *cmd, char *args)
{
   char *path, *service_path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   service_path = args;
   path = _tok(args);

   if (!path)
     {
	fputs("ERROR: missing the object service\n", stderr);
	return 1;
     }
   _tok(path);

   e = e_connman_service_get(service_path);
   if (e_connman_service_move_after
       (e, path, _method_success_check, "service_move_after"))
     printf(":::Moving after %s...\n", path);
   else
     fputs("ERROR: can't move after\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_state(char *cmd, char *args)
{
   const char *state, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_state_get(e, &state))
     printf(":::Service %s State = \"%s\"\n", path, state);
   else
     fputs("ERROR: can't get service state\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_error(char *cmd, char *args)
{
   const char *error, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_error_get(e, &error))
     printf(":::Service %s Error = \"%s\"\n", path, error);
   else
     fputs("ERROR: can't get service error\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_name(char *cmd, char *args)
{
   const char *name, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_name_get(e, &name))
     printf(":::Service %s Name = \"%s\"\n", path, name);
   else
     fputs("ERROR: can't get service name\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_type(char *cmd, char *args)
{
   const char *type, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_type_get(e, &type))
     printf(":::Service %s Type = \"%s\"\n", path, type);
   else
     fputs("ERROR: can't get service type\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_mode(char *cmd, char *args)
{
   const char *mode, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_mode_get(e, &mode))
     printf(":::Service %s Mode = \"%s\"\n", path, mode);
   else
     fputs("ERROR: can't get service mode\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_security(char *cmd, char *args)
{
   const char *security, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_security_get(e, &security))
     printf(":::Service %s Security = \"%s\"\n", path, security);
   else
     fputs("ERROR: can't get service security\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_passphrase(char *cmd, char *args)
{
   const char *passphrase, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_passphrase_get(e, &passphrase))
     printf(":::Service %s Passphrase = \"%s\"\n", path, passphrase);
   else
     fputs("ERROR: can't get service passphrase\n", stderr);
   return 1;
}

static int
_on_cmd_service_set_passphrase(char *cmd, char *args)
{
   char *passphrase, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   path = args;
   passphrase = _tok(args);

   if (!passphrase)
     {
	fputs("ERROR: missing the passphrase value\n", stderr);
	return 1;
     }
   _tok(passphrase);

   e = e_connman_service_get(path);
   if (e_connman_service_passphrase_set
       (e, passphrase, _method_success_check, "service_set_passphrase"))
     printf(":::Service %s passphrase set to \"%s\"\n", path, passphrase);
   else
     fputs("ERROR: can't set service passphrase\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_passphrase_required(char *cmd, char *args)
{
   const char *path;
   bool passphrase;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_passphrase_required_get(e, &passphrase))
     printf(":::Service %s Passphrase Required = %hhu\n", path, passphrase);
   else
     fputs("ERROR: can't get service passphrase required\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_strength(char *cmd, char *args)
{
   const char *path;
   unsigned char strength;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_strength_get(e, &strength))
     printf(":::Service %s Strength = %#02hhx (%d)\n", path, strength, strength);
   else
     fputs("ERROR: can't get service strength\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_favorite(char *cmd, char *args)
{
   const char *path;
   bool favorite;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_favorite_get(e, &favorite))
     printf(":::Service %s Favorite = %hhu\n", path, favorite);
   else
     fputs("ERROR: can't get service favorite\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_immutable(char *cmd, char *args)
{
   const char *path;
   bool immutable;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_immutable_get(e, &immutable))
     printf(":::Service %s Immutable = %hhu\n", path, immutable);
   else
     fputs("ERROR: can't get service immutable\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_auto_connect(char *cmd, char *args)
{
   const char *path;
   bool auto_connect;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_auto_connect_get(e, &auto_connect))
     printf(":::Service %s Auto Connect = %hhu\n", path, auto_connect);
   else
     fputs("ERROR: can't get service auto connect\n", stderr);
   return 1;
}

static int
_on_cmd_service_set_auto_connect(char *cmd, char *args)
{
   char *path, *next_args;
   bool auto_connect;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   path = args;
   next_args = _tok(args);

   if (!next_args)
     {
	fputs("ERROR: missing the auto connect value\n", stderr);
	return 1;
     }
   _tok(next_args);
   auto_connect = !!atol(next_args);

   e = e_connman_service_get(path);
   if (e_connman_service_auto_connect_set
       (e, auto_connect, _method_success_check, "service_set_auto_connect"))
     printf(":::Service %s auto connect set to %d\n", path, auto_connect);
   else
     fputs("ERROR: can't set service auto connect\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_setup_required(char *cmd, char *args)
{
   const char *path;
   bool setup_required;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_setup_required_get(e, &setup_required))
     printf(":::Service %s Setup Required = %hhu\n", path, setup_required);
   else
     fputs("ERROR: can't get service setup required\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_apn(char *cmd, char *args)
{
   const char *apn, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_apn_get(e, &apn))
     printf(":::Service %s APN = \"%s\"\n", path, apn);
   else
     fputs("ERROR: can't get service APN\n", stderr);
   return 1;
}

static int
_on_cmd_service_set_apn(char *cmd, char *args)
{
   char *apn, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   path = args;
   apn = _tok(args);

   if (!apn)
     {
	fputs("ERROR: missing the apn value\n", stderr);
	return 1;
     }
   _tok(apn);

   e = e_connman_service_get(path);
   if (e_connman_service_apn_set
       (e, apn, _method_success_check, "service_set_apn"))
     printf(":::Service %s APN set to \"%s\"\n", path, apn);
   else
     fputs("ERROR: can't set service APN\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_mcc(char *cmd, char *args)
{
   const char *mcc, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_mcc_get(e, &mcc))
     printf(":::Service %s MCC = \"%s\"\n", path, mcc);
   else
     fputs("ERROR: can't get service MCC\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_mnc(char *cmd, char *args)
{
   const char *mnc, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_mnc_get(e, &mnc))
     printf(":::Service %s MNC = \"%s\"\n", path, mnc);
   else
     fputs("ERROR: can't get service MNC\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_roaming(char *cmd, char *args)
{
   const char *path;
   bool roaming;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_roaming_get(e, &roaming))
     printf(":::Service %s Roaming = %hhu\n", path, roaming);
   else
     fputs("ERROR: can't get service roaming\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_ipv4_method(char *cmd, char *args)
{
   const char *ipv4_method, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_ipv4_method_get(e, &ipv4_method))
     printf(":::Service %s IPv4 Method = \"%s\"\n", path, ipv4_method);
   else
     fputs("ERROR: can't get service ipv4 method\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_ipv4_address(char *cmd, char *args)
{
   const char *ipv4_address, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_ipv4_address_get(e, &ipv4_address))
     printf(":::Service %s IPv4 Address = \"%s\"\n", path, ipv4_address);
   else
     fputs("ERROR: can't get service ipv4 address\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_ipv4_gateway(char *cmd, char *args)
{
   const char *ipv4_gateway, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_ipv4_gateway_get(e, &ipv4_gateway))
     printf(":::Service %s IPv4 Gateway = \"%s\"\n", path, ipv4_gateway);
   else
     fputs("ERROR: can't get service ipv4 gateway\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_ipv4_netmask(char *cmd, char *args)
{
   const char *ipv4_netmask, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_ipv4_netmask_get(e, &ipv4_netmask))
     printf(":::Service %s IPv4 Netmask = \"%s\"\n", path, ipv4_netmask);
   else
     fputs("ERROR: can't get service ipv4 netmask\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_ipv4_configuration_method(char *cmd, char *args)
{
   const char *ipv4_method, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_ipv4_configuration_method_get(e, &ipv4_method))
     printf(":::Service %s IPv4 Configuration Method = \"%s\"\n",
	    path, ipv4_method);
   else
     fputs("ERROR: can't get service ipv4_configuration method\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_ipv4_configuration_address(char *cmd, char *args)
{
   const char *ipv4_address, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_ipv4_configuration_address_get(e, &ipv4_address))
     printf(":::Service %s IPv4 Configuration Address = \"%s\"\n",
	    path, ipv4_address);
   else
     fputs("ERROR: can't get service ipv4_configuration address\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_ipv4_configuration_gateway(char *cmd, char *args)
{
   const char *ipv4_gateway, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_ipv4_configuration_gateway_get(e, &ipv4_gateway))
     printf(":::Service %s IPv4 Configuration Gateway = \"%s\"\n",
	    path, ipv4_gateway);
   else
     fputs("ERROR: can't get service ipv4_configuration gateway\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_ipv4_configuration_netmask(char *cmd, char *args)
{
   const char *ipv4_netmask, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_ipv4_configuration_netmask_get(e, &ipv4_netmask))
     printf(":::Service %s IPv4 Configuration Netmask = \"%s\"\n",
	    path, ipv4_netmask);
   else
     fputs("ERROR: can't get service ipv4 configuration netmask\n", stderr);
   return 1;
}

static int
_on_cmd_service_ipv4_configure_dhcp(char *cmd, char *args)
{
   char *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   path = args;
   _tok(args);

   e = e_connman_service_get(path);
   if (e_connman_service_ipv4_configure_dhcp
       (e, _method_success_check, "service_ipv4_configure_dhcp"))
     printf(":::Service %s IPv4 Configuration set to DHCP\n", path);
   else
     fputs("ERROR: can't set service ipv4_configuration dhcp\n", stderr);
   return 1;
}

static int
_on_cmd_service_ipv4_configure_manual(char *cmd, char *args)
{
   char *path, *next_args, *address, *netmask = NULL;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   path = args;
   next_args = _tok(args);
   if (!next_args)
     {
	fputs("ERROR: missing the service address\n", stderr);
	return 1;
     }

   address = next_args;
   next_args = _tok(next_args);
   if (next_args)
     {
	netmask = next_args;
	_tok(next_args);
     }

   e = e_connman_service_get(path);
   if (e_connman_service_ipv4_configure_manual
       (e, address, netmask,
	_method_success_check, "service_ipv4_configure_manual"))
     printf(":::Service %s IPv4 Configuration set to Manual (%s/%s)\n",
	    path, address, netmask);
   else
     fputs("ERROR: can't set service ipv4_configuration manual\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_ethernet_method(char *cmd, char *args)
{
   const char *ethernet_method, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_ethernet_method_get(e, &ethernet_method))
     printf(":::Service %s Ethernet Method = \"%s\"\n", path, ethernet_method);
   else
     fputs("ERROR: can't get service ethernet method\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_ethernet_address(char *cmd, char *args)
{
   const char *ethernet_address, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_ethernet_address_get(e, &ethernet_address))
     printf(":::Service %s Ethernet Address = \"%s\"\n",
	    path, ethernet_address);
   else
     fputs("ERROR: can't get service ethernet address\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_ethernet_mtu(char *cmd, char *args)
{
   const char *path;
   E_Connman_Element *e;
   unsigned short ethernet_mtu;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_ethernet_mtu_get(e, &ethernet_mtu))
     printf(":::Service %s Ethernet MTU = %hu\n", path, ethernet_mtu);
   else
     fputs("ERROR: can't get service ethernet mtu\n", stderr);
   return 1;
}

static int
_on_cmd_service_get_ethernet_netmask(char *cmd, char *args)
{
   const char *ethernet_netmask, *path;
   E_Connman_Element *e;

   if (!args)
     {
	fputs("ERROR: missing the service path\n", stderr);
	return 1;
     }
   _tok(args);
   path = args;

   e = e_connman_service_get(path);
   if (e_connman_service_ethernet_netmask_get(e, &ethernet_netmask))
     printf(":::Service %s Ethernet Netmask = \"%s\"\n",
	    path, ethernet_netmask);
   else
     fputs("ERROR: can't get service ethernet netmask\n", stderr);
   return 1;
}


static int
_on_input(void *data, Ecore_Fd_Handler *fd_handler)
{
   char buf[256];
   char *cmd, *args;
   const struct {
      const char *cmd;
      int (*cb)(char *cmd, char *args);
   } *itr, maps[] = {
     {"quit", _on_cmd_quit},
     {"sync", _on_cmd_sync},
     {"get_all", _on_cmd_get_all},
     {"print", _on_cmd_print},
     {"get_properties", _on_cmd_get_properties},
     {"set_property", _on_cmd_property_set},
     {"manager_get", _on_cmd_manager_get},
     {"manager_get_profiles", _on_cmd_manager_get_profiles},
     {"manager_get_devices", _on_cmd_manager_get_devices},
     {"manager_get_services", _on_cmd_manager_get_services},
     {"manager_register_agent", _on_cmd_manager_register_agent},
     {"manager_unregister_agent", _on_cmd_manager_unregister_agent},
     {"manager_get_state", _on_cmd_manager_get_state},
     {"manager_get_offline_mode", _on_cmd_manager_get_offline_mode},
     {"manager_set_offline_mode", _on_cmd_manager_set_offline_mode},
     {"manager_request_scan", _on_cmd_manager_request_scan},
     {"manager_technology_enable", _on_cmd_manager_technology_enable},
     {"manager_technology_disable", _on_cmd_manager_technology_disable},
     {"manager_get_technologies_available", _on_cmd_manager_get_technologies_available},
     {"manager_get_technologies_enabled", _on_cmd_manager_get_technologies_enabled},
     {"manager_get_technologies_connected", _on_cmd_manager_get_technologies_connected},
     {"manager_profile_remove", _on_cmd_manager_profile_remove},
     {"manager_profile_get_active", _on_cmd_manager_profile_get_active},
     {"manager_profile_set_active", _on_cmd_manager_profile_set_active},
     {"device_propose_scan", _on_cmd_device_propose_scan},
     {"device_get_address", _on_cmd_device_get_address},
     {"device_get_name", _on_cmd_device_get_name},
     {"device_get_type", _on_cmd_device_get_type},
     {"device_get_interface", _on_cmd_device_get_interface},
     {"device_get_powered", _on_cmd_device_get_powered},
     {"device_set_powered", _on_cmd_device_set_powered},
     {"device_get_scan_interval", _on_cmd_device_get_scan_interval},
     {"device_set_scan_interval", _on_cmd_device_set_scan_interval},
     {"device_get_scanning", _on_cmd_device_get_scanning},
     {"device_get_networks", _on_cmd_device_get_networks},
     {"profile_get_name", _on_cmd_profile_get_name},
     {"profile_set_name", _on_cmd_profile_set_name},
     {"profile_get_offline_mode", _on_cmd_profile_get_offline_mode},
     {"profile_set_offline_mode", _on_cmd_profile_set_offline_mode},
     {"profile_get_services", _on_cmd_profile_get_services},
     {"network_get_address", _on_cmd_network_get_address},
     {"network_get_name", _on_cmd_network_get_name},
     {"network_get_connected", _on_cmd_network_get_connected},
     {"network_get_strength", _on_cmd_network_get_strength},
     {"network_get_frequency", _on_cmd_network_get_frequency},
     {"network_get_device", _on_cmd_network_get_device},
     {"network_get_wifi_ssid", _on_cmd_network_get_wifi_ssid},
     {"network_get_wifi_mode", _on_cmd_network_get_wifi_mode},
     {"network_get_wifi_security", _on_cmd_network_get_wifi_security},
     {"network_get_wifi_passphrase", _on_cmd_network_get_wifi_passphrase},
     {"network_get_wifi_channel", _on_cmd_network_get_wifi_channel},
     {"network_get_wifi_eap", _on_cmd_network_get_wifi_eap},
     {"service_connect", _on_cmd_service_connect},
     {"service_disconnect", _on_cmd_service_disconnect},
     {"service_remove", _on_cmd_service_remove},
     {"service_move_before", _on_cmd_service_move_before},
     {"service_move_after", _on_cmd_service_move_after},
     {"service_get_state", _on_cmd_service_get_state},
     {"service_get_error", _on_cmd_service_get_error},
     {"service_get_name", _on_cmd_service_get_name},
     {"service_get_type", _on_cmd_service_get_type},
     {"service_get_mode", _on_cmd_service_get_mode},
     {"service_get_security", _on_cmd_service_get_security},
     {"service_get_passphrase", _on_cmd_service_get_passphrase},
     {"service_set_passphrase", _on_cmd_service_set_passphrase},
     {"service_get_passphrase_required", _on_cmd_service_get_passphrase_required},
     {"service_get_strength", _on_cmd_service_get_strength},
     {"service_get_favorite", _on_cmd_service_get_favorite},
     {"service_get_immutable", _on_cmd_service_get_immutable},
     {"service_get_auto_connect", _on_cmd_service_get_auto_connect},
     {"service_set_auto_connect", _on_cmd_service_set_auto_connect},
     {"service_get_setup_required", _on_cmd_service_get_setup_required},
     {"service_get_apn", _on_cmd_service_get_apn},
     {"service_set_apn", _on_cmd_service_set_apn},
     {"service_get_mcc", _on_cmd_service_get_mcc},
     {"service_get_mnc", _on_cmd_service_get_mnc},
     {"service_get_roaming", _on_cmd_service_get_roaming},
     {"service_get_ipv4_method", _on_cmd_service_get_ipv4_method},
     {"service_get_ipv4_address", _on_cmd_service_get_ipv4_address},
     {"service_get_ipv4_gateway", _on_cmd_service_get_ipv4_gateway},
     {"service_get_ipv4_netmask", _on_cmd_service_get_ipv4_netmask},
     {"service_get_ipv4_configuration_method", _on_cmd_service_get_ipv4_configuration_method},
     {"service_get_ipv4_configuration_address", _on_cmd_service_get_ipv4_configuration_address},
     {"service_get_ipv4_configuration_gateway", _on_cmd_service_get_ipv4_configuration_gateway},
     {"service_get_ipv4_configuration_netmask", _on_cmd_service_get_ipv4_configuration_netmask},
     {"service_ipv4_configure_dhcp", _on_cmd_service_ipv4_configure_dhcp},
     {"service_ipv4_configure_manual", _on_cmd_service_ipv4_configure_manual},
     {"service_get_ethernet_method", _on_cmd_service_get_ethernet_method},
     {"service_get_ethernet_address", _on_cmd_service_get_ethernet_address},
     {"service_get_ethernet_mtu", _on_cmd_service_get_ethernet_mtu},
     {"service_get_ethernet_netmask", _on_cmd_service_get_ethernet_netmask},
     {NULL, NULL}
   };


   if (ecore_main_fd_handler_active_get(fd_handler, ECORE_FD_ERROR))
     {
	fputs("ERROR: reading from stdin, exit\n", stderr);
	return 0;
     }

   if (!ecore_main_fd_handler_active_get(fd_handler, ECORE_FD_READ))
     {
	fputs("ERROR: nothing to read?\n", stderr);
	return 0;
     }

   if (!fgets(buf, sizeof(buf), stdin))
     {
	fprintf(stderr, "ERROR: could not read command: %s\n", strerror(errno));
	ecore_main_loop_quit();
	return 0;
     }

   cmd = buf;
   while (isspace(*cmd))
     cmd++;

   args = strchr(cmd, ' ');
   if (args)
     {
	char *p;

	*args = '\0';
	args++;

	while (isspace(*args))
	  args++;

	p = args + strlen(args) - 1;
	if (*p == '\n')
	  *p = '\0';
     }
   else
     {
	char *p;

	p = cmd + strlen(cmd) - 1;
	if (*p == '\n')
	  *p = '\0';
     }

   if (strcmp(cmd, "help") == 0)
     {
	if (args)
	  {
	     printf("Commands with '%s' in the name:\n", args);
	     for (itr = maps; itr->cmd != NULL; itr++)
	       if (strstr(itr->cmd, args))
		 printf("\t%s\n", itr->cmd);
	  }
	else
	  {
	     fputs("Commands:\n", stdout);
	     for (itr = maps; itr->cmd != NULL; itr++)
	       printf("\t%s\n", itr->cmd);
	  }
	fputc('\n', stdout);
	return 1;
     }

   for (itr = maps; itr->cmd != NULL; itr++)
     if (strcmp(itr->cmd, cmd) == 0)
       return itr->cb(cmd, args);

   printf("unknown command \"%s\", args=%s\n", cmd, args);
   return 1;
}

int
main(int argc, char *argv[])
{
   E_DBus_Connection *c;

   ecore_init();
   e_dbus_init();
   eina_init();

   c = e_dbus_bus_get(DBUS_BUS_SYSTEM);
   if (!c) {
      printf("ERROR: can't connect to system session\n");
      return -1;
   }

   e_connman_system_init(c);
   ecore_event_handler_add(E_CONNMAN_EVENT_ELEMENT_ADD, _on_element_add, NULL);
   ecore_event_handler_add(E_CONNMAN_EVENT_ELEMENT_DEL, _on_element_del, NULL);
   ecore_event_handler_add(E_CONNMAN_EVENT_ELEMENT_UPDATED,
			   _on_element_updated, NULL);

   ecore_main_fd_handler_add
     (0, ECORE_FD_READ | ECORE_FD_ERROR, _on_input, NULL, NULL, NULL);

   ecore_main_loop_begin();

   e_connman_system_shutdown();

   e_dbus_connection_close(c);
   eina_shutdown();
   e_dbus_shutdown();
   ecore_shutdown();

   fputs("DBG: clean exit.\n", stderr);

   return 0;
}
