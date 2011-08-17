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

#include "enna.h"
#include "vfs.h"

static Eina_List *_enna_vfs_all   = NULL;
static Eina_List *_enna_vfs_music = NULL;
static Eina_List *_enna_vfs_video = NULL;
static Eina_List *_enna_vfs_photo = NULL;

/* local subsystem functions */

static int
_sort_cb(const void *d1, const void *d2)
{
   const Enna_Vfs_Class *vfs1 = d1;
   const Enna_Vfs_Class *vfs2 = d2;

   if (vfs1->pri > vfs2->pri)
     return 1;
   else if (vfs1->pri < vfs2->pri)
     return -1;
   else
     return strcasecmp(vfs1->name, vfs2->name);
}

/* externally accessible functions */
int
enna_vfs_init(Evas *evas __UNUSED__)
{
   return 0;
}

void
enna_vfs_register(Enna_Vfs_Class *vfs, ENNA_VFS_CAPS type)
{
   if (!vfs)
     return;

   if (type & ENNA_CAPS_ALL)
     {
        _enna_vfs_all = eina_list_append(_enna_vfs_all, vfs);
        _enna_vfs_all = eina_list_sort(
                                       _enna_vfs_all,
                                       eina_list_count(_enna_vfs_all),
                                       _sort_cb);
     }

   if (type & ENNA_CAPS_MUSIC)
     {
        _enna_vfs_music = eina_list_append(_enna_vfs_music, vfs);
        _enna_vfs_music = eina_list_sort(
                                         _enna_vfs_music,
                                         eina_list_count(_enna_vfs_music),
                                         _sort_cb);
     }

   if (type & ENNA_CAPS_VIDEO)
     {
        _enna_vfs_video = eina_list_append(_enna_vfs_video, vfs);
        _enna_vfs_video = eina_list_sort(
                                         _enna_vfs_video,
                                         eina_list_count(_enna_vfs_video),
                                         _sort_cb);
     }

   if (type & ENNA_CAPS_PHOTO)
     {
        _enna_vfs_photo = eina_list_append(_enna_vfs_photo, vfs);
        _enna_vfs_photo = eina_list_sort(
                                         _enna_vfs_photo,
                                         eina_list_count(_enna_vfs_photo),
                                         _sort_cb);
     }

   //_enna_vfs = eina_list_append(_enna_vfs, vfs);
}

void
enna_vfs_unregister(Enna_Vfs_Class *vfs, ENNA_VFS_CAPS type)
{
   if (!vfs)
     return;

   if (type & ENNA_CAPS_ALL)
     {
        _enna_vfs_all = eina_list_remove(_enna_vfs_all, vfs);
     }


   if (type & ENNA_CAPS_MUSIC)
     {
        _enna_vfs_music = eina_list_remove(_enna_vfs_music, vfs);
     }

   if (type & ENNA_CAPS_VIDEO)
     {
        _enna_vfs_video = eina_list_remove(_enna_vfs_video, vfs);
     }

   if (type & ENNA_CAPS_PHOTO)
     {
        _enna_vfs_photo = eina_list_remove(_enna_vfs_photo, vfs);
     }

}

int
enna_vfs_append(const char *name __UNUSED__, unsigned char type __UNUSED__, Enna_Vfs_Class *vfs __UNUSED__)
{
   return 0;
}

void
enna_vfs_class_remove(const char *name, unsigned char type)
{
   Eina_List *tmp;

   if (!name)
     return;

   tmp = enna_vfs_get (type);
   tmp = eina_list_nth_list (tmp, 0);
   do {
      Enna_Vfs_Class *class = (Enna_Vfs_Class *) tmp->data;
      if (class && !strcmp (class->name, name))
        tmp = eina_list_remove (tmp, class);
   } while ((tmp = eina_list_next (tmp)));
}

Eina_List *
enna_vfs_get(ENNA_VFS_CAPS type)
{
   Eina_List *vfs = NULL;

   if (type & ENNA_CAPS_ALL)
     vfs = eina_list_merge(vfs, _enna_vfs_all);

   if (type & ENNA_CAPS_MUSIC)
     vfs = eina_list_merge(vfs, _enna_vfs_music);

   if (type & ENNA_CAPS_VIDEO)
     vfs = eina_list_merge(vfs, _enna_vfs_video);

   if (type & ENNA_CAPS_PHOTO)
     vfs = eina_list_merge(vfs, _enna_vfs_photo);

   return vfs;
}
