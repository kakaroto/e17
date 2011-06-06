#ifndef E_MOD_MAIN_H
# define E_MOD_MAIN_H

#ifdef ENABLE_NLS
# include <libintl.h>
# define D_(string) dgettext(PACKAGE, string)
#else
# define bindtextdomain(domain,dir)
# define bind_textdomain_codeset(domain,codeset)
# define D_(string) (string)
#endif

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
