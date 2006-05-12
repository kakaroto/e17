/** @file etk_filechooser_widget.c */
#include "etk_filechooser_widget.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <Ecore.h>
#include <Ecore_Data.h>
#include <Ecore_File.h>
#include "etk_theme.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"
#include "etk_tree.h"
#include "etk_tree_model.h"
#include "etk_paned.h"
#include "etk_box.h"
#include "etk_tooltips.h"
#include "config.h"

/* OS-specific to list the mount points */
#ifdef HAVE_LINUX
#include <mntent.h>
#endif

#ifdef HAVE_BSD
#include <sys/param.h>
#include <sys/ucred.h>
#include <sys/mount.h>
#endif

/**
 * @addtogroup Etk_Filechooser_Widget
 * @{
 */
 
#define ETK_FILECHOOSER_FAVS_FILE ".gtk-bookmarks"

enum _Etk_Filechooser_Widget_Signal_Id
{
   ETK_FILECHOOSER_WIDGET_NUM_SIGNALS
};

enum _Etk_Filechooser_Widget_Property_Id
{
   ETK_FILECHOOSER_WIDGET_PATH_PROPERTY,
   ETK_FILECHOOSER_WIDGET_SELECT_MULTIPLE_PROPERTY,
   ETK_FILECHOOSER_WIDGET_SHOW_HIDDEN_PROPERTY
};

typedef struct _Etk_Filechooser_Widget_Icons
{
   void *extension;
   char *icon;
} Etk_Filechooser_Widget_Icons;

static void _etk_filechooser_widget_constructor(Etk_Filechooser_Widget *filechooser_widget);
static void _etk_filechooser_widget_destructor(Etk_Filechooser_Widget *filechooser_widget);
static void _etk_filechooser_widget_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_filechooser_widget_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_filechooser_widget_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_filechooser_widget_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_filechooser_widget_place_activated_cb(Etk_Object *object, Etk_Tree_Row *row, void *data);
static void _etk_filechooser_widget_fav_activated_cb(Etk_Object *object, Etk_Tree_Row *row, void *data);
static void _etk_filechooser_widget_file_activated_cb(Etk_Object *object, Etk_Tree_Row *row, void *data);
static void _etk_filechooser_files_tree_row_mouse_in_cb(Etk_Object *object, Etk_Tree_Row *row, void *data);
static void _etk_filechooser_files_tree_row_mouse_out_cb(Etk_Object *object, Etk_Tree_Row *row, void *data);
static void _etk_filechooser_widget_places_tree_fill(Etk_Filechooser_Widget *fcw);
static void _etk_filechooser_widget_favs_tree_fill(Etk_Filechooser_Widget *fcw);

static Etk_Filechooser_Widget_Icons _etk_filechooser_icons[] =
{
   { "jpg", "mimetypes/image-x-generic_16" },
   { "jpeg", "mimetypes/image-x-generic_16" },
   { "png", "mimetypes/image-x-generic_16" },
   { "bmp", "mimetypes/image-x-generic_16" },
   { "gif", "mimetypes/image-x-generic_16" },
   { "mp3", "mimetypes/audio-x-generic_16" },
   { "ogg", "mimetypes/audio-x-generic_16" },
   { "wav", "mimetypes/audio-x-generic_16" },
   { "avi", "mimetypes/video-x-generic_16" },
   { "mpg", "mimetypes/video-x-generic_16" },
   { "mpeg", "mimetypes/video-x-generic_16" },
   { "gz", "mimetypes/package-x-generic_16" },
   { "tgz", "mimetypes/package-x-generic_16" },
   { "bz2", "mimetypes/package-x-generic_16" },
   { "tbz2", "mimetypes/package-x-generic_16" },
   { "zip", "mimetypes/package-x-generic_16" },
   { "rar", "mimetypes/package-x-generic_16" },
};
static int _etk_filechooser_num_icons = sizeof(_etk_filechooser_icons) / sizeof (_etk_filechooser_icons[0]);

