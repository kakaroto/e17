/** @file etk_config.h */
#ifndef _ETK_CONFIG_H_
#define _ETK_CONFIG_H_

#include "etk_types.h"
#include <Eet.h>

/**
 * @defgroup Etk_Config Config system of Etk
 * @brief The config system saves and loads Etk's internal configuration (theme, font, styles, etc)
 * @{
 */
 
Etk_Bool    etk_config_init(void);
void        etk_config_shutdown(void);

Etk_Bool    etk_config_load(void);
Etk_Bool    etk_config_save(void);

const char *etk_config_widget_theme_get(void);
void        etk_config_widget_theme_set(const char *widget_theme);

const char *etk_config_font_get(void);
void        etk_config_font_set(const char *font);

const char *etk_config_engine_get(void);
void        etk_config_engine_set(const char *engine);

/** @} */

#endif
