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

#ifndef ENNA_VOLUMES_H
#define ENNA_VOLUMES_H

#include "enna.h"

typedef struct _Enna_Volumes_Listener Enna_Volumes_Listener;
typedef struct _Enna_Volume Enna_Volume;
typedef enum _ENNA_VOLUME_TYPE ENNA_VOLUME_TYPE;

typedef void (*EnnaVolumesFunc)(void *data, Enna_Volume *volume);
typedef void (*EnnaVolumeEject)(void *data);


enum _ENNA_VOLUME_TYPE
  {
    VOLUME_TYPE_UNKNOWN,
    VOLUME_TYPE_CAMERA,
    VOLUME_TYPE_AUDIO_PLAYER,
    VOLUME_TYPE_FLASHKEY,
    VOLUME_TYPE_REMOVABLE_DISK,
    VOLUME_TYPE_COMPACT_FLASH,
    VOLUME_TYPE_MEMORY_STICK,
    VOLUME_TYPE_SMART_MEDIA,
    VOLUME_TYPE_SD_MMC,
    VOLUME_TYPE_HDD,
    VOLUME_TYPE_CD,
    VOLUME_TYPE_CDDA,
    VOLUME_TYPE_DVD,
    VOLUME_TYPE_DVD_VIDEO,
    VOLUME_TYPE_BLURAY,
    VOLUME_TYPE_VCD,
    VOLUME_TYPE_SVCD,
    VOLUME_TYPE_NFS,
    VOLUME_TYPE_SMB,
  };

struct _Enna_Volume
{
   ENNA_VOLUME_TYPE type;
   const char *device_name;
   const char *mount_point;
   const char *label;
   EnnaVolumeEject eject;
   Eina_Bool is_ejectable : 1;
};

Enna_Volume *enna_volume_new (void);
void enna_volume_free (Enna_Volume *v);

Enna_Volumes_Listener * enna_volumes_listener_add(const char *name, EnnaVolumesFunc add, EnnaVolumesFunc remove, void *data);
void enna_volumes_listener_del(Enna_Volumes_Listener *vl);
void enna_volumes_add_emit(Enna_Volume *v);
void enna_volumes_remove_emit(Enna_Volume *v);
char *enna_volumes_icon_from_type(Enna_Volume *v);
Eina_List* enna_volumes_get(void);

#endif /* ENNA_VOLUMES_H */
