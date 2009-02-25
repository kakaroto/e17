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

#include "enhance_private.h"

#define BOX_FILL_POLICY(expand, fill) \
   (((expand) ? ETK_BOX_EXPAND : ETK_BOX_NONE) | ((fill) ? ETK_BOX_FILL : ETK_BOX_NONE))

static E_Widget *_e_widget_new(Enhance *en, EXML_Node *node, Etk_Widget *etk_widget, char *id);
static E_Widget *_e_widget_window_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_dialog_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_scrolled_view_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_viewport_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_vbox_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_hbox_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_alignment_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_table_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_tree_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_notebook_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_frame_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_hpaned_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_vpaned_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_label_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_image_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_button_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_check_button_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_toggle_button_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_radio_button_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_entry_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_progress_bar_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_statusbar_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_menu_bar_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_menu_item_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_menu_image_item_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_menu_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_hseparator_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_vseparator_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_hslider_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_vslider_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_textview_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_filechooser_widget_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_iconview_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_toolbar_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_tool_button_handle(Enhance *en, EXML_Node *node);
static E_Widget *_e_widget_tool_toggle_button_handle(Enhance *en, EXML_Node *node);

static EXML_Node *find_node(EXML_Node *node, char *key, char *value)
{
   Ecore_List *props;
   EXML_Node  *prop;
   
   props = node->children;
   ecore_list_first_goto(props);
   prop = ecore_list_current(props);
   while(prop != NULL)
     {
	if(ecore_hash_get(prop->attributes, key))
	  {
	     if(!strcmp(ecore_hash_get(prop->attributes, key), value))
		  return prop;	     
	  }
	if(prop->children)
	  {
	     EXML_Node *ret;
	     if((ret = find_node(prop, key, value)))
	       return ret;
	  }
		
	prop = ecore_list_next(props);	
     }   
   return NULL;
}

static E_Widget *
_e_widget_new(Enhance *en, EXML_Node *node, Etk_Widget *etk_widget, char *id)
{
   E_Widget  *widget;
   
   widget = E_NEW(1, E_Widget);
   widget->wid = etk_widget;
   widget->node = node;
   widget->packing = NULL;
   eina_hash_add(en->widgets, id, widget);

   return widget;
}

static E_Widget *
_e_widget_window_handle(Enhance *en, EXML_Node *node)
{
   E_Widget  *win;
   char      *id;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;

   win = _e_widget_new(en, node, etk_window_new(), id);
   
   if(!strcmp(en->main_window, id))   
     etk_widget_show_all(win->wid);
   
   return win;
}

static E_Widget *
_e_widget_dialog_handle(Enhance *en, EXML_Node *node)
{
   E_Widget  *dia;
   char      *id;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;

   dia = _e_widget_new(en, node, etk_dialog_new(), id);
   return dia;
}

static E_Widget *
_e_widget_scrolled_view_handle(Enhance *en, EXML_Node *node)
{
   E_Widget  *view;
   char      *id;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
   
   view = _e_widget_new(en, node, etk_scrolled_view_new(), id);
   
   return view;
}

static E_Widget *
_e_widget_viewport_handle(Enhance *en, EXML_Node *node)
{
   E_Widget  *port;
   char      *id;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
   
   port = _e_widget_new(en, node, etk_viewport_new(), id);
   
   return port;
}

static E_Widget *
_e_widget_vbox_handle(Enhance *en, EXML_Node *node)
{
   E_Widget  *vbox;
   char      *id;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
   
   vbox = _e_widget_new(en, node, etk_vbox_new(ETK_FALSE, 0), id);
   
   return vbox;
}

static E_Widget *
_e_widget_hbox_handle(Enhance *en, EXML_Node *node)
{
   E_Widget  *hbox;
   char      *id;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
   
   hbox = _e_widget_new(en, node, etk_hbox_new(ETK_FALSE, 0), id);

   return hbox;
}

static E_Widget *
_e_widget_alignment_handle(Enhance *en, EXML_Node *node)
{
   E_Widget  *align;
   char      *id;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
   
   align = _e_widget_new(en, node, etk_alignment_new(0.0, 0.0, 0.0, 0.0), id);
   
   return align;
}

