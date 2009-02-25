#include "E_Hal.h"
#include <config.h>

#if EWL_GUI
#include <Ewl.h>
#include <Efreet.h>
#endif

#include <string.h>

/* test app */

static E_DBus_Connection *conn;
#if EWL_GUI
static int mount_id = 0;
#endif

#define DEVICE_TYPE_STORAGE 1
#define DEVICE_TYPE_VOLUME  2
typedef struct Device Device;
struct Device {
  int type;
  char *udi;
};

typedef struct Storage Storage;
struct Storage {
  int type;
  char *udi;
  char *bus;
  char *drive_type;

  char *model;
  char *vendor;
  char *serial;

  char removable;
  char media_available;
  unsigned long media_size;

  char requires_eject;
  char hotpluggable;
  char media_check_enabled;

  struct {
    char *drive;
    char *volume;
  } icon;

  Eina_List *volumes;
};


typedef struct Volume Volume;
struct Volume {
  int type;
  char *udi;
  char *uuid;
  char *label;
  char *fstype;

  char partition;
  char *partition_label;
  char mounted;
  char *mount_point;

  Storage *storage;
};

void volume_free(Volume *volume);

static Eina_List *storage_devices;
static Eina_List *volumes;

void
devices_dirty(void)
{
#if EWL_GUI
  Ewl_Widget *mvc;

  mvc = ewl_widget_name_find("device_mvc");
  ewl_mvc_dirty_set(EWL_MVC(mvc), 1);
#endif
}


Storage *
storage_new(void)
{
  Storage *s;
  s = calloc(1, sizeof(Storage));
  s->type = DEVICE_TYPE_STORAGE;
  s->volumes = NULL;
  return s;
}

void
storage_free(Storage *storage)
{
  Volume *v;
  Eina_List *l;
  printf("storage_free: %s\n", storage->udi);

  /* disconnect storage from volume */
  EINA_LIST_FOREACH(storage->volumes, l, v)
    v->storage = NULL;
  while (storage->volumes)
    {
       v = eina_list_data_get(storage->volumes);
       volume_free(v);
       storage->volumes = eina_list_remove_list(storage->volumes, storage->volumes);
    }
    
  if (storage->udi) free(storage->udi);
  if (storage->bus) free(storage->bus);
  if (storage->drive_type) free(storage->drive_type);

  if (storage->model) free(storage->model);
  if (storage->vendor) free(storage->vendor);
  if (storage->serial) free(storage->serial);
  if (storage->icon.drive) free(storage->icon.drive);
  if (storage->icon.volume) free(storage->icon.volume);

  free(storage);
}

static int
storage_find_helper(Storage *s, const char *udi)
{
  if (!s->udi) return -1;
  return strcmp(s->udi, udi);
}

Storage *
storage_find(const char *udi)
{
  Storage *s = NULL;
  if (!udi) return NULL;
  s = eina_list_search_unsorted(storage_devices, (Eina_Compare_Cb)storage_find_helper, udi);
  return s;
}

void
storage_remove(const char *udi)
{
  Storage *sto;

  if ((sto = storage_find(udi)))
  {
    storage_devices = eina_list_remove(storage_devices, sto);
    storage_free(sto);
    devices_dirty();
  }
}

