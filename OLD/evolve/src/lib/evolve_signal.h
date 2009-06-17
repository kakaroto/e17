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

#ifndef _EVOLVE_SIGNAL_H
#define _EVOLVE_SIGNAL_H

Evolve_Signal *evolve_signal_new(char *name, char *emission, Evolve *evolve);
void evolve_signal_emit_cb(void *data, Etk_Object *object);
void evolve_signal_connect(Evolve *evolve, char *emission, void (*callback)(char *emission, void *data), void *data);

#endif