static E_Widget *
_e_widget_table_handle(Enhance *en, EXML_Node *node)
{
   E_Widget   *table;
   char       *id;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
   
   table = _e_widget_new(en, node, etk_table_new(1, 1, ETK_TABLE_NOT_HOMOGENEOUS), id);
   
   return table;
}

static E_Widget *
_e_widget_tree_handle(Enhance *en, EXML_Node *node)
{
   E_Widget   *tree;
   char       *id;

   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
   
   tree = _e_widget_new(en, node, etk_tree_new(), id);

   return tree;
}

static E_Widget *
_e_widget_notebook_handle(Enhance *en, EXML_Node *node)
{
   E_Widget   *notebook;
   char       *id;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
    
   notebook = _e_widget_new(en, node, etk_notebook_new(), id);
   
   return notebook;
}

static E_Widget *
_e_widget_frame_handle(Enhance *en, EXML_Node *node)
{
   E_Widget   *frame;
   char       *id;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
      
   frame = _e_widget_new(en, node, etk_frame_new(NULL), id);
   
   return frame;
}

static E_Widget *
_e_widget_hpaned_handle(Enhance *en, EXML_Node *node)
{
   E_Widget   *paned;
   char       *id;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
      
   paned = _e_widget_new(en, node, etk_hpaned_new(), id);
   
   return paned;
}

static E_Widget *
_e_widget_vpaned_handle(Enhance *en, EXML_Node *node)
{
   E_Widget   *paned;
   char       *id;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
      
   paned = _e_widget_new(en, node, etk_vpaned_new(), id);
   
   return paned;
}

static E_Widget *
_e_widget_image_handle(Enhance *en, EXML_Node *node)
{
   E_Widget   *img;
   char       *id;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
      
   img = _e_widget_new(en, node, etk_image_new(), id);
   
   return img;
}

static E_Widget *
_e_widget_label_handle(Enhance *en, EXML_Node *node)
{
   E_Widget   *label;
   char       *id;   
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
   
   label = _e_widget_new(en, node, etk_label_new(NULL), id);
   
   return label;
}

static E_Widget *
_e_widget_button_handle(Enhance *en, EXML_Node *node)
{
   E_Widget  *button;
   char      *id;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
   
   button = _e_widget_new(en, node, etk_button_new(), id);

   return button;
}

static E_Widget *
_e_widget_check_button_handle(Enhance *en, EXML_Node *node)
{
   E_Widget  *button;
   char      *id;   
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
   
   button = _e_widget_new(en, node, etk_check_button_new(), id);
   
   return button;
}

static E_Widget *
_e_widget_toggle_button_handle(Enhance *en, EXML_Node *node)
{
   E_Widget  *button;
   char      *id;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
   
   button = _e_widget_new(en, node, etk_toggle_button_new(), id);
   
   return button;
}

static E_Widget *
_e_widget_radio_button_handle(Enhance *en, EXML_Node *node)
{
   E_Widget   *button;
   Etk_Widget *w = NULL;
   char       *label = NULL;
   char       *group = NULL;   
   char       *id;
   Ecore_List *props;
   EXML_Node  *prop;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
   
   props = node->children;
   ecore_list_first_goto(props);
   prop = ecore_list_current(props);
   while(prop != NULL)
     {
	if(!strcmp(prop->tag, "property"))
	  {
	     char *name;
	     
	     name = ecore_hash_get(prop->attributes, "name");
	     if(!name) { prop = ecore_list_next(props); continue; }
	     
	     if(!strcmp(name, "label"))
	       {
		  if(prop->value)
		    label = strdup(prop->value);
	       }
	     else if(!strcmp(name, "group"))
	       {
		  if(prop->value)
		    group = strdup(prop->value);
	       }
	  }
	prop = ecore_list_next(props);	
     }
   
   ecore_list_first_goto(props);   
      
   if(group)     	
     w = eina_hash_find(en->radio_groups, group);
   
   if(label)
     {
	if(w)     
	  button = _e_widget_new(en, node, 
			   etk_radio_button_new_with_label_from_widget(label, 
			   ETK_RADIO_BUTTON(w)),
			   id);
	else
	  button = _e_widget_new(en, node, 
				 etk_radio_button_new_with_label(label, NULL),
				 id);
     }
   else
     {
	if(w)     
	  button = _e_widget_new(en, node, 
			etk_radio_button_new_from_widget(ETK_RADIO_BUTTON(w)), 
			id);
	else
	  button = _e_widget_new(en, node, etk_radio_button_new(NULL), id);
     }
   
   if(!group)
     eina_hash_add(en->radio_groups, id, button->wid);

   E_FREE(label);
   E_FREE(group);
   
   return button;
}

