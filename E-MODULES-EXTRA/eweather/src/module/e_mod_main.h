#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
# define E_MOD_MAIN_H

# include "EWeather_Smart.h"

typedef struct _Instance Instance;

struct _Instance 
{
   E_Gadcon_Client *gcc;
   Config_Item *ci;
   Evas_Object *obj;
   E_Menu *menu;
   EWeather *eweather;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);

#endif
