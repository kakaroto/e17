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
#include <dlfcn.h>
#include <limits.h>

#include "evolve_private.h"

#define OPTION_NEW \
   o = calloc(1, sizeof(Evolve_Widget_Packing_Option));	\
   o->values = eina_hash_string_superfast_new(NULL)

#define VALUE_ADD(name, val) \
     v = calloc(1, sizeof(Evolve_Widget_Packing_Option_Value)); \
   v->value = val; \
   eina_hash_add(o->values, #name, v)

#define INFO_NEW \
   i = calloc(1, sizeof(Evolve_Widget_Packing_Info)); \
   i->info = eina_hash_string_superfast_new(NULL)

#define INFO_ADD(name) eina_hash_add(i->info, #name, o)

#define INFOS_ADD(name) eina_hash_add(_evolve_widget_packing_infos, #name, i)

#define WIDGET_TYPE_ADD(ev_str, e_str, e_type) \
   type = calloc(1, sizeof(Evolve_Widget_Type)); \
   type->evolve_str = strdup(#ev_str); \
   type->etk_str = strdup(#e_str); \
   type->etk_type = e_type; \
   type->packing_options = eina_hash_string_small_new(NULL);	\
   type->internal_props = eina_hash_string_small_new(NULL);	\
   eina_hash_add(_evolve_widget_types, #ev_str, type)

extern Eina_List *widgets;
extern Eina_List *_evolve_widgets_show_all;
extern Evolve *_evolve_ev;

static Eina_Hash *_evolve_widget_packing_infos = NULL;
static Eina_Hash *_evolve_widget_types = NULL;
void *handle = NULL;

static Eina_Bool evolve_widget_properties_apply_foreach(const Eina_Hash *hash, const void *key, void *data, void *fdata);
static Eina_Bool evolve_widget_container_interal_props_set_foreach(const Eina_Hash *hash, const void *key, void *data, void *fdata);
static Eina_Bool evolve_widget_all_internal_props_set_foreach(const Eina_Hash *hash, const void *key, void *data, void *fdata);
static Eina_Bool evolve_widget_packing_property_to_str_convert_foreach(const Eina_Hash *hash, const void *key, void *data, void *fdata);
static Eina_Bool evolve_widget_code_get_packing_props_foreach(const Eina_Hash *hash, const void *key, void *data, void *fdata);
static Eina_Bool evolve_widget_code_get_props_foreach(const Eina_Hash *hash, const void *key, void *data, void *fdata);

/* populate widget packing info hashes (used at init time) */
void evolve_widget_packing_info_populate()
{
   Evolve_Widget_Packing_Info *i;
   Evolve_Widget_Packing_Option *o;
   Evolve_Widget_Packing_Option_Value *v;

   if (!_evolve_widget_packing_infos)
     _evolve_widget_packing_infos = eina_hash_string_superfast_new(NULL);

   /* box packing info */
   INFO_NEW;
   
   OPTION_NEW;
   VALUE_ADD(__default__, ETK_BOX_START);
   VALUE_ADD(start, ETK_BOX_START);
   VALUE_ADD(end, ETK_BOX_END);
   INFO_ADD(group);
   
   OPTION_NEW;
   VALUE_ADD(__default__, ETK_BOX_EXPAND_FILL);
   VALUE_ADD(none, ETK_BOX_NONE);
   VALUE_ADD(fill, ETK_BOX_FILL);
   VALUE_ADD(expand, ETK_BOX_EXPAND);
   VALUE_ADD(expand_fill, ETK_BOX_EXPAND_FILL);
   VALUE_ADD(shrink_opposite, ETK_BOX_SHRINK_OPPOSITE);
   INFO_ADD(fill_policy);
   
   INFOS_ADD(box);
   
   /* table packing info */
   INFO_NEW;
   
   OPTION_NEW;
   VALUE_ADD(__default__, ETK_TABLE_EXPAND_FILL);
   VALUE_ADD(none, ETK_TABLE_NONE);
   VALUE_ADD(hfill, ETK_TABLE_HFILL);
   VALUE_ADD(vfill, ETK_TABLE_VFILL);
   VALUE_ADD(hexpand, ETK_TABLE_HEXPAND);
   VALUE_ADD(vexpand, ETK_TABLE_VEXPAND);
   VALUE_ADD(fill, ETK_TABLE_FILL);
   VALUE_ADD(expand, ETK_TABLE_EXPAND);
   VALUE_ADD(expand_fill, ETK_TABLE_EXPAND_FILL);
   INFO_ADD(fill_policy);
   
   INFOS_ADD(table);   
}

/* populate widget type hashes (used at init time) */
void evolve_widget_types_populate()
{
   Evolve_Widget_Type *type;
   
   if (!_evolve_widget_types)
     _evolve_widget_types = eina_hash_string_superfast_new(NULL);

   WIDGET_TYPE_ADD(alignment, Etk_Alignment, ETK_ALIGNMENT_TYPE);
   WIDGET_TYPE_ADD(bin, Etk_Bin, ETK_BIN_TYPE);
   WIDGET_TYPE_ADD(box, Etk_Box, ETK_BOX_TYPE);
   WIDGET_TYPE_ADD(hbox, Etk_HBox, ETK_HBOX_TYPE);
   WIDGET_TYPE_ADD(vbox, Etk_VBox, ETK_VBOX_TYPE);
   WIDGET_TYPE_ADD(button, Etk_Button, ETK_BUTTON_TYPE);
   WIDGET_TYPE_ADD(canvas, Etk_Canvas, ETK_CANVAS_TYPE);
   WIDGET_TYPE_ADD(check_button, Etk_Check_Button, ETK_CHECK_BUTTON_TYPE);
   WIDGET_TYPE_ADD(colorpicker, Etk_Colorpicker, ETK_COLORPICKER_TYPE);
   WIDGET_TYPE_ADD(combobox, Etk_Combobox, ETK_COMBOBOX_TYPE);
   WIDGET_TYPE_ADD(combobox_item, Etk_Combobox_Item, ETK_COMBOBOX_ITEM_TYPE);
   WIDGET_TYPE_ADD(container, Etk_Container, ETK_CONTAINER_TYPE);
   WIDGET_TYPE_ADD(dialog, Etk_Dialog, ETK_DIALOG_TYPE);
   WIDGET_TYPE_ADD(drag, Etk_Drag, ETK_DRAG_TYPE);
   WIDGET_TYPE_ADD(embed, Etk_Embed, ETK_EMBED_TYPE);
   WIDGET_TYPE_ADD(entry, Etk_Entry, ETK_ENTRY_TYPE);
   WIDGET_TYPE_ADD(filechooser_widget, Etk_Filechooser_Widget, ETK_FILECHOOSER_WIDGET_TYPE);
   WIDGET_TYPE_ADD(frame, Etk_Frame, ETK_FRAME_TYPE);   
   WIDGET_TYPE_ADD(iconbox, Etk_Iconbox, ETK_ICONBOX_TYPE);
   //WIDGET_TYPE_ADD(iconbox_grid, Etk_Iconbox_Grid, ETK_ICONBOX_GRID_TYPE);
   WIDGET_TYPE_ADD(image, Etk_Image, ETK_IMAGE_TYPE);
   WIDGET_TYPE_ADD(label, Etk_Label, ETK_LABEL_TYPE);
   WIDGET_TYPE_ADD(menu, Etk_Menu, ETK_MENU_TYPE);
   WIDGET_TYPE_ADD(menu_bar, Etk_Menu_Bar, ETK_MENU_BAR_TYPE);
   WIDGET_TYPE_ADD(menu_item, Etk_Menu_Item, ETK_MENU_ITEM_TYPE);
   WIDGET_TYPE_ADD(menu_item_separator, Etk_Menu_Separator_Item, ETK_MENU_ITEM_SEPARATOR_TYPE);
   WIDGET_TYPE_ADD(menu_item_image, Etk_Menu_Item_Image, ETK_MENU_ITEM_IMAGE_TYPE);
   WIDGET_TYPE_ADD(menu_item_check, Etk_Menu_Item_Check, ETK_MENU_ITEM_CHECK_TYPE);
   WIDGET_TYPE_ADD(menu_item_radio, Etk_Menu_Item_Radio, ETK_MENU_ITEM_RADIO_TYPE);
   WIDGET_TYPE_ADD(menu_shell, Etk_Menu_Shell, ETK_MENU_SHELL_TYPE);
   WIDGET_TYPE_ADD(message_dialog, Etk_Message_Dialog, ETK_MESSAGE_DIALOG_TYPE);
   WIDGET_TYPE_ADD(notebook, Etk_Notebook, ETK_NOTEBOOK_TYPE);
   WIDGET_TYPE_ADD(object, Etk_Object, ETK_OBJECT_TYPE);
   WIDGET_TYPE_ADD(paned, Etk_Paned, ETK_PANED_TYPE);
   WIDGET_TYPE_ADD(hpaned, Etk_HPaned, ETK_HPANED_TYPE);
   WIDGET_TYPE_ADD(vpaned, Etk_VPaned, ETK_VPANED_TYPE);
   WIDGET_TYPE_ADD(popup_window, Etk_Popup_Window, ETK_POPUP_WINDOW_TYPE);
   WIDGET_TYPE_ADD(progress_bar, Etk_Progress_Bar, ETK_PROGRESS_BAR_TYPE);
   WIDGET_TYPE_ADD(radio_button, Etk_Radio_Button, ETK_RADIO_BUTTON_TYPE);
   WIDGET_TYPE_ADD(range, Etk_Range, ETK_RANGE_TYPE);
   WIDGET_TYPE_ADD(scrollbar, Etk_Scrollbar, ETK_SCROLLBAR_TYPE);
   WIDGET_TYPE_ADD(hscrollbar, Etk_HScrollbar, ETK_HSCROLLBAR_TYPE);
   WIDGET_TYPE_ADD(vscrollbar, Etk_VScrollbar, ETK_VSCROLLBAR_TYPE);
   WIDGET_TYPE_ADD(scrolled_view, Etk_Scrolled_View, ETK_SCROLLED_VIEW_TYPE);
   WIDGET_TYPE_ADD(separator, Etk_Separator, ETK_SEPARATOR_TYPE);
   WIDGET_TYPE_ADD(hseparator, Etk_HSeparator, ETK_HSEPARATOR_TYPE);
   WIDGET_TYPE_ADD(vseparator, Etk_VSeparator, ETK_VSEPARATOR_TYPE);
   WIDGET_TYPE_ADD(separator, Etk_Separator, ETK_SEPARATOR_TYPE);
   WIDGET_TYPE_ADD(shadow, Etk_Shadow, ETK_SHADOW_TYPE);
   WIDGET_TYPE_ADD(slider, Etk_Slider, ETK_SLIDER_TYPE);
   WIDGET_TYPE_ADD(hslider, Etk_HSlider, ETK_HSLIDER_TYPE);
   WIDGET_TYPE_ADD(vslider, Etk_VSlider, ETK_VSLIDER_TYPE);
   WIDGET_TYPE_ADD(spinner, Etk_Spinner, ETK_SPINNER_TYPE);
   WIDGET_TYPE_ADD(statusbar, Etk_Statusbar, ETK_STATUSBAR_TYPE);
   WIDGET_TYPE_ADD(string, Etk_String, ETK_STRING_TYPE);
   WIDGET_TYPE_ADD(table, Etk_Table, ETK_TABLE_TYPE);
   WIDGET_TYPE_ADD(text_view, Etk_Text_View, ETK_TEXT_VIEW_TYPE);
   WIDGET_TYPE_ADD(textblock, Etk_Textblock, ETK_TEXTBLOCK_TYPE);
   WIDGET_TYPE_ADD(toggle_button, Etk_Toggle_Button, ETK_TOGGLE_BUTTON_TYPE);
   WIDGET_TYPE_ADD(tool_button, Etk_Tool_Button, ETK_TOOL_BUTTON_TYPE);
   WIDGET_TYPE_ADD(tool_toggle_button, Etk_Tool_Toggle_Button, ETK_TOOL_TOGGLE_BUTTON_TYPE);
   WIDGET_TYPE_ADD(toolbar, Etk_Toolbar, ETK_TOOLBAR_TYPE);
   WIDGET_TYPE_ADD(toplevel, Etk_Toplevel, ETK_TOPLEVEL_TYPE);
   WIDGET_TYPE_ADD(tree, Etk_Tree, ETK_TREE_TYPE);
   WIDGET_TYPE_ADD(tree_col, Etk_Tree_Col, ETK_TREE_COL_TYPE);
   WIDGET_TYPE_ADD(viewport, Etk_Viewport, ETK_VIEWPORT_TYPE);
   WIDGET_TYPE_ADD(widget, Etk_Widget, ETK_WIDGET_TYPE);   
   WIDGET_TYPE_ADD(window, Etk_Window, ETK_WINDOW_TYPE);

   /* add internal properties */
   eina_hash_foreach(_evolve_widget_types, evolve_widget_container_interal_props_set_foreach, NULL);
   eina_hash_foreach(_evolve_widget_types, evolve_widget_all_internal_props_set_foreach, NULL);
}

/* create a new Evolve widget object based on the passed type */
Evolve_Widget *evolve_widget_new(char *type)
{
   Evolve_Widget *widget;
   
   widget = calloc(1, sizeof(Evolve_Widget));
   widget->type = strdup(type);
   widget->props = eina_hash_string_superfast_new(NULL);
   widget->packing_props = eina_hash_string_superfast_new(NULL);
   return widget;
}

/* try to find an evolve widget by its name in the list of current widgets */
Evolve_Widget *evolve_widget_find(char *name)
{
   Evolve_Widget *widget;
   Eina_List *l;

   EINA_LIST_FOREACH(widgets, l, widget)
     if (!strcmp(name, widget->name))
       return widget;

   return NULL;
}

/* find and return a widget's constructor using its type */
Evolve_Ctor evolve_widget_ctor_find(char *type)
{
   Evolve_Ctor func;
   char ctor[512];
   
   if (!handle)
     handle = dlopen(NULL, RTLD_NOW | RTLD_LOCAL);
   if (!handle)
     {
	fprintf(stderr, "Evolve ERROR!!\n"
	       "Error dlopen'ing self: %s\n",
	       dlerror());
	return NULL;
     }

   snprintf(ctor, sizeof(ctor), "_etk_%s_new", type);
   
   func = dlsym(handle, ctor);
   if(!func)
     {
	fprintf(stderr, "Evolve ERROR!!!\n"
	       "Error loading dynamic ctor: %s\n",
	       dlerror());
	return NULL;
     }
   
   return func;
}

/* apply an internal property to the given widget, return 1 on success, 0 otherwise */
int evolve_widget_internal_property_apply(Evolve_Widget *widget, Evolve_Property *prop)
{
   if (!strcmp(prop->name, "show_all"))
     {
	if (evolve_property_value_int_get(prop->default_value) == 1)
	  _evolve_widgets_show_all = eina_list_append(_evolve_widgets_show_all, widget);
	return 1;
     }
   else if ((!strcmp(prop->name, "file") && !strcmp(widget->type, "image") &&
	    !eina_hash_find(widget->props, "key")) ||
	    (!strcmp(prop->name, "image") && !strcmp(widget->type, "button") &&
	     !eina_hash_find(widget->props, "key")))
     {
	char key[PATH_MAX];
		
	snprintf(key, sizeof(key), "/etk/images/%s", evolve_property_value_string_get(prop->default_value));
	if (!strcmp(widget->type, "image"))
	  etk_image_set_from_file(ETK_IMAGE(widget->widget), _evolve_ev->eet_filename, key);
	else if (!strcmp(widget->type, "button"))
	  {
	     Etk_Widget *image;
	     
	     image = etk_image_new_from_file(_evolve_ev->eet_filename, key);
	     etk_button_image_set(ETK_BUTTON(widget->widget), ETK_IMAGE(image));
	  }
	return 1;
     }
   else if (!strcmp(prop->name, "theme-file"))
     {
	char file[PATH_MAX];
	
	if (ecore_file_exists(evolve_property_value_string_get(prop->default_value)))
	  snprintf(file, sizeof(file), "%s", evolve_property_value_string_get(prop->default_value));
	else if (_evolve_ev && _evolve_ev->eet_filename)
	  {
	     char *dir;
	     
	     if ((dir = ecore_file_dir_get(_evolve_ev->eet_filename)))
	       {
		  snprintf(file, sizeof(file), "%s/%s", dir, evolve_property_value_string_get(prop->default_value));
		  free(dir);
		  if (ecore_file_exists(file))
		    goto prop_set;
	       }
	     goto last_attemp;
	  }
	else
	  {
last_attemp:
	     snprintf(file, sizeof(file), "./%s", evolve_property_value_string_get(prop->default_value));
	  }	  
	
prop_set:	
	etk_object_properties_set(ETK_OBJECT(widget->widget), prop->name, file, NULL);
	return 1;
     }
   else if (!strcmp(prop->name, "swallow_part"))
     {
	Evolve_Widget *swallow_parent;
	
	if (!widget->parent ||
	    !(swallow_parent = eina_hash_find(_evolve_ev->parents, widget->parent)))
	  return 1;
	
	if (!swallow_parent->widget)
	  return 1;
	
	etk_widget_parent_set(widget->widget, swallow_parent->widget);
	if (!etk_widget_swallow_widget(swallow_parent->widget, evolve_property_value_string_get(prop->default_value), widget->widget))
	  fprintf(stderr, "EVOLVE WARNING!!\n"
		  "Could not swallow widget %s into part %s of widget %s: Error code %d\n",
		  widget->name, evolve_property_value_string_get(prop->default_value), swallow_parent->name,
		  etk_widget_swallow_error_get());
	return 1;
     }
   return 0;
}

/* apply all the properties on the given widget */
void evolve_widget_properties_apply(Evolve_Widget *widget)
{
   if (!widget || !widget->props)
     return;
   
   eina_hash_foreach(widget->props, evolve_widget_properties_apply_foreach,
		     widget);   
}

/* apply all the properties on the given widget */
void evolve_widget_property_apply(Evolve_Widget *widget, Evolve_Property *prop)
{
   if (!widget || !widget->props)
     return;

   if (!prop->default_value)
     return;
   
   if (evolve_widget_internal_property_apply(widget, prop))
     return;
   
   switch (evolve_property_type_get(prop))
     {
      case EVOLVE_PROPERTY_INT:
	etk_object_properties_set(ETK_OBJECT(widget->widget), prop->name, evolve_property_value_int_get(prop->default_value), NULL);
	break;
      case EVOLVE_PROPERTY_DOUBLE:
	etk_object_properties_set(ETK_OBJECT(widget->widget), prop->name, evolve_property_value_double_get(prop->default_value), NULL);
	break;
      case EVOLVE_PROPERTY_STRING:
	etk_object_properties_set(ETK_OBJECT(widget->widget), prop->name, evolve_property_value_string_get(prop->default_value), NULL);
	break;
      default:
	printf("Unkown property type: %s %s %d\n", widget->name, widget->type,
	       evolve_property_type_get(prop));
	break;
     }         
}

/* internal rendering function that works on specific evolve side render operations */
int evolve_widget_internal_render(Evolve_Widget *widget)
{
   if (!strcmp(widget->type, "tree_col"))
     {
	Evolve_Widget *parent;
	
	if (!widget->parent ||
	    !(parent = eina_hash_find(_evolve_ev->parents, widget->parent)))
	  return 1;
				
	widget->widget = (Etk_Widget*)etk_tree_col_new(ETK_TREE(parent->widget), "col",
							 10, 0.0);
	return 1;
     }
   else if (!strcmp(widget->type, "tree_model_text"))
     {
	Evolve_Widget *parent;
	
	if (!widget->parent ||
	    !(parent = eina_hash_find(_evolve_ev->parents, widget->parent)))
	  return 1;
	
	etk_tree_col_model_add(ETK_TREE_COL(parent->widget), 
			       etk_tree_model_text_new());
	widget->widget = NULL;
	
	return 1;
     }
 
   return 0;
}

/* post rendering function ran by evolve itself */
void evolve_widget_post_render(Evolve_Widget *widget)
{
   if (!widget || !widget->type)
     return;
   
   if (!strcmp(widget->type, "tree") && widget->widget)
     {
	etk_tree_build(ETK_TREE(widget->widget));
     }   
}

/* render a widget calling evolve's internal render first */
void evolve_widget_render(Evolve_Widget *widget)
{
   Evolve_Ctor ctor;
   
   if (!widget || !widget->type)
     return;
   
   if (evolve_widget_internal_render(widget))
     return;
   
   if (!(ctor = evolve_widget_ctor_find(widget->type)))
     return;
   
   widget->widget = ctor();   
}

/* render a widget's children */
void evolve_widget_children_render(Evolve *evolve, Evolve_Widget *widget)
{
   Eina_List *l;
   
   if (!evolve || !evolve->widgets || !widget || !widget->widget)
     return;
   
   for (l = evolve->widgets; l; l = l->next)
     {
	Evolve_Widget *child;
	
	child = l->data;
	if (!child->parent || strcmp(widget->name, child->parent))
	  continue;
	
	evolve_widget_render(child);
	evolve_widget_properties_apply(child);
	evolve_widget_reparent(child, widget);
	evolve_widget_children_render(evolve, child);
     }
}

/* connect all signals to a given widget */
void evolve_widget_signals_connect(Evolve_Widget *widget, Evolve *evolve)
{
   Eina_List *l;
   void (*callback)(void);
   void *data = NULL;
   Evolve_Widget_Signal *sig;

   EINA_LIST_FOREACH(widget->signals, l, sig)
     {
	if (!sig->name || (!sig->callback && !sig->emit))
	  {
	     fprintf(stderr, "Signal does not have a name or callback, ignored.\n");
	     return;
	  }
	
	if (sig->emit && !eina_hash_find(evolve->emissions, sig->emit))
	  {
	     Evolve_Signal *esig;
	     
	     esig = evolve_signal_new(sig->name, sig->emit, evolve);
	     etk_signal_connect_swapped(sig->name, ETK_OBJECT(widget->widget), ETK_CALLBACK(evolve_signal_emit_cb), esig);
	     return;
	  }
	
	if (!handle)
	  handle = dlopen(NULL, RTLD_NOW | RTLD_LOCAL);
	if (!handle)
	  {
	     fprintf(stderr, "Evolve ERROR!!\n"
		     "Error dlopen'ing self: %s\n",
		     dlerror());
	     return;
	  }
   
	callback = dlsym(handle, sig->callback);
	if(!callback)
	  {
	     fprintf(stderr, "Evolve ERROR!!!\n"
		     "Error loading dynamic callback: %s\n",
		     dlerror());
	     return;
	  }
		
	if (sig->data_type)
	  {	    
	     if (!strcmp(sig->data_type, "widget"))
	       {
		  Etk_Widget *etk_widget = NULL;
		  		  
		  if (sig->data_name)		  
		    etk_widget = evolve_etk_widget_find(evolve, sig->data_name);
		  data = etk_widget;		  
	       }
	     else
	       {
		  data = dlsym(handle, sig->data_name);
	       }
	  }
	if (sig->swapped)
	  etk_signal_connect_swapped(sig->name, ETK_OBJECT(widget->widget), ETK_CALLBACK(callback), data);
	else
	  etk_signal_connect(sig->name, ETK_OBJECT(widget->widget), ETK_CALLBACK(callback), data);
     }
   
}

/* reparent a widge by packing it into its parent */
void evolve_widget_reparent(Evolve_Widget *child, Evolve_Widget *parent)
{     
   if (!child || !child->widget || !parent || !parent->widget)
     return;
   
   if (eina_hash_find(child->props, "swallow_part"))
     return;
   
   /* window packing */
   /* frame packing */
   if (!strcmp(parent->type, "window") || !strcmp(parent->type, "frame") ||
       !strcmp(parent->type, "shadow") || !strcmp(parent->type, "viewport") ||
       !strcmp(parent->type, "scrolled_view"))
     etk_container_add(ETK_CONTAINER(parent->widget), child->widget);
   /* paned packing */
   else if (!strcmp(parent->type, "hpaned") || !strcmp(parent->type, "vpaned"))
     {
	int position;
	position = evolve_widget_packing_property_int_find(child, "child", -1);
	if (position == 1)
	  {
	     etk_paned_child1_set(ETK_PANED(parent->widget), child->widget,
	      evolve_widget_packing_property_int_find(child, "expand", 1));
	  }
	else
	  {
	     etk_paned_child2_set(ETK_PANED(parent->widget), child->widget,
	      evolve_widget_packing_property_int_find(child, "expand", 1));	     
	  }
     }
   /* notebook */
   else if (!strcmp(parent->type, "notebook"))
     {
	int position;
		
	position = evolve_widget_packing_property_int_find(child, "position", -1);
	/* prepend */
	if (evolve_widget_packing_property_int_find(child, "prepend", 0))
	  {
	     etk_notebook_page_prepend(ETK_NOTEBOOK(parent->widget),
              evolve_widget_packing_property_str_find(child, "tab_label", NULL),
	      child->widget);
	  }
	/* insert */
	else if (position != -1)
	  {
	     etk_notebook_page_insert(ETK_NOTEBOOK(parent->widget),
              evolve_widget_packing_property_str_find(child, "tab_label", NULL),
	      child->widget, position);
	  }
	/* append */
	else
	  {
	     etk_notebook_page_append(ETK_NOTEBOOK(parent->widget),
              evolve_widget_packing_property_str_find(child, "tab_label", NULL),
	      child->widget);
	  }
     }
   /* box packing */
   else if (!strcmp(parent->type, "hbox") || !strcmp(parent->type, "vbox"))
     {
	/* prepend */
	if (evolve_widget_packing_property_int_find(child, "prepend", 0))
	  {
	     etk_box_append(ETK_BOX(parent->widget), child->widget, 
	      evolve_widget_packing_property_to_int_convert("box", "group",
	       evolve_widget_packing_property_str_find(child, "group", NULL)),
	      evolve_widget_packing_property_to_int_convert("box", "fill_policy",
	       evolve_widget_packing_property_str_find(child, "fill_policy", NULL)),
	      evolve_widget_packing_property_int_find(child, "padding", 0));
	  }
	/* insert at position */
	else if (evolve_widget_packing_property_int_find(child, "position",  -1) >= 0)
	  {
	     etk_box_insert_at(ETK_BOX(parent->widget), child->widget, 
	      evolve_widget_packing_property_to_int_convert("box", "group",
	       evolve_widget_packing_property_str_find(child, "group", NULL)),
	      evolve_widget_packing_property_int_find(child, "position", 0),
	      evolve_widget_packing_property_to_int_convert("box", "fill_policy",
	       evolve_widget_packing_property_str_find(child, "fill_policy", NULL)),
	      evolve_widget_packing_property_int_find(child, "padding", 0));
	  }
	/* append */
	else
	  {
	     etk_box_append(ETK_BOX(parent->widget), child->widget, 
	      evolve_widget_packing_property_to_int_convert("box", "group",
	       evolve_widget_packing_property_str_find(child, "group", NULL)),
	      evolve_widget_packing_property_to_int_convert("box", "fill_policy",
	       evolve_widget_packing_property_str_find(child, "fill_policy", NULL)),
	      evolve_widget_packing_property_int_find(child, "padding", 0));
	  }
     }
   /* table packing */
   else if (!strcmp(parent->type, "table"))
     {	
	if(evolve_widget_packing_property_int_find(child, "fill_policy", -1) != -1)
	  {
	     etk_table_attach(ETK_TABLE(parent->widget), child->widget, 
	      evolve_widget_packing_property_int_find(child, "left_attach", 0),
	      evolve_widget_packing_property_int_find(child, "right_attach", 0),
	      evolve_widget_packing_property_int_find(child, "top_attach", 0),
	      evolve_widget_packing_property_int_find(child, "bottom_attach", 0),
	      evolve_widget_packing_property_to_int_convert("table", "fill_policy",
	       evolve_widget_packing_property_str_find(child, "fill_policy", NULL)),
	      evolve_widget_packing_property_int_find(child, "x_padding", 0),
	      evolve_widget_packing_property_int_find(child, "x_padding", 0));
	  }
	else
	  {
	     etk_table_attach_default(ETK_TABLE(parent->widget), child->widget,
	      evolve_widget_packing_property_int_find(child, "left_attach", 0),
	      evolve_widget_packing_property_int_find(child, "right_attach", 0),
	      evolve_widget_packing_property_int_find(child, "top_attach", 0),
	      evolve_widget_packing_property_int_find(child, "bottom_attach", 0));
	  }
     }
   /* toolbar packing */
   else if (!strcmp(parent->type, "toolbar"))
     {
	if (evolve_widget_packing_property_int_find(child, "prepend", 0))	  
	  etk_toolbar_prepend(ETK_TOOLBAR(parent->widget), child->widget, ETK_BOX_START);
	/* insert at position */
	else if (evolve_widget_packing_property_int_find(child, "position",  -1) >= 0)	  
	  etk_toolbar_insert_at(ETK_TOOLBAR(parent->widget), child->widget, ETK_BOX_START,
	   evolve_widget_packing_property_int_find(child, "position", 0));
	/* append */
	else
	  etk_toolbar_append(ETK_TOOLBAR(parent->widget), child->widget, ETK_BOX_START);
     }
}

/* try to find the packing property by name for a widget, return default value if not found */
char *evolve_widget_packing_property_str_find(Evolve_Widget *widget, char *name, char *def)
{
   Evolve_Widget_Property *prop;
      
   prop = eina_hash_find(widget->packing_props, name);
   if (!prop || !prop->value)
     {
	return def;
     }
   else 
     {
	return prop->value;
     }
}

/* try to find the packing property by name for a widget, return default value if not found */
int evolve_widget_packing_property_int_find(Evolve_Widget *widget, char *name, int def)
{
   Evolve_Widget_Property *prop;
   prop = eina_hash_find(widget->packing_props, name);
   if (!prop || !prop->value) 
     return def;
   else return atoi(prop->value);
}

/* try to find the packing property by name for a widget, return default value if not found */
double evolve_widget_packing_property_double_find(Evolve_Widget *widget, char *name, double def)
{
   Evolve_Widget_Property *prop;
   prop = eina_hash_find(widget->packing_props, name);
   if (!prop || !prop->value) 
     return def;
   else return atof(prop->value);
}

/* convert the given packing property to its internal evolve string representation */
char *evolve_widget_packing_property_to_str_convert(char *widget_type, char *prop_name, int prop_value)
{
   Evolve_Widget_Packing_Option *o;
   Evolve_Widget_Packing_Info *i;
   Evolve_Widget_Property *prop;
   char *ret;
   
   if (!(i = eina_hash_find(_evolve_widget_packing_infos, widget_type)))
     return 0;
   if (!(o = eina_hash_find(i->info, prop_name)))
     return 0;
   
   /* We are using this structure out of place here for our special needs */
   prop = calloc(1, sizeof(Evolve_Widget_Property));
   prop->type = prop_value;
   
   eina_hash_foreach(o->values, evolve_widget_packing_property_to_str_convert_foreach, prop);

   if (!prop->value)
     ret = NULL;
   else
     ret = strdup(prop->value);
   
   free(prop->value);
   free(prop);
   
   return ret;
}

/* convert the given packing property from its internal evolve string name to its etk int value */
int evolve_widget_packing_property_to_int_convert(char *widget_type, char *prop_name, char *prop_value)
{
   Evolve_Widget_Packing_Option_Value *v;
   Evolve_Widget_Packing_Option *o;
   Evolve_Widget_Packing_Info *i;
   
   if (!(i = eina_hash_find(_evolve_widget_packing_infos, widget_type)))
     return 0;
   if (!(o = eina_hash_find(i->info, prop_name)))
     return 0;
   if (!(v = eina_hash_find(o->values, prop_value)))
     return 0;
   return v->value;
}

/* convert a widget to evolve code */
char *evolve_widget_code_get(Evolve_Widget *widget)
{
#if 0   
   Etk_String *code;
   char *ret;
   
   if (!widget || !widget->name || !widget->type)
     return NULL;
   
   code = etk_string_new_printf(""
				"widget\n"
				"{\n"
				"   type: \"%s\";\n"
				"   name: \"%s\";\n",
				widget->type, widget->name);
   
   if (widget->parent)
     code = etk_string_append_printf(code, "   parent: \"%s\";\n",
				     widget->parent);
   
   if (widget->props)
     {
	eina_hash_foreach(widget->props, evolve_widget_code_get_props_foreach,
			  &code);
     }
   
   if (widget->packing_props)
     {
	code = etk_string_append_printf(code,
					"   packing\n"
					"   {\n");
	eina_hash_foreach(widget->packing_props, evolve_widget_code_get_packing_props_foreach,
			  &code);	
	code = etk_string_append_printf(code,
					"   }\n");	
     }
   
   if (widget->signals)
     {
	Eina_List *l;
	
	for (l = widget->signals; l; l = l->next)
	  {
	     Evolve_Widget_Signal *sig;
	     
	     sig = l->data;
	     code = etk_string_append_printf(code,
					     "   signal\n"
					     "   {\n");
	     
	     code = etk_string_append_printf(code,
					     "      name: \"%s\";\n",
					     sig->name);
	     if (sig->callback)
	       code = etk_string_append_printf(code,
					       "      callback: \"%s\";\n",
					       sig->callback);
	     if (sig->emit)
	       code = etk_string_append_printf(code,
					       "      emit: \"%s\";\n",
					       sig->emit);
	     
	     if (sig->swapped)
	       code = etk_string_append_printf(code,
					       "      swapped: %d;\n",
					       sig->swapped);
	     
	     if (sig->data_type && sig->data_name)
	       {
		  code = etk_string_append_printf(code,
						  "      data\n"
						  "      {\n");
		  code = etk_string_append_printf(code,
						  "         type: \"%s\";\n",
						  sig->data_type);
		  code = etk_string_append_printf(code,
						  "         name: \"%s\";\n",
						  sig->data_name);
		  code = etk_string_append_printf(code,
						  "      }\n");
	       }
	     code = etk_string_append_printf(code,
					     "   }\n");
	  }
     }
   
   code = etk_string_append_printf(code,
				   "}\n");   
   
   ret = strdup(etk_string_get(code));
   etk_object_destroy(ETK_OBJECT(code));
   return ret;
#endif
   return NULL;
}

/* sort the list of widgets in a tree like fashion where every parent is followed by its children, toplevels first */
Eina_List *evolve_widget_list_sort(Eina_List *widgets)
{
   Eina_List *l;
   Eina_List *sorted = NULL;
   Evolve_Widget *widget;
  
   if (!widgets || !widgets->data)
     return NULL;
   
   l = widgets;
   widget = widgets->data;
   while(widget)
     {		
	if (!widget->parent)
	  {
	     sorted = eina_list_prepend(sorted, widget);
	     widgets = eina_list_remove(widgets, widget);
	     if (!widgets)
	       widget = NULL;
	     else
	       widget = widgets->data;
	     continue;
	  }
	
	for (l = sorted; l; l = l->next)
	  {
	     Evolve_Widget *parent;
	     
	     parent = l->data;
	     if (!strcmp(widget->parent, parent->name))
	       {
		  sorted = eina_list_append_relative(sorted, widget, parent);
		  widgets = eina_list_remove(widgets, widget);
		  if (!widgets)
		    {
		       widget = NULL;
		       goto leave_for;
		    }
		  else
		    {
		       widget = widgets->data;
		       goto leave_for;
		    }
	       }
	  }
	
leave_for:
	
	if (!widgets)
	  {
	     widget = NULL;
	     continue;
	  }
	
	if (eina_list_next(eina_list_data_find_list(widgets, widget)))
	  widget = eina_list_data_get(eina_list_next(eina_list_data_find_list(widgets, widget)));
	else
	  widget = eina_list_data_get(widgets);
     }
   
   eina_list_free(widgets);
   return sorted;
}

/* set the given property on the given widget */
void evolve_widget_property_set(Evolve_Widget *widget, char *name, char *value, int type)
{
   Evolve_Property *prop;
   
   if (!widget)
     return;
   
   if (widget->props && (prop = eina_hash_find(widget->props, name)))
     evolve_property_value_delete(prop->default_value);
   else
     {
	prop = calloc(1, sizeof(Evolve_Property));
	prop->name = strdup(name);
	prop->type = type;
	prop->default_value = NULL;
	eina_hash_add(widget->props, name, prop);
     }
   
   switch(prop->type)
     {
      case EVOLVE_PROPERTY_INT:
	prop->default_value = evolve_property_value_int(atoi(value));
	break;
      case EVOLVE_PROPERTY_DOUBLE:
	prop->default_value = evolve_property_value_double(atof(value));
	break;
      case EVOLVE_PROPERTY_STRING:
	prop->default_value = evolve_property_value_string(value);	
	break;
      default:
	prop->default_value = NULL;
     }
   if (widget->widget)
     evolve_widget_property_apply(widget, prop);
}

/* find a property for the supplied widget, return it, or return NULL if not found */
Evolve_Property *evolve_widget_property_get(Evolve_Widget *widget, char *prop_name)
{
   if (!widget || !widget->props)
     return NULL;
   
   return eina_hash_find(widget->props, prop_name);
}

/* get an etk type given the evolve type */
Etk_Type *evolve_widget_type_to_etk(char *type)
{
   Evolve_Widget_Type *wtype;
   
   if (!type || !(wtype = eina_hash_find(_evolve_widget_types, type)))
     return NULL;
   
   return wtype->etk_type;
}

/* find wether a property exists in the given evolve widget type */
int evolve_widget_type_property_exists(char *type, char *prop_name)
{   
   Etk_Type *property_owner;
   Evolve_Property *property;
   
   if (etk_type_property_find(evolve_widget_type_to_etk(type), prop_name, 
			      &property_owner, &property)
       || evolve_widget_type_internal_property_exists(type, prop_name))
     return 1;
   
   return 0;
}

/* find wether an interal property exists for the given evolve type */
int evolve_widget_type_internal_property_exists(char *type, char *prop_name)
{
   Evolve_Widget_Type *wtype;
   
   if (!type || !prop_name || !(wtype = eina_hash_find(_evolve_widget_types, type)))
     return 0;
   
   if(eina_hash_find(wtype->internal_props, prop_name))
     return 1;
   return 0;
}
       
static Eina_Bool evolve_widget_container_interal_props_set_foreach(const Eina_Hash *hash, const void *key, void *data, void *fdata)
{
   Evolve_Widget_Type *type;
   
   type = data;
   
   if (etk_type_inherits_from(type->etk_type, ETK_CONTAINER_TYPE))
     eina_hash_add(type->internal_props, "show_all", strdup("show_all"));
   
   return 1;
}

static Eina_Bool evolve_widget_all_internal_props_set_foreach(const Eina_Hash *hash, const void *key, void *data, void *fdata)
{
   Evolve_Widget_Type *type;
   
   type = data;     
   eina_hash_add(type->internal_props, "swallow_part", strdup("swallow_part"));
   
   return 1;
}

static Eina_Bool evolve_widget_properties_apply_foreach(const Eina_Hash *hash, const void *key, void *data, void *fdata)
{
   Evolve_Widget *widget;
   Evolve_Property *prop;
   
   widget = fdata;
   prop = data;

   evolve_widget_property_apply(widget, prop);
   return 1;      
}

static Eina_Bool evolve_widget_packing_property_to_str_convert_foreach(const Eina_Hash *hash, const void *key, void *data, void *fdata)
{
   Evolve_Widget_Packing_Option_Value *v;
   Evolve_Widget_Property *prop;
   
   v = data;
   prop = fdata;
   if (v->value == prop->type)
     {
	prop->value = strdup(key);
	return 0;
     }
   return 1;
}

static Eina_Bool evolve_widget_code_get_packing_props_foreach(const Eina_Hash *hash, const void *key, void *data, void *fdata)
{
   Etk_String **code;
   Evolve_Widget_Property *prop;
   
   code = fdata;
   prop = data;
   
   if (prop->type == EVOLVE_WIDGET_PROP_STR)
     *code = etk_string_append_printf(*code,
				      "      %s: \"%s\";\n",
				      key, prop->value);
   else
     *code = etk_string_append_printf(*code,
				      "      %s: %s;\n",
				      key, prop->value);
   return 1;
}

static Eina_Bool evolve_widget_code_get_props_foreach(const Eina_Hash *hash, const void *key, void *data, void *fdata)
{
#if 0   
   Etk_String **code;
   Evolve_Widget_Property *prop;
   
   code = fdata;
   prop = data;
   
   if (prop->type == EVOLVE_WIDGET_PROP_STR)
     *code = etk_string_append_printf(*code,
				      "   %s: \"%s\";\n",
				      key, prop->value);
   else
     *code = etk_string_append_printf(*code,
				      "   %s: %s;\n",
				      key, prop->value);
#endif   
   return 1;
}
