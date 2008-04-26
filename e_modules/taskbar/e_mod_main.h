/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

typedef struct _Config Config;
typedef struct _Config_Item Config_Item;

struct _Config
{
   E_Module *module;
   Evas_List *taskbars;
   Evas_List *borders;
   Evas_List *items;
   E_Menu *menu;
   Evas_List *handlers;
   E_Config_Dialog *config_dialog;
};

struct _Config_Item
{
   const char *id;
   int show_all;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);

void _config_taskbar_module(Config_Item *ci);
void _taskbar_config_updated(Config_Item *ci);
extern Config *taskbar_config;

#endif
