/*
 * enna_volume_manager.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_volume_manager.c is free software copyrighted by Nicolas Aguirre.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Nicolas Aguirre'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * enna_volume_manager.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Nicolas Aguirre OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "enna_volume_manager.h"

#include <string.h>
#include <E_Hal.h>

static E_DBus_Connection *conn;
static int          mount_id = 0;
static Enna        *enna = NULL;

typedef struct Hal_Device Hal_Device;
struct Hal_Device
{
   int                 type;
   char               *udi;
};

Storage            *
portable_audio_player_new(void)
{
   Storage            *s;

   dbg("NEW IPOD\n");

   s = calloc(1, sizeof(Storage));
   s->type = DEV_IPOD;
   s->volumes = ecore_list_new();
   return s;

}

Storage            *
storage_new(void)
{
   Storage            *s;

   s = calloc(1, sizeof(Storage));
   s->type = DEV_UNKNOWN;
   s->volumes = ecore_list_new();
   return s;
}

void
storage_free(Storage * storage)
{
   Volume             *v;

   ecore_list_first_goto(storage->volumes);
   while ((v = ecore_list_next(storage->volumes)))
      v->storage = NULL;
   ecore_list_destroy(storage->volumes);

   if (storage->udi)
      free(storage->udi);
   if (storage->bus)
      free(storage->bus);
   if (storage->drive_type)
      free(storage->drive_type);

   if (storage->model)
      free(storage->model);
   if (storage->vendor)
      free(storage->vendor);
   if (storage->serial)
      free(storage->serial);
   if (storage->icon.drive)
      free(storage->icon.drive);
   if (storage->icon.volume)
      free(storage->icon.volume);

   free(storage);
}

static int
storage_find_helper(Storage * s, const char *udi)
{
   if (!s->udi)
      return -1;
   return strcmp(s->udi, udi);
}

Storage            *
storage_find(const char *udi)
{
   Storage            *s = NULL;

   if (!udi)
      return NULL;
   s = ecore_list_find(enna->storage_devices,
		       ECORE_COMPARE_CB(storage_find_helper), udi);
   return s;
}

void
storage_remove(const char *udi)
{
   if (storage_find(udi))
      ecore_list_remove_destroy(enna->storage_devices);
}

static void
cb_storage_properties(void *data, void *reply_data, DBusError * error)
{
   Storage            *s = data;
   E_Hal_Properties   *ret = reply_data;
   int                 err = 0;

   s->bus = e_hal_property_string_get(ret, "storage.bus", &err);
   if (err)
      goto error;
   s->drive_type = e_hal_property_string_get(ret, "storage.drive_type", &err);
   if (err)
      goto error;
   s->model = e_hal_property_string_get(ret, "storage.model", &err);
   if (err)
      goto error;
   s->vendor = e_hal_property_string_get(ret, "storage.vendor", &err);
   if (err)
      goto error;
   s->serial = e_hal_property_string_get(ret, "storage.serial", &err);

   s->removable = e_hal_property_bool_get(ret, "storage.removable", &err);
   if (err)
      goto error;

   if (s->removable)
     {
	s->media_available =
	   e_hal_property_bool_get(ret, "storage.removable.media_available",
				   &err);
	s->media_size =
	   e_hal_property_uint64_get(ret, "storage.removable.media_size", &err);
     }

   s->requires_eject =
      e_hal_property_bool_get(ret, "storage.requires_eject", &err);
   s->hotpluggable = e_hal_property_bool_get(ret, "storage.hotpluggable", &err);
   s->media_check_enabled =
      e_hal_property_bool_get(ret, "storage.media_check_enabled", &err);

   s->icon.drive = e_hal_property_string_get(ret, "storage.icon.drive", &err);
   s->icon.volume = e_hal_property_string_get(ret, "storage.icon.volume", &err);

   return;

 error:
   storage_remove(s->udi);
}

Storage            *
portable_audio_player_append(const char *udi)
{
   Storage            *s;

   if (!udi)
      return NULL;
   s = portable_audio_player_new();
   s->udi = strdup(udi);
   ecore_list_append(enna->storage_devices, s);
   e_hal_device_get_all_properties(conn, s->udi, cb_storage_properties, s);

   return s;

}

Storage            *
storage_append(const char *udi)
{
   Storage            *s;

   if (!udi)
      return NULL;
   s = storage_new();
   s->udi = strdup(udi);
   ecore_list_append(enna->storage_devices, s);
   e_hal_device_get_all_properties(conn, s->udi, cb_storage_properties, s);

   return s;
}

Volume             *
volume_new(void)
{
   Volume             *v;

   v = calloc(1, sizeof(Volume));
   v->type = DEV_UNKNOWN;
   return v;
}

void
portable_audio_player_free(Volume * volume)
{
   if (!volume)
      return;

   /* disconnect volume from storage */
   if (volume->storage)
     {
	if (ecore_list_goto(volume->storage->volumes, volume))
	   ecore_list_remove(volume->storage->volumes);
     }

   if (volume->udi)
      free(volume->udi);
   if (volume->uuid)
      free(volume->uuid);
   if (volume->label)
      free(volume->label);
   if (volume->fstype)
      free(volume->fstype);
   if (volume->partition_label)
      free(volume->partition_label);
   if (volume->mount_point)
      free(volume->mount_point);
   if (volume->device)
      free(volume->device);
   free(volume);
}

