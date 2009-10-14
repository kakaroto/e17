#ifndef _CONF_H
#define _CONF_H

#include "stickies.h"

int _e_config_init();
int _e_config_shutdown();
E_Config_Version *_e_config_version_parse(char *version);
int _e_config_version_compare(E_Config_Version *v1, E_Config_Version *v2);
void _e_config_defaults_apply(E_Stickies *ss);
int _e_config_load(E_Stickies *ss);
int _e_config_save(E_Stickies *ss);

#endif
