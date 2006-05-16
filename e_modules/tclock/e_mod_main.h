#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#define RESOLUTION_MINUTE 0
#define RESOLUTION_SECOND 1

typedef struct _Config Config;
struct _Config
{
   E_Module *module;
   E_Config_Dialog *config_dialog;
   E_Menu *menu;
   Evas_List *instances;

   Ecore_Timer *tclock_check_timer;
   
   int resolution;
   double poll_time;
   int show_time;
   int show_date;
   char *time_format;
   char *date_format;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);
EAPI int e_modapi_info(E_Module *m);
EAPI int e_modapi_about(E_Module *m);
EAPI int e_modapi_config(E_Module *m);

void _config_tclock_module(void);
void _tclock_config_updated(void);
extern Config *tclock_config;

#endif
