#include "enna.h"
#include <E_Hal.h>
#include <dbus/dbus.h>
#include <libhal.h>
#include <libhal-storage.h>

#include "hal_storage.h"
#include "hal_volume.h"

#define ENNA_MODULE_NAME   "hal"

#define EHAL_STORAGE_NAME  "storage"
#define EHAL_VOLUME_NAME   "volume"
#define EHAL_ACTION_ADD    "DeviceAdded"
#define EHAL_ACTION_REMOVE "DeviceRemoved"
#define EHAL_ACTION_UPDATE "NewCapability"

typedef struct _Enna_Module_Hal
{
    Evas *e;
    Enna_Module *em;
    E_DBus_Connection *dbus;
    DBusConnection *conn;
    LibHalContext *ctx;
    DBusError error;

    Ecore_List *storages;
    Ecore_List *volumes;
} Enna_Module_Hal;

static Enna_Module_Hal *mod;

/***********************************************/
/*          HAL Storage Management             */
/***********************************************/

static void
ehal_add_storage (char *udi)
{
    storage_t *s;

    if (!udi)
        return;

    s = storage_append (mod->ctx, udi);
    ecore_list_append (mod->storages, s);
}

static void
ehal_remove_storage (char *udi)
{
    storage_t *s;

    if (!udi)
        return;

    s = ecore_list_first_goto (mod->storages);
    while ((s = ecore_list_next (mod->storages)))
        if (!strcmp (s->udi, udi))
            storage_free (s);
}

static void
ehal_find_storages (void *data, void *reply, DBusError *error)
{
    E_Hal_Manager_Find_Device_By_Capability_Return *ret = reply;
    char *udi;

    if (!ret || !ret->strings)
        return;

    if (dbus_error_is_set (error))
    {
        dbus_error_free (error);
        return;
    }

    ecore_list_first_goto (ret->strings);
    while ((udi = ecore_list_next (ret->strings)))
        ehal_add_storage (udi);
}

static void
ehal_is_storage (void *data, void *reply, DBusError *error)
{
    E_Hal_Device_Query_Capability_Return *ret = reply;
    char *udi = data;

    if (dbus_error_is_set (error))
    {
        dbus_error_free (error);
        return;
    }

    if (ret && ret->boolean)
        ehal_add_storage (udi);
}

/***********************************************/
/*           HAL Volume Management             */
/***********************************************/