static E_Widget *
_e_widget_entry_handle(Enhance *en, EXML_Node *node)
{
   E_Widget  *entry;
   char      *id;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
      
   entry = _e_widget_new(en, node, etk_entry_new(), id);
         
   return entry;
}

static E_Widget *
_e_widget_progress_bar_handle(Enhance *en, EXML_Node *node)
{
   E_Widget  *bar;
   char      *id;

   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
      
   bar = _e_widget_new(en, node, etk_progress_bar_new(), id);
   
   return bar;
}

static E_Widget *
_e_widget_statusbar_handle(Enhance *en, EXML_Node *node)
{
   E_Widget   *bar;
   char       *id;
      
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
      
   bar = _e_widget_new(en, node, etk_statusbar_new(), id);
      
   return bar;
}

static E_Widget *
_e_widget_menu_bar_handle(Enhance *en, EXML_Node *node)
{
   E_Widget  *bar;
   char      *id;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
   
   bar = _e_widget_new(en, node, etk_menu_bar_new(), id);
   
   return bar;
}

static E_Widget *
_e_widget_menu_item_handle(Enhance *en, EXML_Node *node)
{
   E_Widget   *item;
   char       *id;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
   
   item = _e_widget_new(en, node, etk_menu_item_new(), id);
      
   return item;
}

static E_Widget *
_e_widget_menu_image_item_handle(Enhance *en, EXML_Node *node)
{
   E_Widget   *item;
   char       *id;

   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;

   item = _e_widget_new(en, node, etk_menu_item_image_new(), id);

   return item;
}

static E_Widget *
_e_widget_menu_handle(Enhance *en, EXML_Node *node)
{
   E_Widget   *menu;
   char       *id;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
      
   menu = _e_widget_new(en, node, etk_menu_new(), id);
   
   return menu;
}

static E_Widget *
_e_widget_combo_box_handle(Enhance *en, EXML_Node *node)
{
   E_Widget   *combo;
   char       *id;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
      
   combo = _e_widget_new(en, node, etk_combobox_new(), id);
   
   return combo;
}

static E_Widget *
_e_widget_hseparator_handle(Enhance *en, EXML_Node *node)
{
   E_Widget   *sep;
   char       *id;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
      
   sep = _e_widget_new(en, node, etk_hseparator_new(), id);
   
   return sep;
}

static E_Widget *
_e_widget_vseparator_handle(Enhance *en, EXML_Node *node)
{
   E_Widget   *sep;
   char       *id;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
      
   sep = _e_widget_new(en, node, etk_vseparator_new(), id);
   
   return sep;
}

static E_Widget *
_e_widget_hslider_handle(Enhance *en, EXML_Node *node)
{
   E_Widget   *slider;
   char       *id;
   Ecore_List *props;
   EXML_Node  *prop;
   double      value;
   double      min       = 0;
   double      max       = 0;
   double      step_inc  = 1;
   double      page_inc  = 1;
   double      page_size = 1;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
   
   props = node->children;
   ecore_list_first_goto(props);
   prop = ecore_list_current(props);
   while(prop != NULL)
     {
	if(!strcmp(prop->tag, "property"))
	  {
	     char *name;
	     
	     name = ecore_hash_get(prop->attributes, "name");
	     if(!name) { prop = ecore_list_next(props); continue; }
	     
	     if(!strcmp(name, "adjustment"))
	       {
		  if(prop->value)
		    {
		       char *adj;
		       
		       adj = strdup(prop->value);
		       sscanf(adj, "%lf %lf %lf %lf %lf %lf", &value, &min, &max,
			      &step_inc, &page_inc, &page_size);
		       E_FREE(adj);
		    }
	       }
	  }
	prop = ecore_list_next(props);
     }

   ecore_list_first_goto(props);
   
   slider = _e_widget_new(en, node, etk_hslider_new((double)min, (double)max, 
						    (double)value,
						    (double)step_inc,
						    (double)page_inc), id);   
   return slider;
}

