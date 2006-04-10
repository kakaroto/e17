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

static void _etk_test_iconbox_mouse_up_cb(Etk_Object *object, void *event_info, void *data);
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

/*static char *_etk_test_iconbox_icon_filenames[] =
{
   "/home/simon/etk/data/icons/default/icons/48x48/mimetypes/audio-x-generic.png",
   "/home/simon/etk/data/icons/default/icons/48x48/mimetypes/image-x-generic.png",
   "/home/simon/etk/data/icons/default/icons/48x48/mimetypes/video-x-generic.png",
   "/home/simon/etk/data/icons/default/icons/48x48/mimetypes/text-html.png",
   "/home/simon/etk/data/icons/default/icons/48x48/mimetypes/application-x-executable.png",
   "/home/simon/etk/data/themes/default/images/entry.png",
   "/home/simon/etk/data/themes/default/images/scrollbar_button_up1.png",
   "/home/simon/etk/data/themes/default/images/scrollbar_vdrag2.png"
};
static int _etk_test_iconbox_num_icon_filenames = sizeof(_etk_test_iconbox_icon_filenames) / sizeof(char *);*/

/* Creates the window for the iconbox test */
void etk_test_iconbox_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *iconbox;
   Etk_Iconbox_Model *mini_model;
   //int i;
   
   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }	
  
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), _("Etk Iconbox Test"));
   etk_widget_size_request_set(ETK_WIDGET(win), 100, 100);
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   
   iconbox = etk_iconbox_new();
   etk_container_add(ETK_CONTAINER(win), iconbox);
   etk_signal_connect("mouse_up", ETK_OBJECT(iconbox), ETK_CALLBACK(_etk_test_iconbox_mouse_up_cb), NULL);
   
   /* Create a new iconbox model: mini view */
   mini_model = etk_iconbox_model_new(ETK_ICONBOX(iconbox));
   etk_iconbox_model_geometry_set(mini_model, 150, 20, 2, 2);
   etk_iconbox_model_icon_geometry_set(mini_model, 0, 0, 16, 16, ETK_FALSE, ETK_TRUE);
   etk_iconbox_model_label_geometry_set(mini_model, 20, 0, 130, 16, 0.0, 0.5);
   //etk_iconbox_current_model_set(ETK_ICONBOX(iconbox), mini_model);
   
   /*for (i = 0; i < 500; i++)
      etk_iconbox_append(iconbox, _etk_test_iconbox_icon_filenames[rand() % _etk_test_iconbox_num_icon_filenames], NULL, "filename");*/
   _etk_test_iconbox_folder_set(ETK_ICONBOX(iconbox), NULL);
   
   etk_widget_show_all(win);
}

/* Called when the iconbox is pressed by the mouse */
static void _etk_test_iconbox_mouse_up_cb(Etk_Object *object, void *event_info, void *data)
{
   Etk_Iconbox *iconbox;
   Etk_Iconbox_Icon *icon;
   Etk_Event_Mouse_Up_Down *event;
   Etk_String *new_folder;
   
   if (!(iconbox = ETK_ICONBOX(object)) || !(event = event_info))
      return;
   if (event->button != 1)
      return;
   if (!(icon = etk_iconbox_icon_get_at_xy(iconbox, event->canvas.x, event->canvas.y, ETK_FALSE, ETK_TRUE, ETK_TRUE)))
      return;
   
   new_folder = etk_string_new_printf("%s/%s", etk_string_get(_etk_test_iconbox_current_folder), etk_iconbox_icon_label_get(icon));
   _etk_test_iconbox_folder_set(iconbox, etk_string_get(new_folder));
   etk_object_destroy(ETK_OBJECT(new_folder));
}

/* Sets the folder displayed in the iconbox */
static void _etk_test_iconbox_folder_set(Etk_Iconbox *iconbox, const char *folder)
{
   Ecore_List *files;
   char *filename;
   char file_path[PATH_MAX];
   
   if (!iconbox)
      return;
   if (!folder && !(folder = getenv("HOME")))
      return;
   if (!(files = ecore_file_ls(folder)))
      return;
   
   etk_iconbox_clear(iconbox);
   etk_iconbox_append(iconbox, etk_theme_icon_theme_get(), "actions/go-up_48", "..");
   
   /* First, add the folders */
   ecore_list_goto_first(files);
   while ((filename = ecore_list_next(files)))
   {      
      if (filename[0] == '.')
         continue;
      
      snprintf(file_path, PATH_MAX, "%s/%s", folder, filename);
      if (!ecore_file_is_dir(file_path))
         continue;
      
      etk_iconbox_append(iconbox, etk_theme_icon_theme_get(), "places/folder_48", filename);
   }
   
   /* Then the files */
   ecore_list_goto_first(files);
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
      
      etk_iconbox_append(iconbox, etk_theme_icon_theme_get(), icon ? icon : "mimetypes/text-x-generic_48", filename);
   }
   
   ecore_list_destroy(files);
   
   if (!_etk_test_iconbox_current_folder)
      _etk_test_iconbox_current_folder = etk_string_new(NULL);
   etk_string_set(_etk_test_iconbox_current_folder, folder);
}