static void
cb_storage_properties(void *data, void *reply_data, DBusError *error)
{
  Storage *s = data;
  E_Hal_Properties *ret = reply_data;
  int err = 0;

  if (dbus_error_is_set(error)) 
  {
    // XXX handle...
    dbus_error_free(error);
    goto error;
  }

  s->bus = e_hal_property_string_get(ret, "storage.bus", &err);
  if (err) goto error;
  s->drive_type = e_hal_property_string_get(ret, "storage.drive_type", &err);
  if (err) goto error;
  s->model = e_hal_property_string_get(ret, "storage.model", &err);
  if (err) goto error;
  s->vendor = e_hal_property_string_get(ret, "storage.vendor", &err);
  if (err) goto error;
  s->serial = e_hal_property_string_get(ret, "storage.serial", &err);

  s->removable = e_hal_property_bool_get(ret, "storage.removable", &err);
  if (err) goto error;

  if (s->removable)
  {
    s->media_available = e_hal_property_bool_get(ret, "storage.removable.media_available", &err);
    s->media_size = e_hal_property_uint64_get(ret, "storage.removable.media_size", &err);
  }

  s->requires_eject = e_hal_property_bool_get(ret, "storage.requires_eject", &err);
  s->hotpluggable = e_hal_property_bool_get(ret, "storage.hotpluggable", &err);
  s->media_check_enabled = e_hal_property_bool_get(ret, "storage.media_check_enabled", &err);

  s->icon.drive = e_hal_property_string_get(ret, "storage.icon.drive", &err);
  s->icon.volume = e_hal_property_string_get(ret, "storage.icon.volume", &err);

  devices_dirty();
  /* now fetch the children (volumes?) */

  //printf("Got storage:\n  udi: %s\n  bus: %s\n  drive_type: %s\n  model: %s\n  vendor: %s\n  serial: %s\n  icon.drive: %s\n  icon.volume: %s\n\n", s->udi, s->bus, s->drive_type, s->model, s->vendor, s->serial, s->icon.drive, s->icon.volume);
  return;

error:
  storage_remove(s->udi);
}

Storage *
storage_append(const char *udi)
{
  Storage *s;
  if (!udi) return NULL;
  s = storage_new();
  s->udi = strdup(udi);
  storage_devices = eina_list_append(storage_devices, s);
  e_hal_device_get_all_properties(conn, s->udi, cb_storage_properties, s);
  devices_dirty();
  return s;
}

Volume *
volume_new(void)
{
  Volume *v;
  v = calloc(1, sizeof(Volume));
  v->type = DEVICE_TYPE_VOLUME;
  return v;
}

void
volume_free(Volume *volume)
{
  if (!volume) return;

  /* disconnect volume from storage */
  if (volume->storage)
  {
    volume->storage->volumes = eina_list_remove(volume->storage->volumes, volume);
    volume_free(volume);
  }

  if (volume->udi) free(volume->udi);
  if (volume->uuid) free(volume->uuid);
  if (volume->label) free(volume->label);
  if (volume->fstype) free(volume->fstype);
  if (volume->partition_label) free(volume->partition_label);
  if (volume->mount_point) free(volume->mount_point);

  free(volume);
}

static int
volume_find_helper(Volume *v, const char *udi)
{
  if (!v->udi) return -1;
  return strcmp(v->udi, udi);
}

Volume *
volume_find(const char *udi)
{
  if (!udi) return NULL;
  return eina_list_search_unsorted(volumes, (Eina_Compare_Cb)volume_find_helper, udi);
}

void
volume_remove(const char *udi)
{
  Volume *vol;

  if ((vol = volume_find(udi)))
  {
    volumes = eina_list_remove(volumes, vol);
    volume_free(vol);
    devices_dirty();
  }
}

