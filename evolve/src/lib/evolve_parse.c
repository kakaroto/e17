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

#include <string.h>

#include "evolve_private.h"

#define LAST_WIDGET \
   Evolve_Widget *last_widget; \
   last_widget = eina_list_data_get(eina_list_last(widgets))

extern Eina_List *widgets;
extern char *evolve_filename;
extern int evolve_lnum;
extern int evolve_col;

Etk_String *_evolve_edje_code = NULL;

/* parse a widget's name */
void evolve_parse_name(char *name)
{   
   LAST_WIDGET;
   if (last_widget->name)
     free(last_widget->name);
   last_widget->name = strdup(name);
}

/* parse a new "widget" entry */
void evolve_parse_widget(char *type)
{
   Evolve_Widget *widget;
   
   //printf("widget: %s, new\n", type);
   
   widget = evolve_widget_new(type);
   if (!widget) 
     return;
   widgets = eina_list_append(widgets, widget);   
}

/* parse a widget's parent, bail out of it cant be found */
void evolve_parse_parent(char *parent)
{
   Evolve_Widget *pwidget;
   LAST_WIDGET;
   
   pwidget = evolve_widget_find(parent);
   if (!pwidget)
     {
	fprintf(stderr, "EVOLVE ERROR: Widget %s has parent %s but parent "
		"is not declared yet! Make sure you declare widgets in order, parents, then children.\n",
		last_widget->name, parent);
	exit(-1);
     }
      
   if (last_widget->parent)
     free(last_widget->parent);
   last_widget->parent = strdup(parent);
}

/* parse a property who's value is an int */
int evolve_parse_property_number(char *prop, char *value)
{   
   LAST_WIDGET;
   if (!last_widget)
     return 0;
   
   if (!evolve_widget_type_property_exists(last_widget->type, prop))
     return 0;
   
   evolve_widget_property_set(last_widget, prop, value, EVOLVE_PROPERTY_INT);
   return 1;
}

/* parse a property who's value is a string */
int evolve_parse_property_string(char *prop, char *value)
{   
   LAST_WIDGET;
   if (!last_widget)
     return 0;
   
   if (!evolve_widget_type_property_exists(last_widget->type, prop))
     return 0;   

   evolve_widget_property_set(last_widget, prop, value, EVOLVE_PROPERTY_STRING);
   return 1;
}

/* parse a packing property who's value is an int */
void evolve_parse_packing_property_number(char *prop, char *value)
{
   Evolve_Widget_Property *pstruct;   
   LAST_WIDGET;
   if (!last_widget)
     return;
   
   pstruct = calloc(1, sizeof(Evolve_Widget_Property));
   pstruct->value = strdup(value);
   pstruct->type = EVOLVE_WIDGET_PROP_INT;
   eina_hash_add(last_widget->packing_props, prop, pstruct);
}

/* parse a packing property who's value is a string */
void evolve_parse_packing_property_string(char *prop, char *value)
{
   Evolve_Widget_Property *pstruct;   
   LAST_WIDGET;
   if (!last_widget)
     return;
   
   pstruct = calloc(1, sizeof(Evolve_Widget_Property));
   pstruct->value = strdup(value);
   pstruct->type = EVOLVE_WIDGET_PROP_STR;
   eina_hash_add(last_widget->packing_props, prop, pstruct);
}

/* parse a signal declaration */
void evolve_parse_signal()
{
   Evolve_Widget_Signal *sig;   
   LAST_WIDGET;
   if (!last_widget)
     return;
      
   sig = calloc(1, sizeof(Evolve_Widget_Signal));
   last_widget->signals = eina_list_append(last_widget->signals, sig);
}

/* parse a signal's property */
void evolve_parse_signal_property(char *prop, char *value)
{   
   Evolve_Widget_Signal *sig;
   Eina_List *l;
   LAST_WIDGET;
   
   if (!last_widget || !last_widget->signals || !(l = eina_list_last(last_widget->signals)))
     return;

   sig = l->data;
   
   if (!strcmp(prop, "name"))
     {
	if (sig->name)
	  free(sig->name);
	sig->name = strdup(value);
     }
   else if (!strcmp(prop, "callback"))
     {
	if (sig->callback)
	  free(sig->callback);
	sig->callback = strdup(value);
	if (sig->emit)
	  fprintf(stderr, "Warning: assiging callback for signal '%s' "
		  "while a custom emission (%s) is already attached! "
		  "Removing custom emission!\n",
		  sig->name ? sig->name : "no name", sig->emit);
	free(sig->emit);
	sig->emit = NULL;	
     }
   else if (!strcmp(prop, "swapped"))
     {
	sig->swapped = atoi(value);
     }
   else if (!strcmp(prop, "emit"))
     {
	if (sig->emit)
	  free(sig->emit);
	sig->emit = strdup(value);
	if (sig->callback)
	  fprintf(stderr, "Warning: assiging custom emission for signal '%s' "
		  "while a callback (%s) is already attached! "
		  "Removing callback!\n",
		  sig->name ? sig->name : "no name", sig->callback);
	free(sig->callback);
	sig->callback = NULL;
     }
   else
     {
	fprintf(stderr, "Unexpected property %s for signal declaration.\n"
		"file: %s, line: %d, column: %d\n\n",
		prop, evolve_filename, evolve_lnum, evolve_col);
     }
}

/* parse a signal's data property */
void evolve_parse_signal_data_property(char *prop, char *value)
{
   Evolve_Widget_Signal *sig;
   Eina_List *l;
   LAST_WIDGET;
   
   if (!last_widget || !last_widget->signals || !(l = eina_list_last(last_widget->signals)))
     return;

   sig = l->data;
      
   if (!strcmp(prop, "type"))
     {
	if (strcmp(value, "widget") && strcmp(value, "variable") && strcmp(value, "function"))
	  {
	     fprintf(stderr, "Unexpected property %s for signal data declaration.\n"
		     "file: %s, line: %d, column: %d\n\n",
		     prop, evolve_filename, evolve_lnum, evolve_col);
	     return;
	  }
	sig->data_type = strdup(value);
     }
   else if (!strcmp(prop, "name"))
     sig->data_name = strdup(value);
}

/* parse edje code section */
void evolve_parse_edje(char *code)
{
   if (!_evolve_edje_code)
     _evolve_edje_code = etk_string_new_printf("%s\n", code);
   else
     _evolve_edje_code = etk_string_append_printf(_evolve_edje_code, "%s\n", code);
}
