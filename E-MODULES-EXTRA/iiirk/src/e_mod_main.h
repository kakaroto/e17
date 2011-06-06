#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#ifdef ENABLE_NLS
# include <libintl.h>
# define D_(string) dgettext(PACKAGE, string)
#else
# define bindtextdomain(domain,dir)
# define bind_textdomain_codeset(domain,codeset)
# define D_(string) (string)
#endif

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
