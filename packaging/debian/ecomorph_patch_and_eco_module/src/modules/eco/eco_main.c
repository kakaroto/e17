/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "e.h"
#include "eco_main.h"

/***************************************************************************/
/**/
/* actual module specifics */

static void  _e_mod_run_cb(void *data, E_Menu *m, E_Menu_Item *mi);
static void  _e_mod_menu_add(void *data, E_Menu *m);


static E_Module *conf_module = NULL;
static E_Int_Menu_Augmentation *maug = NULL;

/*static char *eco_config_filename = NULL;
 * static Eet_File *eco_config_file = NULL; */
Eet_Data_Descriptor *eco_edd_group, *eco_edd_option;
/**/
/***************************************************************************/

/***************************************************************************/
/**/


/**/
/***************************************************************************/

/***************************************************************************/
/**/
/* module setup */
EAPI E_Module_Api e_modapi =
{
   E_MODULE_API_VERSION,
     "Ecomorph"
};

static Eina_Hash*
eet_eina_hash_add(Eina_Hash *hash, const char *key, const void *data)
{
  if (!hash) hash = eina_hash_string_superfast_new(NULL);
  if (!hash) return NULL;

  eina_hash_add(hash, key, data);
  return hash;
}

EAPI void *
e_modapi_init(E_Module *m)
{
   char buf[PATH_MAX];

   snprintf(buf, sizeof(buf), "%s/e-module-eco.edj", e_module_dir_get(m));
   edje_file = eina_stringshare_add(buf);

   e_configure_registry_category_add("appearance", 10, _("Look"),
                                     NULL, "enlightenment/appearance");
   e_configure_registry_item_add("appearance/eco", 50, _("Ecomorph"),
                                 NULL, buf, e_int_config_eco);


   maug = e_int_menus_menu_augmentation_add("config/1", _e_mod_menu_add, NULL, NULL, NULL);
   
   if (evil)
     {
	eco_actions_create();
	eco_event_init();
	eco_border_init();
     }

   eco_edd_group = eet_data_descriptor_new("group", sizeof(Eco_Group),
				       NULL, NULL, NULL, NULL,
				       (void  (*) (void *, int (*) (void *, const char *, void *, void *), void *))eina_hash_foreach,
				       (void *(*) (void *, const char *, void *))eet_eina_hash_add,
				       (void  (*) (void *))eina_hash_free);

   eco_edd_option = eet_data_descriptor_new("option", sizeof(Eco_Option),
					(void *(*) (void *))eina_list_next,
					(void *(*) (void *, void *)) eina_list_append,
					(void *(*) (void *))eina_list_data_get,
					(void *(*) (void *))eina_list_free,
					NULL, NULL, NULL);

   EET_DATA_DESCRIPTOR_ADD_BASIC(eco_edd_option, Eco_Option, "type",	 type,		  EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(eco_edd_option, Eco_Option, "int",	 intValue,	  EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(eco_edd_option, Eco_Option, "double",	 doubleValue, EET_T_DOUBLE);
   EET_DATA_DESCRIPTOR_ADD_BASIC(eco_edd_option, Eco_Option, "string",	 stringValue, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_LIST (eco_edd_option, Eco_Option, "list",	 listValue,	  eco_edd_option);

   EET_DATA_DESCRIPTOR_ADD_HASH (eco_edd_group,  Eco_Group,  "options", data, eco_edd_option);
   
   conf_module = m;
   e_module_delayed_set(m, 0);
   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   E_Config_Dialog *cfd;
   while ((cfd = e_config_dialog_get("E", "_config_eco_dialog"))) e_object_del(E_OBJECT(cfd));
   e_configure_registry_item_del("appearance/eco");
   e_configure_registry_category_del("appearance");

   /* remove module-supplied menu additions */
   if (maug)
     {
	e_int_menus_menu_augmentation_del("config/1", maug);
	maug = NULL;
     }
   if (evil)
     {
	eco_actions_free();
	eco_event_shutdown();
	eco_border_shutdown();
     }

   conf_module = NULL;
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   return 1;
}


/* menu item callback(s) */
static void 
_e_mod_run_cb(void *data, E_Menu *m, E_Menu_Item *mi)
{
   e_int_config_eco(e_container_current_get(e_manager_current_get()), NULL);
}

/* menu item add hook */
static void
_e_mod_menu_add(void *data, E_Menu *m)
{
   E_Menu_Item *mi;
   
   mi = e_menu_item_new(m);
   e_menu_item_label_set(mi, _("Ecomorph"));
   e_menu_item_icon_edje_set(mi, edje_file, "icon");
   e_menu_item_callback_set(mi, _e_mod_run_cb, NULL);
}

