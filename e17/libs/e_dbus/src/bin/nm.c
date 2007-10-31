#include <E_Nm.h>
#include <Ecore_Data.h>

#define E_NM_DEVICE_TYPE_WIRED 1
#define E_NM_DEVICE_TYPE_WIRELESS 2

typedef struct NM_Manager NM_Manager;
struct NM_Manager
{
  E_NM_Context *ctx;
  Ecore_List *devices;
};

typedef struct NM_Device NM_Device;
struct NM_Device
{
  int type;
  char *object_path;
  char *device_name;
};

void
cb_manager_get_name(void *data, void *reply, DBusError *err)
{
   NM_Manager *app = data;
   
   if (dbus_error_is_set(err))
     {
	//XXX
	printf("Error: %s - %s\n" ,err->name, err->message);
	return;
     }
   printf("Got name:\n");
   printf("  %s\n", (char *)reply);
}

void
cb_manager_get_type(void *data, void *reply, DBusError *err)
{
   NM_Manager *app = data;
   
   if (dbus_error_is_set(err))
     {
	//XXX
	printf("Error: %s - %s\n" ,err->name, err->message);
	return;
     }
   printf("Got type:\n");
   printf("  %i (0 == unknown, 1 == ethernet, 2 == wireless)\n", (int)(*((dbus_int32_t *)reply)));
}

void
cb_manager_get_hal_udi(void *data, void *reply, DBusError *err)
{
   NM_Manager *app = data;
   
   if (dbus_error_is_set(err))
     {
	//XXX
	printf("Error: %s - %s\n" ,err->name, err->message);
	return;
     }
   printf("Got hal udi:\n");
   printf("  %s\n", (char *)reply);
}

void
cb_manager_get_ip4_address(void *data, void *reply, DBusError *err)
{
   NM_Manager *app = data;
   
   if (dbus_error_is_set(err))
     {
	//XXX
	printf("Error: %s - %s\n" ,err->name, err->message);
	return;
     }
   printf("Got ip4_address:\n");
   printf("  %i.%i.%i.%i\n", 
	  (int)(((*((dbus_int32_t *)reply))      ) & 0xff),
	  (int)(((*((dbus_int32_t *)reply)) >> 8 ) & 0xff),
	  (int)(((*((dbus_int32_t *)reply)) >> 16) & 0xff),
	  (int)(((*((dbus_int32_t *)reply)) >> 24) & 0xff)
	  );
}

void
cb_manager_get_link_active(void *data, void *reply, DBusError *err)
{
   NM_Manager *app = data;
   
   if (dbus_error_is_set(err))
     {
	//XXX
	printf("Error: %s - %s\n" ,err->name, err->message);
	return;
     }
   printf("Got active:\n");
   printf("  %i\n", (int)(*((dbus_bool_t *)reply)));
}

void
cb_manager_wireless_get_strength(void *data, void *reply, DBusError *err)
{
   NM_Manager *app = data;
   
   if (dbus_error_is_set(err))
     {
	//XXX
	printf("Error: %s - %s\n" ,err->name, err->message);
	return;
     }
   printf("Got Wireless Strength:\n");
   printf("  %i\n", (int)(*((dbus_int32_t *)reply)));
}

void
cb_manager_wireless_get_active_network(void *data, void *reply, DBusError *err)
{
   NM_Manager *app = data;
   
   if (dbus_error_is_set(err))
     {
	//XXX
	printf("Error: %s - %s\n" ,err->name, err->message);
	return;
     }
   printf("Got Wireless Active Network:\n");
   printf("  %s\n", (char *)reply);
}

void
cb_manager_wireless_get_networks(void *data, void *reply, DBusError *err)
{
   NM_Manager *app = data;
   Ecore_List *networks;
   const char *net;
   
   if (dbus_error_is_set(err))
     {
	//XXX
	printf("Error: %s - %s\n" ,err->name, err->message);
	return;
     }
   
   networks = reply;
   ecore_list_first_goto(networks);
   printf("Got Wireless Networks:\n");
   while ((net = ecore_list_next(networks)))
     {
	printf("  %s\n", net);
     }
}



void
cb_manager_get_devices(void *data, void *reply, DBusError *err)
{
   NM_Manager *app = data;
   Ecore_List *devices;
   const char *dev;
   
   if (dbus_error_is_set(err))
     {
	//XXX
	printf("Error: %s - %s\n" ,err->name, err->message);
	return;
     }
   
   devices = reply;
   ecore_list_first_goto(devices);
   printf("Got devices:\n");
   while ((dev = ecore_list_next(devices)))
     {
	printf("  %s\n", dev);
	e_nm_device_get_name(app->ctx, dev, cb_manager_get_name, app);
	e_nm_device_get_type(app->ctx, dev, cb_manager_get_type, app);
	e_nm_device_get_hal_udi(app->ctx, dev, cb_manager_get_hal_udi, app);
	e_nm_device_get_ip4_address(app->ctx, dev, cb_manager_get_ip4_address, app);
	e_nm_device_get_link_active(app->ctx, dev, cb_manager_get_link_active, app);
	e_nm_device_wireless_get_strength(app->ctx, dev, cb_manager_wireless_get_strength, app);
	e_nm_device_wireless_get_active_network(app->ctx, dev, cb_manager_wireless_get_active_network, app);
	e_nm_device_wireless_get_networks(app->ctx, dev, cb_manager_wireless_get_networks, app);
     }
}

int 
main(int argc, char **argv)
{
   NM_Manager *app;
   ecore_init();
   ecore_string_init();
   e_dbus_init();
   
   app = calloc(1, sizeof(NM_Manager));
   
   app->ctx = e_nm_new();
   if (!app->ctx)
     {
	printf("Error connecting to system bus. Is it running?\n");
	return 1;
     }
   
   e_nm_get_devices(app->ctx, cb_manager_get_devices, app);
   
   ecore_main_loop_begin();
   
   e_nm_free(app->ctx);
   free(app);
   e_dbus_shutdown();
   ecore_string_shutdown();
   ecore_shutdown();
   return 0;
}
