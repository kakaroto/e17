#ifndef E_MOD_CONFIG_H
# define E_MOD_CONFIG_H

typedef struct _Config Config;
typedef struct _Config_Item Config_Item;

struct _Config 
{
   Eina_List *instances, *items;
   const char *mod_dir;
};
struct _Config_Item 
{
   const char *id;
};

EAPI Config_Item *_weather_config_item_get(const char *id);
EAPI void _weather_config_updated(Config_Item *ci);

extern Config *weather_cfg;

#endif