void
volume_free(Volume * volume)
{
   if (!volume)
      return;

   /* disconnect volume from storage */
   if (volume->storage)
     {
	if (ecore_list_goto(volume->storage->volumes, volume))
	   ecore_list_remove(volume->storage->volumes);
     }

   if (volume->udi)
      free(volume->udi);
   if (volume->uuid)
      free(volume->uuid);
   if (volume->label)
      free(volume->label);
   if (volume->fstype)
      free(volume->fstype);
   if (volume->partition_label)
      free(volume->partition_label);
   if (volume->mount_point)
      free(volume->mount_point);
   if (volume->device)
      free(volume->device);
   free(volume);
}

static int
volume_find_helper(Volume * v, const char *udi)
{
   if (!v->udi)
      return -1;
   return strcmp(v->udi, udi);
}

Volume             *
volume_find(const char *udi)
{
   if (!udi)
      return NULL;
   return ecore_list_find(enna->volumes,
			  ECORE_COMPARE_CB(volume_find_helper), udi);
}

void
volume_remove(const char *udi)
{
   if (volume_find(udi))
     {
	ecore_list_remove_destroy(enna->volumes);

     }
}

static void
cb_volume_mounted(void *user_data, void *method_return, DBusError * error)
{
   Volume             *vol = user_data;

   vol->mounted = 1;

}

