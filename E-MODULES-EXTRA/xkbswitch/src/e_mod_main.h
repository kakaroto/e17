/*
 * Main module header.
 * Contains some i18n stuff, module versioning,
 * config and public prototypes from main.
 */

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

/* Macros used for config file versioning */
/* You can increment the EPOCH value if the old configuration is not
 * compatible anymore, it creates an entire new one.
 * You need to increment GENERATION when you add new values to the
 * configuration file but is not needed to delete the existing conf  */
#define MOD_CONFIG_FILE_EPOCH 0x0001
#define MOD_CONFIG_FILE_GENERATION 0x008d
#define MOD_CONFIG_FILE_VERSION \
   ((MOD_CONFIG_FILE_EPOCH << 16) | MOD_CONFIG_FILE_GENERATION)

#include <libintl.h>
#define D_(str) dgettext(PACKAGE, str)

/* Base config struct. Store Item Count, etc
 * 
 * *module (not written to disk) (E Interaction)
 * *cfd (not written to disk) (config dialog)
 * 
 * Store list of your items that you want to keep. (sorting)
 * Can define per-module config properties here.
 * 
 * Version used to know when user config too old */
typedef struct _Config 
{
    /* Store a reference to the module instance provided by enlightenment in
     * e_modapi_init, in case you need to access it. (not written to disk) */
    E_Module *module;

    /* if you open a config dialog, store a reference to it in a pointer like
     * this one, so if the user opens a second time the dialog, you know it's
     * already open. Also needed for destroy the dialog when we are exiting */
    E_Config_Dialog *cfd;

    /* List of configuration items, when we save to disk, if we have a list of
     * items (Config_Item) they will be saved too */
    Eina_List *conf_items;

    /* config file version */
    int version;

    /* actual config properties; Define your own. (globally per-module) */
    unsigned char switch1;
} Config;

/* This struct used to hold config for individual items from above list */
typedef struct _Config_Item 
{
    /* unique id for every running gadget, this is managed by gadcon */
    const char *id;

    /* actual config properties independently for every running gadget. */
    int switch2;
} Config_Item;

/* Prototypes */

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);

/* Function for calling the module's Configuration Dialog */
E_Config_Dialog *e_int_config_xkbswitch_module(E_Container *con, const char *params);

extern Config *xkbswitch_conf;

#endif