static E_Widget *
_e_widget_vslider_handle(Enhance *en, EXML_Node *node)
{
   E_Widget   *slider;
   char       *id;
   Ecore_List *props;
   EXML_Node  *prop;
   double      value;
   double      min       = 0;
   double      max       = 0;
   double      step_inc  = 0;
   double      page_inc  = 0;
   double      page_size = 0;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
   
   props = node->children;
   ecore_list_first_goto(props);
   prop = ecore_list_current(props);
   while(prop != NULL)
     {
	if(!strcmp(prop->tag, "property"))
	  {
	     char *name;
	     
	     name = ecore_hash_get(prop->attributes, "name");
	     if(!name) { prop = ecore_list_next(props); continue; }
	     
	     if(!strcmp(name, "adjustment"))
	       {
		  if(prop->value)
		    {
		       char *adj;

		       adj = strdup(prop->value);
		       sscanf(adj, "%lf %lf %lf %lf %lf %lf", &value, &min, &max,
			      &step_inc, &page_inc, &page_size);
		       E_FREE(adj);		       
		    }
	       }
	  }
	prop = ecore_list_next(props);
     }
   
   ecore_list_first_goto(props);
      
   slider = _e_widget_new(en, node, etk_vslider_new(min, max, 
						    value,
						    step_inc, 
						    page_inc), id);   
   return slider;
}

static E_Widget *
_e_widget_textview_handle(Enhance *en, EXML_Node *node)
{
   E_Widget *tview;
   char     *id;

   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;

   tview = _e_widget_new(en, node, etk_text_view_new(), id);

   return tview;
}

static E_Widget *
_e_widget_filechooser_widget_handle(Enhance *en, EXML_Node *node)
{
   E_Widget *filechooser;
   char     *id;

   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;

   filechooser = _e_widget_new(en, node, etk_filechooser_widget_new(), id);

   return filechooser;
}

static E_Widget *
_e_widget_iconview_handle(Enhance *en, EXML_Node *node)
{
   E_Widget *iconbox;
   char     *id;

   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;

   iconbox = _e_widget_new(en, node, etk_iconbox_new(), id);

   return iconbox;
}

static E_Widget *
_e_widget_toolbar_handle(Enhance *en, EXML_Node *node)
{
   E_Widget *toolbar;
   char     *id;

   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;

   toolbar = _e_widget_new(en, node, etk_toolbar_new(), id);

   return toolbar;
}

static E_Widget *
_e_widget_tool_button_handle(Enhance *en, EXML_Node *node)
{
   E_Widget *tool_button;
   char     *id;

   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;

   tool_button = _e_widget_new(en, node, etk_tool_button_new(), id);

   return tool_button;
}

static E_Widget *
_e_widget_tool_toggle_button_handle(Enhance *en, EXML_Node *node)
{
   E_Widget *tool_toggle_button;
   char     *id;

   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;

   tool_toggle_button = _e_widget_new(en, node, etk_tool_toggle_button_new(), id);

   return tool_toggle_button;
}

static E_Widget *
_e_widget_spinner_handle(Enhance *en, EXML_Node *node)
{
   E_Widget   *spinner;
   char       *id;
   Ecore_List *props;
   EXML_Node  *prop;
   double      value;
   double      min       = 0;
   double      max       = 0;
   double      step_inc  = 1;
   double      page_inc  = 1;
   double      page_size = 1;
   
   id = ecore_hash_get(node->attributes, "id");
   if(!id) return NULL;
   
   props = node->children;
   ecore_list_first_goto(props);
   prop = ecore_list_current(props);
   while(prop != NULL)
     {
	if(!strcmp(prop->tag, "property"))
	  {
	     char *name;
	     
	     name = ecore_hash_get(prop->attributes, "name");
	     if(!name) { prop = ecore_list_next(props); continue; }
	     
	     if(!strcmp(name, "adjustment"))
	       {
		  if(prop->value)
		    {
		       char *adj;
		       
		       adj = strdup(prop->value);
		       sscanf(adj, "%lf %lf %lf %lf %lf %lf", &value, &min, &max,
			      &step_inc, &page_inc, &page_size);
		       E_FREE(adj);
		    }
	       }
	  }
	prop = ecore_list_next(props);
     }

   ecore_list_first_goto(props);
   
   spinner = _e_widget_new(en, node, etk_spinner_new(min, max, 
						     value,
						     step_inc,
						     page_inc), id);   

   return spinner;
}

