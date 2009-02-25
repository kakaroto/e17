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

#include <stdio.h>
#include <string.h>

#include "evolve_private.h"

typedef struct _Evolve_Pair Evolve_Pair;

struct _Evolve_Pair
{
   void *p1;
   void *p2;
};

Evolve *evolve = NULL;
static Evolve_Ctor _evolve_queue_func_new = NULL;

static Etk_Widget *_evolve_gui_gen_table = NULL;
static Etk_Widget *_evolve_gui_pac_table = NULL;
static Etk_Widget *_evolve_gui_com_table = NULL;
static Etk_Widget *_evolve_gui_sig_table = NULL;

static void evolve_gui_property_entry_text_changed_cb(Etk_Object *object, void *data);
static void evolve_gui_property_spinner_value_changed_cb(Etk_Range *range, double value);

int evolve_gui_init()
{
   evolve = calloc(1, sizeof(Evolve));
   evolve->emissions = eina_hash_string_superfast_new(NULL);
   evolve->callbacks = eina_hash_string_superfast_new(NULL);
   evolve->parents = eina_hash_string_superfast_new(NULL);

   return 1;
}

void evolve_gui_queue_new(void *data)
{
   _evolve_queue_func_new = (Evolve_Ctor)data;
}

void evolve_gui_placeholder_replace(Evolve_Widget *parent, Evolve_Widget *child, Etk_Widget *placeholder)
{
   if (!parent || !parent->type || !child || !child->type || !placeholder)
     return;
   
   if (!strcmp(parent->type, "hbox") || !strcmp(parent->type, "vbox"))
     {
	int pos;
	Etk_Box_Group group;
	char *group_str;
	char pos_str[5];
	Evolve_Widget_Property *prop;
	
	etk_box_child_position_get(ETK_BOX(parent->widget), placeholder,
				   &group, &pos);
	
	group_str = evolve_widget_packing_property_to_str_convert("box", "group", group);
	prop = calloc(1, sizeof(Evolve_Widget_Property));
	prop->name = strdup("group");
	prop->value = group_str;
	prop->type = EVOLVE_WIDGET_PROP_INT;
	eina_hash_add(child->packing_props, "group", prop);
	
	prop = calloc(1, sizeof(Evolve_Widget_Property));
	snprintf(pos_str, sizeof(pos_str), "%d", pos);
	prop->name = strdup("position");
	prop->value = strdup(pos_str);
	prop->type = EVOLVE_WIDGET_PROP_INT;
	eina_hash_add(child->packing_props, "position", prop);
     }
   else if (!strcmp(parent->type, "toolbar"))
     {
	/* [TODO] [3] We are accessing the toolbar's internals, better way? */
	int pos;
	char pos_str[5];
	Evolve_Widget_Property *prop;
	
	etk_box_child_position_get(ETK_BOX(ETK_TOOLBAR(parent->widget)->box),
				   placeholder,
				   NULL, &pos);
	
	prop = calloc(1, sizeof(Evolve_Widget_Property));
	snprintf(pos_str, sizeof(pos_str), "%d", pos);
	prop->name = strdup("position");
	prop->value = strdup(pos_str);
	prop->type = EVOLVE_WIDGET_PROP_INT;
	eina_hash_add(child->packing_props, "position", prop);
     }
   else if (!strcmp(parent->type, "table"))
     {
	int la, ra, ta, ba;
	char la_str[5], ra_str[5], ta_str[5], ba_str[5];
	Evolve_Widget_Property *prop;
	
	etk_table_child_position_get(ETK_TABLE(parent->widget),
				   placeholder,
				   &la, &ra, &ta, &ba);
	
	prop = calloc(1, sizeof(Evolve_Widget_Property));
	snprintf(la_str, sizeof(la_str), "%d", la);
	prop->name = strdup("left_attach");
	prop->value = strdup(la_str);
	prop->type = EVOLVE_WIDGET_PROP_INT;
	eina_hash_add(child->packing_props, "left_attach", prop);
	
	prop = calloc(1, sizeof(Evolve_Widget_Property));
	snprintf(ra_str, sizeof(ra_str), "%d", ra);
	prop->name = strdup("right_attach");	
	prop->value = strdup(ra_str);
	prop->type = EVOLVE_WIDGET_PROP_INT;
	eina_hash_add(child->packing_props, "right_attach", prop);
	
	prop = calloc(1, sizeof(Evolve_Widget_Property));
	snprintf(ta_str, sizeof(ta_str), "%d", ta);
	prop->name = strdup("top_attach");	
	prop->value = strdup(ta_str);
	prop->type = EVOLVE_WIDGET_PROP_INT;
	eina_hash_add(child->packing_props, "top_attach", prop);
	
	prop = calloc(1, sizeof(Evolve_Widget_Property));
	snprintf(ba_str, sizeof(ba_str), "%d", ba);
	prop->name = strdup("bottom_attach");	
	prop->value = strdup(ba_str);
	prop->type = EVOLVE_WIDGET_PROP_INT;
	eina_hash_add(child->packing_props, "bottom_attach", prop);
     }
   else if (!strcmp(parent->type, "notebook"))
     {
	int pos;
	char pos_str[5];
	const char *label;
	Evolve_Widget_Property *prop;
	
	pos = etk_notebook_page_index_get(ETK_NOTEBOOK(parent->widget), placeholder);
	label = etk_notebook_page_tab_label_get(ETK_NOTEBOOK(parent->widget), pos);
	
	prop = calloc(1, sizeof(Evolve_Widget_Property));
	snprintf(pos_str, sizeof(pos_str), "%d", pos);
	prop->name = strdup("position");
	prop->value = strdup(pos_str);
	prop->type = EVOLVE_WIDGET_PROP_INT;
	eina_hash_add(child->packing_props, "position", prop);
		
	if (label)
	  {
	     prop = calloc(1, sizeof(Evolve_Widget_Property));
	     prop->name = strdup("tab_label");
	     prop->value = strdup(label);
	     prop->type = EVOLVE_WIDGET_PROP_STR;
	     eina_hash_add(child->packing_props, "tab_label", prop);
	  }
     }
   etk_container_remove(placeholder);
}