/* ADD Video DVD and send event */
static void
add_videodvd_disk(Volume * v, E_Hal_Device_Get_All_Properties_Return * ret)
{
   int                 err = 0;
   char               *str = NULL;
   Storage            *s = NULL;
   Volume             *dev;

   v->uuid = e_hal_property_string_get(ret, "volume.uuid", &err);
   if (err)
      goto error;

   v->label = e_hal_property_string_get(ret, "volume.label", &err);
   if (err)
      goto error;

   v->fstype = e_hal_property_string_get(ret, "volume.fstype", &err);
   if (err)
      goto error;

   v->mounted = e_hal_property_bool_get(ret, "volume.is_mounted", &err);
   if (err)
      goto error;

   v->partition = e_hal_property_bool_get(ret, "volume.is_partition", &err);
   if (err)
      goto error;

   v->mount_point = e_hal_property_string_get(ret, "volume.mount_point", &err);
   if (err)
      goto error;

   v->device = e_hal_property_string_get(ret, "block.device", &err);

   if (err)
      goto error;
   v->type = DEV_VIDEO_DVD;

   str = e_hal_property_string_get(ret, "info.parent", &err);
   if (!err && str)
     {
	s = storage_find(str);
	if (s)
	  {
	     v->storage = s;
	     ecore_list_append(s->volumes, v);
	  }
	free(str);
	str = NULL;
     }
   dev = malloc(sizeof(Volume));
   dev->type = v->type;
   dev->udi = strdup(v->udi);

   ecore_event_add(enna->events[ENNA_EVENT_DEV_VIDEO_DVD_ADD], dev, NULL, NULL);

   return;

 error:

   if (str)
      free(str);
   volume_remove(v->udi);
   return;
}
static void
add_cdrom_disk(Volume * v, E_Hal_Device_Get_All_Properties_Return * ret,
	       int is_cdrom)
{
   int                 err = 0;
   char               *str = NULL;
   Storage            *s = NULL;
   Volume             *dev;

   v->uuid = e_hal_property_string_get(ret, "volume.uuid", &err);
   if (err)
      goto error;

   v->label = e_hal_property_string_get(ret, "volume.label", &err);
   if (err)
      goto error;

   v->fstype = e_hal_property_string_get(ret, "volume.fstype", &err);
   if (err)
      goto error;

   v->mounted = e_hal_property_bool_get(ret, "volume.is_mounted", &err);
   if (err)
      goto error;

   v->partition = e_hal_property_bool_get(ret, "volume.is_partition", &err);
   if (err)
      goto error;

   v->mount_point = e_hal_property_string_get(ret, "volume.mount_point", &err);
   if (err)
      goto error;

   v->device = e_hal_property_string_get(ret, "block.device", &err);

   if (err)
      goto error;

   if (is_cdrom)
      v->type = DEV_DATA_CD;
   else
      v->type = DEV_DATA_DVD;

   if (v->partition)
     {
	v->partition_label =
	   e_hal_property_string_get(ret, "volume.partition.label", &err);
	if (err)
	   goto error;
     }

   str = e_hal_property_string_get(ret, "info.parent", &err);
   if (!err && str)
     {
	s = storage_find(str);
	if (s)
	  {
	     v->storage = s;
	     ecore_list_append(s->volumes, v);
	  }
	free(str);
	str = NULL;
     }
   dev = malloc(sizeof(Volume));
   dev->type = v->type;
   dev->udi = strdup(v->udi);

   if (is_cdrom)
      ecore_event_add(enna->events[ENNA_EVENT_DEV_DATA_CD_ADD], dev, NULL,
		      NULL);
   else
      ecore_event_add(enna->events[ENNA_EVENT_DEV_DATA_DVD_ADD], dev, NULL,
		      NULL);

   return;

 error:

   if (str)
      free(str);
   volume_remove(v->udi);
   return;

}

/* Detect CDDA device : complete volume informations and send event */
static void
add_cdda_disk(Volume * v, E_Hal_Device_Get_All_Properties_Return * ret)
{
   int                 err = 0;
   char               *str = NULL;
   Storage            *s = NULL;
   Volume             *dev;

   v->uuid = e_hal_property_string_get(ret, "volume.uuid", &err);
   if (err)
      goto error;

   v->label = e_hal_property_string_get(ret, "volume.label", &err);
   if (err)
      goto error;

   v->fstype = e_hal_property_string_get(ret, "volume.fstype", &err);
   if (err)
      goto error;

   v->mounted = e_hal_property_bool_get(ret, "volume.is_mounted", &err);
   if (err)
      goto error;

   v->partition = e_hal_property_bool_get(ret, "volume.is_partition", &err);
   if (err)
      goto error;

   v->mount_point = e_hal_property_string_get(ret, "volume.mount_point", &err);
   if (err)
      goto error;

   v->device = e_hal_property_string_get(ret, "block.device", &err);

   if (err)
      goto error;
   v->type = DEV_AUDIO_CD;

   if (v->partition)
     {
	v->partition_label =
	   e_hal_property_string_get(ret, "volume.partition.label", &err);
	if (err)
	   goto error;
     }

   str = e_hal_property_string_get(ret, "info.parent", &err);
   if (!err && str)
     {
	s = storage_find(str);
	if (s)
	  {
	     v->storage = s;
	     ecore_list_append(s->volumes, v);
	  }
	free(str);
	str = NULL;
     }
   dev = malloc(sizeof(Volume));
   dev->type = v->type;
   dev->udi = strdup(v->udi);

   ecore_event_add(enna->events[ENNA_EVENT_DEV_AUDIO_CD_ADD], dev, NULL, NULL);

   return;

 error:

   if (str)
      free(str);
   volume_remove(v->udi);
   return;
}

