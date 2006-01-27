#include <e.h>
#include "config.h"
#include "e_mod_main.h"

static EMenu *_emenu_init (E_Module *m);

EAPI E_Module_Api e_modapi = 
{
   E_MODULE_API_VERSION,
     "EMenu"
};

EAPI void *
e_modapi_init(E_Module *m) 
{
   EMenu *em;
   
   /* Call the module init here
   em = _emenu_init(m); */
   
   m->config_menu = NULL;
   return em;
}

EAPI int
e_modapi_shutdown(E_Module *m) 
{
   EMenu *em;
   
   em = m->data;
   if (!m) return 0;
   
   if (m->config_menu) 
     {
	e_menu_deactivate(m->config_menu);
	e_object_del(E_OBJECT(m->config_menu));
	m->config_menu = NULL;
     }
   
   /* Call the module shutdown here */
   return 1;
}

EAPI int
e_modapi_save(E_Module *m) 
{
   EMenu *em;
   
   em = m->data;
   if (!em) return 0;
   e_config_domain_save("module.emenu", em->conf_edd, em->conf);
   return 1;
}

EAPI int
e_modapi_info(E_Module *m) 
{
   m->icon_file = strdup(PACKAGE_DATA_DIR"/module_icon.png");
   return 1;
}

EAPI int
e_modapi_about(E_Module *m) 
{
   e_module_dialog_show(_("Enlightenment Menu Module"),
			_("A module to create menus."));
   return 1;
}

EAPI int
e_modapi_config(E_Module *m) 
{
   return 0;
}
