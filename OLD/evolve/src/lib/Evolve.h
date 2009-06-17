/* Evolve
 * Copyright (C) 2007-2008 Hisham Mardam-Bey 
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

#ifndef _EVOLVE_H
#define _EVOLVE_H

#include <Etk.h>
#include <Evas.h>

typedef struct _Evolve Evolve;

int evolve_init();
int evolve_shutdown();

void evolve_print(Evolve *evolve);
  

Evolve     *evolve_etk_load(char *file);

Evolve     *evolve_eet_load(char *file);
int         evolve_eet_save(Evolve *evolve, char *file);

void        evolve_render(Evolve *evolve);
Etk_Widget *evolve_etk_widget_find(Evolve *evolve, char *name);

void        evolve_signal_connect(Evolve *evolve, char *emission, void (*callback)(char *emission, void *data), void *data);

char       *evolve_code_get(Evolve *evolve);

void        evolve_defines_set(Eina_List *defines);

#endif
