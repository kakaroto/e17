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

#ifndef UDISKS_H
#define UDISKS_H

typedef struct _Enna_Storage Enna_Storage;
typedef struct _Enna_Volume  Enna_Volume;

typedef enum
{
  ENNA_VOLUME_OP_TYPE_NONE,
  ENNA_VOLUME_OP_TYPE_MOUNT,
  ENNA_VOLUME_OP_TYPE_UNMOUNT,
  ENNA_VOLUME_OP_TYPE_EJECT
} Enna_Volume_Op_Type;

struct _Enna_Storage
{
   int type;
   const char *udi;
   const char *drive_type;

   const char *model, *vendor, *serial;

   Eina_Bool removable;
   Eina_Bool media_available;
   Eina_Bool system_internal;
   unsigned long long media_size;

   Eina_Bool requires_eject;
   Eina_Bool hotpluggable;
   Eina_Bool media_check_enabled;

   struct
     {
        const char *drive, *volume;
     } icon;

   Eina_List *volumes;

   Eina_Bool validated : 1;
   Eina_Bool trackable : 1;

   const char *bus;
};

struct _Enna_Volume
{
   int type;
   const char *udi, *uuid;
   const char *label, *icon, *fstype;
   unsigned long long size;

   Eina_Bool partition;
   int partition_number;
   const char *partition_label;
   Eina_Bool mounted;
   const char *mount_point;

   const char *parent;
   Enna_Storage *storage;
   Eina_List *mounts;

   Eina_Bool validated : 1;

   Eina_Bool auto_unmount : 1;                  // unmount, when last associated fm window closed
   Eina_Bool first_time;                    // volume discovery in init sequence
   Ecore_Timer *guard;                 // operation guard timer
   Enna_Volume_Op_Type optype;
    //Efm_Mode efm_mode;

   Eina_Bool encrypted;
   Eina_Bool unlocked;

   DBusPendingCall *op;                // d-bus call handle
   void *prop_handler;


};


extern int ENNA_EVENT_VOLUMES_ADDED;
extern int ENNA_EVENT_VOLUMES_REMOVED;
extern int ENNA_EVENT_STORAGE_ADDED;
extern int ENNA_EVENT_STORAGE_REMOVED;
extern int ENNA_EVENT_OP_MOUNT_DONE;
extern int ENNA_EVENT_OP_MOUNT_ERROR;
extern int ENNA_EVENT_OP_UNMOUNT_DONE;
extern int ENNA_EVENT_OP_UNMOUNT_ERROR;
extern int ENNA_EVENT_OP_EJECT_ERROR;

void enna_udisks_init(void);
void enna_udisks_volume_mount(Enna_Volume *v);
void enna_udisks_volume_eject(Enna_Volume *v);
//void enna_udisks_shutdown(void);

#endif /* UDISKS_H */
