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

/** @file etk_theme.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_theme.h"

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Ecore_File.h>
#include <Edje.h>

#include "etk_config.h"
#include "etk_widget.h"

/**
 * @addtogroup Etk_Theme
 * @{
 */

static char *_etk_theme_find(const char *subdir, const char *file);

static int _etk_theme_default_colors[ETK_COLOR_NUM_COLORS][4] =
{
   { 0, 0, 0, 255 },
   { 255, 255, 255, 255 },
   { 158, 158, 0, 255 },
   { 255, 255, 255, 255 },
   { 205, 0, 0, 255 },
   { 255, 255, 255, 255 },
   { 0, 0, 205, 255 },
   { 255, 255, 255, 255 },
   { 0, 140, 0, 255 },
   { 255, 255, 255, 255 },
   { 111, 79, 143, 255 },
   { 255, 255, 255, 255 },
   { 205, 0, 205, 255 },
   { 255, 255, 255, 255 },
   { 145, 87, 26, 255 },
   { 255, 255, 255, 255 },
   { 136, 136, 136, 255 },
   { 255, 255, 255, 255 }
};

static char *_etk_theme_color_names[ETK_COLOR_NUM_COLORS] =
{
   "etk/color/foreground",
   "etk/color/background",
   "etk/color/important/fg",
   "etk/color/important/bg",
   "etk/color/warning/fg",
   "etk/color/warning/bg",
   "etk/color/info/fg",
   "etk/color/info/bg",
   "etk/color/default1/fg",
   "etk/color/default1/bg",
   "etk/color/default2/fg",
   "etk/color/default2/bg",
   "etk/color/default3/fg",
   "etk/color/default3/bg",
   "etk/color/default4/fg",
   "etk/color/default4/bg",
   "etk/color/default5/fg",
   "etk/color/default5/bg"
};

static char *_etk_theme_widget_default = NULL;
static char *_etk_theme_widget_current = NULL;
static char *_etk_theme_icon_default = NULL;
static char *_etk_theme_icon_current = NULL;

/**
 * @internal
 * @brief Initializes the theme-system of Etk
 */
void etk_theme_init(void)
{
   _etk_theme_widget_default = _etk_theme_find("themes", "default");
   _etk_theme_icon_default = _etk_theme_find("icons", "default");

   etk_theme_widget_set_from_name(etk_config_widget_theme_get());
   etk_theme_icon_set_from_name("default");
}

/**
 * @internal
 * @brief Shutdowns the theme system of Etk
 */
void etk_theme_shutdown(void)
{
   free(_etk_theme_widget_default);
   free(_etk_theme_widget_current);
   free(_etk_theme_icon_default);
   free(_etk_theme_icon_current);

   _etk_theme_widget_default = NULL;
   _etk_theme_widget_current = NULL;
   _etk_theme_icon_default = NULL;
   _etk_theme_icon_current = NULL;
}

/**
 * @brief Sets the theme that will be used by new widgets, from its path
 * @param theme_path the path of the widget-theme to use
 * @return Returns ETK_TRUE if the theme has been found, or ETK_FALSE otherwise
 */
Etk_Bool etk_theme_widget_set_from_path(const char *theme_path)
{
   if (!theme_path)
      return ETK_FALSE;

   if (ecore_file_exists(theme_path))
   {
      free(_etk_theme_widget_current);
      _etk_theme_widget_current = strdup(theme_path);
      return ETK_TRUE;
   }
   return ETK_FALSE;
}

/**
 * @brief Sets the theme that will be used by new widgets, from its name
 * @param theme_name the name of the widget-theme to use
 * @return Returns ETK_TRUE if the theme has been found, or ETK_FALSE otherwise
 */
Etk_Bool etk_theme_widget_set_from_name(const char *theme_name)
{
   char *path;
   Etk_Bool result;

   path = etk_theme_widget_find(theme_name);
   result = etk_theme_widget_set_from_path(path);
   free(path);

   return result;
}

