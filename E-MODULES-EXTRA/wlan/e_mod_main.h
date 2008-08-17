#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

typedef struct _Config Config;
typedef struct _Config_Item Config_Item;

struct _Config 
{
   E_Module *module;
   E_Config_Dialog *config_dialog;
   E_Menu *menu;
   Evas_List *instances;
   Evas_List *items;
};

struct _Config_Item
{
   const char *id;
   
   const char *device;   
   double poll_time;
   int show_percent;
   int always_text;
   int real_ignore_buffers;
   int real_ignore_cached;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int   e_modapi_shutdown(E_Module *m);
EAPI int   e_modapi_save(E_Module *m);

void _wlan_config_updated(Config_Item *ci);
void _config_wlan_module(Config_Item *ci);
void _wlan_get_values(Config_Item *ci, int *real, int *swap, int *total_real, int *total_swap);
extern Config *wlan_config;

#endif
