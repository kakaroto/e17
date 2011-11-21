#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define E_CONNMAN_I_KNOW_THIS_API_IS_SUBJECT_TO_CHANGE 1
#include "E_Connman.h"
#include <stdio.h>
#include <string.h>

#define DBG(...) EINA_LOG_DBG(__VA_ARGS__)
#define INF(...) EINA_LOG_INFO(__VA_ARGS__)
#define WRN(...) EINA_LOG_WARN(__VA_ARGS__)
#define ERR(...) EINA_LOG_ERR(__VA_ARGS__)

static int success = 0;
static int failure = 0;
static Ecore_Timer *exiter = NULL;

static Eina_Bool
_test_string_get(E_Connman_Element *element, const char *name, Eina_Bool (*func)(const E_Connman_Element *element, const char **value))
{
   const char *value;
   Eina_Bool ret;

   INF("BEGIN: testing string get %s of element %s...", name, element->path);
   ret = func(element, &value);
   if (ret)
     INF("SUCCESS: testing string get %s of element %s: %s",
	 name, element->path, value);
   else
     WRN("FAILURE: testing string get %s of element %s",
	 name, element->path);

   return ret;
}

static Eina_Bool
_test_string_array_get(E_Connman_Element *element, const char *name, Eina_Bool (*func)(const E_Connman_Element *element, unsigned int *count, const char ***value))
{
   const char **value;
   unsigned int count;
   Eina_Bool ret;

   INF("BEGIN: testing string array get %s of element %s...",
       name, element->path);
   ret = func(element, &count, &value);
   if (ret)
     {
	INF("SUCCESS: testing string array get %s of element %s: %p[%u]",
	    name, element->path, value, count);
     }
   else
     WRN("FAILURE: testing string get %s of element %s",
	 name, element->path);

   return ret;
}

static Eina_Bool
_test_bool_get(E_Connman_Element *element, const char *name, Eina_Bool (*func)(const E_Connman_Element *element, Eina_Bool *value))
{
   Eina_Bool value, ret;

   INF("BEGIN: testing Eina_Bool get %s of element %s...", name, element->path);
   ret = func(element, &value);
   if (ret)
     INF("SUCCESS: testing Eina_Bool get %s of element %s: %hhu",
	 name, element->path, value);
   else
     WRN("FAILURE: testing Eina_Bool get %s of element %s",
	 name, element->path);

   return ret;
}

static Eina_Bool
_test_uchar_get(E_Connman_Element *element, const char *name, Eina_Bool (*func)(const E_Connman_Element *element, unsigned char *value))
{
   unsigned char value;
   Eina_Bool ret;

   INF("BEGIN: testing uchar get %s of element %s...", name, element->path);
   ret = func(element, &value);
   if (ret)
     INF("SUCCESS: testing uchar get %s of element %s: %hhu",
	 name, element->path, value);
   else
     WRN("FAILURE: testing uchar get %s of element %s",
	 name, element->path);

   return ret;
}

static Eina_Bool
_test_ushort_get(E_Connman_Element *element, const char *name, Eina_Bool (*func)(const E_Connman_Element *element, unsigned short *value))
{
   unsigned short value;
   Eina_Bool ret;

   INF("BEGIN: testing ushort get %s of element %s...", name, element->path);
   ret = func(element, &value);
   if (ret)
     INF("SUCCESS: testing ushort get %s of element %s: %hu",
	 name, element->path, value);
   else
     WRN("FAILURE: testing ushort get %s of element %s",
	 name, element->path);

   return ret;
}

static Eina_Bool
_test_uchar_array_get(E_Connman_Element *element, const char *name, Eina_Bool (*func)(const E_Connman_Element *element, unsigned int *count, unsigned char **value))
{
   unsigned char *value;
   unsigned int count;
   Eina_Bool ret;

   INF("BEGIN: testing uchar array get %s of element %s...",
       name, element->path);
   ret = func(element, &count, &value);
   if (ret)
     {
	INF("SUCCESS: testing uchar array get %s of element %s: %p[%u]",
	    name, element->path, value, count);
	free(value);
     }
   else
     WRN("FAILURE: testing uchar array get %s of element %s",
	 name, element->path);

   return ret;
}