/**
 * @brief Gets the path of the current widget-theme file of Etk
 * @return Returns the path of the current widget-theme file
 */
const char *etk_theme_widget_path_get(void)
{
   return _etk_theme_widget_current;
}

/**
 * @brief Gets the name of the current widget-theme file of Etk
 * @return Returns the name of the current widget-theme file, or NULL on failure
 * @note The returned path will have to be freed with free()
 */
char *etk_theme_widget_name_get(void)
{
   const char *start, *end;
   char *name;
   int len;

   if (!_etk_theme_widget_current)
      return NULL;

   if (!(start = strrchr(_etk_theme_widget_current, '/')))
      start = _etk_theme_widget_current;
   else
      start++;

   if ((end = strrchr(start, '.')))
   {
      len = end - start;
      name = malloc(len + 1);
      strncpy(name, start, len);
      name[len] = '\0';
      return name;
   }

   return NULL;
}

/**
 * @brief Gets the list of the available widget-themes. The list contains the names of the themes, not the paths
 * @return Returns an Eina_List of the available widget-themes
 * @note The returned list should be free with etk_theme_available_themes_free()
 */
Eina_List *etk_theme_widget_available_themes_get(void)
{
   Ecore_List *files;
   Eina_List *themes = NULL;
   char path[2][PATH_MAX];
   char *home;
   char *file;
   int i;

   if ((home = getenv("HOME")))
      snprintf(path[0], PATH_MAX, "%s/.e/etk/themes/", home);
   else
      path[0][0] = '\0';
   snprintf(path[1], PATH_MAX, PACKAGE_DATA_DIR "/themes/");

   for (i = 0; i < 2; i++)
   {
      files = ecore_file_ls(path[i]);
      if (files)
      {
         ecore_list_first_goto(files);
         while ((file = ecore_list_next(files)))
            themes = eina_list_append(themes, ecore_file_strip_ext(file));
         ecore_list_destroy(files);
      }
   }

   return themes;
}

/**
 * @brief Finds the path of a widget-theme file, from its name
 * @param theme_name the name of the theme to find
 * @return Returns the path of the corresponding widget-theme file,
 * or NULL if there is no corresponding theme
 * @note The returned value will have to be freed with free()
 */
char *etk_theme_widget_find(const char *theme_name)
{
   return _etk_theme_find("themes", theme_name);
}

/**
 * @brief Sets the theme that will be used by new icons, from its path
 * @param theme_path the path of the icon-theme to use
 * @return Returns ETK_TRUE if the theme has been found, or ETK_FALSE otherwise
 */
Etk_Bool etk_theme_icon_set_from_path(const char *theme_path)
{
   if (!theme_path)
      return ETK_FALSE;

   if (ecore_file_exists(theme_path))
   {
      free(_etk_theme_icon_current);
      _etk_theme_icon_current = strdup(theme_path);
      return ETK_TRUE;
   }
   return ETK_FALSE;
}

/**
 * @brief Sets the theme that will be used by new icons, from its name
 * @param theme_name the name of the icon-theme to use
 * @return Returns ETK_TRUE if the theme has been found, or ETK_FALSE otherwise
 */
Etk_Bool etk_theme_icon_set_from_name(const char *theme_name)
{
   char *path;
   Etk_Bool result;

   path = etk_theme_icon_find(theme_name);
   result = etk_theme_icon_set_from_path(path);
   free(path);

   return result;
}

/**
 * @brief Gets the path of the current icon-theme file of Etk
 * @return Returns the path of the current icon-theme file
 */
const char *etk_theme_icon_path_get(void)
{
   return _etk_theme_icon_current;
}

/**
 * @brief Gets the name of the current icon-theme file of Etk
 * @return Returns the name of the current icon-theme file, or NULL on failure
 * @note The returned path will have to be freed with free()
 */
