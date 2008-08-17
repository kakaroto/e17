#ifndef ENNA_CONFIG_H
#define ENNA_CONFIG_H

#include <Ecore.h>
#include <Ecore_Data.h>
#include <Ecore_File.h>
#include <Evas.h>
#include "enna.h"
typedef struct _Enna_Config Enna_Config;

struct conf_pair
{
   char               *key;
   char               *value;
   struct conf_pair   *next_pair;
};

struct conf_section
{
   char               *section_name;
   struct conf_pair   *values;
   struct conf_section *next_section;
};

struct _Enna_Config
{
   char               *theme;
   Evas_List          *music_extensions;
   Evas_List          *video_extensions;
   Evas_List          *photo_extensions;
   Evas_List          *radio_extensions;
   struct conf_section *sections;
};

EAPI void           enna_config_init(char *config_filename, char *theme_name);

EAPI char          *enna_config_theme_get(void);
EAPI void           enna_config_theme_set(char *theme_name);
EAPI Evas_List     *enna_config_theme_available_get(void);

EAPI Evas_List     *enna_config_extensions_get(char *type);
EAPI void           enna_config_extensions_set(char *type, Evas_List * ext);

EAPI char          *enna_config_get_conf_value(char *section_name,
					       char *key_name);

#define enna_config_get_conf_value_or_default(section_name, key_name, def) (enna_config_get_conf_value(section_name, key_name) ? enna_config_get_conf_value(section_name, key_name) : def)

#endif
