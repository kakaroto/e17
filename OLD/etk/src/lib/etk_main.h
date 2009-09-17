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

/** @file etk_main.h */
#ifndef _ETK_MAIN_H_
#define _ETK_MAIN_H_

#include <Evas.h>

#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Main The main functions of Etk
 * @brief The main functions of Etk, used to initialize or shutdown Etk, and to control the main loop
 * @{
 */

int  etk_init(int argc, char **argv);
int  etk_init_full(int argc, char **argv, const char *custom_opts);
int  etk_shutdown(void);

void etk_main(void);
void etk_main_quit(void);
void etk_main_iterate(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
