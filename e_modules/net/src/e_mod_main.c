#include <e.h>
#include "e_mod_main.h"
#include "e_mod_gadcon.h"
#include "e_mod_config.h"

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *item_edd = NULL;
Config *cfg = NULL;

EAPI E_Module_Api e_modapi = 
{
   E_MODULE_API_VERSION, "Net"
};

EAPI void *
e_modapi_init(E_Module *m) 
{
   item_edd = E_CONFIG_DD_NEW("Config_Item", Config_Item);
   E_CONFIG_VAL(item_edd, Config_Item, id, STR);
   E_CONFIG_VAL(item_edd, Config_Item, device, STR);
   E_CONFIG_VAL(item_edd, Config_Item, limit, INT);
   
   conf_edd = E_CONFIG_DD_NEW("Config", Config);
   E_CONFIG_LIST(conf_edd, Config, items, item_edd);

   cfg = e_config_domain_load("module.net", conf_edd);
   if (!cfg) 
     {
	Config_Item *ci;
	
	cfg = E_NEW(Config, 1);
	ci = _config_item_get("0");
     }
   cfg->mod = m;
   _gc_register();
   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m) 
{
   cfg->mod = NULL;
   _gc_unregister();
   
   if (cfg->cfd) e_object_del(E_OBJECT(cfg->cfd));
   if (cfg->menu) 
     {
	e_menu_post_deactivate_callback_set(cfg->menu, NULL, NULL);
	e_object_del(E_OBJECT(cfg->menu));
	cfg->menu = NULL;
     }
   
   while (cfg->items) 
     {
	Config_Item *ci;
	
	ci = cfg->items->data;
	if (ci->id) evas_stringshare_del(ci->id);
	if (ci->device) evas_stringshare_del(ci->device);
	cfg->items = evas_list_remove_list(cfg->items, cfg->items);
	E_FREE(ci);
     }
   
   E_FREE(cfg);
   E_CONFIG_DD_FREE(item_edd);
   E_CONFIG_DD_FREE(conf_edd);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m) 
{
   Evas_List *l;
   
   for (l = cfg->instances; l; l = l->next) 
     {
	Instance *inst;
	Config_Item *ci;
	
	inst = l->data;
	ci = _config_item_get(inst->gcc->id);
	if (ci->id) evas_stringshare_del(ci->id);
	ci->id = evas_stringshare_add(inst->gcc->id);
     }
   
   e_config_domain_save("module.net", conf_edd, cfg);
   return 1;
}

EAPI int
e_modapi_about(E_Module *m) 
{
   e_module_dialog_show(m, _("Network Monitor Module"), 
			_("Module to monitor network traffic"));
   return 1;
}
