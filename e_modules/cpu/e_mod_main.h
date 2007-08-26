#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

typedef struct _Config Config;
typedef struct _Config_Item Config_Item;

struct _Config 
{
   E_Module        *module;
   E_Config_Dialog *config_dialog;
   E_Menu          *menu, *menu_interval;
   Evas_List       *instances;
   Evas_List       *items;
};

struct _Config_Item 
{
   const char *id;
   double poll_time;
   int show_popup;
   int max_points;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);
EAPI int   e_modapi_about    (E_Module *m);

void _cpu_config_updated     (const char *id);
void _config_cpu_module      (Config_Item *ci, int max);

extern Config *cpu_conf;

#endif