void evolve_gui_general_table_populate(Evolve_Widget *widget)
{
   Etk_Widget *label;
   Etk_Widget *entry;
   Etk_Type *type;
   Eina_List *l, *list = NULL;
   char *str;
   int i = 2;
   
   if (!widget || !widget->type || !(type = evolve_widget_type_to_etk(widget->type)))
     return;

   etk_type_property_list(type, &list);
   
   etk_table_resize(ETK_TABLE(_evolve_gui_gen_table), 0, 0);
   etk_table_resize(ETK_TABLE(_evolve_gui_gen_table), 2, eina_list_count(list) + 2);
   
   label = etk_label_new("Class:");
   entry = etk_entry_new();
   etk_entry_text_set(ETK_ENTRY(entry), type->name);
   etk_widget_disabled_set(entry, ETK_TRUE);   
   etk_table_attach_default(ETK_TABLE(_evolve_gui_gen_table), label,
			    0, 0, 0, 0);
   etk_table_attach_default(ETK_TABLE(_evolve_gui_gen_table), entry,
			    1, 1, 0, 0);
   
   label = etk_label_new("Name:");
   entry = etk_entry_new();
   etk_entry_text_set(ETK_ENTRY(entry), widget->name);
   etk_table_attach_default(ETK_TABLE(_evolve_gui_gen_table), label,
			    0, 0, 1, 1);
   etk_table_attach_default(ETK_TABLE(_evolve_gui_gen_table), entry,
			    1, 1, 1, 1);   

   for (l = list; l; l = l->next)
     {
	Evolve_Property *prop;
	Evolve_Property *eprop;
	Evolve_Property *p;
	Etk_String *value;
	Etk_Widget *vwidget;
	
	if (!(prop = l->data))
	  continue;
	
	str = evolve_util_string_humanize(prop->name);	
	label = etk_label_new(str);
	free(str);
	
	eprop = evolve_widget_property_get(widget, prop->name);
	if (eprop && eprop->default_value)	  
	  p = eprop;	  
	else
	  p = prop;
	
	switch(prop->type)
	  {
	   case EVOLVE_PROPERTY_INT:
	     value = etk_string_new_printf("%d", evolve_property_value_int_get(p->default_value));
	     vwidget = etk_spinner_new(0, 9999, evolve_property_value_int_get(p->default_value), 1, 10);
	     etk_object_data_set_full(ETK_OBJECT(vwidget), "prop", p, NULL);
	     etk_object_data_set_full(ETK_OBJECT(vwidget), "widget", widget, NULL);
	     etk_signal_connect("value-changed", ETK_OBJECT(vwidget),
				ETK_CALLBACK(evolve_gui_property_spinner_value_changed_cb),
				NULL);
	     break;
	   case EVOLVE_PROPERTY_BOOL:
	     value = etk_string_new_printf("%d", evolve_property_value_bool_get(p->default_value));
	     vwidget = etk_combobox_new_default();
	       {
		  Etk_Combobox_Item *item;
		  
		  item = etk_combobox_item_append(ETK_COMBOBOX(vwidget), "True");
		  if (evolve_property_value_bool_get(p->default_value))
		    etk_combobox_active_item_set(ETK_COMBOBOX(vwidget), item);
		  item = etk_combobox_item_append(ETK_COMBOBOX(vwidget), "False");
		  if (!evolve_property_value_bool_get(p->default_value))
		    etk_combobox_active_item_set(ETK_COMBOBOX(vwidget), item);		  
	       }
	     break;
	   case EVOLVE_PROPERTY_CHAR:
	     value = etk_string_new_printf("%c", evolve_property_value_char_get(p->default_value));
	     vwidget = etk_entry_new();
	     etk_entry_text_set(ETK_ENTRY(vwidget), etk_string_get(value));
	     etk_object_data_set_full(ETK_OBJECT(vwidget), "prop", p, NULL);
	     etk_signal_connect("text-changed", ETK_OBJECT(vwidget),
				ETK_CALLBACK(evolve_gui_property_entry_text_changed_cb),
				widget);
	     break;		  
	   case EVOLVE_PROPERTY_FLOAT:
	     value = etk_string_new_printf("%f", evolve_property_value_float_get(p->default_value));
	     vwidget = etk_spinner_new(0.0, 9999.0, evolve_property_value_float_get(p->default_value), 0.1, 1.0);
	     etk_spinner_digits_set(ETK_SPINNER(vwidget), 1);
	     etk_object_data_set_full(ETK_OBJECT(vwidget), "prop", p, NULL);
	     etk_object_data_set_full(ETK_OBJECT(vwidget), "widget", widget, NULL);
	     etk_signal_connect("value-changed", ETK_OBJECT(vwidget),
				ETK_CALLBACK(evolve_gui_property_spinner_value_changed_cb),
				NULL);
	     break;
	   case EVOLVE_PROPERTY_DOUBLE:
	     value = etk_string_new_printf("%g", evolve_property_value_double_get(p->default_value));
	     vwidget = etk_spinner_new(0.0, 9999.0, evolve_property_value_double_get(p->default_value), 0.1, 1.0);
	     etk_spinner_digits_set(ETK_SPINNER(vwidget), 1);
	     etk_object_data_set_full(ETK_OBJECT(vwidget), "prop", p, NULL);
	     etk_object_data_set_full(ETK_OBJECT(vwidget), "widget", widget, NULL);
	     etk_signal_connect("value-changed", ETK_OBJECT(vwidget),
				ETK_CALLBACK(evolve_gui_property_spinner_value_changed_cb),
				NULL);
	     break;
	   case EVOLVE_PROPERTY_SHORT:
	     value = etk_string_new_printf("%d", evolve_property_value_short_get(p->default_value));
	     vwidget = etk_spinner_new(0, 9999, evolve_property_value_short_get(p->default_value), 1, 10);
	     etk_object_data_set_full(ETK_OBJECT(vwidget), "prop", p, NULL);
	     etk_object_data_set_full(ETK_OBJECT(vwidget), "widget", widget, NULL);
	     etk_signal_connect("value-changed", ETK_OBJECT(vwidget),
				ETK_CALLBACK(evolve_gui_property_spinner_value_changed_cb),
				NULL);	     
	     break;
	   case EVOLVE_PROPERTY_LONG:
	     value = etk_string_new_printf("%li", evolve_property_value_long_get(p->default_value));
	     vwidget = etk_spinner_new(0, 9999, evolve_property_value_long_get(p->default_value), 1, 10);
	     etk_object_data_set_full(ETK_OBJECT(vwidget), "prop", p, NULL);
	     etk_object_data_set_full(ETK_OBJECT(vwidget), "widget", widget, NULL);
	     etk_signal_connect("value-changed", ETK_OBJECT(vwidget),
				ETK_CALLBACK(evolve_gui_property_spinner_value_changed_cb),
				NULL);
	     break;
	   case EVOLVE_PROPERTY_POINTER:
	     value = etk_string_new_printf("cant cast PROPERTY_POINTER");
	     vwidget = etk_entry_new();
	     etk_entry_text_set(ETK_ENTRY(vwidget), etk_string_get(value));
	     etk_widget_disabled_set(vwidget, ETK_TRUE);
	     break;		  
	   case EVOLVE_PROPERTY_STRING:
	     value = etk_string_new_printf("%s", evolve_property_value_string_get(p->default_value));
	     vwidget = etk_entry_new();
	     etk_entry_text_set(ETK_ENTRY(vwidget), etk_string_get(value));
	     etk_entry_text_set(ETK_ENTRY(vwidget), etk_string_get(value));
	     etk_object_data_set_full(ETK_OBJECT(vwidget), "prop", p, NULL);
	     etk_signal_connect("text-changed", ETK_OBJECT(vwidget),
				ETK_CALLBACK(evolve_gui_property_entry_text_changed_cb),
				widget);	     
	     break;
	   default:
	     value = etk_string_new_printf("cant determine value");
	     vwidget = etk_entry_new();
	     etk_entry_text_set(ETK_ENTRY(vwidget), etk_string_get(value));
	     break;
	  }	
	
	if (p == prop)
	  {
	     int type;
	     
	     type = prop->type;	     
	     evolve_widget_property_set(widget, prop->name, etk_string_get(value), type);
	     eprop = evolve_widget_property_get(widget, prop->name);	     
	  }
	
	etk_object_destroy(ETK_OBJECT(value));
	
	etk_table_attach_default(ETK_TABLE(_evolve_gui_gen_table), label,
				  0, 0, i, i);
	etk_table_attach_default(ETK_TABLE(_evolve_gui_gen_table), vwidget,
				  1, 1, i, i);
	++i;
     }

   etk_widget_show_all(_evolve_gui_gen_table);      
}

