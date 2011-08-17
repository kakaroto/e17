/*
 * GeeXboX Enna Media Center.
 * Copyright (C) 2005-2010 The Enna Project
 *
 * This file is part of Enna.
 *
 * Enna is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Enna is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Enna; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <string.h>
#include <mntent.h>
#include <libudev.h>

#include "enna.h"
#include "module.h"
#include "vfs.h"
#include "volumes.h"
#include "buffer.h"

#define ENNA_MODULE_NAME   "udev"

#define UDEV_FILTER_SUBSYTEM    "block"
#define UDEV_FILTER_BLK_LOOP    "block/loop"
#define UDEV_FILTER_BLK_RAM     "block/ram"
#define UDEV_TIMER_DELAY        3

#define UDEV_MTAB_FILE          "/etc/mtab"

//#define DEBUG 1

typedef struct _Enna_Module_Udev
{
   Evas *e;
   struct udev *udev;
   struct udev_monitor *monitor;
   Eina_List *volumes;
   Ecore_Timer *timer;
} Enna_Module_Udev;

typedef struct volume_s {
   char *syspath;
   char *device;
   char *name;
   char *type;
   char *fstype;
   int mounted;
   char *mount_point;
   ENNA_VOLUME_TYPE vtype;
   Enna_Volume *evol;
} volume_t;

static const struct {
   const char *name;
   const char *bus;
} drv_bus_mapping[] = {
  { "SATA",                   "scsi"                                    },
  { "SATA",                   "ata"                                     },
  { "IDE",                    "ide"                                     },
  { "USB",                    "usb"                                     },
  { "FireWire",               "ieee1394"                                },
  { "CCW",                    "ccw"                                     },
  { NULL,                     0                                         }
};

static const struct {
   const char *name;
   const char *type;
   ENNA_VOLUME_TYPE vtype;
} drv_type_mapping[] = {
  { "Disk",           "disk",          VOLUME_TYPE_HDD                  },
  { "CD-ROM",         "cdrom",         VOLUME_TYPE_CD                   },
  { "Floppy",         "floppy",        VOLUME_TYPE_HDD                  },
  { "Tape",           "tape",          VOLUME_TYPE_HDD                  },
  { "CompactFlash",   "compact_flash", VOLUME_TYPE_COMPACT_FLASH        },
  { "MemoryStick",    "memory_stick",  VOLUME_TYPE_MEMORY_STICK         },
  { "SmartMedia",     "smart_media",   VOLUME_TYPE_SMART_MEDIA          },
  { "SD/MMC",         "sd_mmc",        VOLUME_TYPE_SD_MMC               },
  { "ZIP",            "zip",           VOLUME_TYPE_HDD                  },
  { "JAZ",            "jaz",           VOLUME_TYPE_HDD                  },
  { "FlashKey",       "flashkey",      VOLUME_TYPE_FLASHKEY             },
  { "MagnetoOptical", "optical",       VOLUME_TYPE_HDD                  },
  { NULL,             NULL,            VOLUME_TYPE_UNKNOWN              }
};

static const struct {
   const char *name;
   const char *property;
   ENNA_VOLUME_TYPE vtype;
} vol_disc_mapping[] = {
  { "BLURAY", "ID_CDROM_MEDIA_BD",                VOLUME_TYPE_BLURAY    },
  { "CDDA",   "ID_CDROM_MEDIA_TRACK_COUNT_AUDIO", VOLUME_TYPE_CDDA      },
  { "VCD",    "ID_CDROM_MEDIA_VCD",               VOLUME_TYPE_VCD       },
  { "SVCD",   "ID_CDROM_MEDIA_SDVD",              VOLUME_TYPE_SVCD      },
  { "DVD",    "ID_CDROM_MEDIA_DVD",               VOLUME_TYPE_DVD_VIDEO },
  { "CD",     "ID_CDROM_MEDIA_CD",                VOLUME_TYPE_CD        },
  { NULL,     NULL,                               VOLUME_TYPE_UNKNOWN   }
};

static Enna_Module_Udev *mod;

static void
scan_device_properties(struct udev_device *dev)
{

}

static volume_t *
volume_new(void)
{
   volume_t *v;

   v = calloc(1, sizeof(volume_t));

   return v;
}

static void
volume_free(volume_t *v)
{
   if (!v)
     return;

   ENNA_FREE(v->syspath);
   ENNA_FREE(v->device);
   ENNA_FREE(v->name);
   ENNA_FREE(v->type);
   ENNA_FREE(v->fstype);
   ENNA_FREE(v->mount_point);
   ENNA_FREE(v);
}

static volume_t *
volume_list_get(const char *syspath)
{
   Eina_List *l;
   volume_t *v;

   if (!mod->volumes || !syspath)
     return NULL;

   EINA_LIST_FOREACH(mod->volumes, l, v)
     {
        if (v->syspath && !strcmp(v->syspath, syspath))
          return v;
     }

   return NULL;
}

static int
disk_get_number(int major, int minor)
{
   switch (major)
     {
      case 3: /* Primary IDE interface */
         return (minor <= 63) ? 1 : 2;
      case 8: /* SCSI disk devices */
         return ((minor / 16) + 1);
      case 11: /* SCSI CD-ROM devices */
         return (minor + 1);
      case 22: /* Secondary IDE interface */
         return (minor <= 63) ? 3 : 4;
      default:
         break;
     }

   return 0;
}

