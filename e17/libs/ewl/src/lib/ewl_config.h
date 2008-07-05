/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_CONFIG_H
#define EWL_CONFIG_H

/**
 * @addtogroup Ewl_Config Ewl_Config: Functions for Manipulating Configuration Data
 *
 * @{
 */

/**
 * The Ewl_Config structure
 */
typedef struct Ewl_Config Ewl_Config;

/**
 * @brief Contains Ewl configuration data
 */
struct Ewl_Config
{
        char *app_name;        /**< The application name*/

        struct
        {
                Ecore_Hash *user;        /**< User data */
                Ecore_Hash *system;        /**< System data */
                Ecore_Hash *instance;        /**< Data for this instance only */
        } data;                                /**< The config data */
};

int              ewl_config_init(void);
void             ewl_config_shutdown(void);
void             ewl_config_cache_init(void);

Ewl_Config      *ewl_config_new(const char *app_name);
void             ewl_config_destroy(Ewl_Config *cfg);

void             ewl_config_string_set(Ewl_Config *cfg, const char *k,
                                                const char *v,
                                                Ewl_State_Type state);
const char      *ewl_config_string_get(Ewl_Config *cfg, const char *k);

void             ewl_config_int_set(Ewl_Config *cfg, const char *k, int v,
                                                Ewl_State_Type state);
int              ewl_config_int_get(Ewl_Config *cfg, const char *k);

void             ewl_config_float_set(Ewl_Config *cfg, const char *k,
                                                float v,
                                                Ewl_State_Type state);
float            ewl_config_float_get(Ewl_Config *cfg, const char *k);

void             ewl_config_color_set(Ewl_Config *cfg, const char *k,
                                                int r, int g, int b, int a,
                                                Ewl_State_Type state);
void             ewl_config_color_get(Ewl_Config *cfg, const char *k,
                                                int *r, int *g,
                                                int *b, int *a);

void             ewl_config_key_remove(Ewl_Config *cfg, const char *k);
void             ewl_config_instance_key_remove(Ewl_Config *cfg, const char *k);
void             ewl_config_system_key_remove(Ewl_Config *cfg, const char *k);
void             ewl_config_user_key_remove(Ewl_Config *cfg, const char *k);

int              ewl_config_can_save_system(Ewl_Config *cfg);
int              ewl_config_system_save(Ewl_Config *cfg);
int              ewl_config_user_save(Ewl_Config *cfg);

/**
 * @}
 */

#endif
