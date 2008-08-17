#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#include "config.h"

#define MOD_CFG_FILE_EPOCH 0x0001
#define MOD_CFG_FILE_GENERATION 0x008d
#define MOD_CFG_FILE_VERSION \
   ((MOD_CFG_FILE_EPOCH << 16) | MOD_CFG_FILE_GENERATION)

typedef struct _Config Config;
struct _Config 
{
   E_Config_Dialog *cfd;

   int version;
   int lock_sliders;
   struct 
     {
        int id;
     } card;
   struct 
     {
        int id;
        const char *name;
     } channel;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);

EAPI E_Config_Dialog *e_int_config_echo_module(E_Container *con, const char *params __UNUSED__);

EAPI void _echo_cb_config_updated(void);

extern Config *echo_cfg;
extern E_Module *echo_mod;

#endif