static int
mtab_is_mounted(const char *devname)
{
   struct mntent *mnt;
   FILE *fp;

   if (!devname)
     return 0;

   fp = fopen(UDEV_MTAB_FILE, "r");
   if (!fp)
     return 0;

   while ((mnt = getmntent(fp)))
     {
        if (!strcmp(mnt->mnt_fsname, devname))
          {
             endmntent(fp);
             return 1;
          }
     }

   endmntent(fp);
   return 0;
}

static char *
mtab_get_mount_point(const char *devname)
{
   struct mntent *mnt;
   FILE *fp;
   char *mt;

   if (!devname)
     return NULL;

   fp = fopen(UDEV_MTAB_FILE, "r");
   if (!fp)
     return NULL;

   while ((mnt = getmntent(fp)))
     {
        if (!strcmp(mnt->mnt_fsname, devname))
          {
             mt = strdup(mnt->mnt_dir);
             endmntent(fp);
             return mt;
          }
     }

   endmntent(fp);
   return NULL;
}

static int
get_property_value_int(struct udev_device *dev, const char *property)
{
   const char *value;

   value = udev_device_get_property_value(dev, property);
   return value ? atoi(value) : 0;
}

static volume_t *
handle_disc(struct udev_device *dev)
{
   const char *id_type;
   volume_t *v;

   int major, minor, disk, media;
   const char *bus, *fs, *label, *vendor, *model, *device;
   char dsk[8] = { 0 }, part[8] = { 0 };
   const char *bus_type = NULL;
   Enna_Buffer *b;
   int i;

   id_type = udev_device_get_property_value(dev, "ID_TYPE");
   if (!id_type)
     return NULL;

   /* only care about CDs */
   if (strcmp(id_type, "cd"))
     return NULL;

   /* ensure a CD has been inserted */
   media = get_property_value_int(dev, "ID_CDROM_MEDIA");
   if (media != 1)
     return NULL;

   v = volume_new();

   device    = udev_device_get_devnode(dev);
   major     = get_property_value_int(dev, "MAJOR");
   minor     = get_property_value_int(dev, "MINOR");

   bus       = udev_device_get_property_value(dev, "ID_BUS");
   fs        = udev_device_get_property_value(dev, "ID_FS_TYPE");

   vendor    = udev_device_get_property_value(dev, "ID_VENDOR");
   model     = udev_device_get_property_value(dev, "ID_MODEL");
   label     = udev_device_get_property_value(dev, "ID_FS_LABEL");

   disk = disk_get_number(major, minor);
   snprintf(dsk, sizeof(dsk), "#%d", disk);

   v->syspath = strdup(udev_device_get_syspath(dev));
   v->device  = strdup(device ? device : "Unknown");
   v->fstype  = strdup(fs ? fs : "Unknown");
   v->mounted = mtab_is_mounted(device);

   if (v->mounted)
     v->mount_point = mtab_get_mount_point(device);

   /* check for disc property: CDDA, VCD, SVCD, DVD, Data CD/DVD */
   for (i = 0; vol_disc_mapping[i].name; i++)
     if (udev_device_get_property_value(dev, vol_disc_mapping[i].property))
       {
          v->type = strdup(vol_disc_mapping[i].name);
          v->vtype = vol_disc_mapping[i].vtype;
          break;
       }

   /* if no type has been found, consider it's data */
   if (!v->type)
     {
        v->type = strdup("CD");
        v->vtype = VOLUME_TYPE_CD;
     }

   /* get bus' type */
   for (i = 0; drv_bus_mapping[i].name; i++)
     if (bus && !strcmp(bus, drv_bus_mapping[i].bus))
       {
          bus_type = drv_bus_mapping[i].name;
          break;
       }
   if (!bus_type)
     bus_type = "Unknown";

   b = enna_buffer_new();
   enna_buffer_appendf(b, "%s %s %s", bus_type, v->type, dsk);

   if (label)
     enna_buffer_appendf(b, " %s", label);
   else
     {
        if (vendor)
          enna_buffer_appendf(b, " %s", vendor);
        if (model)
          enna_buffer_appendf(b, " %s", model);
     }
   enna_buffer_appendf(b, " %s", part);
   v->name = strdup(b->buf);
   enna_buffer_free(b);

   return v;
}

