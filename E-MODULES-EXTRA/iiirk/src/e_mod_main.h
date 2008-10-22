/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#undef  D_
#define D_(str) dgettext(PACKAGE, str)

#undef  MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

typedef struct _Config      Config;
typedef struct _Config_Item Config_Item;

struct _Config
{
   /* just config state */
   E_Module        *module;
   Eina_List       *instances;
   E_Menu          *menu;
   Eina_List       *handlers;
   Eina_List       *items;
   Eina_List	   *config_dialog;
};

struct _Config_Item 
{
   const char *id;
   const char *dir;
   int hide_window;
   int show_label;
   int show_zone;
   int show_desk;
   int icon_label;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);

void iiirk_create_default_profile(const char *text);

void _iiirk_config_update(Config_Item *ci);
void _config_iiirk_module(Config_Item *ci);
extern Config *iiirk_config;

#endif
