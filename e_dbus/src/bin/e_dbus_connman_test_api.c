#include "E_Connman.h"
#include <stdio.h>
#include <string.h>

#define DBG(...) EINA_ERROR_PDBG(__VA_ARGS__)
#define INF(...) EINA_ERROR_PINFO(__VA_ARGS__)
#define WRN(...) EINA_ERROR_PWARN(__VA_ARGS__)
#define ERR(...) EINA_ERROR_PERR(__VA_ARGS__)

static int success = 0;
static int failure = 0;
static Ecore_Timer *exiter = NULL;

static bool
_test_string_get(E_Connman_Element *element, const char *name, bool (*func)(const E_Connman_Element *element, const char **value))
{
   const char *value;
   bool ret;

   INF("BEGIN: testing string get %s of element %s...\n", name, element->path);
   ret = func(element, &value);
   if (ret)
     INF("SUCCESS: testing string get %s of element %s: %s\n",
	 name, element->path, value);
   else
     WRN("FAILURE: testing string get %s of element %s\n",
	 name, element->path);

   return ret;
}

static bool
_test_bool_get(E_Connman_Element *element, const char *name, bool (*func)(const E_Connman_Element *element, bool *value))
{
   bool value, ret;

   INF("BEGIN: testing bool get %s of element %s...\n", name, element->path);
   ret = func(element, &value);
   if (ret)
     INF("SUCCESS: testing bool get %s of element %s: %hhu\n",
	 name, element->path, value);
   else
     WRN("FAILURE: testing bool get %s of element %s\n",
	 name, element->path);

   return ret;
}

static bool
_test_uchar_get(E_Connman_Element *element, const char *name, bool (*func)(const E_Connman_Element *element, unsigned char *value))
{
   unsigned char value;
   bool ret;

   INF("BEGIN: testing uchar get %s of element %s...\n", name, element->path);
   ret = func(element, &value);
   if (ret)
     INF("SUCCESS: testing uchar get %s of element %s: %hhu\n",
	 name, element->path, value);
   else
     WRN("FAILURE: testing uchar get %s of element %s\n",
	 name, element->path);

   return ret;
}

static bool
_test_ushort_get(E_Connman_Element *element, const char *name, bool (*func)(const E_Connman_Element *element, unsigned short *value))
{
   unsigned short value;
   bool ret;

   INF("BEGIN: testing ushort get %s of element %s...\n", name, element->path);
   ret = func(element, &value);
   if (ret)
     INF("SUCCESS: testing ushort get %s of element %s: %hu\n",
	 name, element->path, value);
   else
     WRN("FAILURE: testing ushort get %s of element %s\n",
	 name, element->path);

   return ret;
}

static bool
_test_uchar_array_get(E_Connman_Element *element, const char *name, bool (*func)(const E_Connman_Element *element, unsigned int *count, unsigned char **value))
{
   unsigned char *value;
   unsigned int count;
   bool ret;

   INF("BEGIN: testing ushort get %s of element %s...\n", name, element->path);
   ret = func(element, &count, &value);
   if (ret)
     {
	INF("SUCCESS: testing ushort get %s of element %s: %p\n",
	    name, element->path, value);
	free(value);
     }
   else
     WRN("FAILURE: testing ushort get %s of element %s\n",
	 name, element->path);

   return ret;
}

static bool
_test_element_get(E_Connman_Element *element, const char *name, bool (*func)(const E_Connman_Element *element, E_Connman_Element **value))
{
   E_Connman_Element *value;
   bool ret;

   INF("BEGIN: testing element get %s of element %s...\n", name, element->path);
   ret = func(element, &value);
   if (ret)
     INF("SUCCESS: testing element get %s of element %s: %p\n",
	 name, element->path, value);
   else
     WRN("FAILURE: testing element get %s of element %s\n",
	 name, element->path);

   return ret;
}

