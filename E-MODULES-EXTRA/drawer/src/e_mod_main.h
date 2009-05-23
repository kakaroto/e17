/*
 * vim:ts=8:sw=3:sts=8:et:cino=>5n-3f0^-2{2,t0,(0
 */
#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#include <Drawer.h>

/* Macros used for config file versioning */
#define MOD_CONFIG_FILE_EPOCH 0x0001
#define MOD_CONFIG_FILE_GENERATION 0x008e
#define MOD_CONFIG_FILE_VERSION \
   ((MOD_CONFIG_FILE_EPOCH << 16) | MOD_CONFIG_FILE_GENERATION)

/* Setup the E Module Version, Needed to check if module can run. */
EAPI extern E_Module_Api e_modapi;

/* E API Module Interface Declarations
 * 
 * Need to initialize, shutdown, save the module */
EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);

/* Function for calling the modules config dialog */
EAPI E_Config_Dialog *e_int_config_drawer_module(E_Container *con, Config_Item *ci);
#endif
