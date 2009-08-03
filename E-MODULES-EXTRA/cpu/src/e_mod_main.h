#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

typedef struct _Config Config;
typedef struct _Config_Item Config_Item;

struct _Config 
{
   E_Module        *module;
   E_Config_Dialog *config_dialog;
   E_Menu          *menu, *menu_interval;
   Eina_List       *instances;
   Eina_List       *items;
};

struct _Config_Item 
{
   const char *id;
   double interval;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);

//void _config_cpu_module      (Config_Item *ci);

extern Config *cpu_conf;

#endif
