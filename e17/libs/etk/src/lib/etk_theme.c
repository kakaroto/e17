/** @file etk_theme.c */
#include "etk_theme.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Ecore_File.h>
#include <Edje.h>
#include "config.h"

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
   
   /* TODO: adds support of non default themes */
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
const char *etk_theme_widget_default_theme_get()
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
 * @param evas the canvas where to add the new object
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
 * @brief Tries to load a theme object from the current widget theme, and then, @n
 * if it fails, it tries to load the object from the default etk theme
 * @param evas the canvas where to add the new object
 * @param group the edje group to load
 * @return Returns the new object, or NULL on failure
 */
Evas_Object *etk_theme_object_load_from_widget_theme(Evas *evas, const char *group)
{
   Evas_Object *object;
   
   if (!evas || !group)
      return NULL;
   
   if (!(object = etk_theme_object_load(evas, etk_theme_widget_theme_get(), group)))
      return etk_theme_object_load_from_widget_default_theme(evas, group);
   else
      return object;
}

/**
 * @brief Tries to load a theme object from the default widget theme
 * @param evas the canvas where to add the new object
 * @param group the edje group to load
 * @return Returns the new object, or NULL on failure
 */
Evas_Object *etk_theme_object_load_from_widget_default_theme(Evas *evas, const char *group)
{
   if (!evas || !group)
      return NULL;
   return etk_theme_object_load(evas, etk_theme_widget_default_theme_get(), group);
}

/**
 * @brief Tries to load a theme object from the current widget theme, and then, @n
 * if it fails, it tries to load the object from the default etk theme
 * @param evas the canvas where to add the new object
 * @param group the edje group to load
 * @return Returns the new object, or NULL on failure
 */
Evas_Object *etk_theme_object_load_from_icon_theme(Evas *evas, const char *group)
{
   Evas_Object *object;
   
   if (!evas || !group)
      return NULL;
   
   if (!(object = etk_theme_object_load(evas, etk_theme_icon_theme_get(), group)))
      return etk_theme_object_load_from_icon_default_theme(evas, group);
   else
      return object;
}

/**
 * @brief Tries to load a theme object from the default widget theme
 * @param evas the canvas where to add the new object
 * @param group the edje group to load
 * @return Returns the new object, or NULL on failure
 */
Evas_Object *etk_theme_object_load_from_icon_default_theme(Evas *evas, const char *group)
{
   if (!evas || !group)
      return NULL;
   return etk_theme_object_load(evas, etk_theme_icon_default_theme_get(), group);
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Finds the theme called "theme_name" in the subdir "subdir" and returns its path, or NULL on failure */
static char *_etk_theme_find(const char *subdir, const char *theme_name)
{
   char *path;
   
   if (!theme_name || !subdir)
      return ETK_FALSE;
   
   path = malloc(strlen(subdir) + strlen(PACKAGE_DATA_DIR"//.edj") + strlen(theme_name) + 1);
   sprintf(path, PACKAGE_DATA_DIR"/%s/%s.edj", subdir, theme_name);
   if (ecore_file_exists(path))
      return path;
   
   free(path);
   return NULL;
   
   /* TODO: look for the themes in the ~/.etk/themes */
}