static void
cb_volume_properties(void *data, void *reply_data, DBusError *error)
{
  Volume *v = data;
  Storage *s = NULL;
  E_Hal_Device_Get_All_Properties_Return *ret = reply_data;
  int err = 0;
  char *str = NULL;

  if (dbus_error_is_set(error)) 
  {
    // XXX handle...
    dbus_error_free(error);
    goto error;
  }

  /* skip volumes with volume.ignore set */
  if (e_hal_property_bool_get(ret, "volume.ignore", &err) || err)
    goto error;

  /* skip volumes that aren't filesystems */
  str = e_hal_property_string_get(ret, "volume.fsusage", &err);
  if (err || !str) goto error;
  if (strcmp(str, "filesystem")) goto error;
  free(str);
  str = NULL;

  v->uuid = e_hal_property_string_get(ret, "volume.uuid", &err);
  if (err) goto error;

  v->label = e_hal_property_string_get(ret, "volume.label", &err);
  if (err) goto error;

  v->fstype = e_hal_property_string_get(ret, "volume.fstype", &err);
  if (err) goto error;

  v->mounted = e_hal_property_bool_get(ret, "volume.is_mounted", &err);
  if (err) goto error;

  v->partition = e_hal_property_bool_get(ret, "volume.is_partition", &err);
  if (err) goto error;

  v->mount_point = e_hal_property_string_get(ret, "volume.mount_point", &err);
  if (err) goto error;

  if (v->partition)
  {
    v->partition_label = e_hal_property_string_get(ret, "volume.partition.label", &err);
    if (err) goto error;
  }

  str = e_hal_property_string_get(ret, "info.parent", &err);
  if (!err && str)
  {
    s = storage_find(str);
    if (s)
    {
      v->storage = s;
      s->volumes = eina_list_append(s->volumes, v);
    }
    free(str);
    str = NULL;
  }
  
  //printf("Got volume\n  udi: %s\n  uuid: %s\n  fstype: %s\n  label: %s\n  partition: %d\n  partition_label: %s\n  mounted: %d\n  mount_point: %s\n\n", v->udi, v->uuid, v->fstype,  v->label, v->partition, v->partition ? v->partition_label : "(not a partition)", v->mounted, v->mount_point);
  //if (s) printf("  for storage: %s\n", s->udi);
  //else printf("  storage unknown\n");

  devices_dirty();
  return;

error:
  if (str) free(str);
  volume_remove(v->udi);
  return;
  
}

void
volume_setup(Volume *v)
{
  e_hal_device_get_all_properties(conn, v->udi, cb_volume_properties, v);
}

Volume *
volume_append(const char *udi)
{
  Volume *v;
  if (!udi) return NULL;
  printf("ADDING %s\n", udi);
  v = volume_new();
  v->udi = strdup(udi);
  volumes = eina_list_append(volumes, v);
  volume_setup(v);

  //this will get called when volume_setup() returns, which is more important
  //devices_dirty();

  return v;
}

static void
cb_test_get_all_devices(void *user_data, void *reply_data, DBusError *error)
{
  E_Hal_Manager_Get_All_Devices_Return *ret = reply_data;
  Eina_List *l;
  char *device;
  
  if (!ret || !ret->strings) return;

  if (dbus_error_is_set(error)) 
  {
    // XXX handle...
    dbus_error_free(error);
    return;
  }

  EINA_LIST_FOREACH(ret->strings, l, device)
  {
    printf("device: %s\n", device);
  }
}

static void
cb_test_find_device_by_capability_storage(void *user_data, void *reply_data, DBusError *error)
{
  E_Hal_Manager_Find_Device_By_Capability_Return *ret = reply_data;
  Eina_List *l;
  char *device;
  
  if (!ret || !ret->strings) return;

  if (dbus_error_is_set(error)) 
  {
    // XXX handle...
    dbus_error_free(error);
    return;
  }

  EINA_LIST_FOREACH(ret->strings, l, device)
    storage_append(device);
}

static void
cb_test_find_device_by_capability_volume(void *user_data, void *reply_data, DBusError *error)
{
  E_Hal_Manager_Find_Device_By_Capability_Return *ret = reply_data;
  Eina_List *l;
  char *device;
  
  if (!ret || !ret->strings) return;

  if (dbus_error_is_set(error)) 
  {
    // XXX handle...
    dbus_error_free(error);
    return;
  }

  EINA_LIST_FOREACH(ret->strings, l, device)
    volume_append(device);
}

static void
cb_is_storage(void *user_data, void *reply_data, DBusError *error)
{
  char *udi = user_data;
  E_Hal_Device_Query_Capability_Return *ret = reply_data;

  if (dbus_error_is_set(error)) 
  {
    // XXX handle...
    dbus_error_free(error);
    goto error;
  }

  if (ret && ret->boolean)
    storage_append(udi);

error:
  free(udi);
}

