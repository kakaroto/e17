#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>

#include <Ecore.h>

#include <E_Ukit.h>
#include <E_DBus.h>

static E_DBus_Connection *econ = NULL;

Eina_Bool
print_prop(const Eina_Hash *hash, const void *key, void *data, __UNUSED__ void *fdata)
{
   const Eina_List *strlist, *l;
   char *y;
   E_Ukit_Property *p = data;
   E_Ukit_Properties props;
   int err = 0;

   props.properties = (Eina_Hash*)hash;
   switch (p->type)
     {
        case E_UKIT_PROPERTY_TYPE_STRING:
          printf("\t%s = [%s]\n", (char*)key, e_ukit_property_string_get(&props, key, &err));
          break;
        case E_UKIT_PROPERTY_TYPE_INT:
          printf("\t%s = [%d]\n", (char*)key, e_ukit_property_int_get(&props, key, &err));
          break;
        case E_UKIT_PROPERTY_TYPE_UINT32:
          printf("\t%s = [%llu]\n", (char*)key, (long long unsigned)e_ukit_property_uint32_get(&props, key, &err));
          break;
        case E_UKIT_PROPERTY_TYPE_UINT64:
          printf("\t%s = [%llu]\n", (char*)key, (long long unsigned)e_ukit_property_uint64_get(&props, key, &err));
          break;
        case E_UKIT_PROPERTY_TYPE_INT64:
          printf("\t%s = [%lld]\n", (char*)key, (long long int)e_ukit_property_int64_get(&props, key, &err));
          break;
        case E_UKIT_PROPERTY_TYPE_BOOL:
          printf("\t%s = [%d]\n", (char*)key, e_ukit_property_bool_get(&props, key, &err));
          break;
        case E_UKIT_PROPERTY_TYPE_DOUBLE:
          printf("\t%s = [%f]\n", (char*)key, e_ukit_property_double_get(&props, key, &err));
          break;
        case E_UKIT_PROPERTY_TYPE_STRLIST:
          printf("\t%s = [", (char*)key);
          strlist = e_ukit_property_strlist_get(&props, key, &err);
          EINA_LIST_FOREACH(strlist, l, y)
           printf("%s%s", y, (l->next) ? " " : "");
          printf("]\n");
          break;
     }

   return EINA_TRUE;
}

static void
hash_props(void *user_data, void *reply_data, DBusError *error)
{
   printf("%s:\n", (char*)user_data);
   E_Ukit_Get_All_Properties_Return *ret = reply_data;

   if (!ret || dbus_error_is_set(error))
     {
        free(user_data);
        dbus_error_free(error);
        return;
     }

   eina_hash_foreach(ret->properties, print_prop, NULL);
   printf("\n");
   free(user_data);
}

static void
test_mount(void *user_data, void *reply_data, DBusError *error)
{
   E_Ukit_Get_Property_Return *ret = reply_data;

   if (!ret || dbus_error_is_set(error))
     {
        free(user_data);
        dbus_error_free(error);
        return;
     }

   if (ret->val.b)
     {
        printf("[%s] is mounted!\n\tGrabbing more stats to fill your screen...\n", (char*)user_data);
        e_udisks_get_all_properties(econ, user_data, hash_props, strdup(user_data));
     }
   else printf("[%s] is not mounted!\n", (char*)user_data);
   free(user_data);
}

static void
print_devs(void *user_data, void *reply_data, DBusError *error)
{
   E_Ukit_Get_All_Devices_Return *ret = reply_data;
   Eina_List *l;
   char *udi;

   if (!ret || !ret->strings || dbus_error_is_set(error))
     {
        free(user_data);
        dbus_error_free(error);
        return;
     }

   EINA_LIST_FOREACH(ret->strings, l, udi)
     {
        if (!strcmp((char*)user_data, "disks"))
          e_udisks_get_property(econ, udi, "DeviceIsMounted", test_mount, strdup(udi));
        else
          e_upower_get_all_properties(econ, udi, hash_props, strdup(udi));
     }
   free(user_data);
}

static Eina_Bool
my_quit(__UNUSED__ void *data)
{
   ecore_main_loop_quit();
   return ECORE_CALLBACK_CANCEL;
}

int main(void)
{
   ecore_init();
   eina_init();
   e_dbus_init();
   e_ukit_init();

   econ = e_dbus_bus_get(DBUS_BUS_SYSTEM);
   if (econ)
     {
        e_udisks_get_all_devices(econ, print_devs, strdup("disks"));
        e_upower_get_all_devices(econ, print_devs, strdup("power"));
     }
   /*add a short timer to quit to try and ensure that all the tests run*/
   ecore_timer_add(1, my_quit, NULL);
   ecore_main_loop_begin();

   if (econ) e_dbus_connection_close(econ);
   e_ukit_shutdown();
   e_dbus_shutdown();
   eina_shutdown();
   ecore_shutdown();
   
   return 0;
}