static Eina_Bool
_test_element_get(E_Connman_Element *element, const char *name, Eina_Bool (*func)(const E_Connman_Element *element, E_Connman_Element **value))
{
   E_Connman_Element *value;
   Eina_Bool ret;

   INF("BEGIN: testing element get %s of element %s...", name, element->path);
   ret = func(element, &value);
   if (ret)
     INF("SUCCESS: testing element get %s of element %s: %p",
	 name, element->path, value);
   else
     WRN("FAILURE: testing element get %s of element %s",
	 name, element->path);

   return ret;
}

static Eina_Bool
_test_elements_get(E_Connman_Element *element, const char *name, Eina_Bool (*func)(const E_Connman_Element *element, unsigned int *count, E_Connman_Element ***elements))
{
   E_Connman_Element **value;
   unsigned int count;
   Eina_Bool ret;

   INF("BEGIN: testing elements get %s of element %s...",
       name, element->path);
   ret = func(element, &count, &value);
   if (ret)
     {
	INF("SUCCESS: testing elements get %s of element %s: %p",
	    name, element->path, value);
	free(value);
     }
   else
     WRN("FAILURE: testing elements get %s of element %s",
	 name, element->path);

   return ret;
}

static Eina_Bool
_test_element_get_global(const char *name, Eina_Bool (*func)(E_Connman_Element **value))
{
   E_Connman_Element *value;
   Eina_Bool ret;

   INF("BEGIN: testing element get %s", name);
   ret = func(&value);
   if (ret)
     INF("SUCCESS: testing element get %s: %p", name, value);
   else
     WRN("FAILURE: testing element get %s", name);

   return ret;
}

static Eina_Bool
_test_elements_get_global(const char *name, Eina_Bool (*func)(unsigned int *count, E_Connman_Element ***elements))
{
   E_Connman_Element **value;
   unsigned int count;
   Eina_Bool ret;

   INF("BEGIN: testing elements get %s", name);
   ret = func(&count, &value);
   if (ret)
     {
	INF("SUCCESS: testing elements get %s: %p", name, value);
	free(value);
     }
   else
     WRN("FAILURE: testing elements get %s", name);

   return ret;
}

static Eina_Bool
_test_string_get_global(const char *name, Eina_Bool (*func)(const char **value))
{
   const char *value;
   Eina_Bool ret;

   INF("BEGIN: testing string get %s...", name);
   ret = func(&value);
   if (ret)
     INF("SUCCESS: testing string get %s: %s", name, value);
   else
     WRN("FAILURE: testing string get %s", name);

   return ret;
}

static Eina_Bool
_test_bool_get_global(const char *name, Eina_Bool (*func)(Eina_Bool *value))
{
   Eina_Bool value, ret;

   INF("BEGIN: testing Eina_Bool get %s...", name);
   ret = func(&value);
   if (ret)
     INF("SUCCESS: testing Eina_Bool get %s: %hhu", name, value);
   else
     WRN("FAILURE: testing Eina_Bool get %s", name);

   return ret;
}

static Eina_Bool
_test_strings_get_global(const char *name, Eina_Bool (*func)(unsigned int *count, const char ***elements))
{
   const char **value;
   unsigned int count;
   Eina_Bool ret;

   INF("BEGIN: testing strings get %s", name);
   ret = func(&count, &value);
   if (ret)
     {
	INF("SUCCESS: testing strings get %s: %p", name, value);
	free(value);
     }
   else
     WRN("FAILURE: testing strings get %s", name);

   return ret;
}