static void
cb_is_volume(void *user_data, void *reply_data, DBusError *error)
{
  char *udi = user_data;
  E_Hal_Device_Query_Capability_Return *ret = reply_data;

  if (dbus_error_is_set(error)) 
  {
    // XXX handle...
    dbus_error_free(error);
    goto error;
  }

  if (ret && ret->boolean)
    volume_append(udi);

error:
  free(udi);
}

static void
cb_signal_device_added(void *data, DBusMessage *msg)
{
  DBusError err;
  char *udi;
  int ret;

  dbus_error_init(&err);
  dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &udi, DBUS_TYPE_INVALID);
  udi = strdup(udi);
  printf("Device added: %s\n", udi); 
  ret = e_hal_device_query_capability(conn, udi, "storage", cb_is_storage, strdup(udi));
  e_hal_device_query_capability(conn, udi, "volume", cb_is_volume, strdup(udi));
}

void
cb_signal_device_removed(void *data, DBusMessage *msg)
{
  DBusError err;
  char *udi;

  dbus_error_init(&err);

  dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &udi, DBUS_TYPE_INVALID);
  printf("Removed: %s\n", udi);
  storage_remove(udi);
  volume_remove(udi);
}

void
cb_signal_new_capability(void *data, DBusMessage *msg)
{
  DBusError err;
  char *udi, *capability;

  dbus_error_init(&err);

  dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &udi, DBUS_TYPE_STRING, &capability, DBUS_TYPE_INVALID);
  if (!strcmp(capability, "storage"))
    storage_append(udi);

}

/*** gui ***/

#if EWL_GUI
static void
cb_window_destroy(Ewl_Widget *w, void *ev, void *data)
{
  ewl_main_quit();
}

static void
cb_window_close(Ewl_Widget *w, void *ev, void *data)
{
  ewl_widget_destroy(w);
}

static void
cb_volume_unmounted(void *user_data, void *method_return, DBusError *error)
{
  Volume *vol = user_data;
  vol->mounted = 0;
  printf("Volume unmounted reply: %s\n", vol->udi);
}

static void
cb_volume_mounted(void *user_data, void *method_return, DBusError *error)
{
  Volume *vol = user_data;
  vol->mounted = 1;
  printf("Volume mounted reply: %s\n", vol->udi);
}

static void
cb_device_view_clicked(Ewl_Widget *w, void *ev, void *data)
{
  Device *dev = data;
  char buf[4096];

  printf("Device clicked: %s\n", dev->udi);
  if (dev->type == DEVICE_TYPE_VOLUME)
  {
    Volume *vol = (Volume *)dev;
    if (vol->mounted)
    {
      e_hal_device_volume_unmount(conn, vol->udi, NULL, cb_volume_unmounted, vol);
    }
    else
    {
      char *mount_point;
#if 0
      if (vol->mount_point && vol->mount_point[0])
        mount_point = vol->mount_point;
#endif
      if (vol->label && vol->label[0])
        mount_point = vol->label;
      else if (vol->uuid && vol->uuid[0])
        mount_point = vol->uuid;
      else
      {
        // XXX need to check this...
        snprintf(buf, sizeof(buf), "%d\n", mount_id++);
        mount_point = buf;
      }

      printf("Attempting to mount %s to %s\n", vol->udi, mount_point);
      e_hal_device_volume_mount(conn, vol->udi, mount_point, vol->fstype, NULL, cb_volume_mounted, vol);
    }
  }
}


static Ewl_Widget *
cb_device_view_constructor(void)
{
  Ewl_Widget *icon;

  icon = ewl_icon_simple_new();
  ewl_box_orientation_set(EWL_BOX(icon), EWL_ORIENTATION_HORIZONTAL);
  return icon;
}

