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

#define RESOLUTION_MINUTE 0
#define RESOLUTION_SECOND 1

typedef struct _Config Config;
typedef struct _Config_Item Config_Item;

struct _Config
{
  E_Module *module;
  E_Config_Dialog *config_dialog;
  E_Menu *menu;
  Eina_List *instances;
  Eina_List *items;
};

struct _Config_Item
{
  const char *id;
  const char *player;
  int show_popup;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init (E_Module * m);
EAPI int e_modapi_shutdown (E_Module * m);
EAPI int e_modapi_save (E_Module * m);

void _config_empris_module (Config_Item * ci);
void _empris_config_updated (Config_Item * ci);
extern Config *empris_config;

#endif
