#include <e.h>
#include <EMenu.h>
#include "config.h"
#include "e_mod_main.h"

static EMenu *_emenu_init             (E_Module *m);
static void   _emenu_shutdown         (EMenu *em);
static void   _emenu_menu_add         (void *data, E_Menu *m);
static void   _emenu_menu_cb_generate (void *data, E_Menu *m, E_Menu_Item *mi);

EAPI E_Module_Api e_modapi = 
{
   E_MODULE_API_VERSION,
     "EMenu"
};

EAPI void *
e_modapi_init(E_Module *m) 
{
   EMenu *em;
   
   /* Init the module */
   em = _emenu_init(m);    
   return em;
}

EAPI int
e_modapi_shutdown(E_Module *m) 
{
   EMenu *em;
   
   em = m->data;
   if (!m) return 0;
      
   _emenu_shutdown(em);   
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

/* Disabled for now
EAPI int
e_modapi_config(E_Module *m) 
{
   return 0;
}
*/

static EMenu *
_emenu_init(E_Module *m) 
{
   EMenu *em;
   
   em = E_NEW(EMenu, 1);
   if (!em) return NULL;
   
   em->conf_edd = E_CONFIG_DD_NEW("EMenu_Config", Config);
   #undef T
   #undef D
   #define T Config
   #define D em->conf_edd
   
   em->conf = e_config_domain_load("module.emenu", em->conf_edd);
   if (!em->conf) 
     {
	em->conf = E_NEW(Config, 1);
     }
   else 
     {
	/* Handle Whatever config loading we need */
     }
   
   em->augment = e_int_menus_menu_augmentation_add("config", _emenu_menu_add, em, NULL, NULL);   
   return em;
}

static void
_emenu_shutdown(EMenu *em) 
{
   E_CONFIG_DD_FREE(em->conf_edd);
   if (em->augment) 
     e_int_menus_menu_augmentation_del("config", em->augment);
   free(em->conf);
   free(em);
}

static void
_emenu_menu_add(void *data, E_Menu *m) 
{
   EMenu *em;
   E_Menu_Item *mi;
   
   em = data;

   mi = e_menu_item_new(m);
   e_menu_item_label_set(mi, _("Generate Menus"));
   e_menu_item_callback_set(mi, _emenu_menu_cb_generate, em);
}

static void
_emenu_menu_cb_generate(void *data, E_Menu *m, E_Menu_Item *mi) 
{
   EMenu *em;
   
   em = data;
   e_module_dialog_show(_("Enlightenment Menu Module"),
			_("Generate Menus."));
   
   emenu_generate();
}