static void
device_view_device_set(Ewl_Icon *icon, Device *dev)
{
  char buf[500];
  const char *path;
  const char *icon_name = NULL;

  if (dev->type == DEVICE_TYPE_STORAGE)
  {
    Storage *s = (Storage *)dev;

    if (s->icon.drive && s->icon.drive[0])
      icon_name = s->icon.drive;

    snprintf(buf, sizeof(buf), "%s", (s->model && s->model[0]) ? s->model : "Unknown drive");
  }
  else if (dev->type == DEVICE_TYPE_VOLUME)
  {
    Volume *v = (Volume *)dev;
    if (v->storage && v->storage->icon.volume && v->storage->icon.volume[0])
      icon_name = v->storage->icon.volume;
    snprintf(buf, sizeof(buf), "%s (%s)", (v->label && v->label[0]) ? v->label : "Unlabeled Volume", v->fstype ? v->fstype : "Unknown");
  }
  else
  {
    fprintf(stderr, "Invalid device type.");
    buf[0] = 0;
  }

  ewl_icon_label_set(icon, buf);
  if (icon_name)
  {
    //printf("find icon path: %s\n", icon_name);
    path = efreet_icon_path_find("Tango", icon_name, 32);
    //printf("found: %s\n", path);
    if (path)
    {
      ewl_icon_image_set(icon, path, NULL);
      free(path);
    }
  }
  ewl_callback_append(EWL_WIDGET(icon), EWL_CALLBACK_CLICKED, cb_device_view_clicked, dev);

}

static void
cb_device_view_assign(Ewl_Widget *w, void *data)
{
  Device *dev = data;
  device_view_device_set(EWL_ICON(w), dev);
}

static Ewl_Widget *
cb_device_view_header_fetch(void *data, int column)
{
  Ewl_Widget *label;

  label = ewl_label_new();
  ewl_label_text_set(EWL_LABEL(label), "Device");
  ewl_widget_show(label);
  return label;
}

static int
cb_device_tree_expandable_get(void *data, unsigned int row)
{
  Eina_List *devices;
  Device *dev;

  devices = data;
  if (!devices) return FALSE;

  dev = eina_list_nth(devices, row);
  if (!dev) return FALSE;

  if (dev->type == DEVICE_TYPE_STORAGE)
  {
    Storage *s = (Storage *)dev;
    if (eina_list_count(s->volumes) > 0)
      return TRUE;
  }

  return FALSE;
}

static void *
cb_device_tree_expansion_data_fetch(void *data, unsigned int parent)
{
  Eina_List *devices;
  Device *dev;
  Storage *s;
  dev = data;

  devices = data;
  if (!devices) return NULL;

  dev = eina_list_nth(devices, parent);
  
  if (!dev) return NULL;
  if (dev->type != DEVICE_TYPE_STORAGE) return NULL;
  s = (Storage *)dev;
  return s->volumes;
}

#if 0
Ewl_Widget *
mountbox_list_new(void)
{
  Ewl_Widget *list;
  Ewl_Model *model;
  Ewl_View *view;

  model = ewl_model_ecore_list_get();

  view = ewl_view_new();
  ewl_view_constructor_set(view, cb_device_view_constructor);
  ewl_view_assign_set(view, EWL_VIEW_ASSIGN(cb_device_view_assign));

  list = ewl_list_new();
  ewl_mvc_model_set(EWL_MVC(list), model);
  ewl_mvc_view_set(EWL_MVC(list), view);
  ewl_mvc_data_set(EWL_MVC(list), volumes);

  ewl_widget_name_set(list, "volume_mvc");

  return list;
}
#endif

Ewl_Widget *
mountbox_tree_new(void)
{
  Ewl_Widget *tree;
  Ewl_Model *model;
  Ewl_View *view;

  model = ewl_model_ecore_list_get();
  ewl_model_expandable_set(model, cb_device_tree_expandable_get);
  ewl_model_expansion_data_fetch_set(model, cb_device_tree_expansion_data_fetch);

  view = ewl_view_new();
  ewl_view_constructor_set(view, cb_device_view_constructor);
  ewl_view_assign_set(view, EWL_VIEW_ASSIGN(cb_device_view_assign));
  ewl_view_header_fetch_set(view, cb_device_view_header_fetch);

  tree = ewl_tree2_new();
  ewl_mvc_model_set(EWL_MVC(tree), model);
  ewl_mvc_data_set(EWL_MVC(tree), storage_devices);
  ewl_tree2_column_append(EWL_TREE2(tree), view, FALSE);

  ewl_widget_name_set(tree, "device_mvc");

  return tree;
}

