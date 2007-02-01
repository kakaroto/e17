/** @file etk_theme.c */
#include "etk_theme.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Ecore_File.h>
#include <Edje.h>
#include "etk_widget.h"
#include "etk_config.h"
#include "config.h"

static char *_etk_theme_find(const char *subdir, const char *file);

static char *_etk_theme_widget_default = NULL;
static char *_etk_theme_widget_current = NULL;
static char *_etk_theme_icon_default = NULL;
static char *_etk_theme_icon_current = NULL;
static char *_etk_theme_colors[ETK_COLOR_NUM_COLORS];

/**
 * @internal
 * @brief Initializes the themz system of Etk
 */
void etk_theme_init()
{
   _etk_theme_widget_default = _etk_theme_find("themes", "default");
   _etk_theme_icon_default = _etk_theme_find("icons", "default");
   
   etk_theme_widget_set(etk_config_widget_theme_get());
   etk_theme_icon_set("default");

   _etk_theme_colors[ETK_COLOR_FOREGROUND] = strdup("etk/color/foreground");
   _etk_theme_colors[ETK_COLOR_BACKGROUND] = strdup("etk/color/background");
   _etk_theme_colors[ETK_COLOR_IMPORTANT_FG] = strdup("etk/color/important/fg");
   _etk_theme_colors[ETK_COLOR_IMPORTANT_BG] = strdup("etk/color/important/bg");
   _etk_theme_colors[ETK_COLOR_WARNING_FG] = strdup("etk/color/warning/fg");
   _etk_theme_colors[ETK_COLOR_WARNING_BG] = strdup("etk/color/warning/bg");
   _etk_theme_colors[ETK_COLOR_INFO_FG] = strdup("etk/color/info/fg");
   _etk_theme_colors[ETK_COLOR_INFO_BG] = strdup("etk/color/info/bg");
   _etk_theme_colors[ETK_COLOR_DEFAULT1_FG] = strdup("etk/color/default1/fg");
   _etk_theme_colors[ETK_COLOR_DEFAULT1_BG] = strdup("etk/color/default1/bg");
   _etk_theme_colors[ETK_COLOR_DEFAULT2_FG] = strdup("etk/color/default2/fg");
   _etk_theme_colors[ETK_COLOR_DEFAULT2_BG] = strdup("etk/color/default2/bg");
   _etk_theme_colors[ETK_COLOR_DEFAULT3_FG] = strdup("etk/color/default3/fg");
   _etk_theme_colors[ETK_COLOR_DEFAULT3_BG] = strdup("etk/color/default3/bg");
   _etk_theme_colors[ETK_COLOR_DEFAULT4_FG] = strdup("etk/color/default4/fg");
   _etk_theme_colors[ETK_COLOR_DEFAULT4_BG] = strdup("etk/color/default4/bg");
   _etk_theme_colors[ETK_COLOR_DEFAULT5_FG] = strdup("etk/color/default5/fg");
   _etk_theme_colors[ETK_COLOR_DEFAULT5_BG] = strdup("etk/color/default5/bg");
}

/**
 * @internal
 * @brief Shutdowns the theme system of Etk
 */
void etk_theme_shutdown()
{
   int i;

   free(_etk_theme_widget_default);
   free(_etk_theme_widget_current);
   free(_etk_theme_icon_default);
   free(_etk_theme_icon_current);
   
   _etk_theme_widget_default = NULL;
   _etk_theme_widget_current = NULL;
   _etk_theme_icon_default = NULL;
   _etk_theme_icon_current = NULL;

   for (i = 0; i < ETK_COLOR_NUM_COLORS; i++)
   {
      if (_etk_theme_colors[i])
	 free(_etk_theme_colors[i]);
   }
}

/**
 * @brief Gets the path of the current widget-theme file of Etk
 * @return Returns the path of the current widget-theme file
 */
const char *etk_theme_widget_get()
{
   return _etk_theme_widget_current;
}

