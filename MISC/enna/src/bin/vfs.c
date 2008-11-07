/*
 * enna_vfs.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_vfs.c is free software copyrighted by Nicolas Aguirre.
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
 * enna_vfs.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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

#include "enna.h"
#include "vfs.h"

static Eina_List *_enna_vfs_music = NULL;
static Eina_List *_enna_vfs_video = NULL;
static Eina_List *_enna_vfs_photo = NULL;

/* local subsystem functions */

/* externally accessible functions */
int enna_vfs_init(Evas *evas)
{

    return 0;
}

int enna_vfs_append(const char *name, unsigned char type,
        Enna_Class_Vfs *vfs)
{
    if (!vfs)
        return -1;

    if ((type & ENNA_CAPS_MUSIC) == ENNA_CAPS_MUSIC)
        _enna_vfs_music = eina_list_append(_enna_vfs_music, vfs);

    if ((type & ENNA_CAPS_VIDEO) == ENNA_CAPS_VIDEO)
        _enna_vfs_video = eina_list_append(_enna_vfs_video, vfs);

    if ((type & ENNA_CAPS_PHOTO) == ENNA_CAPS_PHOTO)
        _enna_vfs_photo = eina_list_append(_enna_vfs_photo, vfs);

    return 0;
}

Eina_List *
enna_vfs_get(ENNA_VFS_CAPS type)
{

    if (type == ENNA_CAPS_MUSIC)
    return _enna_vfs_music;
    else if (type == ENNA_CAPS_VIDEO)
    {
        enna_log (ENNA_MSG_EVENT, NULL, "return vfs video");
        return _enna_vfs_video;
    }
    else if (type == ENNA_CAPS_PHOTO)
    return _enna_vfs_photo;
    else
    return NULL;
}

static Enna_Vfs_File * enna_vfs_create_inode(char *uri, char *label,
        char *icon, char *icon_file, int dir)
{
    Enna_Vfs_File *f;

    f = calloc(1, sizeof(Enna_Vfs_File));
    f->uri = uri ? strdup(uri) : NULL;
    f->label = label ? strdup(label) : NULL;
    f->icon = icon ? strdup(icon) : NULL;
    f->icon_file = icon_file ? strdup(icon_file) : NULL;
    f->is_directory = dir;

    return f;
}

Enna_Vfs_File * enna_vfs_create_file(char *uri, char *label, char *icon,
        char *icon_file)
{
    return enna_vfs_create_inode(uri, label, icon, icon_file, 0);
}

Enna_Vfs_File * enna_vfs_create_directory(char *uri, char *label, char *icon,
        char *icon_file)
{
    return enna_vfs_create_inode(uri, label, icon, icon_file, 1);
}

void enna_vfs_remove(Enna_Vfs_File *f)
{
    if (!f)
        return;

    ENNA_FREE(f->uri);
    ENNA_FREE(f->label);
    ENNA_FREE(f->icon);
    ENNA_FREE(f->icon_file);
    ENNA_FREE(f);
}
