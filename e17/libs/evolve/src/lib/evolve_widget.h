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

#ifndef _EVOLVE_WIDGET_H
#define _EVOLVE_WIDGET_H

typedef struct _Evolve_Widget_Packing_Option_Value Evolve_Widget_Packing_Option_Value;
typedef struct _Evolve_Widget_Packing_Option Evolve_Widget_Packing_Option;
typedef struct _Evolve_Widget_Packing_Info Evolve_Widget_Packing_Info;
typedef struct _Evolve_Widget_Type Evolve_Widget_Type;

struct _Evolve_Widget_Packing_Option_Value
{
   int value;
};

struct _Evolve_Widget_Packing_Option
{
   Evas_Hash *values;
};

struct _Evolve_Widget_Packing_Info
{
   Evas_Hash *info;
};

struct _Evolve_Widget_Type
{
      char *evolve_str;
      char *etk_str;
      Etk_Type *etk_type;
      Evas_Hash *packing_options;
      Evas_Hash *internal_props;
};

void evolve_widget_packing_info_populate();
void evolve_widget_types_populate();

Evolve_Widget *evolve_widget_new(char *type);
Evolve_Widget *evolve_widget_find(char *name);

Evolve_Ctor evolve_widget_ctor_find(char *type); 
  
int  evolve_widget_internal_property_apply(Evolve_Widget *widget, Evolve_Property *prop);
void evolve_widget_properties_apply(Evolve_Widget *widget);
void evolve_widget_property_apply(Evolve_Widget *widget, Evolve_Property *prop);

int  evolve_widget_internal_render(Evolve_Widget *widget);
void evolve_widget_post_render(Evolve_Widget *widget);
void evolve_widget_render(Evolve_Widget *widget);
void evolve_widget_children_render(Evolve *evolve, Evolve_Widget *widget);
void evolve_widget_signals_connect(Evolve_Widget *widget, Evolve *evolve);
void evolve_widget_reparent(Evolve_Widget *child, Evolve_Widget *parent);
  
char  *evolve_widget_packing_property_str_find(Evolve_Widget *widget, char *name, char *def);
int    evolve_widget_packing_property_int_find(Evolve_Widget *widget, char *name, int def);
double evolve_widget_packing_property_double_find(Evolve_Widget *widget, char *name, double def);

char *evolve_widget_packing_property_to_str_convert(char *widget_type,  char *prop_name, int prop_value);
int   evolve_widget_packing_property_to_int_convert(char *widget_type, char *prop_name, char *prop_value);

char *evolve_widget_code_get(Evolve_Widget *widget);
  
Evas_List *evolve_widget_list_sort(Evas_List *widgets);

void evolve_widget_property_set(Evolve_Widget *widget, char *name, char *value, int type);
Evolve_Property *evolve_widget_property_get(Evolve_Widget *widget, char *prop_name);

Etk_Type *evolve_widget_type_to_etk(char *type);
int evolve_widget_type_property_exists(char *type, char *prop_name);
int evolve_widget_type_internal_property_exists(char *type, char *prop_name);
  
#endif