char *etk_theme_icon_name_get(void)
{
   const char *start, *end;
   char *name;
   int len;

   if (!_etk_theme_icon_current)
      return NULL;

   if (!(start = strrchr(_etk_theme_icon_current, '/')))
      start = _etk_theme_icon_current;
   else
      start++;

   if ((end = strrchr(start, '.')))
   {
      len = end - start;
      name = malloc(len + 1);
      strncpy(name, start, len);
      name[len] = '\0';
      return name;
   }

   return NULL;
}

/**
 * @brief Gets the list of the available icon-themes. The list contains the names of the themes, not the paths
 * @return Returns an Eina_List of the available icon-themes
 * @note The returned list should be free with etk_theme_available_themes_free()
 */
Eina_List *etk_theme_icon_available_themes_get(void)
{
   Ecore_List *files;
   Eina_List *themes = NULL;
   char path[2][PATH_MAX];
   char *home;
   char *file;
   int i;

   if ((home = getenv("HOME")))
      snprintf(path[0], PATH_MAX, "%s/.e/etk/icons/", home);
   else
      path[0][0] = '\0';
   snprintf(path[1], PATH_MAX, PACKAGE_DATA_DIR "/icons/");

   for (i = 0; i < 2; i++)
   {
      files = ecore_file_ls(path[i]);
      if (files)
      {
         ecore_list_first_goto(files);
         while ((file = ecore_list_next(files)))
            themes = eina_list_append(themes, ecore_file_strip_ext(file));
         ecore_list_destroy(files);
      }
   }

   return themes;
}

/**
 * @brief Finds the path of a icon-theme file, from its name
 * @param theme_name the name of the theme to find
 * @return Returns the path of the corresponding icon-theme file,
 * or NULL if there is no corresponding theme
 * @note The returned value will have to be freed with free()
 */
char *etk_theme_icon_find(const char *theme_name)
{
   return _etk_theme_find("icons", theme_name);
}

/**
 * @brief A function used to free the list returned by etk_theme_widget_available_themes_get() or
 * etk_theme_icon_available_themes_get()
 * @param themes the list of themes to free
 */
void etk_theme_available_themes_free(Eina_List *themes)
{
   while (themes)
   {
      free(themes->data);
      themes = eina_list_remove_list(themes, themes);
   }
}

/**
 * @brief Checks whether the given theme-group exists. The edje-group to check is named
 * "etk/parent_group/group" if @a parent_group is not NULL, or "etk/group" if @a parent_group is NULL
 * @param file the path of a theme-file. If @a file is NULL, the current Etk's theme-file is used
 * @param group the main theme-group
 * @param parent_group the parent theme-group. It can be NULL
 * @return Returns ETK_TRUE if the theme-group exists, ETK_FALSE otherwise
 */
Etk_Bool etk_theme_group_exists(const char *file, const char *group, const char *parent_group)
{
   char full_group[PATH_MAX];

   if (!file)
      file = _etk_theme_widget_current ? _etk_theme_widget_current : _etk_theme_widget_default;
   if (!group || group[0] == '\0' || !file)
      return ETK_FALSE;

   if (parent_group && parent_group[0] != '\0')
      snprintf(full_group, PATH_MAX, "etk/%s/%s", parent_group, group);
   else
      snprintf(full_group, PATH_MAX, "etk/%s", group);

   return edje_file_group_exists(file, full_group);
}

/**
 * @brief Loads an edje-group from a theme-file and sets it to the object. The edje-group to load is named
 * "etk/parent_group/group" if @a parent_group is not NULL, or "etk/group" if @a parent_group is NULL
 * @param object an Edje object
 * @param file the path of the theme-file. If @a file is NULL, the current Etk's theme-file is used
 * @param group the theme-group of the object
 * @param parent_group the theme-group of the parent of the object. It can be NULL
 * @return Returns ETK_TRUE on success, ETK_FALSE on failure
 */