static void
vfs_add_volume_entry (volume_t *v)
{
    char name[256], tmp[4096];
    int caps = 0;
    const char *icon = NULL;
    const char *type = NULL;
    const char *uri = NULL;
    Enna_Volume *evol;
    char *ic = NULL;

    if (!v)
        return;

    switch (v->type)
    {
        /* discard unknown volumes */
    case VOLUME_TYPE_UNKNOWN:
        return;

    case VOLUME_TYPE_HDD:
        /* discarded un-accessible HDDs */
        if (!v->mounted)
            return;

        caps = ENNA_CAPS_MUSIC | ENNA_CAPS_VIDEO | ENNA_CAPS_PHOTO;

        switch (v->s->type)
        {
        case LIBHAL_DRIVE_TYPE_CAMERA:
            ic = "icon/dev/camera";
            break;

        case LIBHAL_DRIVE_TYPE_PORTABLE_AUDIO_PLAYER:
            ic = "icon/dev/ipod";
            break;

        case LIBHAL_DRIVE_TYPE_FLASHKEY:
        case LIBHAL_DRIVE_TYPE_REMOVABLE_DISK:
            ic = "icon/dev/usbstick";
            break;

        case LIBHAL_DRIVE_TYPE_COMPACT_FLASH:
        case LIBHAL_DRIVE_TYPE_MEMORY_STICK:
        case LIBHAL_DRIVE_TYPE_SMART_MEDIA:
        case LIBHAL_DRIVE_TYPE_SD_MMC:
            ic = "icon/dev/memorycard";
            break;

        default:
            ic = "icon/dev/hdd";
            break;
        }

        icon = eina_stringshare_add(ic);
	type =  eina_stringshare_add("file://");
	snprintf (tmp, sizeof (tmp), "file://%s", v->mount_point);
	uri = eina_stringshare_add(tmp);
        break;

    case VOLUME_TYPE_CD:
        caps = ENNA_CAPS_MUSIC | ENNA_CAPS_VIDEO | ENNA_CAPS_PHOTO;
        icon =  eina_stringshare_add("icon/dev/cdrom");
	type =  eina_stringshare_add("file://");
	snprintf (tmp, sizeof (tmp), "file://%s", v->mount_point);
	uri = eina_stringshare_add(tmp);
        break;

    case VOLUME_TYPE_CDDA:
        caps = ENNA_CAPS_MUSIC;
        icon =  eina_stringshare_add("icon/dev/cdda2");
	type =  eina_stringshare_add("cdda://");
	uri  =  eina_stringshare_add("cdda://");
        break;

    case VOLUME_TYPE_DVD:
        caps = ENNA_CAPS_MUSIC | ENNA_CAPS_VIDEO | ENNA_CAPS_PHOTO;
        icon =  eina_stringshare_add("icon/dev/dvd");
	type =  eina_stringshare_add("file://");
	snprintf (tmp, sizeof (tmp), "file://%s", v->mount_point);
	uri = eina_stringshare_add(tmp);

        break;

    case VOLUME_TYPE_DVD_VIDEO:
        caps = ENNA_CAPS_VIDEO;
        icon =  eina_stringshare_add("icon/dev/dvd");
	type =  eina_stringshare_add("dvd://"); /* Need a way to manage dvdnav:// !*/
	uri  =  eina_stringshare_add("dvd://"); /* Need a way to manage dvdnav:// !*/
        break;

    case VOLUME_TYPE_VCD:
    case VOLUME_TYPE_SVCD:
        caps = ENNA_CAPS_VIDEO;
        icon = eina_stringshare_add("icon/dev/cdrom");
	type =  eina_stringshare_add("vcd://");
	uri =  eina_stringshare_add("vcd://");
	break;
    }

    /* get volume displayed name/label */
    memset (name, '\0', sizeof (name));
    if (v->partition_label)
        snprintf (name, sizeof (name), "%s", v->partition_label);
    else if (v->label)
        snprintf (name, sizeof (name), "%s", v->label);
    else if (v->s)
        snprintf (name, sizeof (name), "%s %s", v->s->vendor, v->s->model);

    if (!v->name)
        v->name = strdup (name);

    evol = calloc(1, sizeof(Enna_Volume));
    evol->name = strdup(name);
    evol->label = strdup(name);
    evol->icon = eina_stringshare_add(icon);
    evol->type = eina_stringshare_add(type);
    evol->uri = eina_stringshare_add(uri);
    enna_volumes_append(evol->type, evol);
    v->enna_volume = evol;
}

static void
ehal_add_volume (char *udi)
{
    volume_t *v;

    if (!udi)
        return;

    v = volume_append (mod->ctx, udi);
    if (v && v->parent)
    {
        storage_t *s;
        s = storage_find (mod->storages, v->parent);
        if (s)
            v->s = s;
    }

    ecore_list_append (mod->volumes, v);
    vfs_add_volume_entry (v);
}

static void
ehal_remove_volume (char *udi)
{
    volume_t *v;

    if (!udi)
        return;

    v = ecore_list_first_goto (mod->volumes);
    while ((v = ecore_list_next (mod->volumes)))
    {
        if (v->udi && !strcmp (v->udi, udi))
        {
	    if (v->enna_volume)
	    {
		enna_volumes_remove(v->enna_volume->type, v->enna_volume);
		printf("remove volume : %s\n", v->enna_volume->label);
	    }
            volume_free (v);
        }
    }
}

static void
ehal_find_volumes (void *data, void *reply, DBusError *error)
{
    E_Hal_Manager_Find_Device_By_Capability_Return *ret = reply;
    char *udi;

    if (!ret || !ret->strings)
        return;

    if (dbus_error_is_set (error))
    {
        dbus_error_free (error);
        return;
    }

    ecore_list_first_goto (ret->strings);
    while ((udi = ecore_list_next (ret->strings)))
        ehal_add_volume (udi);
}

