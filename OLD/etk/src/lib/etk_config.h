/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

/** @file etk_config.h */
#ifndef _ETK_CONFIG_H_
#define _ETK_CONFIG_H_

#include <Eet.h>

#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Config Config system of Etk
 * @brief The config system saves and loads Etk's internal configuration (theme, font, styles, etc)
 * @{
 */

Etk_Bool    etk_config_init(void);
void        etk_config_shutdown(void);

Etk_Bool    etk_config_load(void);
Etk_Bool    etk_config_save(void);

const char *etk_config_wm_theme_get(void);
void        etk_config_wm_theme_set(const char *wm_theme);

const char *etk_config_widget_theme_get(void);
void        etk_config_widget_theme_set(const char *widget_theme);

const char *etk_config_font_get(void);
void        etk_config_font_set(const char *font);

const char *etk_config_engine_get(void);
void        etk_config_engine_set(const char *engine);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