Ewl_Widget *
mountbox_mainwin_new(void)
{
  Ewl_Widget *win, *box, *list;
  win = ewl_window_new();
  ewl_window_title_set(EWL_WINDOW(win), "EWL Mountbox");
  ewl_window_class_set(EWL_WINDOW(win), "ewl_mountbox");
  ewl_window_name_set(EWL_WINDOW(win), "ewl_mountbox");

  ewl_object_fill_policy_set(EWL_OBJECT(win), EWL_FLAG_FILL_ALL);
  ewl_object_size_request(EWL_OBJECT(win), 400, 400);

  ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, cb_window_close, NULL);
  ewl_callback_append(win, EWL_CALLBACK_DESTROY, cb_window_destroy, NULL);

  box = ewl_vbox_new();
  ewl_container_child_append(EWL_CONTAINER(win), box);
  ewl_widget_show(box);

  list = mountbox_tree_new();
  ewl_container_child_append(EWL_CONTAINER(box), list);
  ewl_widget_show(list);

  return win;
}
#endif
int 
main(int argc, char **argv)
{
#if EWL_GUI
  Ewl_Widget *win;
#endif
  Storage *sto;
  Volume *vol;

  ecore_init();
  eina_stringshare_init();
  e_dbus_init();

#if EWL_GUI
  efreet_init();
  if (!ewl_init(&argc, argv))
  {
    fprintf(stderr, "Unable to init EWL.\n");
    return 1;
  }
#endif

  conn = e_dbus_bus_get(DBUS_BUS_SYSTEM);
  if (!conn)
  {
    printf("Error connecting to system bus. Is it running?\n");
    return 1;
  }

  storage_devices = NULL;
  volumes = NULL;

#if EWL_GUI
  win = mountbox_mainwin_new();
  ewl_widget_show(win);
#endif

  e_hal_manager_get_all_devices(conn, cb_test_get_all_devices, NULL);
  e_hal_manager_find_device_by_capability(conn, "storage", cb_test_find_device_by_capability_storage, NULL);
  e_hal_manager_find_device_by_capability(conn, "volume", cb_test_find_device_by_capability_volume, NULL);
  
  e_dbus_signal_handler_add(conn, "org.freedesktop.Hal", "/org/freedesktop/Hal/Manager", "org.freedesktop.Hal.Manager", "DeviceAdded", cb_signal_device_added, NULL);
  e_dbus_signal_handler_add(conn, "org.freedesktop.Hal", "/org/freedesktop/Hal/Manager", "org.freedesktop.Hal.Manager", "DeviceRemoved", cb_signal_device_removed, NULL);
  e_dbus_signal_handler_add(conn, "org.freedesktop.Hal", "/org/freedesktop/Hal/Manager", "org.freedesktop.Hal.Manager", "NewCapability", cb_signal_new_capability, NULL);

#if EWL_GUI
  ewl_main();
  efreet_shutdown();
  ewl_shutdown();
#else
  ecore_main_loop_begin();
#endif
  while (storage_devices)
    {
       sto = eina_list_data_get(storage_devices);
       storage_free(sto);
       storage_devices = eina_list_remove_list(storage_devices, storage_devices);
    }
  while (volumes)
    {
       vol = eina_list_data_get(volumes);
       volume_free(vol);
       volumes = eina_list_remove_list(volumes, volumes);
    }
  e_dbus_connection_close(conn);
  e_dbus_shutdown();
  eina_stringshare_shutdown();
  ecore_shutdown();
  return 1;
}
