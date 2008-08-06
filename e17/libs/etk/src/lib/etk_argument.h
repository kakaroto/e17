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

/** @file etk_argument.h */
#ifndef _ETK_ARGUMENT_H_
#define _ETK_ARGUMENT_H_

#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Argument Argument parser
 * @brief A couple of functions to parse the arguments of a program
 * @{
 */

void     etk_argument_init(int argc, char **argv, const char *custom_opts);
void     etk_argument_shutdown(void);
void     etk_argument_get(int *argc, char ***argv);

Etk_Bool etk_argument_is_set(const char *long_name, char short_name, Etk_Bool remove);
Etk_Bool etk_argument_value_get(const char *long_name, char short_name, Etk_Bool remove, char **value);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
