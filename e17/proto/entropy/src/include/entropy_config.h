#ifndef _ENTROPY_CONFIG_H_
#define _ENTROPY_CONFIG_H_

#include "entropy.h"
#include "entropy_gui.h"


struct entropy_config {
	char* config_dir;
	char* config_dir_and_file;
};

entropy_config* entropy_config_init(entropy_core* core);
void entropy_config_destroy(entropy_config* config);
char* entropy_config_str_get(char* module, char* key);
void entropy_config_str_set(char* module, char* key, char* value);


Ecore_Hash *
entropy_config_standard_structures_parse (entropy_gui_component_instance * instance,
						char *config);


#define ENTROPY_CONFIG_INT_UNDEFINED 65535

#endif