/**
 * @brief Sets the theme that will be used by new widgets
 * @param theme_name the name of the widget-theme to use
 * @return Returns ETK_TRUE if the theme has been found, or ETK_FALSE otherwise
 */
Etk_Bool etk_theme_widget_set(const char *theme_name)
{
   char *path;
   
   if (!theme_name)
      return ETK_FALSE;
   
   if ((path = _etk_theme_find("themes", theme_name)))
   {
      free(_etk_theme_widget_current);
      _etk_theme_widget_current = path;
      return ETK_TRUE;
   }
   return ETK_FALSE;
}

/**
 * @brief Gets the list of available widget-themes
 * @return Returns an Evas_List of available widget-themes.
 * The items of the list will have to be freed with free(), and the list will have to be freed with evas_list_free()
 */
Evas_List *etk_theme_widget_available_themes_get()
{
   Ecore_List *files;
   Evas_List *themes = NULL;
   char *home;
   char *path;
   char *file;
   
   if ((home = getenv("HOME")))
   {
      /* TODO: etk_config_dir_get? */
      path = malloc(strlen(home) + strlen("/.e/etk/themes/") + 1);
      sprintf(path, "%s/.e/etk/themes/", home);
      files = ecore_file_ls(path);
      if (files)
      {
	 ecore_list_goto_first(files);
	 while ((file = ecore_list_next(files)))      
	    themes = evas_list_append(themes, ecore_file_strip_ext(file));
         ecore_list_destroy(files);
      }
      free(path);
   }
   
   files = ecore_file_ls(PACKAGE_DATA_DIR "/themes/");
   if (files)
   {
      ecore_list_goto_first(files);
      while ((file = ecore_list_next(files)))
	 themes = evas_list_append(themes, ecore_file_strip_ext(file));
      ecore_list_destroy(files);
   }
   
   return themes;
}

/**
 * @brief Gets the path of the current icon-theme file of Etk
 * @return Returns the path of the current icon-theme file
 */
const char *etk_theme_icon_get()
{
   return _etk_theme_icon_current;
}

/**
 * @brief Sets the theme that will be used by new icons
 * @param theme_name the name of the icon-theme to use
 * @return Returns ETK_TRUE if the theme has been found, or ETK_FALSE otherwise
 */
Etk_Bool etk_theme_icon_set(const char *theme_name)
{
   char *path;
   
   if (!theme_name)
      return ETK_FALSE;
   
   if ((path = _etk_theme_find("icons", theme_name)))
   {
      free(_etk_theme_icon_current);
      _etk_theme_icon_current = path;
      return ETK_TRUE;
   }
   return ETK_FALSE;
}

/**
 * @brief Gets the list of available icon-themes
 * @return Returns an Evas_List of available icon-themes.
 * The items of the list will have to be freed with free(), and the list will have to be freed with evas_list_free()
 */
