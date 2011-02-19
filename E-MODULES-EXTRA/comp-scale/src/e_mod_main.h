#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#define MOD_CONFIG_FILE_EPOCH 0x0003
#define MOD_CONFIG_FILE_GENERATION 0x0001
#define MOD_CONFIG_FILE_VERSION					\
  ((MOD_CONFIG_FILE_EPOCH << 16) | MOD_CONFIG_FILE_GENERATION)


#include <libintl.h>
#define D_(str) dgettext(PACKAGE, str)

typedef struct _Config_Item Config_Item;
typedef struct _Config Config;

struct _Config
{
  E_Module	*module;
  Eina_List	*conf_items;
  int		 version;

  unsigned char tight;
  unsigned char grow;

  double	scale_duration;

  double	spacing;

  E_Config_Dialog	*cfd;
};


struct _Config_Item
{
  const char *id;
  int switch2;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);


E_Config_Dialog *e_int_config_scale_module(E_Container *con, const char *params);

extern Config *scale_conf;

#endif
