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

   E_Config_Binding_Key	decrease_vol_key;
   E_Config_Binding_Key	increase_vol_key;
   E_Config_Binding_Key	mute_key;
};

struct _Config_Item
{
   const char *id;
   
   int card_id;
   int channel_id;
   int mode;
	
   const char *app;
   int         use_app;
      
   int    show_popup;
   double popup_speed;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);

void _config_mixer_module    (void *data, Config_Item *ci);

extern Config *mixer_config;

#endif
