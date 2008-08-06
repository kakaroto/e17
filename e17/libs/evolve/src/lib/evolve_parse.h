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

#ifndef _EVOLVE_PARSE_H
#define _EVOLVE_PARSE_H

void evolve_parse_name(char *name);
void evolve_parse_widget(char *type);
void evolve_parse_parent(char *parent);
int  evolve_parse_property_number(char *prop, char *value);
int  evolve_parse_property_string(char *prop, char *value);
void evolve_parse_packing_property_number(char *prop, char *value);
void evolve_parse_packing_property_string(char *prop, char *value);
void evolve_parse_signal();
void evolve_parse_signal_property(char *prop, char *value);
void evolve_parse_signal_data_property(char *prop, char *value);
void evolve_parse_edje(char *code);
  
#endif