Etk_Bool etk_theme_edje_object_set(Evas_Object *object, const char *file, const char *group, const char *parent_group)
{
   char full_group[PATH_MAX];

   if (!object)
      return ETK_FALSE;

   if (!file)
      file = _etk_theme_widget_current ? _etk_theme_widget_current : _etk_theme_widget_default;
   if (!group || group[0] == '\0' || !file)
   {
      edje_object_file_set(object, NULL, NULL);
      return ETK_FALSE;
   }

   if (parent_group && parent_group[0] != '\0')
      snprintf(full_group, PATH_MAX, "etk/%s/%s", parent_group, group);
   else
      snprintf(full_group, PATH_MAX, "etk/%s", group);

   if (edje_object_file_set(object, file, full_group))
      return ETK_TRUE;
   else
   {
      edje_object_file_set(object, NULL, NULL);
      return ETK_FALSE;
   }
}

/**
 * @brief Loads an edje-group from a theme-file and sets it to the object.
 * Equivalent to etk_theme_edje_object_set(object, etk_widget_theme_file_get(parent), group, etk_widget_theme_group_get(parent))
 * @param object an Edje object
 * @param group the theme-group of the object
 * @param parent the theme-parent of the object
 * @return Returns ETK_TRUE on success, ETK_FALSE on failure
 */
Etk_Bool etk_theme_edje_object_set_from_parent(Evas_Object *object, const char *group, Etk_Widget *parent)
{
   if (!object)
      return ETK_FALSE;

   return etk_theme_edje_object_set(object, etk_widget_theme_file_get(parent),
         group, etk_widget_theme_group_get(parent));
}

/**
 * @brief Gets the components of the given color. The color depends on the theme used
 * @param file the path to the theme-file where to find the given color. If @a file is NULL,
 * the current widget-theme file will be used
 * @param color_type the color whose components will be returned
 * @param r the location where to store the 'red' component of the color
 * @param g the location where to store the 'green' component of the color
 * @param b the location where to store the 'blue' component of the color
 * @param a the location where to store the 'alpha' component of the color
 * @return Returns ETK_TRUE if the color has been found, ETK_FALSE otherwise
 * @note Note that even if the color has not been found (because it is not defined in the theme),
 * the components will still be set to their default values, so the color will still be usable
 */
Etk_Bool etk_theme_color_get(const char *file, Etk_Color_Type color_type, int *r, int *g, int *b, int *a)
{
   char *color_string;

   if (!file)
      file = _etk_theme_widget_current ? _etk_theme_widget_current : _etk_theme_widget_default;
   if (color_type < 0 || color_type >= ETK_COLOR_NUM_COLORS)
      color_type = ETK_COLOR_FOREGROUND;

   if (file)
   {
      color_string = edje_file_data_get(file, _etk_theme_color_names[color_type]);
      if (color_string && sscanf(color_string, "%d %d %d %d", r, g, b, a) == 4)
      {
         free(color_string);
         return ETK_TRUE;
      }
      free(color_string);
   }

   /* The color has not been found, we set it to its default value */
   if (r)   *r = _etk_theme_default_colors[color_type][0];
   if (g)   *g = _etk_theme_default_colors[color_type][1];
   if (b)   *b = _etk_theme_default_colors[color_type][2];
   if (a)   *a = _etk_theme_default_colors[color_type][3];

   return ETK_FALSE;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Finds the theme called "theme_name" in the subdir "subdir" and returns its path, or NULL on failure */
static char *_etk_theme_find(const char *subdir, const char *theme_name)
{
   char path[PATH_MAX];
   char *home;

   if (!theme_name || !subdir)
      return NULL;

   if ((home = getenv("HOME")))
   {
      snprintf(path, PATH_MAX, "%s/.e/etk/%s/%s.edj", home, subdir, theme_name);
      if (ecore_file_exists(path))
         return strdup(path);
   }

   snprintf(path, PATH_MAX, PACKAGE_DATA_DIR"/%s/%s.edj", subdir, theme_name);
   if (ecore_file_exists(path))
      return strdup(path);

   return NULL;
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Theme
 *
 * TODOC
 */