E_Widget *
_e_widget_handle(Enhance *en, EXML_Node *node)
{
   char *class;
   
   class = ecore_hash_get(node->attributes, "class");
   if(!class) return NULL;
   
#if DEBUG  
   printf("Handling widget: %s\n", class);		    
#endif
   
   if(!strcmp(class, "GtkWindow"))     
     return _e_widget_window_handle(en, node);        
   else if(!strcmp(class, "GtkDialog"))
     return _e_widget_dialog_handle(en, node);
   else if(!strcmp(class, "GtkVBox"))
     return _e_widget_vbox_handle(en, node);
   else if(!strcmp(class, "GtkVButtonBox"))
     return _e_widget_vbox_handle(en, node);   
   else if(!strcmp(class, "GtkHBox"))
     return _e_widget_hbox_handle(en, node);
   else if(!strcmp(class, "GtkHButtonBox"))
     return _e_widget_hbox_handle(en, node);   
   else if(!strcmp(class, "GtkFrame"))
     return _e_widget_frame_handle(en, node);
   else if(!strcmp(class, "GtkScrolledWindow"))
     return _e_widget_scrolled_view_handle(en, node);
   else if(!strcmp(class, "GtkViewport"))
     return _e_widget_viewport_handle(en, node);   
   else if(!strcmp(class, "GtkHPaned"))
     return _e_widget_hpaned_handle(en, node);
   else if(!strcmp(class, "GtkVPaned"))
     return _e_widget_vpaned_handle(en, node);   
   else if(!strcmp(class, "GtkButton"))
     return _e_widget_button_handle(en, node);
   else if(!strcmp(class, "GtkCheckButton"))
     return _e_widget_check_button_handle(en, node);
   else if(!strcmp(class, "GtkRadioButton"))
     return _e_widget_radio_button_handle(en, node);
   else if(!strcmp(class, "GtkToggleButton"))
     return _e_widget_toggle_button_handle(en, node);
   else if(!strcmp(class, "GtkLabel"))
     return _e_widget_label_handle(en, node);
   else if(!strcmp(class, "GtkImage"))
     return _e_widget_image_handle(en, node);   
   else if(!strcmp(class, "GtkEntry"))
     return _e_widget_entry_handle(en, node);
   else if(!strcmp(class, "GtkProgressBar"))
     return _e_widget_progress_bar_handle(en, node);   
   else if(!strcmp(class, "GtkAlignment"))
     return _e_widget_alignment_handle(en, node);
   else if(!strcmp(class, "GtkTable"))
     return _e_widget_table_handle(en, node);
   else if(!strcmp(class, "GtkTreeView"))
     return _e_widget_tree_handle(en, node);
   else if(!strcmp(class, "GtkNotebook"))
     return _e_widget_notebook_handle(en, node);   
   else if(!strcmp(class, "GtkStatusbar"))
     return _e_widget_statusbar_handle(en, node);
   else if(!strcmp(class, "GtkMenuBar"))
     return _e_widget_menu_bar_handle(en, node);
   else if(!strcmp(class, "GtkMenuItem"))
     return _e_widget_menu_item_handle(en, node);
   else if(!strcmp(class, "GtkImageMenuItem"))
     return _e_widget_menu_image_item_handle(en, node);
   else if(!strcmp(class, "GtkMenu"))
     return _e_widget_menu_handle(en, node);   
   else if(!strcmp(class, "GtkComboBox"))
     return _e_widget_combo_box_handle(en, node);   
   else if(!strcmp(class, "GtkHSeparator"))
     return _e_widget_hseparator_handle(en, node);
   else if(!strcmp(class, "GtkVSeparator"))
     return _e_widget_vseparator_handle(en, node);
   else if(!strcmp(class, "GtkHScale"))
     return _e_widget_hslider_handle(en, node);
   else if(!strcmp(class, "GtkVScale"))
     return _e_widget_vslider_handle(en, node);
   else if(!strcmp(class, "GtkTextView"))
     return _e_widget_textview_handle(en, node);
   else if(!strcmp(class, "GtkFileChooserWidget"))
     return _e_widget_filechooser_widget_handle(en, node);
   else if(!strcmp(class, "GtkIconView"))
     return _e_widget_iconview_handle(en, node);
   else if(!strcmp(class, "GtkToolbar"))
     return _e_widget_toolbar_handle(en, node);
   else if(!strcmp(class, "GtkToolButton"))
     return _e_widget_tool_button_handle(en, node);
   else if(!strcmp(class, "GtkToggleToolButton"))
     return _e_widget_tool_toggle_button_handle(en, node);
   else if(!strcmp(class, "GtkSpinButton"))
     return _e_widget_spinner_handle(en, node);
   return NULL;
}