static bool
_test_elements_get(E_Connman_Element *element, const char *name, bool (*func)(const E_Connman_Element *element, unsigned int *count, E_Connman_Element ***elements))
{
   E_Connman_Element **value;
   unsigned int count;
   bool ret;

   INF("BEGIN: testing elements get %s of element %s...\n",
       name, element->path);
   ret = func(element, &count, &value);
   if (ret)
     {
	INF("SUCCESS: testing elements get %s of element %s: %p\n",
	    name, element->path, value);
	free(value);
     }
   else
     WRN("FAILURE: testing elements get %s of element %s\n",
	 name, element->path);

   return ret;
}

static bool
_test_elements_get_global(const char *name, bool (*func)(unsigned int *count, E_Connman_Element ***elements))
{
   E_Connman_Element **value;
   unsigned int count;
   bool ret;

   INF("BEGIN: testing elements get %s\n", name);
   ret = func(&count, &value);
   if (ret)
     {
	INF("SUCCESS: testing elements get %s: %p\n", name, value);
	free(value);
     }
   else
     WRN("FAILURE: testing elements get %s\n", name);

   return ret;
}

static bool
_test_string_get_global(const char *name, bool (*func)(const char **value))
{
   const char *value;
   bool ret;

   INF("BEGIN: testing string get %s...\n", name);
   ret = func(&value);
   if (ret)
     INF("SUCCESS: testing string get %s: %s\n", name, value);
   else
     WRN("FAILURE: testing string get %s\n", name);

   return ret;
}

static bool
_test_bool_get_global(const char *name, bool (*func)(bool *value))
{
   bool value, ret;

   INF("BEGIN: testing bool get %s...\n", name);
   ret = func(&value);
   if (ret)
     INF("SUCCESS: testing bool get %s: %hhu\n", name, value);
   else
     WRN("FAILURE: testing bool get %s\n", name);

   return ret;
}

struct test_desc
{
   const char *name;
   enum {
     TEST_DESC_TYPE_STRING_GET,
     TEST_DESC_TYPE_BOOL_GET,
     TEST_DESC_TYPE_UCHAR_GET,
     TEST_DESC_TYPE_USHORT_GET,
     TEST_DESC_TYPE_UCHAR_ARRAY_GET,
     TEST_DESC_TYPE_ELEMENT_GET,
     TEST_DESC_TYPE_ELEMENTS_GET,
     TEST_DESC_TYPE_ELEMENTS_GET_GLOBAL,
     TEST_DESC_TYPE_STRING_GET_GLOBAL,
     TEST_DESC_TYPE_BOOL_GET_GLOBAL,
     TEST_DESC_TYPE_LAST
   } type;
   union {
      bool (*string_get)(const E_Connman_Element *element, const char **value);
      bool (*bool_get)(const E_Connman_Element *element, bool *value);
      bool (*uchar_get)(const E_Connman_Element *element, unsigned char *value);
      bool (*ushort_get)(const E_Connman_Element *element, unsigned short*value);
      bool (*uchar_array_get)(const E_Connman_Element *element, unsigned int *count, unsigned char **value);
      bool (*element_get)(const E_Connman_Element *element, E_Connman_Element **value);
      bool (*elements_get)(const E_Connman_Element *element, unsigned int *count, E_Connman_Element ***elements);
      bool (*elements_get_global)(unsigned int *count, E_Connman_Element ***elements);
      bool (*string_get_global)(const char **value);
      bool (*bool_get_global)(bool *value);
      void *dummy;
   } func;
   bool may_fail;
};