struct test_desc
{
   const char *name;
   enum {
     TEST_DESC_TYPE_STRING_GET,
     TEST_DESC_TYPE_STRING_ARRAY_GET,
     TEST_DESC_TYPE_BOOL_GET,
     TEST_DESC_TYPE_UCHAR_GET,
     TEST_DESC_TYPE_USHORT_GET,
     TEST_DESC_TYPE_UCHAR_ARRAY_GET,
     TEST_DESC_TYPE_ELEMENT_GET,
     TEST_DESC_TYPE_ELEMENTS_GET,
     TEST_DESC_TYPE_ELEMENT_GET_GLOBAL,
     TEST_DESC_TYPE_ELEMENTS_GET_GLOBAL,
     TEST_DESC_TYPE_STRING_GET_GLOBAL,
     TEST_DESC_TYPE_BOOL_GET_GLOBAL,
     TEST_DESC_TYPE_STRINGS_GET_GLOBAL,
     TEST_DESC_TYPE_LAST
   } type;
   union {
      Eina_Bool (*string_get)(const E_Connman_Element *element, const char **value);
      Eina_Bool (*string_array_get)(const E_Connman_Element *element, unsigned int *count, const char ***value);
      Eina_Bool (*bool_get)(const E_Connman_Element *element, Eina_Bool *value);
      Eina_Bool (*uchar_get)(const E_Connman_Element *element, unsigned char *value);
      Eina_Bool (*ushort_get)(const E_Connman_Element *element, unsigned short*value);
      Eina_Bool (*uchar_array_get)(const E_Connman_Element *element, unsigned int *count, unsigned char **value);
      Eina_Bool (*element_get)(const E_Connman_Element *element, E_Connman_Element **value);
      Eina_Bool (*elements_get)(const E_Connman_Element *element, unsigned int *count, E_Connman_Element ***elements);
      Eina_Bool (*element_get_global)(E_Connman_Element **element);
      Eina_Bool (*elements_get_global)(unsigned int *count, E_Connman_Element ***elements);
      Eina_Bool (*string_get_global)(const char **value);
      Eina_Bool (*bool_get_global)(Eina_Bool *value);
      Eina_Bool (*strings_get_global)(unsigned int *count, const char ***strings);
      void *dummy;
   } func;
   Eina_Bool may_fail;
};

