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

/** @file etk_config.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_config.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include <Ecore_File.h>
#include <Evas.h>

#include "etk_utils.h"

/**
 * @addtogroup Etk_Config
 * @{
 */

typedef struct _Etk_Config_General
{
   char *wm_theme;
   char *widget_theme;
   char *font;
   char *engine;
} Etk_Config_General;

typedef struct _Etk_Config_Version
{
   int major;
   int minor;
   int patch;
   int subpatch;
} Etk_Config_Version;

typedef struct _Etk_Config
{
   Etk_Config_General *general;
   Etk_Config_Version *version;
} Etk_Config;

#define NEWD(str, typ) \
   eet_data_descriptor_new(str, sizeof(typ), \
			      (void *(*) (void *))eina_list_next, \
			      (void *(*) (void *, void *))eina_list_append, \
			      (void *(*) (void *))eina_list_data_get, \
			      (void *(*) (void *))eina_list_free, \
			      (void  (*) (const Eina_Hash *, Eina_Bool (*) (const Eina_Hash *, const void *, void *, void *), const void *))eina_hash_foreach, \
			      (Eina_Bool (*) (const Eina_Hash *, const void *, void *))eina_hash_add, \
			      (void  (*) (Eina_Hash *))eina_hash_free)

#define FREED(eed) \
       if (eed) \
	   { \
	      eet_data_descriptor_free((eed)); \
	      (eed) = NULL; \
	   }

#define CFG_GEN_NEWI(str, it, type) EET_DATA_DESCRIPTOR_ADD_BASIC(_etk_config_gen_edd, Etk_Config_General, str, it, type)
#define CFG_VER_NEWI(str, it, type) EET_DATA_DESCRIPTOR_ADD_BASIC(_etk_config_ver_edd, Etk_Config_Version, str, it, type)

static Eet_Data_Descriptor *_etk_config_gen_edd = NULL;
static Eet_Data_Descriptor *_etk_config_ver_edd = NULL;
static Etk_Config *_etk_config = NULL;

static void _etk_config_defaults_apply();
static int _etk_config_version_compare(Etk_Config_Version *v1, Etk_Config_Version *v2);
static Etk_Config_Version * _etk_config_version_parse(char *version);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Initializes the config system
 * @return Returns true if initialization was successful, false otherwise
 */
Etk_Bool etk_config_init(void)
{
   char     *home;
   char      buf[PATH_MAX];

   home = getenv("HOME");
   if (!home)
   {
      ETK_WARNING("Cant find home directory!");
      return ETK_FALSE;
   }

   /* make sure ~/.e exists and is a dir */
   snprintf(buf, sizeof(buf), "%s/.e", home);
   if (!ecore_file_is_dir(buf))
   {
      if (ecore_file_exists(buf))
      {
	 ETK_WARNING("Cant create config path!");
	 return ETK_FALSE;
      }

      if (!ecore_file_mkdir(buf))
      {
	 ETK_WARNING("Cant create config path!");
	 return ETK_FALSE;
      }
   }

   /* make sure ~/.e/etk exists and is a dir */
   snprintf(buf, sizeof(buf), "%s/.e/etk", home);
   if (!ecore_file_is_dir(buf))
   {
      if (ecore_file_exists(buf))
      {
	 ETK_WARNING("Cant create config path!");
	 return ETK_FALSE;
      }

      if (!ecore_file_mkdir(buf))
      {
	 ETK_WARNING("Cant create config path!");
	 return ETK_FALSE;
      }
   }

   _etk_config_gen_edd = NEWD("Etk_Config_General", Etk_Config_General);
   CFG_GEN_NEWI("wmt", wm_theme, EET_T_STRING);
   CFG_GEN_NEWI("wt", widget_theme, EET_T_STRING);
   CFG_GEN_NEWI("fn", font, EET_T_STRING);
   CFG_GEN_NEWI("en", engine, EET_T_STRING);

   _etk_config_ver_edd = NEWD("Etk_Config_Version", Etk_Config_Version);
   CFG_VER_NEWI("mj", major, EET_T_INT);
   CFG_VER_NEWI("mn", minor, EET_T_INT);
   CFG_VER_NEWI("pa", patch, EET_T_INT);
   CFG_VER_NEWI("sp", subpatch, EET_T_INT);

   return ETK_TRUE;
}