Evas_List *etk_theme_icon_available_themes_get()
{
   Ecore_List *files;
   Evas_List *themes = NULL;
   char *home;
   char *path;
   char *file;
   
   if ((home = getenv("HOME")))
   {
      /* TODO: etk_config_dir_get? */
      path = malloc(strlen(home) + strlen("/.e/etk/icons/") + 1);
      sprintf(path, "%s/.e/etk/icons/", home);
      files = ecore_file_ls(path);
      if (files)
      {
	 ecore_list_goto_first(files);
	 while ((file = ecore_list_next(files)))      
	    themes = evas_list_append(themes, ecore_file_strip_ext(file));
         ecore_list_destroy(files);
      }
      free(path);
   }
   
   files = ecore_file_ls(PACKAGE_DATA_DIR "/icons/");
   if (files)
   {
      ecore_list_goto_first(files);
      while ((file = ecore_list_next(files)))
	 themes = evas_list_append(themes, ecore_file_strip_ext(file));
      ecore_list_destroy(files);
   }
   
   return themes;
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
   char *full_group;
   
   if (!file)
      file = _etk_theme_widget_current ? _etk_theme_widget_current : _etk_theme_widget_default;
   if (!group || *group == '\0' || !file)
      return ETK_FALSE;
   
   if (parent_group && *parent_group)
   {
      full_group = malloc(strlen("etk//") + strlen(parent_group) + strlen(group) + 1);
      sprintf(full_group, "etk/%s/%s", parent_group, group);
   }
   else
   {
      full_group = malloc(strlen("etk/") + strlen(group) + 1);
      sprintf(full_group, "etk/%s", group);
   }
   
   /* Checks if the theme-group exists */
   if (edje_file_group_exists(file, full_group))
   {
      free(full_group);
      return ETK_TRUE;
   }
   else
   {
      char *alias;
      char *alt_group;
      int ret;
      
      alias = malloc(strlen("alias: ") + strlen(full_group) + 1);
      sprintf(alias, "alias: %s", full_group);
      alt_group = edje_file_data_get(file, alias);
      free(full_group);
      free(alias);
      
      ret = edje_file_group_exists(file, alt_group);
      free(alt_group);
      
      return ret;
   }
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
   char *full_group;
   
   if (!object)
      return ETK_FALSE;
   
   if (!file)
      file = _etk_theme_widget_current ? _etk_theme_widget_current : _etk_theme_widget_default;
   if (!group || *group == '\0' || !file)
   {
      edje_object_file_set(object, NULL, NULL);
      return ETK_FALSE;
   }
   
   if (parent_group && *parent_group)
   {
      full_group = malloc(strlen("etk//") + strlen(parent_group) + strlen(group) + 1);
      sprintf(full_group, "etk/%s/%s", parent_group, group);
   }
   else
   {
      full_group = malloc(strlen("etk/") + strlen(group) + 1);
      sprintf(full_group, "etk/%s", group);
   }
   
   /* Load the object */
   if (edje_object_file_set(object, file, full_group))
   {
      free(full_group);
      return ETK_TRUE;
   }
   else
   {
      char *alias;
      char *alt_group;
      int ret;
      
      alias = malloc(strlen("alias: ") + strlen(full_group) + 1);
      sprintf(alias, "alias: %s", full_group);
      alt_group = edje_file_data_get(file, alias);
      free(full_group);
      free(alias);
      
      ret = edje_object_file_set(object, file, alt_group);
      free(alt_group);
      
      if (ret)
         return ETK_TRUE;
      else
      {
         edje_object_file_set(object, NULL, NULL);
         return ETK_FALSE;
      }
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
   return etk_theme_edje_object_set(object, etk_widget_theme_file_get(parent), group, etk_widget_theme_group_get(parent));
}

/**
 * @brief Gets the color type from the theme-file.
 * @param color_type an Etk_Color_Type
 * @return Returns an Etk_Color with the color filled from the theme-file, or color with -1 if it wasn't found
 */
Etk_Color etk_theme_color_get(Etk_Color_Type color_type)
{
   Etk_Color color;
   const char *file;
   char *color_string;

   file = _etk_theme_widget_current ? _etk_theme_widget_current : _etk_theme_widget_default;
   color_string = edje_file_data_get(file, _etk_theme_colors[color_type]);
   if (!color_string || sscanf(color_string, "%d %d %d %d", &color.r, &color.g, &color.b, &color.a) != 4)
   {
      Etk_Color color = { .r = -1, .g = -1, .b = -1, .a = -1 };
      if (color_string) free(color_string);
      return color;
   }

   if (color_string) free(color_string);
   return color;
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
      path = malloc(strlen(home) + strlen(subdir) + strlen("/.e/etk//.edj") + strlen(theme_name) + 1);
      sprintf(path, "%s/.e/etk/%s/%s.edj", home, subdir, theme_name);
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