void evolve_gui_mouse_down_select_cb(Etk_Object *object, Etk_Event_Mouse_Down *event, void *data)
{
   Evolve_Widget *widget;
   
   widget = data;
   
   if (!widget)
     return;
   
   evolve_gui_general_table_populate(widget);
}

void evolve_gui_mouse_down_new_cb(Etk_Object *object, Etk_Event_Mouse_Down *event, void *data)
{
   Eina_List *l;
   Evolve_Widget *parent;
   Evolve_Widget *child;
   int num = 0;
   char name[64];
   
   parent = data;
   
   if (!_evolve_queue_func_new || !parent)
     return;
      
   /* [FIXME] [5] Fix this cast */
   child = (Evolve_Widget *)_evolve_queue_func_new();
   
   for (l = evolve->widgets; l; l = l->next)
     {
	Evolve_Widget *widget = l->data;
	
	if (widget && widget->type && !strcmp(widget->type, child->type))
	  ++num;
     }
   
   snprintf(name, sizeof(name), "%s%d", child->type, num);
   child->name = strdup(name);
   child->parent = strdup(parent->name);
   
   if (ETK_IS_CONTAINER(parent->widget))
     evolve_gui_placeholder_replace(parent, child, ETK_WIDGET(object));
   
   etk_signal_connect("mouse-down", ETK_OBJECT(child->widget),
		      ETK_CALLBACK(evolve_gui_mouse_down_select_cb), child);
   
   evolve->widgets = eina_list_append(evolve->widgets, child);
   evolve_widget_reparent(child, parent);
   etk_widget_show_all(parent->widget);
   _evolve_queue_func_new = NULL;
}