/**
 * @brief Shutdowns the config system
 */
void etk_config_shutdown(void)
{
   FREED(_etk_config_gen_edd);
   FREED(_etk_config_ver_edd);
   if (_etk_config)
   {
      free(_etk_config->version);
      if (_etk_config->general)
      {
         free(_etk_config->general->wm_theme);
         free(_etk_config->general->widget_theme);
         free(_etk_config->general->font);
         free(_etk_config->general->engine);
         free(_etk_config->general);
      }
      free(_etk_config);
   }
}

/**
 * @brief Loads Etk's config from disk
 * @return Returns ETK_TRUE on a successful load, ETK_FALSE otherwise.
 */
Etk_Bool etk_config_load(void)
{
   Eet_File *ef;
   char buf[PATH_MAX];
   char *home;

   home = getenv("HOME");
   if (!home)
     {
	_etk_config_defaults_apply();
	return ETK_FALSE;
     }

   snprintf(buf, sizeof(buf), "%s/.e/etk/config.eet", home);

   if (!ecore_file_exists(buf) || ecore_file_size(buf) == 0)
   {
      /* no saved config */
      _etk_config_defaults_apply();
      return ETK_FALSE;
   }

   ef = eet_open(buf, EET_FILE_MODE_READ);
   if (!ef)
   {
      ETK_WARNING("Cant open configuration file! Using program defaults.");
      return ETK_FALSE;
   }

   if (!_etk_config)
     _etk_config = malloc(sizeof(Etk_Config));
   else
   {
      free(_etk_config->version);
      if (_etk_config->general)
      {
	 free(_etk_config->general->wm_theme);
	 free(_etk_config->general->widget_theme);
	 free(_etk_config->general->font);
	 free(_etk_config->general->engine);
	 free(_etk_config->general);
      }
   }

   _etk_config->version = NULL;
   _etk_config->version = eet_data_read(ef, _etk_config_ver_edd, "config/version");
   if(!_etk_config->version)
     {
	ETK_WARNING("Incompatible configuration file! Creating new one.");
	eet_close(ef);
	_etk_config_defaults_apply();
	return ETK_FALSE;
     }
   else
     {
	Etk_Config_Version *v;

	v = _etk_config_version_parse(VERSION);
	if(_etk_config_version_compare(v, _etk_config->version) != 0)
	  {
	     ETK_WARNING("Your version / configuration of Etk is not valid!");
	     eet_close(ef);
	     free(v);
	     _etk_config_defaults_apply();
	     return ETK_FALSE;
	  }
	free(v);
     }

   _etk_config->general = eet_data_read(ef, _etk_config_gen_edd, "config/general");
   if (!_etk_config->general)
     _etk_config_defaults_apply();

   eet_close(ef);
   return ETK_TRUE;
}

/**
 * @brief Saves Etk's config to disk
 * @return Returns ETK_TRUE on a successful save, ETK_FALSE otherwise.
 */
Etk_Bool etk_config_save(void)
{
   Eet_File *ef;
   char buf[PATH_MAX];
   char *home;
   Etk_Bool ret;

   home = getenv("HOME");
   if (!home)
     return 0;

   snprintf(buf, sizeof(buf), "%s/.e/etk/config.eet", home);

   ef = eet_open(buf, EET_FILE_MODE_WRITE);
   if (!ef)
     return 0;

   ret = eet_data_write(ef, _etk_config_ver_edd, "config/version",
			_etk_config->version, 1);
   if (!ret)
     ETK_WARNING("Problem saving config!");

   ret = eet_data_write(ef, _etk_config_gen_edd, "config/general",
			_etk_config->general, 1);
   if (!ret)
     ETK_WARNING("Problem saving config/stickies!");

   eet_close(ef);
   return ret;
}

/**
 * @brief Get Etk's current wm theme
 * @return Returns the current wm theme used by Etk
 */
const char *etk_config_wm_theme_get(void)
{
   if (!_etk_config || !_etk_config->general)
     return NULL;
   return _etk_config->general->wm_theme;
}

