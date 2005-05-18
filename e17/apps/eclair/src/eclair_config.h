#ifndef _ECLAIR_CONFIG_H_
#define _ECLAIR_CONFIG_H_

#include "eclair_private.h"

Evas_Bool eclair_config_init(Eclair_Config *config);
void eclair_config_shutdown(Eclair_Config *config);
void eclair_config_load(Eclair_Config *config);
void eclair_config_save(Eclair_Config *config);
void eclair_config_set_default(Eclair_Config *config);
void eclair_config_set_prop_string(Eclair_Config *config, const char *node_name, const char *prop_name, const char *prop_value);
void eclair_config_set_prop_int(Eclair_Config *config, const char *node_name, const char *prop_name, int prop_value);
void eclair_config_set_prop_float(Eclair_Config *config, const char *node_name, const char *prop_name, float prop_value);
int eclair_config_get_prop_string(Eclair_Config *config, const char *node_name, const char *prop_name, char **prop_value);
int eclair_config_get_prop_int(Eclair_Config *config, const char *node_name, const char *prop_name, int *prop_value);
int eclair_config_get_prop_float(Eclair_Config *config, const char *node_name, const char *prop_name, float *prop_value);

#endif
