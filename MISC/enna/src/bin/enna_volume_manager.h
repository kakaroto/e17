#ifndef ENNA_VOLUME_MANAGER_H
#define ENNA_VOLUME_MANAGER_H

#include "enna.h"

typedef enum _DEVICE_TYPE DEVICE_TYPE;
typedef struct _Device Device;
typedef struct Storage Storage;
struct Storage
{
   int                 type;
   char               *udi;
   char               *bus;
   char               *drive_type;

   char               *model;
   char               *vendor;
   char               *serial;

   char                removable;
   char                media_available;
   unsigned long       media_size;

   char                requires_eject;
   char                hotpluggable;
   char                media_check_enabled;

   struct
   {
      char               *drive;
      char               *volume;
   } icon;

   Ecore_List         *volumes;
};

typedef struct Volume Volume;
struct Volume
{
   int                 type;
   char               *udi;
   char               *uuid;
   char               *label;
   char               *fstype;

   char                partition;
   char               *partition_label;
   char                mounted;
   char               *mount_point;
   char               *device;

   Storage            *storage;
};

enum _DEVICE_TYPE
{
   DEV_VIDEO_DVD,
   DEV_AUDIO_CD,
   DEV_DATA_CD,
   DEV_DATA_DVD,
   DEV_USBDISK,
   DEV_BLANK_DVD,
   DEV_BLANK_CD,
   DEV_CAMERA,
   DEV_IPOD,
   DEV_IPOD_DEVICE,
   DEV_UNKNOWN
};

struct _Device
{
   char               *udi;
   char               *mount_point;
   char               *device;
   char               *volume_label;
   DEVICE_TYPE         type;
};
EAPI int            enna_volume_manager_init(Enna * enna);
EAPI void           enna_volume_manager_free(Enna * enna);
EAPI void           enna_volume_manager_print_infos(void);
#endif
