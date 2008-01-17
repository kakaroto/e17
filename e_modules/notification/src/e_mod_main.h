#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#include "config.h"

#define MOD_CFG_FILE_EPOCH 0x0001
#define MOD_CFG_FILE_GENERATION 0x0001
#define MOD_CFG_FILE_VERSION \
   ((MOD_CFG_FILE_EPOCH << 16) | MOD_CFG_FILE_GENERATION)

#undef __UNUSED__
#define __UNUSED__ __attribute__((unused))

typedef enum _Popup_Direction Popup_Direction;
enum _Popup_Direction
{
  DIRECTION_UP,
  DIRECTION_DOWN,
  DIRECTION_LEFT,
  DIRECTION_RIGHT
};

typedef struct _Config Config;
struct _Config 
{
   E_Config_Dialog *cfd;

   int version;
   Popup_Direction direction;
   int gap;
   struct 
     {
        int x;
        int y;
     } placement;
};

EAPI extern E_Module_Api e_modapi;

EAPI void  *e_modapi_init(E_Module *m);
EAPI int    e_modapi_shutdown(E_Module *m);
EAPI int    e_modapi_save(E_Module *m);

EAPI E_Config_Dialog *e_int_config_notification_module(E_Container *con, 
                                                       const char *params __UNUSED__);

extern E_Module *notification_mod;
extern Config   *notification_cfg;

#endif
