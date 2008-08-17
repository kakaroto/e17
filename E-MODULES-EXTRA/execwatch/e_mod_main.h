#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

typedef struct _Config Config;
typedef struct _Config_Item Config_Item;

struct _Config 
{
   E_Module        *module;
   E_Config_Dialog *config_dialog;
   E_Menu          *menu;
   Evas_List       *instances;
   Evas_List       *items;
   Ecore_Event_Handler *exe_del;
};

struct _Config_Item 
{
   const char *id;

   const char *display_name;
   const char *status_cmd;
   const char *dblclk_cmd;
   const char *okstate_string;
   int okstate_exitcode;
   int okstate_lines;
   int okstate_mode;
   int refresh_after_dblclk_cmd;
   double poll_time_mins;
   double poll_time_hours;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);

void _config_execwatch_module(Config_Item *ci);
void _execwatch_config_updated(Config_Item *ci);

extern Config *execwatch_config;

#endif
