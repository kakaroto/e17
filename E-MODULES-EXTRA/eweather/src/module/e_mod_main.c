#include <e.h>
#include "config.h"
#include "e_mod_config.h"
#include "e_mod_main.h"
#include "e_mod_gadcon.h"

/* local function variables */
static Eina_Bool _cb_cfg_timer(void *data);

/* local variables */
static E_Config_DD *conf_edd = NULL;
static E_Config_DD *item_edd = NULL;
Config *weather_cfg = NULL;

EAPI E_Module_Api e_modapi = { E_MODULE_API_VERSION, "EWeather" };

/* public functions */
void *
e_modapi_init(E_Module *m) 
{
#ifdef ENABLE_NLS
   char buff[PATH_MAX];

   snprintf(buff, sizeof(buff), "%s/locale", m->dir);
   bindtextdomain(PACKAGE, buff);
   bind_textdomain_codeset(PACKAGE, "UTF-8");
#endif

   item_edd = E_CONFIG_DD_NEW("Config_Item", Config_Item);
   E_CONFIG_VAL(item_edd, Config_Item, id, STR);
   E_CONFIG_VAL(item_edd, Config_Item, celcius, INT);
   E_CONFIG_VAL(item_edd, Config_Item, location, STR);
   E_CONFIG_VAL(item_edd, Config_Item, google, STR);
   E_CONFIG_VAL(item_edd, Config_Item, plugin, STR);
   E_CONFIG_VAL(item_edd, Config_Item, poll_time, DOUBLE);

   conf_edd = E_CONFIG_DD_NEW("Config", Config);
   E_CONFIG_LIST(conf_edd, Config, items, item_edd);

   weather_cfg = e_config_domain_load("module.eweather", conf_edd);
   
   if (weather_cfg) 
     {
        if ((weather_cfg->version >> 16) < MOD_CONFIG_FILE_EPOCH) 
          {
             /* config too old */
             //_weather_config_free();
	     ecore_timer_add(1.0, _cb_cfg_timer,
			     D_("Weather Module Configuration data needed "
                                "upgrading. Your old configuration<br> has been"
                                " wiped and a new set of defaults initialized. "
                                "This<br>will happen regularly during "
                                "development, so don't report a<br>bug. "
                                "This simply means Weather module needs "
                                "new configuration<br>data by default for "
                                "usable functionality that your old<br>"
                                "configuration simply lacks. This new set of "
                                "defaults will fix<br>that by adding it in. "
                                "You can re-configure things now to your<br>"
                                "liking. Sorry for the inconvenience.<br>"));
          }
        else if (weather_cfg->version > MOD_CONFIG_FILE_VERSION) 
          {
             /* config too new */
             _weather_config_free();
	     ecore_timer_add(1.0, _cb_cfg_timer, 
			     D_("Your Weather Module configuration is NEWER "
                                "than the Weather Module version. This is "
                                "very<br>strange. This should not happen unless"
                                " you downgraded<br>the Weather Module or "
                                "copied the configuration from a place where"
                                "<br>a newer version of the Weather Module "
                                "was running. This is bad and<br>as a "
                                "precaution your configuration has been now "
                                "restored to<br>defaults. Sorry for the "
                                "inconvenience.<br>"));
          }
     }

   if (!weather_cfg) _weather_config_new();

   weather_cfg->mod_dir = eina_stringshare_add(m->dir);

   _gc_register();
   return m;
}

int 
e_modapi_shutdown(E_Module *m) 
{
   /* unregister from gadcon */
   _gc_unregister();

   while (weather_cfg->items) 
     {
        Config_Item *ci;

        ci = weather_cfg->items->data;
        weather_cfg->items = 
          eina_list_remove_list(weather_cfg->items, weather_cfg->items);
        E_FREE(ci);
     }

   /* module is exiting, we don't need the module directory anymore */
   if (weather_cfg->mod_dir) eina_stringshare_del(weather_cfg->mod_dir);
   weather_cfg->mod_dir = NULL;

   /* free the config structure */
   _weather_config_free();

   /* free the edd's */
   E_CONFIG_DD_FREE(item_edd);
   E_CONFIG_DD_FREE(conf_edd);
   return 1;
}

int 
e_modapi_save(E_Module *m) 
{
   e_config_domain_save("module.eweather", conf_edd, weather_cfg);
   return 1;
}

/* local functions */
static Eina_Bool 
_cb_cfg_timer(void *data) 
{
    //function not found
   //e_util_dialog_show_internal(D_("Weather Configuration Updated"), data);
   return EINA_FALSE;
}
