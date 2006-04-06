/* 
 * vim: ts=8
 */
/*
#include <stdio.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_File.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include <Esmart/Esmart_Text_Entry.h>
#include <Esmart/Esmart_Draggies.h>
*/

#include "config.h"

#include "e_mod_main.h"
#include "e_mod_volume.h"
#include "e_mod_cdialog.h"

char *module_root = NULL;
char *module_theme = NULL;

E_Module_Api e_modapi = {
   E_MODULE_API_VERSION,
   "eVolume"
};

void *
e_modapi_init(E_Module *module)
{
   Volume *volume;

   bindtextdomain(PACKAGE, LOCALEDIR);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   module_root = e_module_dir_get(module);

   {
      char buf[128];

      snprintf(buf, 128, "%s/volume.edj", module_root);
      module_theme = strdup(buf);
   }

   /* actually init buttons */
   volume = e_volume_init(NULL);
   module->config_menu = volume->config_menu;
   return volume;
}

int
e_modapi_shutdown(E_Module *module)
{
   Volume *volume;

   if (module->config_menu)
      module->config_menu = NULL;

   volume = module->data;

   if (volume)
      e_volume_shutdown(volume);

   return 1;
}

int
e_modapi_save(E_Module *module)
{
   Volume *volume;

   volume = module->data;
   e_config_domain_save("module.evolume", conf_edd, volume->conf);
   return 1;
}

int
e_modapi_info(E_Module *module)
{
   char buf[1024];

   snprintf(buf, 1024, "%s/module_icon.png", module_root);
   module->icon_file = strdup(buf);
   return 1;
}

int
e_modapi_about(E_Module *module __UNUSED__)
{
   e_module_dialog_show(_("Enlightenment Evolume Module"), 
			_("A simple module to give E17 a volume control for some mixers."));
   return 1;
}

int
e_modapi_config(E_Module *module) 
{
   Volume *v;
   E_Container *con;
   Evas_List *l;
   
   v = module->data;
   if (!v)
      return 0;
   if (!v->faces)
      return 0;

   con = e_container_current_get(e_manager_current_get());
   for (l = v->faces; l; l = l->next) 
     {
	Volume_Face *vf;
	
	vf = l->data;
	if (!vf)
	  continue;
	
	if (vf->con == con) 
	  {
	     e_volume_config_module(vf->con, vf);
	     break;
	  }
     }
   return 1;   
}