static char *_etk_filechooser_unsupported_fs[] = { "proc", "sysfs", "tmpfs", "devpts", "usbfs", "procfs", "devfs" };
static int _etk_filechooser_num_unsupported_fs = sizeof(_etk_filechooser_unsupported_fs) / sizeof (_etk_filechooser_unsupported_fs[0]);
/* static Etk_Signal *_etk_filechooser_widget_signals[ETK_FILECHOOSER_WIDGET_NUM_SIGNALS]; */

static Ecore_Timer *_etk_filechooser_timer = NULL;
static char _etk_filechooser_tooltip_text[PATH_MAX + 128];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Filechooser_Widget
 * @return Returns the type on an Etk_Filechooser_Widget
 */
Etk_Type *etk_filechooser_widget_type_get()
{
   static Etk_Type *filechooser_widget_type = NULL;

   if (!filechooser_widget_type)
   {
      filechooser_widget_type = etk_type_new("Etk_Filechooser_Widget", ETK_WIDGET_TYPE, sizeof(Etk_Filechooser_Widget), ETK_CONSTRUCTOR(_etk_filechooser_widget_constructor), ETK_DESTRUCTOR(_etk_filechooser_widget_destructor));

      //_etk_filechooser_widget_signals[ETK_FILECHOOSER_WIDGET_TEXT_POPPED_SIGNAL] = etk_signal_new("text_popped", filechooser_widget_type, -1, etk_marshaller_VOID__INT_POINTER, NULL, NULL);
      
      etk_type_property_add(filechooser_widget_type, "path", ETK_FILECHOOSER_WIDGET_PATH_PROPERTY, ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(filechooser_widget_type, "select_multiple", ETK_FILECHOOSER_WIDGET_SELECT_MULTIPLE_PROPERTY, ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      etk_type_property_add(filechooser_widget_type, "show_hidden", ETK_FILECHOOSER_WIDGET_SHOW_HIDDEN_PROPERTY, ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));

      filechooser_widget_type->property_set = _etk_filechooser_widget_property_set;
      filechooser_widget_type->property_get = _etk_filechooser_widget_property_get;
   }

   return filechooser_widget_type;
}

/**
 * @brief Creates a new file chooser widget
 * @return Returns the new file chooser widget
 */
Etk_Widget *etk_filechooser_widget_new()
{
   return etk_widget_new(ETK_FILECHOOSER_WIDGET_TYPE, NULL);
}

/**
 * @brief Sets the current forlder of the filechooser
 * @param filechooser_widget 
 */
void etk_filechooser_widget_current_folder_set(Etk_Filechooser_Widget *filechooser_widget, const char *folder)
{
   Ecore_List *files;
   char *filename;
   char file_path[PATH_MAX];
   time_t mod_time;
   struct tm *mod_time2;
   char mod_time_string[128];
   
   if (!filechooser_widget)
      return;
   if (!folder && !(folder = getenv("HOME")))
      return;
   if (!(files = ecore_file_ls(folder)))
      return;
   
   free(filechooser_widget->current_folder);
   filechooser_widget->current_folder = strdup(folder);
   etk_tree_clear(ETK_TREE(filechooser_widget->files_tree));
   
   /* TODO: Do not walk through the list twice!! */
   ecore_list_goto_first(files);
   while ((filename = ecore_list_next(files)))
   {
      if (!filechooser_widget->show_hidden) 
      {
         if (filename[0] == '.')
            continue;
      }
      
      snprintf(file_path, PATH_MAX, "%s/%s", folder, filename);
      if (!ecore_file_is_dir(file_path))
         continue;
      
      mod_time = ecore_file_mod_time(file_path);
      mod_time2 = gmtime(&mod_time);
      /* TODO: compiler warning? */
      strftime(mod_time_string, 128, "%x", mod_time2);
      
      etk_tree_append(ETK_TREE(filechooser_widget->files_tree),
         filechooser_widget->files_name_col, etk_theme_icon_theme_get(), "places/folder_16", filename,
         filechooser_widget->files_date_col, mod_time_string, NULL);
   }
   
   ecore_list_goto_first(files);
   while ((filename = ecore_list_next(files)))
   {
      const char *ext;
      char *icon = NULL;
      int i;
      
      if (!filechooser_widget->show_hidden) 
      {
         if (filename[0] == '.')
            continue;
      }
      
      snprintf(file_path, PATH_MAX, "%s/%s", folder, filename);
      if (ecore_file_is_dir(file_path))
         continue;
      
      if ((ext = strrchr(filename, '.')) && (ext = ext + 1))
      {
         for (i = 0; i < _etk_filechooser_num_icons; i++)
         {
            if (strcasecmp(ext, _etk_filechooser_icons[i].extension) == 0)
            {
               icon = _etk_filechooser_icons[i].icon;
               break;
            }
         }
      }
      mod_time = ecore_file_mod_time(file_path);
      mod_time2 = gmtime(&mod_time);
      /* TODO: compiler warning? */
      strftime(mod_time_string, 128, "%x", mod_time2);
      
      etk_tree_append(ETK_TREE(filechooser_widget->files_tree),
         filechooser_widget->files_name_col, etk_theme_icon_theme_get(), icon ? icon : "mimetypes/text-x-generic_16", filename,
         filechooser_widget->files_date_col, mod_time_string, NULL);
   }
   
   ecore_list_destroy(files);
}

/**
 * @brief Retrieves the current folder
 * @return Returns the current folder
 */
const char *etk_filechooser_widget_current_folder_get(Etk_Filechooser_Widget *filechooser_widget) 
{
   if (!filechooser_widget)
      return NULL;
   return filechooser_widget->current_folder;
}

/**
 * @brief Retrives the current file selected
 * @return Returns filename
 */
const char *etk_filechooser_widget_selected_file_get(Etk_Filechooser_Widget *filechooser_widget)
{
   const char *filename;
   Etk_Tree_Row *row;

   if (!filechooser_widget && !(filechooser_widget->files_tree))
      return NULL;

   row = etk_tree_selected_row_get(ETK_TREE(filechooser_widget->files_tree));
   etk_tree_row_fields_get(row, filechooser_widget->files_name_col, NULL, NULL, &filename, NULL);

   return filename;
}

/**
 * @brief Retrives the current files selected
 * @return Returns filenames list
 */
Evas_List *etk_filechooser_widget_selected_files_get(Etk_Filechooser_Widget *filechooser_widget)
{
    char *filename;
    Evas_List *files = NULL;
    Evas_List *selected_rows;
    Evas_List *l;

    selected_rows = etk_tree_selected_rows_get(ETK_TREE(filechooser_widget->files_tree));
    for(l = selected_rows; l; l = l->next)
     {
       etk_tree_row_fields_get(l->data, filechooser_widget->files_name_col, NULL, NULL, &filename, NULL);
       files = evas_list_append(files, filename);
     }

    return files;
}

/**
 * @brief Sets if the filechooser widget can select multiple files
 * @param filechooser_widget a filechooser widget
 * @param select_multiple ETK_TRUE to allow the filechooser to select multiple files
 */
void etk_filechooser_widget_select_multiple_set(Etk_Filechooser_Widget *filechooser_widget, Etk_Bool select_multiple)
{
   if (!filechooser_widget || !filechooser_widget->files_tree)
      return;
   etk_tree_multiple_select_set(ETK_TREE(filechooser_widget->files_tree), select_multiple);   
}

/**
 * @brief Retrieves if the file chooser widget can select multiple files
 * @return Returns if the file chooser widget can select multiple files
 */
Etk_Bool etk_filechooser_widget_select_multiple_get(Etk_Filechooser_Widget *filechooser_widget)
{
   if (filechooser_widget)
      return ETK_FALSE;
   /* TODO: notify */
   return etk_tree_multiple_select_get(ETK_TREE(filechooser_widget->files_tree));
}

/**
 * @brief Sets if the file chooser widget can show hidden files
 * @param filechooser_widget a filechooser widget
 * @param show_hidden ETK_TRUE to allow the filechooser to show hidden files
 */
void etk_filechooser_widget_show_hidden_set(Etk_Filechooser_Widget *filechooser_widget, Etk_Bool show_hidden)
{
   if (!filechooser_widget)
      return;

   filechooser_widget->show_hidden = show_hidden;
   /* TODO: !! */
   //etk_filechooser_widget_current_folder_set(filechooser_widget, filechooser_widget->current_folder);
   etk_object_notify(ETK_OBJECT(filechooser_widget), "show_hidden");
}

/**
 * @brief Retrieve if the file chooser widget can show hidden files
 * @return Returns if the file chooser widget can show hidden files
 */
Etk_Bool etk_filechooser_widget_show_hidden_get(Etk_Filechooser_Widget *filechooser_widget)
{
   if (!filechooser_widget)
      return ETK_FALSE;
   return filechooser_widget->show_hidden;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_filechooser_widget_constructor(Etk_Filechooser_Widget *fcw)
{
   Etk_Widget *vpaned;
   
   if (!fcw)
      return;
   
   ETK_WIDGET(fcw)->size_request = _etk_filechooser_widget_size_request;
   ETK_WIDGET(fcw)->size_allocate = _etk_filechooser_widget_size_allocate;
   fcw->current_folder = NULL;
   fcw->select_multiple = ETK_FALSE;
   fcw->show_hidden = ETK_FALSE;
   
   fcw->hpaned = etk_hpaned_new();
   etk_widget_parent_set(fcw->hpaned, ETK_WIDGET(fcw));
   etk_widget_show(fcw->hpaned);
   etk_widget_visibility_locked_set(fcw->hpaned, ETK_TRUE);
   
   vpaned = etk_vpaned_new();
   etk_paned_add1(ETK_PANED(fcw->hpaned), vpaned, ETK_FALSE);
   etk_widget_show(vpaned);
   etk_widget_visibility_locked_set(vpaned, ETK_TRUE);
   
   fcw->places_tree = etk_tree_new();
   etk_widget_size_request_set(fcw->places_tree, 180, 180);
   etk_paned_add1(ETK_PANED(vpaned), fcw->places_tree, ETK_TRUE);
   fcw->places_col = etk_tree_col_new(ETK_TREE(fcw->places_tree), _("Places"), etk_tree_model_icon_text_new(ETK_TREE(fcw->places_tree), ETK_TREE_FROM_EDJE), 120);
   etk_tree_build(ETK_TREE(fcw->places_tree));
   etk_widget_show(fcw->places_tree);
   etk_widget_visibility_locked_set(fcw->places_tree, ETK_TRUE);
   etk_signal_connect("row_activated", ETK_OBJECT(fcw->places_tree), ETK_CALLBACK(_etk_filechooser_widget_place_activated_cb), fcw);
   
   fcw->fav_tree = etk_tree_new();
   etk_widget_size_request_set(fcw->fav_tree, 180, 180);
   etk_paned_add2(ETK_PANED(vpaned), fcw->fav_tree, ETK_TRUE);
   fcw->fav_col = etk_tree_col_new(ETK_TREE(fcw->fav_tree), _("Favorites"), etk_tree_model_icon_text_new(ETK_TREE(fcw->fav_tree), ETK_TREE_FROM_EDJE), 120);
   etk_tree_build(ETK_TREE(fcw->fav_tree));
   etk_widget_show(fcw->fav_tree);
   etk_widget_visibility_locked_set(fcw->fav_tree, ETK_TRUE);
   etk_signal_connect("row_activated", ETK_OBJECT(fcw->fav_tree), ETK_CALLBACK(_etk_filechooser_widget_fav_activated_cb), fcw);
   
   fcw->files_tree = etk_tree_new();
   etk_widget_size_request_set(fcw->files_tree, 400, 120);
   etk_paned_add2(ETK_PANED(fcw->hpaned), fcw->files_tree, ETK_TRUE);
   fcw->files_name_col = etk_tree_col_new(ETK_TREE(fcw->files_tree), _("Filename"), etk_tree_model_icon_text_new(ETK_TREE(fcw->files_tree), ETK_TREE_FROM_EDJE), 100);
   etk_tree_col_expand_set(fcw->files_name_col, ETK_TRUE);
   fcw->files_date_col = etk_tree_col_new(ETK_TREE(fcw->files_tree), _("Date"), etk_tree_model_text_new(ETK_TREE(fcw->files_tree)), 60);
   etk_tree_build(ETK_TREE(fcw->files_tree));
   etk_widget_show(fcw->files_tree);
   etk_widget_visibility_locked_set(fcw->files_tree, ETK_TRUE);
   etk_signal_connect("row_activated", ETK_OBJECT(fcw->files_tree), ETK_CALLBACK(_etk_filechooser_widget_file_activated_cb), fcw);
   etk_signal_connect("row_mouse_in", ETK_OBJECT(fcw->files_tree), ETK_CALLBACK(_etk_filechooser_files_tree_row_mouse_in_cb), fcw);
   etk_signal_connect("row_mouse_out", ETK_OBJECT(fcw->files_tree), ETK_CALLBACK(_etk_filechooser_files_tree_row_mouse_out_cb), fcw);

   _etk_filechooser_widget_places_tree_fill(ETK_FILECHOOSER_WIDGET(fcw));
   _etk_filechooser_widget_favs_tree_fill(ETK_FILECHOOSER_WIDGET(fcw));
   
   /* Go to home */
   etk_filechooser_widget_current_folder_set(ETK_FILECHOOSER_WIDGET(fcw), NULL);
}

/* Destroys the file chooser */
static void _etk_filechooser_widget_destructor(Etk_Filechooser_Widget *filechooser_widget)
{
   if (!filechooser_widget)
      return;
   free(filechooser_widget->current_folder);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_filechooser_widget_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Filechooser_Widget *filechooser_widget;

   if (!(filechooser_widget = ETK_FILECHOOSER_WIDGET(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_FILECHOOSER_WIDGET_SELECT_MULTIPLE_PROPERTY:
         etk_filechooser_widget_select_multiple_set(filechooser_widget, etk_property_value_bool_get(value));
         break;
      case ETK_FILECHOOSER_WIDGET_SHOW_HIDDEN_PROPERTY:
         etk_filechooser_widget_show_hidden_set(filechooser_widget, etk_property_value_bool_get(value));
         break;
      /* TODO: ETK_FILECHOOSER_WIDGET_PATH_PROPERTY */
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_filechooser_widget_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Filechooser_Widget *filechooser_widget;

   if (!(filechooser_widget = ETK_FILECHOOSER_WIDGET(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_FILECHOOSER_WIDGET_SELECT_MULTIPLE_PROPERTY:
         etk_property_value_bool_set(value, etk_filechooser_widget_select_multiple_get(filechooser_widget));
         break;
      case ETK_FILECHOOSER_WIDGET_SHOW_HIDDEN_PROPERTY:
         etk_property_value_bool_set(value, etk_filechooser_widget_show_hidden_get(filechooser_widget));
         break;
      /* TODO: ETK_FILECHOOSER_WIDGET_PATH_PROPERTY */
      default:
         break;
   }
}

/* Calculates the ideal size for the filechooser */
static void _etk_filechooser_widget_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   Etk_Filechooser_Widget *fcw;
   
   if (!(fcw = ETK_FILECHOOSER_WIDGET(widget)) || !size_requisition)
      return;
   etk_widget_size_request(fcw->hpaned, size_requisition);
}

/* Renders the filechooser in the allocated geometry */
static void _etk_filechooser_widget_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Filechooser_Widget *fcw;
   
   if (!(fcw = ETK_FILECHOOSER_WIDGET(widget)))
      return;
   etk_widget_size_allocate(fcw->hpaned, geometry);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when a row of the "places" tree is activated (double-click or space) */
static void _etk_filechooser_widget_place_activated_cb(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
   Etk_Filechooser_Widget *filechooser_widget;
   char *selected_dir;
   
   if (!(filechooser_widget = ETK_FILECHOOSER_WIDGET(data)) || !(selected_dir = etk_tree_row_data_get(row)))
     return;
   
   if (strcmp(selected_dir, "..") == 0)
   {
      char back[PATH_MAX];
      
      snprintf(back, PATH_MAX, "%s/..", filechooser_widget->current_folder);
      etk_filechooser_widget_current_folder_set(filechooser_widget, back);
   }
   else
      etk_filechooser_widget_current_folder_set(filechooser_widget, selected_dir);
}

/* Called when a row of the "favorites" tree is activated (double-click or space) */
static void _etk_filechooser_widget_fav_activated_cb(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
   Etk_Filechooser_Widget *filechooser_widget;
   
   if (!(filechooser_widget = ETK_FILECHOOSER_WIDGET(data)))
     return;
   etk_filechooser_widget_current_folder_set(filechooser_widget, etk_tree_row_data_get(row));
}

/* Called when a row of the "files" tree is activated (double-click or space) */
static void _etk_filechooser_widget_file_activated_cb(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
   Etk_Filechooser_Widget *filechooser_widget;
   char *selected_file;
   char file_path[PATH_MAX];
   
   if (!(filechooser_widget = ETK_FILECHOOSER_WIDGET(data)))
      return;

   etk_tree_row_fields_get(row, filechooser_widget->files_name_col, NULL, NULL, &selected_file, NULL);
   snprintf(file_path, PATH_MAX, "%s/%s", filechooser_widget->current_folder, selected_file);

   if(_etk_filechooser_timer)
     ecore_timer_del(_etk_filechooser_timer);
   _etk_filechooser_timer = NULL;
   etk_tooltips_pop_down();
   
   if (ecore_file_exists(file_path))
   {
      if (ecore_file_is_dir(file_path))
         etk_filechooser_widget_current_folder_set(filechooser_widget, file_path);
      else /* TODO */
         ;
   }
}

static int _etk_filechooser_timer_cb(void *data)
{
   Etk_Filechooser_Widget *fcw;
   
   _etk_filechooser_timer = NULL;
   
   if (!(fcw = ETK_FILECHOOSER_WIDGET(data)) || !_etk_filechooser_tooltip_text)
     return 0;
   
   etk_tooltips_tip_set(ETK_WIDGET(fcw), _etk_filechooser_tooltip_text);
   return 0;
}

/* Called when the mouse enters a row */
static void _etk_filechooser_files_tree_row_mouse_in_cb(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
   Etk_Filechooser_Widget *fcw;
   char *selected_file;
   char file_path[PATH_MAX];
   
   if (!(fcw = ETK_FILECHOOSER_WIDGET(data)))
     return;   
   
   etk_tree_row_fields_get(row, fcw->files_name_col, NULL, NULL, &selected_file, NULL);      
   
   snprintf(file_path, PATH_MAX, "%s/%s", fcw->current_folder, selected_file);
   if(!ecore_file_exists(file_path))
     return;
   
   snprintf(_etk_filechooser_tooltip_text, sizeof(_etk_filechooser_tooltip_text), " %s <br> %d Kb <br> ", selected_file, ecore_file_size(file_path) / 1024);

   _etk_filechooser_timer = ecore_timer_add(0.08, _etk_filechooser_timer_cb, fcw);
}

/* Called when the mouse leaves a row */
static void _etk_filechooser_files_tree_row_mouse_out_cb(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
   Etk_Filechooser_Widget *fcw;
      
   if (!(fcw = ETK_FILECHOOSER_WIDGET(data)))
     return;   
   
   if(_etk_filechooser_timer)
     ecore_timer_del(_etk_filechooser_timer);
   _etk_filechooser_timer = NULL;
   etk_tooltips_tip_set(ETK_WIDGET(fcw), NULL);
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Fills the "places" tree with some default elements, and with the mount points */
static void _etk_filechooser_widget_places_tree_fill(Etk_Filechooser_Widget *fcw)
{
   char *home;
   Etk_Tree_Row *row;
   
   if (!fcw)
      return;
   
   etk_tree_freeze(ETK_TREE(fcw->places_tree));
   
   row = etk_tree_append(ETK_TREE(fcw->places_tree), fcw->places_col, etk_theme_icon_theme_get(), "actions/go-up_16", "..", NULL);
   etk_tree_row_data_set_full(row, strdup(".."), free);
   
   if ((home = getenv("HOME")))
   {
      row = etk_tree_append(ETK_TREE(fcw->places_tree), fcw->places_col, etk_theme_icon_theme_get(), "places/user-home_16", _("Home"), NULL);
      etk_tree_row_data_set_full(row, strdup(home), free);
   }
   
   row = etk_tree_append(ETK_TREE(fcw->places_tree), fcw->places_col, etk_theme_icon_theme_get(), "devices/computer_16", _("Root"), NULL);
   etk_tree_row_data_set_full(row, strdup("/"), free);
   
   /* Adds the mount points */
#ifdef HAVE_LINUX
   {
      FILE *mtab;
      struct mntent *mount_point;
      Etk_Bool fs_supported;
      int i;
         
      if ((mtab = setmntent("/etc/mtab", "rt")))
      {
         while ((mount_point = getmntent(mtab)))
         {
            if (strcmp(mount_point->mnt_dir, "/") == 0 || strcmp(mount_point->mnt_dir, "/home") == 0)
               continue;
            
            fs_supported = ETK_TRUE;
            for (i = 0; i < _etk_filechooser_num_unsupported_fs; i++)
            {
               if (strcmp(mount_point->mnt_type, _etk_filechooser_unsupported_fs[i]) == 0)
               {
                  fs_supported = ETK_FALSE;
                  break;
               }
            }
            if (!fs_supported)
               continue;
            
            row = etk_tree_append(ETK_TREE(fcw->places_tree), fcw->places_col, etk_theme_icon_theme_get(), "devices/drive-harddisk_16", ecore_file_get_file(mount_point->mnt_dir), NULL);
            etk_tree_row_data_set_full(row, strdup(mount_point->mnt_dir), free);
         }
         endmntent(mtab);
      }
   }
#endif
   
#ifdef HAVE_BSD
   {
      struct statfs *mount_points;
      int num_mount_points;
      Etk_Bool fs_supported;
      int i, j;
      
      num_mount_points = getmntinfo(&mount_points, MNT_NOWAIT);
      for (i = 0; i < num_mount_points; i++)
      {
         if (strcmp(mount_points[i].f_mntonname, "/") == 0 || strcmp(mount_points[i].f_mntonname, "/home") == 0)
               continue;
            
         fs_supported = ETK_TRUE;
         for (j = 0; j < _etk_filechooser_num_unsupported_fs; j++)
         {
            if (strcmp(mount_points[i].f_fstypename, _etk_filechooser_unsupported_fs[j]) == 0)
            {
               fs_supported = ETK_FALSE;
               break;
            }
         }
         if (!fs_supported)
            continue;
         
         row = etk_tree_append(ETK_TREE(fcw->places_tree), fcw->places_col, etk_theme_icon_theme_get(), "devices/drive-harddisk_16", ecore_file_get_file(mount_points[i].f_mntonname), NULL);
         etk_tree_row_data_set_full(row, strdup(mount_points[i].f_mntonname), free);
      }
   }
#endif
   
   etk_tree_thaw(ETK_TREE(fcw->places_tree));
}

/* Get favorites from file in ~/.gtk-bookmarks */
static void _etk_filechooser_widget_favs_tree_fill(Etk_Filechooser_Widget *fcw)
{
   char *folder;
   FILE *f;
   char file_path[PATH_MAX];
   char line[PATH_MAX];
   char fav[PATH_MAX];
   
   if (!fcw)
     return;
   if (!(folder = getenv("HOME")))
     return;
   
   snprintf(file_path, sizeof(file_path), "%s/%s", folder, ETK_FILECHOOSER_FAVS_FILE);
   if (!(f = fopen(file_path, "r")))
      return;
   
   etk_tree_freeze(ETK_TREE(fcw->fav_tree));
   while (fgets(line, PATH_MAX, f))
   {
      Etk_Tree_Row *row;
      
      if (sscanf(line, "file://%s", fav) == 1)
      {
         row = etk_tree_append(ETK_TREE(fcw->fav_tree), fcw->fav_col, etk_theme_icon_theme_get(), "places/folder_16", ecore_file_get_file(fav), NULL);
         etk_tree_row_data_set_full(row, strdup(fav), free);
      }
   }
   etk_tree_thaw(ETK_TREE(fcw->fav_tree));
   
   fclose(f);
}

/** @} */
