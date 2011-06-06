#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#include <Drawer.h>

#ifdef ENABLE_NLS
# include <libintl.h>
# define D_(string) dgettext(PACKAGE, string)
#else
# define bindtextdomain(domain,dir)
# define bind_textdomain_codeset(domain,codeset)
# define D_(string) (string)
#endif

/* Macros used for config file versioning */
#define MOD_CONFIG_FILE_EPOCH 0x0001
#define MOD_CONFIG_FILE_GENERATION 0x008e
#define MOD_CONFIG_FILE_VERSION \
   ((MOD_CONFIG_FILE_EPOCH << 16) | MOD_CONFIG_FILE_GENERATION)

/* Base config struct. Store Item Count, etc
 * 
 * *module (not written to disk) (E Interaction)
 * *cfd (not written to disk) (config dialog)
 * 
 * Store list of your items that you want to keep. (sorting)
 * Can define per-module config properties here.
 * 
 * Version used to know when user config too old */
struct _Config 
{
   E_Module *module;
   E_Config_Dialog *cfd;

   Eina_List *conf_items;

   /* config file version */
   int version;
};

/* This struct used to hold config for individual items from above list */
struct _Config_Item 
{
   /* unique id */
   const char *id;

   /* Source and view plugin names */
   const char *source;
   const char *view;
   const char *composite;
};

/* Setup the E Module Version, Needed to check if module can run. */
EAPI extern E_Module_Api e_modapi;

/* E API Module Interface Declarations
 * 
 * Need to initialize, shutdown, save the module */
EAPI void            * e_modapi_init(E_Module *m);
EAPI int               e_modapi_shutdown(E_Module *m);
EAPI int               e_modapi_save(E_Module *m);

Eina_List       * drawer_plugins_list(Drawer_Plugin_Category cat);
void              drawer_plugins_list_free(Eina_List *list);
Drawer_Plugin   * drawer_plugin_load(Config_Item *ci, Drawer_Plugin_Category cat, const char *name);
Evas_Object     * drawer_plugin_config_button_get(Config_Item *ci, Evas *evas, Drawer_Plugin_Category cat);

/* Function for calling the modules config dialog */
EAPI E_Config_Dialog * e_int_config_drawer_module(E_Container *con, Config_Item *ci);

extern Config *drawer_conf;
#endif
