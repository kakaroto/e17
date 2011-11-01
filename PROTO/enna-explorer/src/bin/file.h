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

#ifndef FILE_H
#define FILE_H

#include <Eina.h>

#define ENNA_META_KEY_FREESPACE       "free_space"
#define ENNA_META_KEY_SIZE            "size"
#define ENNA_META_KEY_USESPACE        "used_space"
#define ENNA_META_KEY_PERCENT_USED    "percent_used"

#define ENNA_FILE_IS_BROWSABLE(file)                                    \
  ((file->type == ENNA_FILE_MENU) || (file->type == ENNA_FILE_DIRECTORY) || (file->type == ENNA_FILE_VOLUME))

typedef struct _Enna_File Enna_File;
typedef enum _Enna_File_Type Enna_File_Type;
typedef struct _Enna_File_Meta_Class Enna_File_Meta_Class;
typedef struct _Enna_File_Action Enna_File_Action;

struct _Enna_File_Meta_Class
{
   const char *(* meta_get)(void *data, Enna_File *file, const char *key);
   void (* meta_set)(void *data, Enna_File *file, const char *key, const char *value);
   void (* meta_del)(void *data);
};

enum _Enna_File_Type
  {
    ENNA_FILE_MENU,
    ENNA_FILE_DIRECTORY,
    ENNA_FILE_VOLUME,
    ENNA_FILE_FILE,
    ENNA_FILE_ARTIST,
    ENNA_FILE_ALBUM,
    ENNA_FILE_TRACK,
    ENNA_FILE_FILM
  };

struct _Enna_File
{
   const char *name;
   const char *uri;
   const char *label;
   const char *icon;
   const char *icon_file;
   const char *mrl;
   const char *mime;
   Enna_File_Type type;
   Enna_File_Meta_Class *meta_class;
   void *meta_data;
   Eina_List *actions;
   Eina_List *callbacks;
   int refcount;
};

struct _Enna_File_Action
{
   const char *name;
   const char *label;
   const char *icon;
   void *priv;
   void (*run)(void *priv, Enna_File *file);
   Enna_File *file;
};

typedef void (*Enna_File_Update_Cb) (void *data, Enna_File *file);

Enna_File *enna_file_dup(Enna_File *file);
Enna_File *enna_file_ref(Enna_File *file);
Enna_File *enna_file_from_uri_new(const char *uri);
void enna_file_free(Enna_File *f);
void enna_file_meta_add(Enna_File *f, Enna_File_Meta_Class *meta_class, void *data);
const char * enna_file_meta_get(Enna_File *f, const char *key);
Enna_File_Action *enna_file_action_new(Enna_File *f, const char *name, const char *label, const char *icon, void (*run)(void *priv, Enna_File *file), void *priv);

void enna_file_action_add(Enna_File *f, Enna_File_Action *action);
Eina_List *enna_file_actions_get(Enna_File *f);
void enna_file_action_run(Enna_File_Action *action);

void enna_file_meta_set(Enna_File *f, const char *key, const void *data);
Enna_File *enna_file_file_add(const char *name, const char *uri,
                              const char *mrl, const char *label,
                              const char *icon);
Enna_File *enna_file_track_add(const char *name, const char *uri,
                               const char *mrl, const char *label,
                               const char *icon);
Enna_File *enna_file_film_add(const char *name, const char *uri,
                              const char *mrl, const char *label,
                              const char *icon);
Enna_File *enna_file_album_add(const char *name, const char *uri,
                               const char *label, const char *icon);
Enna_File *enna_file_directory_add(const char *name, const char *uri,
                                   const char *mrl, const char *label, const char *icon);
Enna_File *enna_file_menu_add(const char *name, const char *uri,
                              const char *label, const char *icon);
Enna_File *enna_file_volume_add(const char *name, const char *uri,
                                const char *label, const char *icon);


void enna_file_meta_callback_add(Enna_File *file, Enna_File_Update_Cb func, void *data);
void *enna_file_meta_callback_del(Enna_File *file, Enna_File_Update_Cb func);
void enna_file_meta_callback_call(Enna_File *file);


#endif /* FILE_H */
