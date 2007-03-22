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
cb_manager_get_devices(void *data, void *reply, DBusError *err)
{
  Ecore_List *devices;
  const char *dev;
  if (dbus_error_is_set(err))
  {
    //XXX
    printf("Error: %s - %s\n" ,err->name, err->message);
    return;
  }

  devices = reply;
  ecore_list_goto_first(devices);
  printf("Got devices:\n");
  while ((dev = ecore_list_next(devices)))
  {
    printf("  %s\n", dev);
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
}
