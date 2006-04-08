#include "etk_test.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <Ecore_Data.h>
#include <Ecore_File.h>

static void _etk_test_iconbox_folder_set(Etk_Iconbox *iconbox, const char *folder);

static char *_etk_test_iconbox_icon_filenames[] =
{
   "/home/simon/etk/data/icons/default/icons/48x48/mimetypes/audio-x-generic.png",
   "/home/simon/etk/data/icons/default/icons/48x48/mimetypes/image-x-generic.png",
   "/home/simon/etk/data/icons/default/icons/48x48/mimetypes/video-x-generic.png",
   "/home/simon/etk/data/icons/default/icons/48x48/mimetypes/text-html.png",
   "/home/simon/etk/data/icons/default/icons/48x48/mimetypes/application-x-executable.png"
};
static int _etk_test_iconbox_num_icon_filenames = sizeof(_etk_test_iconbox_icon_filenames) / sizeof(char *);

/* Creates the window for the iconbox test */
void etk_test_iconbox_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *iconbox;
   Etk_Iconbox_Model *mini_model;
   int i;
   
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
   
   /* Create a new iconbox model: mini view */
   mini_model = etk_iconbox_model_new(ETK_ICONBOX(iconbox));
   etk_iconbox_model_geometry_set(mini_model, 150, 20, 2, 2);
   etk_iconbox_model_icon_geometry_set(mini_model, 0, 0, 16, 16);
   etk_iconbox_model_label_geometry_set(mini_model, 20, 0, 130, 16, 0.0, 0.5);
   //etk_iconbox_current_model_set(ETK_ICONBOX(iconbox), mini_model);
   
   /*for (i = 0; i < 500; i++)
      etk_iconbox_append(iconbox, _etk_test_iconbox_icon_filenames[rand() % _etk_test_iconbox_num_icon_filenames], NULL, "filename");*/
   _etk_test_iconbox_folder_set(ETK_ICONBOX(iconbox), NULL);
   
   etk_widget_show_all(win);
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
         /*for (i = 0; i < _etk_filechooser_num_icons; i++)
         {
            if (strcasecmp(ext, _etk_filechooser_icons[i].extension) == 0)
            {
               icon = _etk_filechooser_icons[i].icon;
               break;
            }
         }*/
      }
      
      etk_iconbox_append(iconbox, etk_theme_icon_theme_get(), "mimetypes/audio-x-generic_48", filename);
   }
   
   ecore_list_destroy(files);
}
