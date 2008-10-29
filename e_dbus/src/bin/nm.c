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

static void
cb_manager_get_devices(void *data, void *reply, DBusError *err)
{
    NM_Manager *app = data;
    E_NM_Device *device;

    if (dbus_error_is_set(err))
    {
        printf("Error: %s - %s\n", err->name, err->message);
        ecore_list_destroy(reply);
        ecore_main_loop_quit();
        return;
    }
   
    app->devices = reply;
    ecore_list_first_goto(app->devices);
    while ((device = ecore_list_next(app->devices)))
    {
        e_nm_device_dump_device(device);
    }
    ecore_list_destroy(app->devices);
    ecore_main_loop_quit();
}

int 
main(int argc, char **argv)
{
    NM_Manager *app;
    ecore_init();
    eina_stringshare_init();
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
    eina_stringshare_shutdown();
    ecore_shutdown();
    return 0;
}