static void
cb_volume_properties(void *data, void *reply_data, DBusError * error)
{
   Volume             *v = data;
   char                buf[4096];
   Volume             *dev;
   Storage            *s = NULL;
   E_Hal_Device_Get_All_Properties_Return *ret = reply_data;
   int                 err = 0;
   char               *str = NULL;

   if (e_hal_property_bool_get(ret, "volume.is_disc", &err))
     {
	if (e_hal_property_bool_get(ret, "volume.disc.has_audio", &err))
	  {
	     add_cdda_disk(v, ret);
	     return;
	  }
	else if (e_hal_property_bool_get(ret, "volume.disc.is_videodvd", &err))
	  {
	     add_videodvd_disk(v, ret);
	     return;
	  }
	else if (e_hal_property_bool_get(ret, "volume.disc.has_data", &err))
	  {
	     str = e_hal_property_string_get(ret, "volume.disc.type", &err);
	     dbg("cdrom str : %s\n", str);

	     if (!strcmp(str, "cd_rom") || !strcmp(str, "cd_r")
		 || !strcmp(str, "cd_rw"))
	       {
		  add_cdrom_disk(v, ret, 1);
		  return;
	       }
	     else if (!strcmp(str, "dvd_rom") || !strcmp(str, "dvd_ram")
		      || !strcmp(str, "dvd_r") || !strcmp(str, "dvd_plus_r")
		      || !strcmp(str, "dvd_plus_rw"))
	       {
		  add_cdrom_disk(v, ret, 0);
		  return;
	       }
	  }
     }

   /* skip volumes with volume.ignore set */
   if (e_hal_property_bool_get(ret, "volume.ignore", &err) || err)
      goto error;

   /* skip volumes that aren't filesystems */
   str = e_hal_property_string_get(ret, "volume.fsusage", &err);
   if (err || !str)
      goto error;
   if (strcmp(str, "filesystem"))
      goto error;
   free(str);
   str = NULL;

   v->uuid = e_hal_property_string_get(ret, "volume.uuid", &err);
   if (err)
      goto error;

   v->label = e_hal_property_string_get(ret, "volume.label", &err);
   if (err)
      goto error;

   v->fstype = e_hal_property_string_get(ret, "volume.fstype", &err);
   if (err)
      goto error;

   v->mounted = e_hal_property_bool_get(ret, "volume.is_mounted", &err);
   if (err)
      goto error;

   v->partition = e_hal_property_bool_get(ret, "volume.is_partition", &err);
   if (err)
      goto error;

   v->mount_point = e_hal_property_string_get(ret, "volume.mount_point", &err);
   printf("%s v->mount_point\n", v->mount_point);

   if (err)
      goto error;

   if (v->partition)
     {
	v->partition_label =
	   e_hal_property_string_get(ret, "volume.partition.label", &err);
	if (err)
	   goto error;
     }

   str = e_hal_property_string_get(ret, "info.parent", &err);
   if (!err && str)
     {
	s = storage_find(str);
	if (s)
	  {
	     v->storage = s;
	     if (s->type == DEV_IPOD)
	       {
		  v->type = DEV_IPOD_DEVICE;
		  dev = malloc(sizeof(Volume));
		  dev->type = v->type;
		  dev->udi = strdup(v->udi);
		  ecore_event_add(enna->events[ENNA_EVENT_DEV_IPOD_ADD], dev,
				  NULL, NULL);

		  ecore_list_append(s->volumes, v);
		  return;
	       }
	     // ignore Disk with IDE or SCSI bus type
	     else if (!strcmp(s->bus, "ide") || !strcmp(s->bus, "scsi"))
		goto error;

	     else
		ecore_list_append(s->volumes, v);
	  }
	free(str);
	str = NULL;
     }

   if (v->type == DEV_UNKNOWN)
     {
	char               *mount_point;

	dbg("This device is a volume try to mount it\n");
	if (v->mount_point && v->mount_point[0])
	   mount_point = v->mount_point;
	if (v->label && v->label[0])
	   mount_point = v->label;
	else if (v->uuid && v->uuid[0])
	   mount_point = v->uuid;
	else
	  {
	     // XXX need to check this...
	     snprintf(buf, sizeof(buf), "/media/vol_%d", mount_id++);
	     mount_point = buf;
	  }
	e_hal_device_volume_mount(conn, v->udi, mount_point, v->fstype, NULL,
				  cb_volume_mounted, v);
	if (v->mount_point)
	   free(v->mount_point);
	snprintf(buf, sizeof(buf), "/media/%s", mount_point);
	v->mount_point = strdup(buf);

	v->type = DEV_USBDISK;
	dev = malloc(sizeof(Volume));
	dev->type = v->type;
	dev->udi = strdup(v->udi);

	ecore_event_add(enna->events[ENNA_EVENT_DEV_USBDISK_ADD], dev, NULL,
			NULL);
	return;
     }

   return;

 error:
   if (str)
      free(str);
   volume_remove(v->udi);
   return;

}

