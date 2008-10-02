#ifndef _ENNA_CONFIG_H_
#define _ENNA_CONFIG_H_

typedef struct _Enna_Config Enna_Config;
typedef struct _Enna_Config_Data Enna_Config_Data;

typedef enum _ENNA_CONFIG_TYPE ENNA_CONFIG_TYPE;

typedef struct _Config_Pair Config_Pair;

enum _ENNA_CONFIG_TYPE
{
    ENNA_CONFIG_STRING,
    ENNA_CONFIG_STRING_LIST,
    ENNA_CONFIG_INT
};

struct _Enna_Config
{
    /* Theme */
    const char *theme;
    const char *theme_file;
    int fullscreen;
    int use_covers;
    int use_snapshots;
    const char *engine;
    const char *backend;
    const char *verbosity;
    /* Module Music */
    Evas_List *music_local_root_directories;
    Evas_List *music_filters;
    Evas_List *video_filters;
    Evas_List *photo_filters;
};

struct _Enna_Config_Data
{
    char *section;
    Evas_List *pair;
};

struct _Config_Pair
{
    char *key;
    char *value;
};

EAPI Enna_Config *enna_config;

EAPI const char *enna_config_theme_get(void);
EAPI const char *enna_config_theme_file_get(const char *s);
EAPI void enna_config_value_store(void *var, char *section,
        ENNA_CONFIG_TYPE type, Config_Pair *pair);
EAPI Enna_Config_Data *enna_config_module_pair_get(const char *module_name);
EAPI void enna_config_init(void);
EAPI void enna_config_shutdown(void);
#endif
