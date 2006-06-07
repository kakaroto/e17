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
   Ecore_Event_Handler *exe_exit_handler;
};

struct _Config_Item 
{
   const char *id;

   unsigned char show_label;
   unsigned char use_exec;
   const char *exec;
   double check_time;
   
   int type;
   int port;
   const char *host;
   const char *user;
   const char *pass;
   const char *path;
   const char *path_current;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int   e_modapi_shutdown(E_Module *m);
EAPI int   e_modapi_save(E_Module *m);
EAPI int   e_modapi_about(E_Module *m);

void _config_mail_module(Config_Item *ci);
void _mail_config_updated(const char *id);
extern Config *mail_config;

#endif
