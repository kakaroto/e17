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

   double delay_time;
   unsigned char use_import;
   unsigned char use_scrot;
   const char *location;
   const char *filename;
   unsigned char use_app;
   const char *app;
   struct
   {
      unsigned char use_img_border;
      unsigned char use_dither;
      unsigned char use_frame;
      unsigned char use_mono;
      unsigned char use_window;
      unsigned char use_silent;
      unsigned char use_trim;
   } import;

   struct
   {
      unsigned char use_img_border;
      unsigned char use_thumb;
   } scrot;
};

EAPI extern E_Module_Api e_modapi;

EAPI int e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);
EAPI int e_modapi_about(E_Module *m);

void _config_ss_module(Config_Item *ci);
extern Config *ss_config;

#endif