/**
 * @brief Sets Etk's wm theme (will not be applied on the fly)
 * @param theme The wm theme file to use
 */
void etk_config_wm_theme_set(const char *wm_theme)
{
   if (!_etk_config || !_etk_config->general)
     return;

   /* TODO: make sure the theme exists */
   free(_etk_config->general->wm_theme);
   _etk_config->general->wm_theme = strdup(wm_theme);
}

/**
 * @brief Get Etk's current widget theme
 * @return Returns the current widget theme used by Etk
 */
const char *etk_config_widget_theme_get(void)
{
   if (!_etk_config || !_etk_config->general)
     return NULL;
   return _etk_config->general->widget_theme;
}

/**
 * @brief Sets Etk's widget theme (will not be applied on the fly)
 * @param theme The widget theme file to use
 */
void etk_config_widget_theme_set(const char *widget_theme)
{
   if (!_etk_config || !_etk_config->general)
     return;

   /* TODO: make sure the theme exists */
   free(_etk_config->general->widget_theme);
   _etk_config->general->widget_theme = strdup(widget_theme);
}

/**
 * @brief Get Etk's current font
 * @return Returns the current font used by Etk
 */
const char *etk_config_font_get(void)
{
   if (!_etk_config || !_etk_config->general)
     return NULL;
   return _etk_config->general->font;
}

/**
 * @brief Sets Etk's font (will not be applied on the fly)
 * @param font The font to use
 */
void etk_config_font_set(const char *font)
{
   if (!_etk_config || !_etk_config->general)
     return;

   /* TODO: make sure the font exists */
   free(_etk_config->general->font);
   _etk_config->general->font = strdup(font);
}

/**
 * @brief Get Etk's current engine
 * @return Returns the current engine used by Etk
 */
const char *etk_config_engine_get(void)
{
   if (!_etk_config || !_etk_config->general)
     return NULL;
   return _etk_config->general->engine;
}

/**
 * @brief Sets Etk's engine (will not be applied on the fly)
 * @param engine The engine to use
 */
void etk_config_engine_set(const char *engine)
{
   if (!_etk_config || !_etk_config->general)
     return;

   free(_etk_config->general->engine);
   _etk_config->general->engine = strdup(engine);
}

/* applies the default configuration */
static void _etk_config_defaults_apply(void)
{
   if (_etk_config)
   {
      free(_etk_config->version);
      if (_etk_config->general)
      {
	 free(_etk_config->general->wm_theme);
	 free(_etk_config->general->widget_theme);
	 free(_etk_config->general->font);
	 free(_etk_config->general->engine);
	 free(_etk_config->general);
      }
   }
   else
     _etk_config = malloc(sizeof(Etk_Config));

   _etk_config->version = _etk_config_version_parse(VERSION);
   _etk_config->general = malloc(sizeof(Etk_Config_General));
   _etk_config->general->wm_theme = strdup("default");
   _etk_config->general->widget_theme = strdup("default");
   _etk_config->general->font = strdup("Vera");
   _etk_config->general->engine = strdup("ecore_evas_software_x11");
}

/* compares 2 versions of the form major.minor.patch.subpatch */
static int _etk_config_version_compare(Etk_Config_Version *v1, Etk_Config_Version *v2)
{
   if (v1->major > v2->major)
     return 1;
   else if (v1->major < v2->major)
     return -1;

   if (v1->minor > v2->minor)
     return 1;
   else if (v1->minor < v2->minor)
     return -1;

   if (v1->patch > v2->patch)
     return 1;
   else if (v1->patch < v2->patch)
     return -1;

   if (v1->subpatch > v2->subpatch)
     return 1;
   else if (v1->subpatch < v2->subpatch)
     return -1;

   return 0;
}

/* parses a string of the form x.y.x.abc into an Etk_Config_Version struct */
static Etk_Config_Version *_etk_config_version_parse(char *version)
{
   Etk_Config_Version *v;
   int res;

   v = malloc(sizeof(Etk_Config_Version));
   res = sscanf(version, "%d.%d.%d.%d", &v->major, &v->minor, &v->patch, &v->subpatch);

   if (res < 4)
     return NULL;

   return v;
}