#define TEST_DESC_STRING_GET(_func, may_fail)				\
  {#_func, TEST_DESC_TYPE_STRING_GET, .func.string_get=_func, may_fail}
#define TEST_DESC_BOOL_GET(_func, may_fail)				\
  {#_func, TEST_DESC_TYPE_BOOL_GET, .func.bool_get=_func, may_fail}
#define TEST_DESC_UCHAR_GET(_func, may_fail)				\
  {#_func, TEST_DESC_TYPE_UCHAR_GET, .func.uchar_get=_func, may_fail}
#define TEST_DESC_USHORT_GET(_func, may_fail)				\
  {#_func, TEST_DESC_TYPE_USHORT_GET, .func.ushort_get=_func, may_fail}
#define TEST_DESC_UCHAR_ARRAY_GET(_func, may_fail)			\
  {#_func, TEST_DESC_TYPE_UCHAR_ARRAY_GET, .func.uchar_array_get=_func, may_fail}
#define TEST_DESC_ELEMENT_GET(_func, may_fail)				\
  {#_func, TEST_DESC_TYPE_ELEMENT_GET, .func.element_get=_func, may_fail}
#define TEST_DESC_ELEMENTS_GET(_func, may_fail)				\
  {#_func, TEST_DESC_TYPE_ELEMENTS_GET, .func.elements_get=_func, may_fail}
#define TEST_DESC_ELEMENTS_GET_GLOBAL(_func, may_fail)			\
  {#_func, TEST_DESC_TYPE_ELEMENTS_GET_GLOBAL, .func.elements_get_global=_func, may_fail}
#define TEST_DESC_STRING_GET_GLOBAL(_func, may_fail)			\
  {#_func, TEST_DESC_TYPE_STRING_GET_GLOBAL, .func.string_get_global=_func, may_fail}
#define TEST_DESC_BOOL_GET_GLOBAL(_func, may_fail)			\
  {#_func, TEST_DESC_TYPE_BOOL_GET_GLOBAL, .func.bool_get_global=_func, may_fail}
#define TEST_DESC_SENTINEL {NULL, TEST_DESC_TYPE_LAST, .func.dummy=NULL}

static bool
_test_element(E_Connman_Element *element, const struct test_desc *test_descs)
{
   const struct test_desc *itr;
   int total, ok = 0, fail = 0;
   bool ret = 1;

   for (itr = test_descs; itr->type != TEST_DESC_TYPE_LAST; itr++)
     {
	bool r;

	switch (itr->type)
	  {
	   case TEST_DESC_TYPE_STRING_GET:
	      r = _test_string_get(element, itr->name, itr->func.string_get);
	      break;
	   case TEST_DESC_TYPE_BOOL_GET:
	      r = _test_bool_get(element, itr->name, itr->func.bool_get);
	      break;
	   case TEST_DESC_TYPE_UCHAR_GET:
	      r = _test_uchar_get(element, itr->name, itr->func.uchar_get);
	      break;
	   case TEST_DESC_TYPE_USHORT_GET:
	      r = _test_ushort_get(element, itr->name, itr->func.ushort_get);
	      break;
	   case TEST_DESC_TYPE_UCHAR_ARRAY_GET:
	      r = _test_uchar_array_get
		(element, itr->name, itr->func.uchar_array_get);
	      break;
	   case TEST_DESC_TYPE_ELEMENT_GET:
	      r = _test_element_get
		(element, itr->name, itr->func.element_get);
	      break;
	   case TEST_DESC_TYPE_ELEMENTS_GET:
	      r = _test_elements_get
		(element, itr->name, itr->func.elements_get);
	      break;
	   case TEST_DESC_TYPE_ELEMENTS_GET_GLOBAL:
	      r = _test_elements_get_global
		(itr->name, itr->func.elements_get_global);
	      break;
	   case TEST_DESC_TYPE_STRING_GET_GLOBAL:
	      r = _test_string_get_global
		(itr->name, itr->func.string_get_global);
	      break;
	   case TEST_DESC_TYPE_BOOL_GET_GLOBAL:
	      r = _test_bool_get_global
		(itr->name, itr->func.bool_get_global);
	      break;
	   default:
	      ERR("unknown test type %d (%s)\n", itr->type, itr->name);
	      r = 0;
	      break;
	  }

	if (r || itr->may_fail)
	  ok++;
	else
	  {
	     ERR("test failed %s, element %s [%s]\n",
		 itr->name, element->path, element->interface);
	     fail++;
	     ret = 0;
	  }
     }

   total = ok + failure;
   success += ok;
   failure += fail;
   if (total == 0)
     {
	INF("no tests for %s [%s]\n", element->path, element->interface);
	return 1;
     }

   INF("testing %s success: %d, failure: %d: %d%% [%s]\n",
       element->path, ok, fail, (ok * 100) / total,
       element->interface);

   return ret;
}

static const struct test_desc test_desc_manager[] = {
  TEST_DESC_STRING_GET_GLOBAL(e_connman_manager_state_get, 0),
  TEST_DESC_STRING_GET_GLOBAL(e_connman_manager_policy_get, 0),
  //TEST_DESC_STRING_SET_GLOBAL(e_connman_manager_policy_set, 0),
  TEST_DESC_BOOL_GET_GLOBAL(e_connman_manager_offline_mode_get, 0),
  //TEST_DESC_BOOL_SET_GLOBAL(e_connman_manager_offline_mode_set, 0),
  TEST_DESC_ELEMENTS_GET_GLOBAL(e_connman_manager_profiles_get, 0),
  TEST_DESC_ELEMENTS_GET_GLOBAL(e_connman_manager_devices_get, 0),
  TEST_DESC_ELEMENTS_GET_GLOBAL(e_connman_manager_connections_get, 1),
  TEST_DESC_SENTINEL
};

static const struct test_desc test_desc_device[] = {
  TEST_DESC_STRING_GET(e_connman_device_name_get, 0),
  TEST_DESC_STRING_GET(e_connman_device_type_get, 0),
  TEST_DESC_STRING_GET(e_connman_device_interface_get, 0),
  TEST_DESC_STRING_GET(e_connman_device_policy_get, 0),
  //TEST_DESC_STRING_SET(e_connman_device_policy_set, 0),
  TEST_DESC_UCHAR_GET(e_connman_device_priority_get, 0),
  //TEST_DESC_UCHAR_SET(e_connman_device_priority_set, 0),
  TEST_DESC_BOOL_GET(e_connman_device_powered_get, 0),
  //TEST_DESC_BOOL_SET(e_connman_device_powered_set, 0),
  TEST_DESC_USHORT_GET(e_connman_device_scan_interval_get, 1),
  //TEST_DESC_USHORT_SET(e_connman_device_scan_interval_set, 1),
  TEST_DESC_BOOL_GET(e_connman_device_scanning_get, 1),
  TEST_DESC_ELEMENTS_GET(e_connman_device_networks_get, 1),
  TEST_DESC_SENTINEL
};

static const struct test_desc test_desc_profile[] = {
  TEST_DESC_STRING_GET(e_connman_profile_name_get, 0),
  TEST_DESC_SENTINEL
};

static const struct test_desc test_desc_connection[] = {
  TEST_DESC_STRING_GET(e_connman_connection_type_get, 0),
  TEST_DESC_STRING_GET(e_connman_connection_interface_get, 0),
  TEST_DESC_UCHAR_GET(e_connman_connection_strength_get, 0),
  TEST_DESC_BOOL_GET(e_connman_connection_default_get, 0),
  TEST_DESC_ELEMENT_GET(e_connman_connection_device_get, 0),
  TEST_DESC_ELEMENT_GET(e_connman_connection_network_get, 0),
  TEST_DESC_STRING_GET(e_connman_connection_ipv4_method_get, 0),
  TEST_DESC_STRING_GET(e_connman_connection_ipv4_address_get, 0),
  TEST_DESC_SENTINEL
};

static const struct test_desc test_desc_network[] = {
  TEST_DESC_STRING_GET(e_connman_network_name_get, 0),
  TEST_DESC_BOOL_GET(e_connman_network_available_get, 0),
  TEST_DESC_BOOL_GET(e_connman_network_connected_get, 0),
  TEST_DESC_BOOL_GET(e_connman_network_remember_get, 0),
  //TEST_DESC_BOOL_SET(e_connman_network_remember_set, 0),
  TEST_DESC_UCHAR_GET(e_connman_network_strength_get, 1),
  TEST_DESC_ELEMENT_GET(e_connman_network_device_get, 0),
  TEST_DESC_UCHAR_ARRAY_GET(e_connman_network_wifi_ssid_get, 1),
  TEST_DESC_STRING_GET(e_connman_network_wifi_mode_get, 1),
  // TEST_DESC_STRING_SET(e_connman_network_wifi_mode_set, 1),
  TEST_DESC_STRING_GET(e_connman_network_wifi_security_get, 1),
  // TEST_DESC_STRING_SET(e_connman_network_wifi_security_set, 1),
  TEST_DESC_STRING_GET(e_connman_network_wifi_passphrase_get, 1),
  //TEST_DESC_STRING_SET(e_connman_network_wifi_passphrase_set, 1),
  TEST_DESC_SENTINEL
};

static int
_quit(void *data)
{
   ecore_main_loop_quit();
   return 0;
}

static int
_on_exiter(void *data)
{
   e_connman_system_shutdown();
   ecore_idle_enterer_add(_quit, NULL);
   exiter = NULL;
   return 0;
}

static void
_exiter_reschedule(void)
{
   if (exiter)
     ecore_timer_del(exiter);
   exiter = ecore_timer_add(10, _on_exiter, NULL);
}

struct test_element_timer_data
{
   E_Connman_Element *element;
   Ecore_Timer *timer;
};

static int
_test_element_timer(void *data)
{
   struct test_element_timer_data *d = data;
   E_Connman_Element *element = d->element;

   if (e_connman_element_is_device(element))
     _test_element(element, test_desc_device);
   else if (e_connman_element_is_profile(element))
     _test_element(element, test_desc_profile);
   else if (e_connman_element_is_connection(element))
     _test_element(element, test_desc_connection);
   else if (e_connman_element_is_network(element))
     _test_element(element, test_desc_network);
   else if (e_connman_element_is_manager(element))
     _test_element(element, test_desc_manager);
   else
     ERR("!!! don't know how to test %s [%s]\n",
	 element->path, element->interface);

   _exiter_reschedule();

   d->timer = NULL;
   return 0;
}

static void
_element_listener(void *data, const E_Connman_Element *element)
{
   struct test_element_timer_data *d = data;
   if (d->timer)
     ecore_timer_del(d->timer);
   d->timer = ecore_timer_add(1.0, _test_element_timer, d);
   _exiter_reschedule();
}

static void
_element_listener_free(void *data)
{
   struct test_element_timer_data *d = data;
   if (d->timer)
     ecore_timer_del(d->timer);
   free(d);
}

static int
_on_element_add(void *data, int type, void *info)
{
   E_Connman_Element *element = info;
   struct test_element_timer_data *d;

   d = malloc(sizeof(*d));
   if (!d)
     return 1;

   d->element = element;
   d->timer = ecore_timer_add(1.0, _test_element_timer, d);
   e_connman_element_listener_add
     (element, _element_listener, d, _element_listener_free);

   return 1;
}

static int
_on_element_del(void *data, int type, void *info)
{
   return 1;
}

static int
_on_element_updated(void *data, int type, void *info)
{
   return 1;
}

int
main(int argc, char *argv[])
{
   E_DBus_Connection *c;
   int total;

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

   _exiter_reschedule();

   ecore_main_loop_begin();

   e_dbus_connection_close(c);
   eina_stringshare_dump();
   eina_shutdown();
   e_dbus_shutdown();
   ecore_shutdown();

   total = success + failure;
   if (total == 0)
     fputs("DBG: clean exit, no tests executed.\n", stderr);
   else
     fprintf(stderr, "DBG: clean exit, success: %d, failure: %d, %d%%\n",
	     success, failure, (success * 100) / total);

   return 0;
}