static void
ehal_is_volume (void *data, void *reply, DBusError *error)
{
    E_Hal_Device_Query_Capability_Return *ret = reply;
    char *udi = data;

    if (dbus_error_is_set (error))
    {
        dbus_error_free (error);
        return;
    }

    if (ret && ret->boolean)
      ehal_add_volume (udi);
}

/***********************************************/
/*           HAL Device Management             */
/***********************************************/

static void
ehal_device_added (void *data, DBusMessage *msg)
{
    Enna_Module_Hal *mod = data;
    DBusError err;
    char *udi;

    dbus_error_init (&err);
    dbus_message_get_args (msg, &err,
                           DBUS_TYPE_STRING, &udi, DBUS_TYPE_INVALID);

    enna_log (ENNA_MSG_EVENT, ENNA_MODULE_NAME,
              "Adding newly discovered device: %s", udi);

    e_hal_device_query_capability (mod->dbus, udi, EHAL_STORAGE_NAME,
                                   ehal_is_storage, udi);
    e_hal_device_query_capability (mod->dbus, udi, EHAL_VOLUME_NAME,
                                   ehal_is_volume, udi);
}

static void
ehal_device_removed (void *data, DBusMessage *msg)
{
    DBusError err;
    char *udi;

    dbus_error_init (&err);

    dbus_message_get_args (msg, &err,
                           DBUS_TYPE_STRING, &udi, DBUS_TYPE_INVALID);

    enna_log (ENNA_MSG_EVENT, ENNA_MODULE_NAME,
              "Removing newly disconnected device: %s", udi);

    ehal_remove_storage (udi);
    ehal_remove_volume (udi);
}

/* Module interface */

Enna_Module_Api module_api = {
    ENNA_MODULE_VERSION,
    ENNA_MODULE_VOLUME,
    "volume_hal"
};

void
module_init (Enna_Module *em)
{
    if (!em)
        return;

    mod = calloc(1, sizeof(Enna_Module_Hal));
    mod->em = em;
    em->mod = mod;

    mod->dbus = e_dbus_bus_get (DBUS_BUS_SYSTEM);
    if (!mod->dbus)
        goto edbus_error;

    dbus_error_init (&mod->error);
    mod->conn = dbus_bus_get (DBUS_BUS_SYSTEM, &mod->error);
    if (!mod->conn)
        goto dbus_error;

    mod->ctx = libhal_ctx_new ();
    if (!mod->ctx)
        goto hal_error;

    libhal_ctx_set_dbus_connection (mod->ctx, mod->conn);
    libhal_ctx_init (mod->ctx, &mod->error);

    mod->storages = ecore_list_new ();
    ecore_list_free_cb_set (mod->storages, ECORE_FREE_CB (storage_free));

    mod->volumes = ecore_list_new ();
    ecore_list_free_cb_set (mod->volumes, ECORE_FREE_CB (volume_free));

    e_hal_manager_find_device_by_capability (mod->dbus,
                                             EHAL_STORAGE_NAME,
                                             ehal_find_storages, mod);
    e_hal_manager_find_device_by_capability (mod->dbus,
                                             EHAL_VOLUME_NAME,
                                             ehal_find_volumes, mod);

    e_dbus_signal_handler_add (mod->dbus, E_HAL_SENDER, E_HAL_MANAGER_PATH,
                               E_HAL_MANAGER_INTERFACE, EHAL_ACTION_ADD,
                               ehal_device_added, mod);

    e_dbus_signal_handler_add (mod->dbus, E_HAL_SENDER, E_HAL_MANAGER_PATH,
                               E_HAL_MANAGER_INTERFACE, EHAL_ACTION_REMOVE,
                               ehal_device_removed, NULL);

    return;

 hal_error:
    dbus_connection_unref (mod->conn);
    dbus_error_free (&mod->error);

 dbus_error:
    e_dbus_connection_close (mod->dbus);
    e_dbus_shutdown ();

 edbus_error:
    return;
}

void
module_shutdown (Enna_Module *em)
{
    Enna_Module_Hal *mod;

    mod = em->mod;;

    libhal_ctx_shutdown (mod->ctx, &mod->error);
    libhal_ctx_free (mod->ctx);

    dbus_connection_unref (mod->conn);
    dbus_error_free (&mod->error);

    e_dbus_connection_close (mod->dbus);
    e_dbus_shutdown ();
}
