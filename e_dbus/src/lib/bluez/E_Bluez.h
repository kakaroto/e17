#ifndef E_BLUEZ_H
#define E_BLUEZ_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <Eina.h>
#include <Ecore.h>
#include <E_DBus.h>

#ifdef EAPI
#undef EAPI
#endif
#ifdef _MSC_VER
# ifdef BUILDING_DLL
#  define EAPI __declspec(dllexport)
# else
#  define EAPI __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

  /* Ecore Events */
  extern int E_BLUEZ_EVENT_MANAGER_IN;
  extern int E_BLUEZ_EVENT_MANAGER_OUT;
  extern int E_BLUEZ_EVENT_ELEMENT_ADD;
  extern int E_BLUEZ_EVENT_ELEMENT_DEL;
  extern int E_BLUEZ_EVENT_ELEMENT_UPDATED;
  extern int E_BLUEZ_EVENT_DEVICE_FOUND;

  typedef struct _E_Bluez_Element E_Bluez_Element;
  typedef struct _E_Bluez_Array E_Bluez_Array;
  typedef struct _E_Bluez_Device_Found E_Bluez_Device_Found;

  struct _E_Bluez_Element
  {
     const char *path;
     const char *interface;
     E_DBus_Signal_Handler *signal_handler;
     E_DBus_Signal_Handler *device_found_handler;
     Eina_Inlist *props;

     /* private */
     struct {
	Eina_Inlist *properties_get;
	Eina_Inlist *property_set;
	Eina_Inlist *agent_register;
	Eina_Inlist *agent_unregister;
	Eina_Inlist *start_discovery;
	Eina_Inlist *stop_discovery;
	Eina_Inlist *create_paired_device;
     } _pending;
     struct {
	Ecore_Idler *changed;
     } _idler;
     Eina_Inlist *_listeners;
     int _references;
  };

  struct _E_Bluez_Array
  {
     int type;
     Eina_Array *array;
  };

  struct _E_Bluez_Device_Found
  {
	  E_Bluez_Element *adapter;
	  const char *name;
	  E_Bluez_Array *array;
  };

  /* General Public API */
  EAPI unsigned int e_bluez_system_init(E_DBus_Connection *edbus_conn) EINA_ARG_NONNULL(1);
  EAPI unsigned int e_bluez_system_shutdown(void);

  /* Manager Methods */
  EAPI E_Bluez_Element *e_bluez_manager_get(void) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_bluez_manager_default_adapter(E_DBus_Method_Return_Cb cb, void *data) EINA_WARN_UNUSED_RESULT;

  /* Adapter Methods */
  EAPI void e_bluez_adapter_device_found_free(E_Bluez_Device_Found *device) EINA_ARG_NONNULL(1);
  EAPI const char *e_bluez_adapter_device_found_get_alias(E_Bluez_Device_Found *device) EINA_ARG_NONNULL(1);
  EAPI bool e_bluez_adapter_agent_register(E_Bluez_Element *element, const char *object_path, const char *capability, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_bluez_adapter_agent_unregister(E_Bluez_Element *element, const char *object_path, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_bluez_adapter_address_get(E_Bluez_Element *element, const char **address) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_bluez_adapter_powered_get(E_Bluez_Element *element, bool *powered) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_bluez_adapter_powered_set(E_Bluez_Element *profile, bool powered, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_bluez_adapter_start_discovery(E_Bluez_Element *element, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_bluez_adapter_stop_discovery(E_Bluez_Element *element, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;
bool e_bluez_adapter_create_paired_device(E_Bluez_Element *element, const char *object_path, const char *capability, const char *device, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1,2,4) EINA_WARN_UNUSED_RESULT;

  /* Devices Methods */
  EAPI bool e_bluez_device_name_get(E_Bluez_Element *element, const char **name) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  /* Low-Level API:
   *
   * Should just be used to work around problems until proper solution
   * is made into e_bluez.
   */
  EAPI bool e_bluez_manager_sync_elements(void);

  EAPI bool e_bluez_elements_get_all(unsigned int *count, E_Bluez_Element ***p_elements) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_bluez_elements_get_all_type(const char *type, unsigned int *count, E_Bluez_Element ***p_elements) EINA_ARG_NONNULL(1, 2, 3) EINA_WARN_UNUSED_RESULT;
  EAPI E_Bluez_Element *e_bluez_element_get(const char *path);

  EAPI void e_bluez_element_listener_add(E_Bluez_Element *element, void (*cb)(void *data, const E_Bluez_Element *element), const void *data, void (*free_data)(void *data)) EINA_ARG_NONNULL(1, 2);
  EAPI void e_bluez_element_listener_del(E_Bluez_Element *element, void (*cb)(void *data, const E_Bluez_Element *element), const void *data) EINA_ARG_NONNULL(1, 2);

  EAPI int e_bluez_element_ref(E_Bluez_Element *element) EINA_ARG_NONNULL(1);
  EAPI int e_bluez_element_unref(E_Bluez_Element *element) EINA_ARG_NONNULL(1);

  EAPI void e_bluez_element_print(FILE *fp, const E_Bluez_Element *element) EINA_ARG_NONNULL(1, 2);
  EAPI void e_bluez_element_array_print(FILE *fp, E_Bluez_Array *array);

  EAPI bool e_bluez_element_properties_sync(E_Bluez_Element *element) EINA_ARG_NONNULL(1);
  EAPI bool e_bluez_element_properties_sync_full(E_Bluez_Element *element, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1);

  EAPI bool e_bluez_element_property_set(E_Bluez_Element *element, const char *prop, int type, const void *value) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_bluez_element_property_set_full(E_Bluez_Element *element, const char *prop, int type, const void *value, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_bluez_element_property_dict_set_full(E_Bluez_Element *element, const char *prop, const char *key, int type, const void *value, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1, 2, 3) EINA_WARN_UNUSED_RESULT;

  EAPI void e_bluez_element_properties_list(const E_Bluez_Element *element, bool (*cb)(void *data, const E_Bluez_Element *element, const char *name, int type, const void *value), const void *data) EINA_ARG_NONNULL(1, 2);

  EAPI bool e_bluez_element_property_type_get_stringshared(const E_Bluez_Element *element, const char *name, int *type) EINA_ARG_NONNULL(1, 2, 3) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_bluez_element_property_type_get(const E_Bluez_Element *element, const char *name, int *type) EINA_ARG_NONNULL(1, 2, 3) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_bluez_element_property_dict_get_stringshared(const E_Bluez_Element *element, const char *dict_name, const char *key_name, int *type, void *value) EINA_ARG_NONNULL(1, 2, 4) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_bluez_element_property_get_stringshared(const E_Bluez_Element *element, const char *name, int *type, void *value) EINA_ARG_NONNULL(1, 2, 4) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_bluez_element_property_get(const E_Bluez_Element *element, const char *name, int *type, void *value) EINA_ARG_NONNULL(1, 2, 4) EINA_WARN_UNUSED_RESULT;

#ifdef __cplusplus
}
#endif
#endif /* E_BLUEZ_H */