void
volume_setup(Volume * v)
{
   e_hal_device_get_all_properties(conn, v->udi, cb_volume_properties, v);
}

Volume             *
volume_append(const char *udi)
{
   Volume             *v;

   if (!udi)
      return NULL;

   v = volume_new();
   v->udi = strdup(udi);
   ecore_list_append(enna->volumes, v);
   volume_setup(v);

   return v;
}

static void
cb_test_find_device_by_capability_storage(void *user_data, void *reply_data,
					  DBusError * error)
{
   E_Hal_Manager_Find_Device_By_Capability_Return *ret = reply_data;
   char               *device;

   if (!ret || !ret->strings)
      return;

   ecore_list_first_goto(ret->strings);
   while ((device = ecore_list_next(ret->strings)))
      storage_append(device);
}

static void
cb_test_find_device_by_capability_volume(void *user_data, void *reply_data,
					 DBusError * error)
{
   E_Hal_Manager_Find_Device_By_Capability_Return *ret = reply_data;
   char               *device;

   if (!ret || !ret->strings)
      return;

   ecore_list_first_goto(ret->strings);
   while ((device = ecore_list_next(ret->strings)))
      volume_append(device);
}

static void
cb_test_find_device_by_capability_portable_audio_player(void *user_data,
							void *reply_data,
							DBusError * error)
{
   E_Hal_Manager_Find_Device_By_Capability_Return *ret = reply_data;
   char               *device;

   if (!ret || !ret->strings)
      return;

   ecore_list_first_goto(ret->strings);
   while ((device = ecore_list_next(ret->strings)))
      portable_audio_player_append(device);
}

static void
cb_is_portable_audio_player(void *user_data, void *reply_data,
			    DBusError * error)
{
   char               *udi = user_data;
   E_Hal_Device_Query_Capability_Return *ret = reply_data;

   if (ret && ret->boolean)
      portable_audio_player_append(udi);

   free(udi);
}

static void
cb_is_storage(void *user_data, void *reply_data, DBusError * error)
{
   char               *udi = user_data;
   E_Hal_Device_Query_Capability_Return *ret = reply_data;

   if (ret && ret->boolean)
      storage_append(udi);

   free(udi);
}

static void
cb_is_volume(void *user_data, void *reply_data, DBusError * error)
{
   char               *udi = user_data;
   E_Hal_Device_Query_Capability_Return *ret = reply_data;

   if (ret && ret->boolean)
      volume_append(udi);

   free(udi);
}

static void
cb_signal_device_added(void *data, DBusMessage * msg)
{
   DBusError           err;
   char               *udi;
   int                 ret;

   dbus_error_init(&err);
   dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &udi, DBUS_TYPE_INVALID);
   udi = strdup(udi);

   ret =
      e_hal_device_query_capability(conn, udi, "storage", cb_is_storage,
				    strdup(udi));
   e_hal_device_query_capability(conn, udi, "portable_audio_player",
				 cb_is_portable_audio_player, strdup(udi));
   e_hal_device_query_capability(conn, udi, "volume", cb_is_volume,
				 strdup(udi));
}