static volume_t *
handle_partition(struct udev_device *dev)
{
   int major, minor, disk, partition;
   const char *usage, *type, *bus, *fs;
   const char *label, *vendor, *model, *device;
   volume_t *v;
   char dsk[8] = { 0 }, part[8] = { 0 };
   const char *bus_type = NULL, *drive_type = NULL;
   Enna_Buffer *b;
   int i;

   /* ensure we're dealing with a proper filesystem on partition */
   usage = udev_device_get_property_value(dev, "ID_FS_USAGE");
   if (!usage || strcmp(usage, "filesystem"))
     return NULL;;

   v = volume_new();

   device    = udev_device_get_devnode(dev);
   major     = get_property_value_int(dev, "MAJOR");
   minor     = get_property_value_int(dev, "MINOR");
   partition = get_property_value_int(dev, "DKD_PARTITION_NUMBER");

   type      = udev_device_get_property_value(dev, "ID_TYPE");
   bus       = udev_device_get_property_value(dev, "ID_BUS");
   fs        = udev_device_get_property_value(dev, "ID_FS_TYPE");
   //devname   = udev_device_get_property_value(dev, "DEVNAME");

   vendor    = udev_device_get_property_value(dev, "ID_VENDOR");
   model     = udev_device_get_property_value(dev, "ID_MODEL");
   label     = udev_device_get_property_value(dev, "ID_FS_LABEL");

   disk = disk_get_number(major, minor);
   snprintf(dsk,  sizeof(dsk),  "#%d", disk);
   snprintf(part, sizeof(part), "(%d)", partition);

   v->syspath = strdup(udev_device_get_syspath(dev));
   v->device  = strdup(device ? device : "Unknown");
   v->type    = strdup("HDD");
   v->fstype  = strdup(fs ? fs : "Unknown");
   v->mounted = mtab_is_mounted(device);

   if (v->mounted)
     v->mount_point = mtab_get_mount_point(device);

   /* get bus' type */
   for (i = 0; drv_bus_mapping[i].name; i++)
     if (bus && !strcmp(bus, drv_bus_mapping[i].bus))
       {
          bus_type = drv_bus_mapping[i].name;
          break;
       }
   if (!bus_type)
     bus_type = "Unknown";

   /* get drive's type */
   for (i = 0; drv_type_mapping[i].name; i++)
     if (type && !strcmp(type, drv_type_mapping[i].type))
       {
          drive_type = drv_type_mapping[i].name;
          v->vtype   = drv_type_mapping[i].vtype;
          break;
       }
   if (!drive_type)
     drive_type = "Unknown";

   b = enna_buffer_new();
   enna_buffer_appendf(b, "%s %s %s", bus_type, drive_type, dsk);
   if (label)
     enna_buffer_appendf(b, " %s", label);
   else
     {
        if (vendor)
          enna_buffer_appendf(b, " %s", vendor);
        if (model)
          enna_buffer_appendf(b, " %s", model);
     }
   enna_buffer_appendf(b, " %s", part);
   v->name = strdup(b->buf);
   enna_buffer_free(b);

   return v;
}

