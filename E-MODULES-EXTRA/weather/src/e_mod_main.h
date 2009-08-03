#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#define SIMPLE_DISPLAY 0
#define DETAILED_DISPLAY 1
#define DEGREES_F 0
#define DEGREES_C 1

typedef struct _Config Config;
typedef struct _Config_Item Config_Item;

struct _Config
{
   E_Module *module;
   E_Config_Dialog *config_dialog;
   Eina_List *instances;
   Eina_List *items;
   E_Menu *menu;
};

struct _Config_Item
{
   const char *id;

   double poll_time;
   int display, degrees;
   const char *host, *code;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int   e_modapi_shutdown(E_Module *m);
EAPI int   e_modapi_save(E_Module *m);

void _config_weather_module(Config_Item *ci);
void _weather_config_updated(Config_Item *ci);
void _weather_convert_degrees(void *data);

extern Config *weather_config;

#endif
