#include <E_Nm.h>
#include <Ecore_Data.h>
#include <dbus/dbus.h>

#define E_NM_DEVICE_TYPE_WIRED 1
#define E_NM_DEVICE_TYPE_WIRELESS 2

typedef struct NM_Manager NM_Manager;
struct NM_Manager
{
  E_NM_Context *ctx;
  Ecore_List *devices;
};

void
cb_manager_get_udi(void *data, void *reply, DBusError *err)
{
    DBusMessageIter iter, sub;
    char *udi;

    if (dbus_error_is_set(err))
     {
	printf("Error: %s - %s\n" ,err->name, err->message);
	return;
     }

    dbus_message_iter_init(reply, &iter);
	dbus_message_iter_recurse(&iter, &sub);
    dbus_message_iter_get_basic(&sub, &udi);

    printf("Got udi: %s\n", udi);
}

void
cb_manager_get_interface(void *data, void *reply, DBusError *err)
{
    DBusMessageIter iter, sub;
    char *interface;

    if (dbus_error_is_set(err))
     {
	printf("Error: %s - %s\n" ,err->name, err->message);
	return;
     }

    dbus_message_iter_init(reply, &iter);
    dbus_message_iter_recurse(&iter, &sub);
    dbus_message_iter_get_basic(&sub, &interface);

    printf("Got interface: %s\n", interface);
}

void
cb_manager_get_driver(void *data, void *reply, DBusError *err)
{
    DBusMessageIter iter, sub;
    char *driver;

    if (dbus_error_is_set(err))
     {
	printf("Error: %s - %s\n" ,err->name, err->message);
	return;
     }

    dbus_message_iter_init(reply, &iter);
    dbus_message_iter_recurse(&iter, &sub);
    dbus_message_iter_get_basic(&sub, &driver);

    printf("Got driver: %s\n", driver);
}

void
cb_manager_get_capabilities(void *data, void *reply, DBusError *err)
{
    DBusMessageIter iter, sub;
    dbus_uint32_t caps;

    if (dbus_error_is_set(err))
     {
	printf("Error: %s - %s\n" ,err->name, err->message);
	return;
     }

    dbus_message_iter_init(reply, &iter);
    dbus_message_iter_recurse(&iter, &sub);
    dbus_message_iter_get_basic(&sub, &caps);

    printf("Got capabilities: %i\n", caps);
}

void
cb_manager_get_ip4address(void *data, void *reply, DBusError *err)
{
    DBusMessageIter iter, sub;
    dbus_int32_t ip;

    if (dbus_error_is_set(err))
     {
	printf("Error: %s - %s\n" ,err->name, err->message);
	return;
     }

    dbus_message_iter_init(reply, &iter);
    dbus_message_iter_recurse(&iter, &sub);
    dbus_message_iter_get_basic(&sub, &ip);

    printf("Got IPv4 address: %i.%i.%i.%i\n",
	   (ip       & 0xff),
	   ((ip >> 8 ) & 0xff),
	   ((ip >> 16) & 0xff),
	   ((ip >> 24) & 0xff)
	   );
}

void
cb_manager_get_state(void *data, void *reply, DBusError *err)
{
    DBusMessageIter iter, sub;
    dbus_uint32_t state;

    if (dbus_error_is_set(err))
     {
	printf("Error: %s - %s\n" ,err->name, err->message);
	return;
     }

    dbus_message_iter_init(reply, &iter);
    dbus_message_iter_recurse(&iter, &sub);
    dbus_message_iter_get_basic(&sub, &state);

    printf("Got state: %i\n", state);
}

void
cb_manager_get_ip4config(void *data, void *reply, DBusError *err)
{
    DBusMessageIter iter, sub;
    char *ip4config;

    if (dbus_error_is_set(err))
     {
	printf("Error: %s - %s\n" ,err->name, err->message);
	return;
     }

    dbus_message_iter_init(reply, &iter);
    dbus_message_iter_recurse(&iter, &sub);
    dbus_message_iter_get_basic(&sub, &ip4config);

    printf("Got IPv4 config path: %s\n", ip4config);
}

void
cb_manager_get_carrier(void *data, void *reply, DBusError *err)
{
    DBusMessageIter iter, sub;
    dbus_uint32_t carrier;

   if (dbus_error_is_set(err))
     {
	printf("Error: %s - %s\n" ,err->name, err->message);
	return;
     }

    dbus_message_iter_init(reply, &iter);
    dbus_message_iter_recurse(&iter, &sub);
    dbus_message_iter_get_basic(&sub, &carrier);

    printf("Got carrier: %i\n", carrier);
}

void
cb_manager_get_type(void *data, void *reply, DBusError *err)
{
    DBusMessageIter iter, sub;
    dbus_uint32_t type;

    if (dbus_error_is_set(err))
      {
	 printf("Error: %s - %s\n" ,err->name, err->message);
	 return;
      }

    dbus_message_iter_init(reply, &iter);
    dbus_message_iter_recurse(&iter, &sub);
    dbus_message_iter_get_basic(&sub, &type);

    printf("Got type: %i (1 = ethernet, 2 = wireless)\n", type);
}

void
cb_manager_get_devices(void *data, void *reply, DBusError *err)
{
   NM_Manager *app = data;
   const char *dev;

   if (dbus_error_is_set(err))
     {
	printf("Error: %s - %s\n" ,err->name, err->message);
	return;
     }
   
   app->devices = reply;
   ecore_list_first_goto(app->devices);
   printf("Got devices:\n");
   while ((dev = ecore_list_next(app->devices)))
     {
	printf("%s\n", dev);
        e_nm_device_get_udi(app->ctx, dev, cb_manager_get_udi, app);
	e_nm_device_get_interface(app->ctx, dev, cb_manager_get_interface, app);
	e_nm_device_get_driver(app->ctx, dev, cb_manager_get_driver, app);
	e_nm_device_get_capabilities(app->ctx, dev, cb_manager_get_capabilities, app);
	e_nm_device_get_ip4address(app->ctx, dev, cb_manager_get_ip4address, app);
	e_nm_device_get_state(app->ctx, dev, cb_manager_get_state, app);
	/* FIXME: Getting the ip4config needs fixing */
	//e_nm_device_get_ip4config(app->ctx, dev, cb_manager_get_ip4config, app);
	e_nm_device_get_carrier(app->ctx, dev, cb_manager_get_carrier, app);
	e_nm_device_get_type(app->ctx, dev, cb_manager_get_type, app);
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