void
_e_widget_parent_add(E_Widget *parent, E_Widget *child)
{
   char *parent_class;

   parent_class = ecore_hash_get(parent->node->attributes, "class");
   if(!parent_class) return;

#if DEBUG   
   printf("packing %s into %s\n", (char *)ecore_hash_get(child->node->attributes, "class"), parent_class);
#endif
   
   if(!strcmp(parent_class, "GtkWindow"))
     {
	etk_container_add(ETK_CONTAINER(parent->wid), child->wid);
     }
   if(!strcmp(parent_class, "GtkDialog"))
     {
        int padding = 0;
        Etk_Bool expand = ETK_TRUE;
        Etk_Bool fill   = ETK_TRUE;
        EXML_Node *area_node;
        EXML_Node *prop;
        Ecore_List *props;

        /* Go the <packing> node */
        props = child->node->parent->parent->parent->children;
        ecore_list_last_goto(props);
        prop = ecore_list_current(props);
        if (!strcmp(prop->tag, "packing"))
          {
            /* Take the packing properties */
            props = prop->children;
            ecore_list_first_goto(props);
            prop = ecore_list_current(props);

            /* Parse the packing properties */
            while (prop)
              {
                if (!strcmp(ecore_hash_get(prop->attributes, "name"), "padding"))
                  {
                    padding = atoi(prop->value);
                  }
                else if (!strcmp(ecore_hash_get(prop->attributes, "name"), "expand"))
                  {
                    if (!strcmp(prop->value, "False"))
                      expand = ETK_FALSE;
                  }
                else if (!strcmp(ecore_hash_get(prop->attributes, "name"), "fill"))
                  {
                    if (!strcmp(prop->value, "False"))
                      fill = ETK_FALSE;
                  }
                ecore_list_next(props);
                prop = ecore_list_current(props);
              }
          }

        /* Check if the child is in the action_area */
        if ((area_node = child->node->parent->parent->parent)
            && ecore_hash_get(area_node->attributes, "internal-child")
            && !strcmp(ecore_hash_get(area_node->attributes, "internal-child"),
                       "action_area"))
                {

                  if (ETK_IS_BUTTON(child->wid))
                    {
                      prop = find_node(child->node, "name", "response_id"); 
                      etk_dialog_pack_button_in_action_area(
                                            ETK_DIALOG(parent->wid),
                                            ETK_BUTTON(child->wid),
                                            atoi(prop->value),
                                            ETK_BOX_START,
                                            BOX_FILL_POLICY(expand, fill),
                                            padding);
                    }
                  else
                    {
                      etk_dialog_pack_widget_in_action_area(
                                            ETK_DIALOG(parent->wid), child->wid,
                                            ETK_BOX_START,
                                            BOX_FILL_POLICY(expand, fill),
                                            padding);
                    }
                }
        else
          {
            etk_dialog_pack_in_main_area(ETK_DIALOG(parent->wid), 
                                         child->wid, 
                                         ETK_BOX_START,
                                         BOX_FILL_POLICY(expand, fill),
                                         padding);
          }
     }
   if(!strcmp(parent_class, "GtkFrame"))
     {
	if(child->packing)
	  {
	     if(child->packing->type)
	       {
		  if(!strcmp(child->packing->type, "label_item"))
		    etk_frame_label_set(ETK_FRAME(parent->wid),
					etk_label_get(ETK_LABEL(child->wid)));
	       }
	  }
	else
	  etk_container_add(ETK_CONTAINER(parent->wid), child->wid);
     }
   if(!strcmp(parent_class, "GtkScrolledWindow"))
     {
	etk_container_add(ETK_CONTAINER(parent->wid), child->wid);
     }
   if(!strcmp(parent_class, "GtkViewport"))
     {
	etk_container_add(ETK_CONTAINER(parent->wid), child->wid);
     }   
   if(!strcmp(parent_class, "GtkHPaned") || !strcmp(parent_class, "GtkVPaned"))
     {
	Etk_Widget *w = NULL;
	Etk_Bool   expand = ETK_TRUE;
	
	if(child->packing)
	  {
	     if(child->packing->shrink == ETK_TRUE)
	       expand = ETK_FALSE;
	  }
	
	if((w = etk_paned_child1_get(ETK_PANED(parent->wid))) != NULL)	  
	  etk_paned_child2_set(ETK_PANED(parent->wid), child->wid, expand);
	else
	  etk_paned_child1_set(ETK_PANED(parent->wid), child->wid, expand);
     }
   if(!strcmp(parent_class, "GtkNotebook"))
     {
	if(child->packing)
	  {
	     if(child->packing->type)
	       {
		  if(!strcmp(child->packing->type, "tab"))
		    {
		       etk_notebook_page_tab_label_set(ETK_NOTEBOOK(parent->wid),
 		        etk_notebook_current_page_get(ETK_NOTEBOOK(parent->wid)),
		        etk_label_get(ETK_LABEL(child->wid)));
		       etk_notebook_current_page_set(ETK_NOTEBOOK(parent->wid), 0);
		    }		       		  
	       }
	     else
	       {
		  int num;
		  
		  num = etk_notebook_page_append(ETK_NOTEBOOK(parent->wid), "ChangeMe", child->wid);
		  etk_notebook_current_page_set(ETK_NOTEBOOK(parent->wid), num);		  
	       }
	  }
	else
	  {
	     etk_notebook_page_append(ETK_NOTEBOOK(parent->wid), "ChangeMe", child->wid);
	  }
     }   
   if(!strcmp(parent_class, "GtkAlignment"))
     {
	etk_container_add(ETK_CONTAINER(parent->wid), child->wid);
     }   
   else if(!strcmp(parent_class, "GtkMenuItem"))
     {
	etk_menu_item_submenu_set(ETK_MENU_ITEM(parent->wid), 
				  ETK_MENU(child->wid));
     }
   else if(!strcmp(parent_class, "GtkMenuBar"))
     {
	etk_menu_shell_append(ETK_MENU_SHELL(parent->wid), 
			      ETK_MENU_ITEM(child->wid));
     }
   else if(!strcmp(parent_class, "GtkMenu"))
     {
	etk_menu_shell_append(ETK_MENU_SHELL(parent->wid), 
			      ETK_MENU_ITEM(child->wid));
     }   
   else if(!strcmp(parent_class, "GtkVBox") || !strcmp(parent_class, "GtkHBox") ||
	   !strcmp(parent_class, "GtkVButtonBox") || !strcmp(parent_class, "GtkHButtonBox"))
     {
        Etk_Box_Fill_Policy fill_policy;
	Etk_Box_Group box_group = ETK_BOX_START;
	int      padding;
		
	if(child->packing)
	  {
             fill_policy = BOX_FILL_POLICY(child->packing->expand, child->packing->fill);
	     padding = child->packing->padding;
	     if (child->packing->box_group) 
	       box_group = child->packing->box_group;
	  }
        else
          {
             fill_policy = ETK_BOX_EXPAND_FILL;
             padding = 0;
          }
	
	etk_box_append(ETK_BOX(parent->wid), child->wid, box_group, fill_policy, padding);
     }
   
   else if(!strcmp(parent_class, "GtkTable"))
     {
	int left_attach   = 0;
	int right_attach  = 0;
	int top_attach    = 0;
	int bottom_attach = 0;
	int x_padding     = 0;
	int y_padding     = 0;
	int flags_set     = 0;	
	Etk_Table_Fill_Policy fill_policy = ETK_TABLE_EXPAND_FILL;
	
	if(child->packing)
	  {
	     if(child->packing->left_attach > 0)
	       left_attach = child->packing->left_attach;
	     if(child->packing->right_attach > 0)
	       right_attach = child->packing->right_attach;
	     if(child->packing->top_attach > 0)
	       top_attach = child->packing->top_attach;
	     if(child->packing->bottom_attach > 0)
	       bottom_attach = child->packing->bottom_attach;
	     if(child->packing->y_padding > 0)
	       y_padding = child->packing->y_padding;
	     if(child->packing->y_padding > 0)
	       y_padding = child->packing->y_padding;
	     if(child->packing->x_options)
	       {
		  /* Glade 2 compatibility */
		  if(strstr(child->packing->x_options, "fill"))
		    {
		       fill_policy = ETK_TABLE_HFILL;
		       flags_set = 1;
		    }
		  else if(strstr(child->packing->x_options, "expand"))
		    {
		       if(flags_set)
			 fill_policy |= ETK_TABLE_HEXPAND;
		       else
			 fill_policy = ETK_TABLE_HEXPAND;
		       flags_set = 1;
		    }
		  /* Glade 3 compatibility */
		  else if(strstr(child->packing->x_options, "GTK_EXPAND"))
		    {
		       fill_policy = (fill_policy ^ ETK_TABLE_HFILL) & fill_policy;
		       flags_set = 1;
		    }
		  else if(strstr(child->packing->x_options, "GTK_FILL"))
		    {
		       fill_policy = (fill_policy ^ ETK_TABLE_HEXPAND) & fill_policy;
		       flags_set = 1;
		    }
		  else if(strstr(child->packing->x_options, ""))
		    {
		       fill_policy = (fill_policy ^ (ETK_TABLE_HEXPAND | ETK_TABLE_HFILL)) & fill_policy;
		       flags_set = 1;
		    }
	       }
	     if(child->packing->y_options)
	       {
		  /* Glade 2 compatibility */
		  if(strstr(child->packing->y_options, "fill"))
		    {
		       if(flags_set)
			 fill_policy |= ETK_TABLE_VFILL;
		       else
			 fill_policy = ETK_TABLE_VFILL;
		       flags_set = 1;
		    }
		  else if(strstr(child->packing->y_options, "expand"))
		    {
		       if(flags_set)
			 fill_policy |= ETK_TABLE_VEXPAND;
		       else
			 fill_policy = ETK_TABLE_VEXPAND;
		       flags_set = 1;
		    }
		  /* Glade 3 compatibility */
		  else if(strstr(child->packing->y_options, "GTK_EXPAND"))
		    {
		       fill_policy = (fill_policy ^ ETK_TABLE_VFILL) & fill_policy;
		       flags_set = 1;
		    }
		  else if(strstr(child->packing->y_options, "GTK_FILL"))
		    {
		       fill_policy = (fill_policy ^ ETK_TABLE_VEXPAND) & fill_policy;
		       flags_set = 1;
		    }
		  else if(strstr(child->packing->y_options, ""))
		    {
		       fill_policy = (fill_policy ^ (ETK_TABLE_VEXPAND | ETK_TABLE_VFILL)) & fill_policy;
		       flags_set = 1;
		    }
	       }
	     
	     if(!flags_set)
	       fill_policy = ETK_TABLE_EXPAND_FILL;
	  }
	
	/* NOTE: we have a problem here:
	 * in GTK, we do: table_attach(table, widget, 0, 1, ...)
	 * to attach to col 0 row 0. in etk, we need to do:
	 * table_attach(table, widget, 0, 0, ...)
	 */
	if(right_attach - left_attach == 1)
	  right_attach = left_attach;
	if(bottom_attach - top_attach == 1)
	  bottom_attach = top_attach;	
		       
	etk_table_attach(ETK_TABLE(parent->wid), child->wid, left_attach,
			 right_attach, top_attach, bottom_attach, 
			 fill_policy, x_padding, y_padding);
     }
   else if(!strcmp(parent_class, "GtkButton") ||
           !strcmp(parent_class, "GtkToggleButton") ||
           !strcmp(parent_class, "GtkCheckButton"))
     {
        if(ETK_IS_IMAGE(child->wid))
          etk_button_image_set(ETK_BUTTON(parent->wid), ETK_IMAGE(child->wid));
        else
          etk_container_add(ETK_CONTAINER(parent->wid), child->wid);

        etk_widget_pass_mouse_events_set(child->wid, ETK_TRUE);
     }
   else if(!strcmp(parent_class, "GtkToolbar"))
     {
	etk_toolbar_append(ETK_TOOLBAR(parent->wid), child->wid, ETK_BOX_START);
     }	
}