static void
volume_add_notification(volume_t *v)
{
   const char *uri = NULL;
   Enna_Volume *evol;
   char tmp[1024];

   if (!v)
     return;

   switch (v->vtype)
     {
        /* discard unknown volumes */
      case VOLUME_TYPE_UNKNOWN:
         return;

      case VOLUME_TYPE_HDD:
      case VOLUME_TYPE_REMOVABLE_DISK:
      case VOLUME_TYPE_COMPACT_FLASH:
      case VOLUME_TYPE_MEMORY_STICK:
      case VOLUME_TYPE_SMART_MEDIA:
      case VOLUME_TYPE_SD_MMC:
      case VOLUME_TYPE_FLASHKEY:
      case VOLUME_TYPE_CD:
      case VOLUME_TYPE_DVD:
         if (!v->mounted)
           return;

         snprintf(tmp, sizeof(tmp), "file://%s", v->mount_point);
         uri = eina_stringshare_add(tmp);
         break;

      case VOLUME_TYPE_CDDA:
         uri = eina_stringshare_add("cdda://");
         break;

      case VOLUME_TYPE_DVD_VIDEO:
         uri  =  eina_stringshare_add("dvd://");
         break;

      case VOLUME_TYPE_VCD:
         uri =  eina_stringshare_add("vcd://");
         break;

      case VOLUME_TYPE_SVCD:
         uri =  eina_stringshare_add("vcd://");
         break;

      default:
         return;
     }

   evol               = enna_volume_new();
   evol->label        = eina_stringshare_add(v->name);
   evol->type         = v->vtype;
   evol->mount_point  = eina_stringshare_add(uri);
   evol->device_name  = eina_stringshare_add(v->device);

   evol->eject        = NULL;
   evol->is_ejectable = EINA_FALSE;

   v->evol = evol;
   enna_volumes_add_emit(evol);
}

static void
add_device(struct udev_device *dev)
{
   const char *syspath;
   const char *devtype;
   volume_t *v = NULL;

   /* retrieve the device's syspath */
   syspath = udev_device_get_syspath(dev);
   if (!syspath)
     return;

   /* ensure the device does not already exists in our table */
   if (volume_list_get(syspath))
     return;

   devtype = udev_device_get_property_value(dev, "DEVTYPE");
   if (!devtype)
     return;

   if (!strcmp(devtype, "disk"))
     v = handle_disc(dev);
   else if (!strcmp(devtype, "partition"))
     v = handle_partition(dev);

   if (v)
     {
        mod->volumes = eina_list_append(mod->volumes, v);
        volume_add_notification(v);
     }
}

static void
remove_device(struct udev_device *dev)
{
   const char *syspath;
   volume_t *v;

   /* retrieve the device's syspath */
   syspath = udev_device_get_syspath(dev);
   if (!syspath)
     return;

   v = volume_list_get(syspath);
   if (!v)
     return;

   enna_volumes_remove_emit(v->evol);
   mod->volumes = eina_list_remove(mod->volumes, v);
   volume_free(v);
}

