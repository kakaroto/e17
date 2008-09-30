/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "etk_test.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <Ecore_Data.h>
#include <Ecore_File.h>

typedef struct _Etk_Test_Iconbox_Types
{
   void *extension;
   char *icon;
} Etk_Test_Iconbox_Types;

static Etk_Bool _etk_test_iconbox_mouse_down_cb(Etk_Object *object, Etk_Event_Mouse_Up *event, void *data);
static void _etk_test_iconbox_folder_set(Etk_Iconbox *iconbox, const char *folder);

static Etk_Test_Iconbox_Types _etk_test_iconbox_types[] =
{
   { "jpg", "mimetypes/image-x-generic_48" },
   { "jpeg", "mimetypes/image-x-generic_48" },
   { "png", "mimetypes/image-x-generic_48" },
   { "bmp", "mimetypes/image-x-generic_48" },
   { "gif", "mimetypes/image-x-generic_48" },
   { "mp3", "mimetypes/audio-x-generic_48" },
   { "ogg", "mimetypes/audio-x-generic_48" },
   { "wav", "mimetypes/audio-x-generic_48" },
   { "avi", "mimetypes/video-x-generic_48" },
   { "mpg", "mimetypes/video-x-generic_48" },
   { "mpeg", "mimetypes/video-x-generic_48" },
   { "gz", "mimetypes/package-x-generic_48" },
   { "tgz", "mimetypes/package-x-generic_48" },
   { "bz2", "mimetypes/package-x-generic_48" },
   { "tbz2", "mimetypes/package-x-generic_48" },
   { "zip", "mimetypes/package-x-generic_48" },
   { "rar", "mimetypes/package-x-generic_48" },
};
static int _etk_test_iconbox_num_types = sizeof(_etk_test_iconbox_types) / sizeof (_etk_test_iconbox_types[0]);
static Etk_String *_etk_test_iconbox_current_folder = NULL;
static Etk_Widget *win = NULL;

/* Creates the window for the iconbox test */
void etk_test_iconbox_window_create(void *data)
{
   Etk_Widget *iconbox;

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Iconbox Test");
   etk_window_resize(ETK_WINDOW(win), 600, 330);
   etk_signal_connect_by_code(ETK_WINDOW_DELETE_EVENT_SIGNAL, ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);

   iconbox = etk_iconbox_new();
   etk_iconbox_emblem_position_set(ETK_ICONBOX(iconbox), ETK_ICONBOX_EMBLEM_POSITION_TL);

   etk_container_add(ETK_CONTAINER(win), iconbox);
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_DOWN_SIGNAL, ETK_OBJECT(iconbox), ETK_CALLBACK(_etk_test_iconbox_mouse_down_cb), NULL);

   _etk_test_iconbox_folder_set(ETK_ICONBOX(iconbox), NULL);
   etk_widget_show_all(win);
}

/* Called when the iconbox is pressed by the mouse */
static Etk_Bool _etk_test_iconbox_mouse_down_cb(Etk_Object *object, Etk_Event_Mouse_Up *event, void *data)
{
   Etk_Iconbox *iconbox;
   Etk_Iconbox_Icon *icon;
   Etk_String *new_folder;
   char *parent;

   if (!(iconbox = ETK_ICONBOX(object)))
      return ETK_TRUE;
   if (event->button != 1 || !(event->flags & ETK_MOUSE_DOUBLE_CLICK) || (event->flags & ETK_MOUSE_TRIPLE_CLICK))
      return ETK_TRUE;
   if (!(icon = etk_iconbox_icon_get_at_xy(iconbox, event->canvas.x, event->canvas.y, ETK_FALSE, ETK_TRUE, ETK_TRUE)))
      return ETK_TRUE;

   if (!strcmp(etk_iconbox_icon_label_get(icon), ".."))
   {
      parent = ecore_file_dir_get(etk_string_get(_etk_test_iconbox_current_folder));
      _etk_test_iconbox_folder_set(iconbox, parent);
      free(parent);
   }
   else
   {
      if (etk_string_length_get(_etk_test_iconbox_current_folder) == 1)
         new_folder = etk_string_new_printf("/%s", etk_iconbox_icon_label_get(icon));
      else
         new_folder = etk_string_new_printf("%s/%s", etk_string_get(_etk_test_iconbox_current_folder),
                                            etk_iconbox_icon_label_get(icon));
      _etk_test_iconbox_folder_set(iconbox, etk_string_get(new_folder));
      etk_object_destroy(ETK_OBJECT(new_folder));
   }
   
   return ETK_TRUE;
}

/* Sets the folder displayed in the iconbox */
static void _etk_test_iconbox_folder_set(Etk_Iconbox *iconbox, const char *folder)
{
   Ecore_List *files;
   char *filename;
   char file_path[PATH_MAX];
   Etk_Iconbox_Icon *iicon;

   if (!iconbox)
      return;
   if (!folder && !(folder = getenv("HOME")))
      return;
   if (!(files = ecore_file_ls(folder)))
      return;

   etk_iconbox_clear(iconbox);
   etk_iconbox_append(iconbox, etk_theme_icon_path_get(), "actions/go-up_48", "..");

   /* First, add the folders */
   ecore_list_first_goto(files);
   while ((filename = ecore_list_next(files)))
   {
      if (filename[0] == '.')
         continue;

      snprintf(file_path, PATH_MAX, "%s/%s", folder, filename);
      if (!ecore_file_is_dir(file_path))
         continue;

      iicon = etk_iconbox_append(iconbox, etk_theme_icon_path_get(), "places/folder_48", filename);
      
      if (!ecore_file_can_read(file_path))
         etk_iconbox_icon_emblem_set_from_stock(iicon, "unreadable");
      else if (!ecore_file_can_write(file_path))
         etk_iconbox_icon_emblem_set_from_stock(iicon, "readonly");
   }

   /* Then the files */
   ecore_list_first_goto(files);
   while ((filename = ecore_list_next(files)))
   {
      const char *ext;
      char *icon = NULL;
      int i;

      if (filename[0] == '.')
         continue;

      snprintf(file_path, PATH_MAX, "%s/%s", folder, filename);
      if (ecore_file_is_dir(file_path))
         continue;

      if ((ext = strrchr(filename, '.')) && (ext = ext + 1))
      {
         for (i = 0; i < _etk_test_iconbox_num_types; i++)
         {
            if (strcasecmp(ext, _etk_test_iconbox_types[i].extension) == 0)
            {
               icon = _etk_test_iconbox_types[i].icon;
               break;
            }
         }
      }

      iicon = etk_iconbox_append(iconbox, etk_theme_icon_path_get(), icon ? icon : "mimetypes/text-x-generic_48", filename);
      if (!ecore_file_can_read(file_path))
         etk_iconbox_icon_emblem_set_from_stock(iicon, "unreadable");
      else if (!ecore_file_can_write(file_path))
         etk_iconbox_icon_emblem_set_from_stock(iicon, "readonly");
      else if (ecore_file_can_exec(file_path))
         etk_iconbox_icon_emblem_set_from_stock(iicon, "system");
   }

   ecore_list_destroy(files);

   if (!_etk_test_iconbox_current_folder)
      _etk_test_iconbox_current_folder = etk_string_new(NULL);
   etk_string_set(_etk_test_iconbox_current_folder, folder);
   etk_window_title_set(ETK_WINDOW(win), folder);
}