void
cb_signal_device_removed(void *data, DBusMessage * msg)
{
   DBusError           err;
   char               *udi;

   dbus_error_init(&err);

   dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &udi, DBUS_TYPE_INVALID);

   storage_remove(udi);
   volume_remove(udi);

}

void
cb_signal_new_capability(void *data, DBusMessage * msg)
{
   DBusError           err;
   char               *udi, *capability;

   dbus_error_init(&err);

   dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &udi, DBUS_TYPE_STRING,
			 &capability, DBUS_TYPE_INVALID);
   if (!strcmp(capability, "storage"))
      storage_append(udi);

}

EAPI void
enna_volume_manager_print_infos()
{

#if 0
   Storage            *s;

   s = ecore_list_first_goto(enna->storage_devices);
   while ((s = ecore_list_next(enna->storage_devices)))
     {
	Volume             *v;

	printf
	   ("Got storage:\n  udi: %s\n  bus: %s\n  drive_type: %s\n  model: %s\n  vendor: %s\n  serial: %s\n  icon.drive: %s\n  icon.volume: %s\n",
	    s->udi, s->bus, s->drive_type, s->model, s->vendor, s->serial,
	    s->icon.drive, s->icon.volume);
	v = ecore_list_first_goto(s->volumes);
	while ((v = ecore_list_next(s->volumes)))
	  {
	     printf("*********************\n");
	     printf
		("\tGot volume\n\t  udi: %s\n\t  uuid: %s\n\t  fstype: %s\n\t  label: %s\n\t  partition: %d\n\t  partition_label: %s\n\t  mounted: %d\n\t  mount_point: %s\n\n",
		 v->udi, v->uuid, v->fstype, v->label, v->partition,
		 v->partition ? v->partition_label : "(not a partition)",
		 v->mounted, v->mount_point);
	     if (s)
		printf("  for storage: %s\n", s->udi);
	     else
		printf("  storage unknown\n");
	     printf("*********************\n");
	  }

	printf("---------------------------------------\n");
     }
#endif
}

EAPI int
enna_volume_manager_init(Enna * _enna)
{
   e_dbus_init();
   ecore_string_init();
   conn = e_dbus_bus_get(DBUS_BUS_SYSTEM);

   if (!conn)
     {
	dbg("Error connecting to system bus. Is it running?\n");
	return 0;
     }

   enna = _enna;

   enna->storage_devices = ecore_list_new();
   ecore_list_free_cb_set(enna->storage_devices, ECORE_FREE_CB(storage_free));
   enna->volumes = ecore_list_new();
   ecore_list_free_cb_set(enna->volumes, ECORE_FREE_CB(volume_free));
   e_hal_manager_find_device_by_capability(conn, "portable_audio_player",
					   cb_test_find_device_by_capability_portable_audio_player,
					   NULL);
   e_hal_manager_find_device_by_capability(conn, "storage",
					   cb_test_find_device_by_capability_storage,
					   NULL);
   e_hal_manager_find_device_by_capability(conn, "volume",
					   cb_test_find_device_by_capability_volume,
					   NULL);

   e_dbus_signal_handler_add(conn, "org.freedesktop.Hal",
			     "/org/freedesktop/Hal/Manager",
			     "org.freedesktop.Hal.Manager", "DeviceAdded",
			     cb_signal_device_added, NULL);
   e_dbus_signal_handler_add(conn, "org.freedesktop.Hal",
			     "/org/freedesktop/Hal/Manager",
			     "org.freedesktop.Hal.Manager", "DeviceRemoved",
			     cb_signal_device_removed, NULL);
   e_dbus_signal_handler_add(conn, "org.freedesktop.Hal",
			     "/org/freedesktop/Hal/Manager",
			     "org.freedesktop.Hal.Manager", "NewCapability",
			     cb_signal_new_capability, NULL);

   return 1;
}