#define TEST_DESC_STRING_GET(_func, may_fail)				\
  {#_func, TEST_DESC_TYPE_STRING_GET, .func.string_get=_func, may_fail}
#define TEST_DESC_STRING_ARRAY_GET(_func, may_fail)				\
  {#_func, TEST_DESC_TYPE_STRING_ARRAY_GET, .func.string_array_get=_func, may_fail}
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
#define TEST_DESC_ELEMENT_GET_GLOBAL(_func, may_fail)			\
  {#_func, TEST_DESC_TYPE_ELEMENT_GET_GLOBAL, .func.element_get_global=_func, may_fail}
#define TEST_DESC_ELEMENTS_GET_GLOBAL(_func, may_fail)			\
  {#_func, TEST_DESC_TYPE_ELEMENTS_GET_GLOBAL, .func.elements_get_global=_func, may_fail}
#define TEST_DESC_STRING_GET_GLOBAL(_func, may_fail)			\
  {#_func, TEST_DESC_TYPE_STRING_GET_GLOBAL, .func.string_get_global=_func, may_fail}
#define TEST_DESC_BOOL_GET_GLOBAL(_func, may_fail)			\
  {#_func, TEST_DESC_TYPE_BOOL_GET_GLOBAL, .func.bool_get_global=_func, may_fail}
#define TEST_DESC_STRINGS_GET_GLOBAL(_func, may_fail)			\
  {#_func, TEST_DESC_TYPE_STRINGS_GET_GLOBAL, .func.strings_get_global=_func, may_fail}
#define TEST_DESC_SENTINEL {NULL, TEST_DESC_TYPE_LAST, .func.dummy=NULL}

static Eina_Bool
_test_element(E_Connman_Element *element, const struct test_desc *test_descs)
{
   const struct test_desc *itr;
   int total, ok = 0, fail = 0;
   Eina_Bool ret = EINA_TRUE;

   for (itr = test_descs; itr->type != TEST_DESC_TYPE_LAST; itr++)
     {
	Eina_Bool r;

	switch (itr->type)
	  {
	   case TEST_DESC_TYPE_STRING_GET:
	      r = _test_string_get(element, itr->name, itr->func.string_get);
	      break;
	   case TEST_DESC_TYPE_STRING_ARRAY_GET:
	      r = _test_string_array_get
		(element, itr->name, itr->func.string_array_get);
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
	   case TEST_DESC_TYPE_ELEMENT_GET_GLOBAL:
	      r = _test_element_get_global
		(itr->name, itr->func.element_get_global);
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
	   case TEST_DESC_TYPE_STRINGS_GET_GLOBAL:
	      r = _test_strings_get_global
		(itr->name, itr->func.strings_get_global);
	      break;
	   default:
	      ERR("unknown test type %d (%s)", itr->type, itr->name);
	      r = 0;
	      break;
	  }

	if (r || itr->may_fail)
	  ok++;
	else
	  {
	     ERR("test failed %s, element %s [%s]",
		 itr->name, element->path, element->interface);
	     fail++;
	     ret = EINA_FALSE;
	  }
     }

   total = ok + failure;
   success += ok;
   failure += fail;
   if (total == 0)
     {
	INF("no tests for %s [%s]", element->path, element->interface);
	return EINA_TRUE;
     }

   INF("testing %s success: %d, failure: %d: %d%% [%s]",
       element->path, ok, fail, (ok * 100) / total,
       element->interface);

   return ret;
}

static const struct test_desc test_desc_manager[] = {
  TEST_DESC_STRING_GET_GLOBAL(e_connman_manager_state_get, 0),
  TEST_DESC_BOOL_GET_GLOBAL(e_connman_manager_offline_mode_get, 0),
  //TEST_DESC_STRING_SET_GLOBAL(e_connman_manager_request_scan, 0),
  //TEST_DESC_BOOL_SET_GLOBAL(e_connman_manager_offline_mode_set, 0),
  TEST_DESC_ELEMENTS_GET_GLOBAL(e_connman_manager_profiles_get, 0),
  TEST_DESC_ELEMENTS_GET_GLOBAL(e_connman_manager_services_get, 1),
  TEST_DESC_STRING_GET_GLOBAL(e_connman_manager_technology_default_get, 0),
  TEST_DESC_STRINGS_GET_GLOBAL(e_connman_manager_technologies_available_get, 0),
  TEST_DESC_STRINGS_GET_GLOBAL(e_connman_manager_technologies_enabled_get, 0),
  TEST_DESC_STRINGS_GET_GLOBAL(e_connman_manager_technologies_connected_get, 0),
  TEST_DESC_ELEMENT_GET_GLOBAL(e_connman_manager_profile_active_get, 0),
  TEST_DESC_SENTINEL
};

static const struct test_desc test_desc_profile[] = {
  TEST_DESC_STRING_GET(e_connman_profile_name_get, 1),
  //TEST_DESC_STRING_SET(e_connman_profile_name_set, 1),
  TEST_DESC_BOOL_GET(e_connman_profile_offline_mode_get, 0),
  //TEST_DESC_BOOL_SET(e_connman_profile_offline_mode_set, 0),
  TEST_DESC_ELEMENTS_GET(e_connman_profile_services_get, 1),
  TEST_DESC_SENTINEL
};

static const struct test_desc test_desc_service[] = {
  /* TODO: need to check exactly what properties may fail */
  TEST_DESC_STRING_GET(e_connman_service_state_get, 1),
  TEST_DESC_STRING_GET(e_connman_service_error_get, 1),
  TEST_DESC_STRING_GET(e_connman_service_name_get, 0),
  TEST_DESC_STRING_GET(e_connman_service_type_get, 0),
  TEST_DESC_STRING_ARRAY_GET(e_connman_service_security_get, 1),
  TEST_DESC_STRING_GET(e_connman_service_passphrase_get, 1),
  //TEST_DESC_STRING_SET(e_connman_service_passphrase_set, 1),
  TEST_DESC_BOOL_GET(e_connman_service_passphrase_required_get, 1),
  TEST_DESC_UCHAR_GET(e_connman_service_strength_get, 1),
  TEST_DESC_BOOL_GET(e_connman_service_favorite_get, 0),
  TEST_DESC_BOOL_GET(e_connman_service_immutable_get, 0),
  TEST_DESC_BOOL_GET(e_connman_service_auto_connect_get, 0),
  //TEST_DESC_BOOL_SET(e_connman_service_auto_connect_set, 1),
  TEST_DESC_BOOL_GET(e_connman_service_roaming_get, 1),
  TEST_DESC_STRING_GET(e_connman_service_ipv4_method_get, 1),
  TEST_DESC_STRING_GET(e_connman_service_ipv4_address_get, 1),
  TEST_DESC_STRING_GET(e_connman_service_ipv4_gateway_get, 1),
  TEST_DESC_STRING_GET(e_connman_service_ipv4_netmask_get, 1),
  TEST_DESC_STRING_GET(e_connman_service_ipv4_configuration_method_get, 1),
  TEST_DESC_STRING_GET(e_connman_service_ipv4_configuration_address_get, 1),
  TEST_DESC_STRING_GET(e_connman_service_ipv4_configuration_gateway_get, 1),
  TEST_DESC_STRING_GET(e_connman_service_ipv4_configuration_netmask_get, 1),
  TEST_DESC_STRING_GET(e_connman_service_ethernet_method_get, 1),
  TEST_DESC_STRING_GET(e_connman_service_ethernet_address_get, 1),
  TEST_DESC_USHORT_GET(e_connman_service_ethernet_mtu_get, 1),
  TEST_DESC_SENTINEL
};

static Eina_Bool
_quit(__UNUSED__ void *data)
{
   ecore_main_loop_quit();
   return ECORE_CALLBACK_CANCEL;
}

static Eina_Bool
_on_exiter(__UNUSED__ void *data)
{
   e_connman_system_shutdown();
   ecore_idle_enterer_add(_quit, NULL);
   exiter = NULL;
   return ECORE_CALLBACK_CANCEL;
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

static Eina_Bool
_test_element_timer(void *data)
{
   struct test_element_timer_data *d = data;
   E_Connman_Element *element = d->element;

   if (e_connman_element_is_profile(element))
     _test_element(element, test_desc_profile);
   else if (e_connman_element_is_manager(element))
     _test_element(element, test_desc_manager);
   else if (e_connman_element_is_service(element))
     _test_element(element, test_desc_service);
   else
     ERR("!!! don't know how to test %s [%s]",
	 element->path, element->interface);

   _exiter_reschedule();

   d->timer = NULL;
   return ECORE_CALLBACK_CANCEL;
}

static void
_element_listener(void *data, __UNUSED__ const E_Connman_Element *element)
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

static Eina_Bool
_on_element_add(__UNUSED__ void *data, __UNUSED__ int type, void *info)
{
   E_Connman_Element *element = info;
   struct test_element_timer_data *d;

   d = malloc(sizeof(*d));
   if (!d)
     return ECORE_CALLBACK_PASS_ON;

   d->element = element;
   d->timer = ecore_timer_add(1.0, _test_element_timer, d);
   e_connman_element_listener_add
     (element, _element_listener, d, _element_listener_free);

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_on_element_del(__UNUSED__ void *data, __UNUSED__ int type, __UNUSED__ void *info)
{
   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_on_element_updated(__UNUSED__ void *data, __UNUSED__ int type, __UNUSED__ void *info)
{
   return ECORE_CALLBACK_PASS_ON;
}

int
main(__UNUSED__ int argc, __UNUSED__ char *argv[])
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