void evolve_gui_prop_dialog_tables_set(Etk_Widget *g_table, Etk_Widget *p_table, Etk_Widget *c_table, Etk_Widget *s_table)
{
   _evolve_gui_gen_table = g_table;
   _evolve_gui_pac_table = p_table;
   _evolve_gui_com_table = c_table;
   _evolve_gui_sig_table = s_table;   
}

static void evolve_gui_property_entry_text_changed_cb(Etk_Object *object, void *data)
{
   Evolve_Widget *widget;
   Evolve_Property *prop;
   
   prop = etk_object_data_get(object, "prop");
   widget = data;
   
   evolve_widget_property_set(widget, prop->name, (char*)etk_entry_text_get(ETK_ENTRY(object)), prop->type);
}

static void evolve_gui_property_spinner_value_changed_cb(Etk_Range *range, double value)
{
   Etk_Spinner *spinner;
   Evolve_Property *prop;
   Evolve_Widget *widget;
   char value_str[64];
   
   if (!(spinner = ETK_SPINNER(range)))
     return;
   
   prop = etk_object_data_get(ETK_OBJECT(spinner), "prop");
   widget = etk_object_data_get(ETK_OBJECT(spinner), "widget");
 
   snprintf(value_str, sizeof(value_str), "%f", value);
   evolve_widget_property_set(widget, prop->name, value_str, prop->type);
}
