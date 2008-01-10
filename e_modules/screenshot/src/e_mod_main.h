#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#define MOD_CONFIG_FILE_EPOCH 0x0001
#define MOD_CONFIG_FILE_GENERATION 0x008d
#define MOD_CONFIG_FILE_VERSION \
   ((MOD_CONFIG_FILE_EPOCH << 16) | MOD_CONFIG_FILE_GENERATION)

typedef struct _Config Config;

struct _Config 
{
   int version;
   int quality, thumb_size, mode;
   double delay;
   unsigned char prompt, use_app, use_bell;
   unsigned char use_thumb;
   const char *location, *filename, *app;
   E_Config_Dialog *cfd;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);

EAPI E_Config_Dialog *e_int_config_screenshot_module(E_Container *con, const char *params __UNUSED__);

extern Config *ss_cfg;
extern E_Module *ss_mod;

#endif
