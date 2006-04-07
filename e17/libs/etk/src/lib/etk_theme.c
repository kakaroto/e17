/** @file etk_theme.c */
#include "etk_theme.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Ecore_File.h>
#include <Edje.h>
#include "etk_widget.h"
#include "etk_string.h"
#include "config.h"

#define ETK_THEME_PARENT_GET(widget)      ((widget)->theme_parent ? (widget)->theme_parent : (widget)->parent)

static char *_etk_theme_find(const char *subdir, const char *file);

static char *_etk_theme_default_widget_theme = NULL;
static char *_etk_theme_widget_theme = NULL;
static char *_etk_theme_default_icon_theme = NULL;
static char *_etk_theme_icon_theme = NULL;

/** @brief Initializes the theming system of Etk. Do not call it manually, etk_init() calls it! */
void etk_theme_init()
{
   _etk_theme_default_widget_theme = _etk_theme_find("themes", "default");
   _etk_theme_default_icon_theme = _etk_theme_find("icons", "default");
   
   /* TODO: etk_config: add support of non default themes */
   etk_theme_widget_theme_set("default");
   etk_theme_icon_theme_set("default");
}

/** @brief Shutdowns the theming system of Etk. Do not call it manually, etk_shutdown() calls it! */
void etk_theme_shutdown()
{
   free(_etk_theme_default_widget_theme);
   _etk_theme_default_widget_theme = NULL;
   free(_etk_theme_widget_theme);
   _etk_theme_widget_theme = NULL;
   
   free(_etk_theme_default_icon_theme);
   _etk_theme_default_icon_theme = NULL;
   free(_etk_theme_icon_theme);
   _etk_theme_icon_theme = NULL;
}

/**
 * @brief Gets the widget theme file of Etk
 * @return Returns the path of the widget theme file
 */
const char *etk_theme_widget_theme_get()
{
   return _etk_theme_widget_theme;
}

/**
 * @brief Sets the theme to use for the widgets of Etk
 * @param theme_name the name of the theme to use
 * @return Returns ETK_TRUE if the theme has been found, or ETK_FALSE otherwize
 */
Etk_Bool etk_theme_widget_theme_set(const char *theme_name)
{
   char *path;
   
   if (!theme_name)
      return ETK_FALSE;
   
   if ((path = _etk_theme_find("themes", theme_name)))
   {
      free(_etk_theme_widget_theme);
      _etk_theme_widget_theme = path;
      return ETK_TRUE;
   }
   return ETK_FALSE;
}

/**
 * @brief Gets the default widget theme file of Etk
 * @return Returns the path of the default widget theme file
 */
const char *etk_theme_default_widget_theme_get()
{
   return _etk_theme_default_widget_theme;
}

/**
 * @brief Gets the icon theme file of Etk
 * @return Returns the path of the icon theme file
 */
const char *etk_theme_icon_theme_get()
{
   return _etk_theme_icon_theme;
}

/**
 * @brief Sets the theme to use for the icons of Etk
 * @param theme_name the name of the theme to use
 * @return Returns ETK_TRUE if the theme has been found, or ETK_FALSE otherwize
 */
Etk_Bool etk_theme_icon_theme_set(const char *theme_name)
{
   char *path;
   
   if (!theme_name)
      return ETK_FALSE;
   
   if ((path = _etk_theme_find("icons", theme_name)))
   {
      free(_etk_theme_icon_theme);
      _etk_theme_icon_theme = path;
      return ETK_TRUE;
   }
   return ETK_FALSE;
}

/**
 * @brief Gets the default icon theme file of Etk
 * @return Returns the path of the default icon theme file
 */
const char *etk_theme_icon_default_theme_get()
{
   return _etk_theme_default_icon_theme;
}

/**
 * @brief Creates and loads an edje object from the file "filename" and the group "group"
 * @param evas the canvas where the new object will be added
 * @param filename the .edj file which you want to load the edje object from
 * @param group the edje group to load
 * @return Returns the new edje object, or NULL on failure
 */