static void
change_device(struct udev_device *dev)
{
   remove_device(dev);
   add_device(dev);
}

static void
handle_device(struct udev_device *dev, const char *action)
{
   scan_device_properties(dev);

   /* check whether we need to add or remove the device from our list */
   if (!action || !strcmp(action, "add"))
     add_device(dev);
   else if (!strcmp(action, "change"))
     change_device(dev);
   else
     remove_device(dev);
}

static void
scan_block_devices(void)
{
   struct udev_enumerate *en;
   struct udev_list_entry *device_list, *device_p;

   en = udev_enumerate_new(mod->udev);
   if (!en)
     return;

   /* we only care about block devices */
   udev_enumerate_add_match_subsystem(en, UDEV_FILTER_SUBSYTEM);

   /* scan for available devices */
   udev_enumerate_scan_devices(en);
   device_list = udev_enumerate_get_list_entry(en);

   udev_list_entry_foreach(device_p, device_list)
     {
        struct udev_device *dev;
        const char *syspath;
        const char *action;

        /* based on sys path, discard irrelevant ones */
        syspath = udev_list_entry_get_name(device_p);
        if (strstr(syspath, UDEV_FILTER_BLK_LOOP) ||
            strstr(syspath, UDEV_FILTER_BLK_RAM))
          continue;

        dev = udev_device_new_from_syspath(mod->udev, syspath);
        if (!dev)
          continue;

        action = udev_device_get_action(dev);
        if (!action ||
            !strcmp(action, "add") ||
            !strcmp(action, "remove") ||
            !strcmp(action, "change"))
          handle_device(dev, action);

        udev_device_unref(dev);
     }

   udev_enumerate_unref(en);
}

static Eina_Bool
scan_timer_cb(void *data)
{
   scan_block_devices();
   return 1; /* go on */
}

/* Module interface */

#ifdef USE_STATIC_MODULES
#undef MOD_PREFIX
#define MOD_PREFIX enna_mod_volume_udev
#endif /* USE_STATIC_MODULES */

static void
module_init(Enna_Module *em)
{
   int err;

   if (!em)
     return;

   mod = calloc(1, sizeof(Enna_Module_Udev));
   em->mod = mod;

   /* udev listeners */
   mod->udev = udev_new();
   if (!mod->udev)
     goto err_udev;

   mod->monitor = udev_monitor_new_from_netlink(mod->udev, "udev");
   if (!mod->monitor)
     goto err_monitor;

   err = udev_monitor_enable_receiving(mod->monitor);
   if (err)
     goto err_receiver;

   err = udev_monitor_filter_add_match_subsystem_devtype
     (mod->monitor, UDEV_FILTER_SUBSYTEM, NULL);
   if (err)
     goto err_filter;

   scan_block_devices();
   mod->timer = ecore_timer_add(UDEV_TIMER_DELAY, scan_timer_cb, NULL);

   return;

 err_filter:
 err_receiver:
   udev_monitor_unref(mod->monitor);
 err_monitor:
   udev_unref(mod->udev);
 err_udev:

   return;
}

static void
module_shutdown(Enna_Module *em)
{
   Enna_Module_Udev *mod;
   volume_t *v;

   mod = em->mod;

   ENNA_TIMER_DEL(mod->timer);

   if (mod->monitor)
     udev_monitor_unref(mod->monitor);
   if (mod->udev)
     udev_unref(mod->udev);

   EINA_LIST_FREE(mod->volumes, v)
     volume_free(v);
}

Enna_Module_Api ENNA_MODULE_API = {
  ENNA_MODULE_VERSION,
  "volume_udev",
  "Volumes from udev",
  NULL,
  "This module provide support for removable volumes",
  "bla bla bla<br><b>bla bla bla</b><br><br>bla.",
  {
    module_init,
    module_shutdown
  }
};
