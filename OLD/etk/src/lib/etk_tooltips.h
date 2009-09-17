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

/** @file etk_tooltips.h */
#ifndef _ETK_TOOLTIPS_H_
#define _ETK_TOOLTIPS_H_

#include "etk_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Tooltips Etk_Tooltips
 * @{
 */

void etk_tooltips_init();
void etk_tooltips_shutdown();

void etk_tooltips_enable();
void etk_tooltips_disable();

void etk_tooltips_tip_set(Etk_Widget *widget, const char *text);
const char *etk_tooltips_tip_get(Etk_Widget *widget);

Etk_Bool etk_tooltips_tip_visible();

void etk_tooltips_pop_up(Etk_Widget *widget);
void etk_tooltips_pop_down();

/** @} */

#ifdef __cplusplus
}
#endif

#endif
