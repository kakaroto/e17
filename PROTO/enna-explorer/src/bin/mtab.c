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

#include <stdio.h>
#include <string.h>
#include <mntent.h>

#include "enna.h"
#include "enna_config.h"
#include "module.h"
#include "volumes.h"

#define ENNA_MODULE_NAME   "mtab"

#define MTAB_FILE          "/etc/mtab"

typedef enum {
  MTAB_TYPE_NONE,
  MTAB_TYPE_NFS,
  MTAB_TYPE_SMB,
} MTAB_TYPE;

static Eina_List *_mount_points = NULL;

/***************************************/
/*           mtab handling             */
/***************************************/

static void
mtab_add_mnt(MTAB_TYPE t, char *fsname, char *dir)
{
   Enna_Volume *v;
   char name[512], tmp[1024], srv[128], share[128];
   char *p;
   ENNA_VOLUME_TYPE type = VOLUME_TYPE_UNKNOWN;

   if(t == MTAB_TYPE_NONE)
     return;

   if(!fsname || !dir)
     return;

   memset(name,  '\0', sizeof(name));
   memset(tmp,   '\0', sizeof(tmp));
   memset(srv,   '\0', sizeof(srv));
   memset(share, '\0', sizeof(share));

   snprintf(tmp, sizeof(tmp), "file://%s", dir);

   switch(t)
     {
      case MTAB_TYPE_NFS:
         p = strchr(fsname, ':');
         if(!p)
           return;
         strncpy(srv, fsname, p - fsname);
         strcpy(share, p + 1);
         snprintf(name, sizeof(name), "[NFS] %s on %s", share, srv);
         type = VOLUME_TYPE_NFS;
         break;

      case MTAB_TYPE_SMB:
         p = strchr(fsname + 2, '/');
         if(!p)
           return;
         strncpy(srv, fsname + 2, p - (fsname + 2));
         strcpy(share, p + 1);
         snprintf(name, sizeof(name), "[SAMBA] %s on %s", share, srv);
         type = VOLUME_TYPE_SMB;
         break;

      default:
         break;
     }

   v              = enna_volume_new ();
   v->type        = type;
   v->device_name = eina_stringshare_add(srv);
   v->label       = eina_stringshare_add(name);
   v->mount_point = eina_stringshare_add(tmp);

   _mount_points = eina_list_append(_mount_points, v);

   enna_volumes_add_emit(v);
}

static void
mtab_parse(void)
{
   struct mntent *mnt;
   FILE *fp;

   fp = fopen(MTAB_FILE, "r");
   if(!fp)
     return;

   while((mnt = getmntent(fp)))
     {
        MTAB_TYPE type = MTAB_TYPE_NONE;

        if(!strcmp(mnt->mnt_type, "nfs") ||
           !strcmp(mnt->mnt_type, "nfs4"))
          type = MTAB_TYPE_NFS;
        else if(!strcmp(mnt->mnt_type, "smbfs") ||
                !strcmp(mnt->mnt_type, "cifs"))
          type = MTAB_TYPE_SMB;
        else
          continue;

        mtab_add_mnt(type, mnt->mnt_fsname, mnt->mnt_dir);
     }

   endmntent(fp);
}

/* Module interface */

#ifdef USE_STATIC_MODULES
#undef MOD_PREFIX
#define MOD_PREFIX enna_mod_volume_mtab
#endif /* USE_STATIC_MODULES */

static void
module_init(Enna_Module *em)
{
   mtab_parse();
}

static void
module_shutdown(Enna_Module *em)
{
   Enna_Volume *v;
   Eina_List *l, *l_next;
   EINA_LIST_FOREACH_SAFE(_mount_points, l, l_next, v)
     {
        _mount_points = eina_list_remove(_mount_points, v);
        enna_volume_free (v);
     }
   _mount_points = NULL;
}

Enna_Module_Api ENNA_MODULE_API =
  {
    ENNA_MODULE_VERSION,
    "volume_mtab",
    "Volumes from mtab",
    NULL,
    "This module provide support for already mounted volumes",
    "bla bla bla<br><b>bla bla bla</b><br><br>bla.",
    {
      module_init,
      module_shutdown
    }
  };
