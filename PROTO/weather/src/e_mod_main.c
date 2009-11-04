#include <e.h>
#include "config.h"
#include "e_mod_config.h"
#include "e_mod_main.h"
#include "e_mod_gadcon.h"

/* local variables */
static E_Config_DD *conf_edd = NULL;
static E_Config_DD *item_edd = NULL;
Config *weather_cfg = NULL;

EAPI E_Module_Api e_modapi = { E_MODULE_API_VERSION, "Weather" };

/* public functions */
EAPI void *
e_modapi_init(E_Module *m) 
{
#ifdef ENABLE_NLS
   char buff[PATH_MAX];

   snprintf(buff, PATH_MAX, "%s/locale", m->dir);
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");
#endif

   item_edd = E_CONFIG_DD_NEW("Config_Item", Config_Item);
   E_CONFIG_VAL(item_edd, Config_Item, id, STR);

   conf_edd = E_CONFIG_DD_NEW("Config", Config);
   E_CONFIG_LIST(conf_edd, Config, items, item_edd);

   weather_cfg = e_config_domain_load("module.weather", conf_edd);
   if (!weather_cfg) 
     {
        Config_Item *ci;

        weather_cfg = E_NEW(Config, 1);
        ci = _weather_config_item_get("0");
     }
   weather_cfg->mod_dir = eina_stringshare_add(m->dir);

   _gc_register();
   return m;
}

EAPI int 
e_modapi_shutdown(E_Module *m) 
{
   _gc_unregister();
   while (weather_cfg->items) 
     {
        Config_Item *ci;

        ci = weather_cfg->items->data;
        weather_cfg->items = 
          eina_list_remove_list(weather_cfg->items, weather_cfg->items);
        E_FREE(ci);
     }

   if (weather_cfg->mod_dir) eina_stringshare_del(weather_cfg->mod_dir);
   weather_cfg->mod_dir = NULL;

   E_FREE(weather_cfg);
   E_CONFIG_DD_FREE(item_edd);
   E_CONFIG_DD_FREE(conf_edd);
   return 1;
}

EAPI int 
e_modapi_save(E_Module *m) 
{
   e_config_domain_save("module.weather", conf_edd, weather_cfg);
   return 1;
}
