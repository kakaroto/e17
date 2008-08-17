/* Enhance
 * Copyright (C) 2006-2008 Hisham Mardam-Bey, Samuel Mendes, Ugo Riboni, Simon Treny
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

#ifndef EN_PRIV_H
#define EN_PRIV_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

#include <EXML.h>
#include <Ecore_Data.h>
#include <Evas.h>
#include <Etk.h>

#include "config.h"

#include "Enhance.h"
#include "enhance_widget.h"

typedef void (*etk_callback_type)(void);

void      _e_property_handle(Enhance *en, EXML_Node *node);
void      _e_signal_handle(Enhance *en, EXML_Node *node);
void      _e_traverse_packing_xml(Enhance *en, E_Widget *widget);
void      _e_traverse_property_xml(Enhance *en);
void      _e_traverse_signal_xml(Enhance *en);
void      _e_traverse_child_xml(Enhance *en);
E_Widget *_e_traverse_widget_xml(Enhance *en);
void      _e_traverse_xml(Enhance *en);
void      _en_stock_items_hash_init(void);

#endif