Evas_Object *etk_theme_object_load(Evas *evas, const char *filename, const char *group)
{
   Evas_Object *object;
   
   if (!evas || !filename || !group)
      return NULL;
   
   if (!(object = edje_object_add(evas)))
      return NULL;
   
   if (!edje_object_file_set(object, filename, group))
   {
      evas_object_del(object);
      return NULL;
   }
   return object;
}

/**
 * @brief Loads the theme object from a theme parent widget
 * @param evas the canvas where the new object will be added
 * @param theme_parent the theme parent
 * @param filename the .edj file that contains the object to load: if filename is NULL, then the theme file of the theme @n
 * parent will be used. If no parent have a non-null theme file or if the file doesn't contain the object, the default theme @n
 * file of Etk will be used.
 * @param group the group of the object to load. If a theme parent has been given, the group will also inherit the theme group @n
 * of this parent: for example, if @a group is "button" and the theme parent's theme group is "combobox", then the edje group used @n
 * will be "combobox/button"
 */
Evas_Object *etk_theme_object_load_from_parent(Evas *evas, Etk_Widget *theme_parent, const char *filename, const char *group)
{
   const char *edje_files[3];
   Etk_String *edje_group_str;
   char *edje_group, *grp;
   Etk_Widget *parent;
   Evas_Object *object;
   int i;
   
   if (!evas || !group)
      return NULL;
   
   /* Build the edje group string */
   edje_group_str = etk_string_new(group);
   for (parent = theme_parent; parent; parent = ETK_THEME_PARENT_GET(parent))
   {
      if (parent->theme_group)
         etk_string_prepend_printf(edje_group_str, "%s/", parent->theme_group);
   }
   edje_group = strdup(etk_string_get(edje_group_str));
   etk_object_destroy(ETK_OBJECT(edje_group_str));
   
   /* Build the list of theme files */
   edje_files[0] = filename;
   for (parent = theme_parent; parent && !edje_files[0]; parent = ETK_THEME_PARENT_GET(parent))
      edje_files[0] = parent->theme_file;
   if (!edje_files[0])
   {
      edje_files[0] = etk_theme_widget_theme_get();
      edje_files[1] = etk_theme_default_widget_theme_get();
      edje_files[2] = NULL;
   }
   else
   {
      edje_files[1] = etk_theme_widget_theme_get();
      edje_files[2] = etk_theme_default_widget_theme_get();
   }
   
   /* Try to load the theme object from the files */
   object = NULL;
   for (i = 0; i < 3 && !object; i++)
   {
      if (!edje_files[i])
         continue;
      
      grp = edje_group;
      while (grp && !object)
      {
         object = etk_theme_object_load(evas, edje_files[i], grp);
         
         if ((grp = strchr(grp, '/')))
            grp++;
      }
   }
   
   free(edje_group);
   return object;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Finds the theme called "theme_name" in the subdir "subdir" and returns its path, or NULL on failure */
static char *_etk_theme_find(const char *subdir, const char *theme_name)
{
   char *home;
   char *path;
   
   if (!theme_name || !subdir)
      return ETK_FALSE;
   
   if ((home = getenv("HOME")))
   {
      /* TODO: etk_config_dir_get? */
      path = malloc(strlen(home) + strlen(subdir) + strlen("/.etk//.edj") + strlen(theme_name) + 1);
      sprintf(path, "%s/.etk/%s/%s.edj", home, subdir, theme_name);
      if (ecore_file_exists(path))
         return path;
      else
         free(path);
   }
   
   path = malloc(strlen(subdir) + strlen(PACKAGE_DATA_DIR"//.edj") + strlen(theme_name) + 1);
   sprintf(path, PACKAGE_DATA_DIR"/%s/%s.edj", subdir, theme_name);
   if (ecore_file_exists(path))
      return path;
   
   free(path);
   return NULL;
}
