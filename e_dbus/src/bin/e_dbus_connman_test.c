#include "E_Connman.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

static void
_elements_print(E_Connman_Element **elements, int count)
{
   int i;
   for (i = 0; i < count; i++)
     {
	printf("--- element %d:\n", i);
	e_connman_element_print(stdout, elements[i]);
     }
   free(elements);
   printf("END: all elements count = %d\n", count);
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
   if (e_connman_manager_agent_register(path, NULL, NULL))
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
   if (e_connman_manager_agent_unregister(path, NULL, NULL))
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
   if (e_connman_manager_offline_mode_set(offline, NULL, NULL))
     printf(":::Manager Offline Mode set to %hhu\n", offline);
   else
     fputs("ERROR: can't set manager offline mode\n", stderr);
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
   if (e_connman_device_propose_scan(e, NULL, NULL))
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
   if (e_connman_device_powered_set(e, powered, NULL, NULL))
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
   if (e_connman_device_scan_interval_set(e, scan_interval, NULL, NULL))
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
   if (e_connman_profile_offline_mode_set(e, offline, NULL, NULL))
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
   if (e_connman_service_connect(e, NULL, NULL))
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
   if (e_connman_service_disconnect(e, NULL, NULL))
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
   if (e_connman_service_remove(e, NULL, NULL))
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
   if (e_connman_service_move_before(e, path, NULL, NULL))
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
   if (e_connman_service_move_after(e, path, NULL, NULL))
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
   if (e_connman_service_passphrase_set(e, passphrase, NULL, NULL))
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
   if (e_connman_service_auto_connect_set(e, auto_connect, NULL, NULL))
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
   if (e_connman_service_apn_set(e, apn, NULL, NULL))
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
     printf(":::Service %s ipv4 method = \"%s\"\n", path, ipv4_method);
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
     printf(":::Service %s ipv4 address = \"%s\"\n", path, ipv4_address);
   else
     fputs("ERROR: can't get service ipv4 address\n", stderr);
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
     {"profile_get_offline_mode", _on_cmd_profile_get_offline_mode},
     {"profile_set_offline_mode", _on_cmd_profile_set_offline_mode},
     {"profile_get_services", _on_cmd_profile_get_services},
     {"network_get_address", _on_cmd_network_get_address},
     {"network_get_name", _on_cmd_network_get_name},
     {"network_get_connected", _on_cmd_network_get_connected},
     {"network_get_strength", _on_cmd_network_get_strength},
     {"network_get_device", _on_cmd_network_get_device},
     {"network_get_wifi_ssid", _on_cmd_network_get_wifi_ssid},
     {"network_get_wifi_mode", _on_cmd_network_get_wifi_mode},
     {"network_get_wifi_security", _on_cmd_network_get_wifi_security},
     {"network_get_wifi_passphrase", _on_cmd_network_get_wifi_passphrase},
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
