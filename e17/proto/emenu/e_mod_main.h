#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

typedef struct _Config Config;
struct _Config 
{
   
};

typedef struct _EMenu EMenu;
struct _EMenu 
{
   E_Menu *config_menu;
   
   E_Config_DD *conf_edd;
   Config *conf;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);
EAPI int   e_modapi_info     (E_Module *m);
EAPI int   e_modapi_about    (E_Module *m);
EAPI int   e_modapi_config   (E_Module *m);

#endif
