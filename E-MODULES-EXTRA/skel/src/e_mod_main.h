#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

/* Macros used for config file versioning */
// FIXME: what we need to change when we update our conf ? what is hte other value for ? how they works ?
#define MOD_CONFIG_FILE_EPOCH 0x0001
#define MOD_CONFIG_FILE_GENERATION 0x008d
#define MOD_CONFIG_FILE_VERSION \
   ((MOD_CONFIG_FILE_EPOCH << 16) | MOD_CONFIG_FILE_GENERATION)

/* More mac/def; Define your own. What do you need ? */
#define CONN_DEVICE_ETHERNET 0

/* Gettext: you need to use the D_ prefix for all your messages,
 * like  printf D_("Hello World\n");  so can be replaced by gettext */
#define D_(str) dgettext(PACKAGE, str)

/* We create a structure config for our module, and also a config structure
 * for every item element (you can have multiple gadgets for the same module) */
typedef struct _Config Config;
typedef struct _Config_Item Config_Item;

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
// FIXME: explain what is that
   E_Module *module;
// FIXME: explain what is that
   E_Config_Dialog *cfd;

   /* The list; their location on screen ? */
// FIXME: explain what contains this list
   Eina_List *conf_items;

   /* config file version */
   int version;

   /* actual config properties; Define your own. (globally per-module) */
   unsigned char switch1;
};

/* This struct used to hold config for individual items from above list */
struct _Config_Item 
{
   /* unique id for every running gadget */
   const char *id;

   /* actual config properties independently for every running gadget. */
   int switch2;
};

/* Setup the E Module Version, Needed to check if module can run. */
// FIXME: what exactly this does ? better detailed explanation
EAPI extern E_Module_Api e_modapi;

/* E API Module Interface Declarations
 *
 * e_modapi_init:     it is called when e17 initialize the module, note that
 *                    a module can be loaded but not initialized (running)
 * e_modapi_shutdown: it is called when e17 is closing, so calling the modules
 *                    to finish
 * e_modapi_save:     this is called when e17 or by another reason is requeested
 *                    to save the configuration file                      */
EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);

/* Function for calling the module's Configuration Dialog */
EAPI E_Config_Dialog *e_int_config_skel_module(E_Container *con);

extern Config *skel_conf;

#endif
