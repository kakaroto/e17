#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

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
  double poll_time;
  const char *hostname;
  int port;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init (E_Module * m);
EAPI int e_modapi_shutdown (E_Module * m);
EAPI int e_modapi_save (E_Module * m);

void _config_mpdule_module (Config_Item * ci);
void _mpdule_config_updated (Config_Item * ci);
extern Config *mpdule_config;

#endif
