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
 *
 * Etk Preferences tool - rewritten by quaker - July 2009
 */

#include <string.h>
#include <limits.h>
#include <Ecore_File.h>
#include <Ecore_Data.h>
#include <Etk.h>
#include "config.h"
// initialize some functions etc
void etk_prefs_show();

static Etk_Widget *_etk_prefs_theme_tab_create();
static Etk_Widget *_etk_prefs_other_tab_create();
static void _etk_prefs_theme_row_selected_cb(Etk_Object *object, Etk_Tree_Row *row, void *data);
static void _etk_prefs_engine_row_selected_cb(Etk_Object *object, Etk_Tree_Row *row);
static Etk_Widget *_etk_prefs_theme_preview_get(const char *theme);
static void _etk_prefs_response_cb(Etk_Object *object, int response_id, void *data);
static void _etk_prefs_switch_cb(Etk_Object *object, void *data);
static void _etk_prefs_quit_cb(void *data);
static void _etk_prefs_apply();
  
static char *_etk_prefs_widget_theme = NULL;
static char *_etk_prefs_engine = NULL;
static char *_widget_theme = NULL;

/* main function */
int main(int argc, char **argv)
{
   // if etk init fails, print error msg to stderr
   if(!etk_init(argc, argv))
   {
      fprintf(stderr, _("Could not init Etk. Exiting...\n"));
      return 1;
   }
   // if you set --help in arguments, it will print some help information
   if(etk_argument_is_set("help", 0, ETK_FALSE))
   {
      printf(_("Usage: etk_prefs ARGUMENTS\n"));
      printf(_("ARGUMENTS:\n"));
      printf(_("   --help : print this message\n"));
      printf(_("   --theme=THEME : set theme from command line\n"));
      printf(_("   --themes-list : list themes\n"));
      return 0;
   }
   else if(etk_argument_is_set("themes-list", 0, ETK_FALSE)) // or if you will set --themes-list, it will print a list of available themes
   {
      Eina_List *themes;
      Eina_List *l;
      char *theme;
      themes = etk_theme_widget_available_themes_get();
      for(l = themes; l; l = l->next)
      {	 
	 theme = l->data;
         printf("%s\n", theme);
      }
      return 0;
   }
   
   etk_argument_value_get("theme", 0, ETK_FALSE, &_widget_theme); // if --theme argument set, it will set theme from commandline, if bad theme provided, it will print error to stderr
   if (_widget_theme)
   {
      if (etk_theme_widget_find(_widget_theme))
      {
         _etk_prefs_widget_theme = strdup(_widget_theme);   
         _etk_prefs_apply(); // apply changes
         etk_config_save(); // save them
         return 0;
      }
      else
      {
         fprintf(stderr, _("Theme %s not found. Exitting..\n"), _widget_theme); // print error message
         return 1;
      }
   }
   
   etk_prefs_show(); // show etk_prefs dialog
   
   etk_main(); // main loop
   etk_shutdown(); // shutdown etk
   
   return 0; // return success
}

/* main etk_prefs dialog */
void etk_prefs_show()
{
   Etk_Widget *dialog; // initialize some widgets
   Etk_Widget *switch_button;
   Etk_Widget *notebook;
   
   /* main dialog to hold everything */
   dialog = etk_dialog_new();
   etk_window_title_set(ETK_WINDOW(dialog), _("Etk Preferences")); // set title - translatable
   etk_window_wmclass_set(ETK_WINDOW(dialog), "Etk Preferences", "Etk Preferences");
   etk_signal_connect_swapped_by_code(ETK_OBJECT_DESTROYED_SIGNAL, ETK_OBJECT(dialog),
				   ETK_CALLBACK(_etk_prefs_quit_cb), NULL);

   /* this will hold the current pref's contents */
   notebook = etk_notebook_new();
   etk_notebook_tabs_visible_set(ETK_NOTEBOOK(notebook), ETK_FALSE); 

   /* Some buttons */
   etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_CLOSE, ETK_RESPONSE_CLOSE);
   etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_APPLY, ETK_RESPONSE_APPLY);
   etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_OK, ETK_RESPONSE_OK);   
   etk_signal_connect("response", ETK_OBJECT(dialog), ETK_CALLBACK(_etk_prefs_response_cb), dialog);
   
   /* button for switching pages */
   switch_button = etk_button_new_with_label(_("Go to Engines page"));
   etk_signal_connect_by_code(ETK_BUTTON_CLICKED_SIGNAL, ETK_OBJECT(switch_button), ETK_CALLBACK(_etk_prefs_switch_cb), notebook);
   /* add the button into dialog action area */
   etk_dialog_pack_widget_in_action_area(ETK_DIALOG(dialog), switch_button, ETK_BOX_END, ETK_BOX_EXPAND, 0);  
   
   etk_container_border_width_set(ETK_CONTAINER(dialog), 5);
   etk_dialog_pack_in_main_area(ETK_DIALOG(dialog), notebook, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   /* create tabs */
   etk_notebook_page_append(ETK_NOTEBOOK(notebook), "Theme", _etk_prefs_theme_tab_create());
   etk_notebook_page_append(ETK_NOTEBOOK(notebook), "Engine", _etk_prefs_other_tab_create());
   
   etk_widget_show_all(dialog);
}

/* theme page */
static Etk_Widget *_etk_prefs_theme_tab_create()
{
   Eina_List *themes;
   Eina_List *l;   
   char *theme;
   
   Etk_Widget *preview;
   Etk_Widget *theme_list;
   
   Etk_Widget *preview_hbox;
   Etk_Widget *frame;
   Etk_Widget *vbox;
   Etk_Tree_Col *col1;
   Etk_Tree_Row *row;
      
   /* main vbox */
   vbox = etk_vbox_new(ETK_FALSE, 0);
   
   /* hbox to hold tree and preview */
   preview_hbox = etk_hbox_new(ETK_FALSE, 5);
   etk_box_append(ETK_BOX(vbox), preview_hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   /* the preview */
   preview = etk_frame_new(_("Preview"));
      
   /* tree to hold the theme list */
   theme_list = etk_tree_new();
   etk_widget_size_request_set(theme_list, 180, 240);
   etk_tree_mode_set(ETK_TREE(theme_list), ETK_TREE_MODE_LIST);
   etk_tree_multiple_select_set(ETK_TREE(theme_list), ETK_FALSE);
   etk_tree_rows_height_set(ETK_TREE(theme_list), 45);   
   etk_tree_headers_visible_set(ETK_TREE(theme_list), ETK_FALSE);
   etk_signal_connect("row-selected", ETK_OBJECT(theme_list), ETK_CALLBACK(_etk_prefs_theme_row_selected_cb), preview);
   
   col1 = etk_tree_col_new(ETK_TREE(theme_list), "Themes", 150, 0.0);
   etk_tree_col_model_add(col1, etk_tree_model_text_new());
   etk_tree_build(ETK_TREE(theme_list));   
   
   /* scan for themes and add them to the list */
   themes = etk_theme_widget_available_themes_get();
   for(l = themes; l; l = l->next)
     {
        const char *widget_theme = etk_config_widget_theme_get();
	
	theme = l->data;
        row = etk_tree_row_append(ETK_TREE(theme_list), NULL, col1, theme,  NULL);
	if (widget_theme)
        if (!strcmp(theme, widget_theme))
	    etk_tree_row_select(row);
     }
   
   /* pack tree + preview widget */
   etk_box_append(ETK_BOX(preview_hbox), theme_list, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_box_append(ETK_BOX(preview_hbox), preview, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
      
   etk_container_border_width_set(ETK_CONTAINER(vbox), 5);
   /* add frame */
   frame = etk_frame_new(_("Themes"));
   etk_container_add(ETK_CONTAINER(frame), vbox);
   
   return frame;
}

static Etk_Widget *_etk_prefs_other_tab_create()
{
   Eina_List *engines;
   Eina_List *l;   
   char *engine;
   
   Etk_Widget *engine_list;
   
   Etk_Widget *frame;
   Etk_Widget *vbox;
   Etk_Tree_Col *col1;
   Etk_Tree_Row *row;
      
   /* main vbox */
   vbox = etk_vbox_new(ETK_FALSE, 0);
   
   /* tree to hold the engines list */
   engine_list = etk_tree_new();
   etk_widget_size_request_set(engine_list, 180, 240);
   etk_tree_mode_set(ETK_TREE(engine_list), ETK_TREE_MODE_LIST);
   etk_tree_multiple_select_set(ETK_TREE(engine_list), ETK_FALSE);
   etk_tree_rows_height_set(ETK_TREE(engine_list), 45);   
   etk_tree_headers_visible_set(ETK_TREE(engine_list), ETK_FALSE);
   etk_signal_connect("row-selected", ETK_OBJECT(engine_list), ETK_CALLBACK(_etk_prefs_engine_row_selected_cb), NULL);
   
   col1 = etk_tree_col_new(ETK_TREE(engine_list), "Engines", 150, 0.0);
   etk_tree_col_model_add(col1, etk_tree_model_text_new());
   etk_tree_build(ETK_TREE(engine_list));   
   
   /* scan for engines and add them to the list */
   engines = etk_engine_list_get();
   for(l = engines; l; l = l->next)
     {
        const char *engine_current = etk_config_engine_get();
	
	engine = l->data;
        row = etk_tree_row_append(ETK_TREE(engine_list), NULL, col1, engine,  NULL);
	if (engine_current)
        if (!strcmp(engine, engine_current))
	    etk_tree_row_select(row);
     }
   
   /* pack tree */
   etk_box_append(ETK_BOX(vbox), engine_list, ETK_BOX_START, ETK_BOX_NONE, 0);
      
   etk_container_border_width_set(ETK_CONTAINER(vbox), 5);

   frame = etk_frame_new(_("Engines"));
   etk_container_add(ETK_CONTAINER(frame), vbox);
   
   return frame;
}

/* switch pages callback */
static void _etk_prefs_switch_cb(Etk_Object *object, void *data)
{
   if(!strcmp(etk_button_label_get(ETK_BUTTON(object)), _("Go to Engines page")))
   {
      etk_notebook_page_next(ETK_NOTEBOOK(data));
      etk_button_label_set(ETK_BUTTON(object), _("Go to Themes page"));
   }
   else
   {
      etk_notebook_page_prev(ETK_NOTEBOOK(data));
      etk_button_label_set(ETK_BUTTON(object), _("Go to Engines page"));
   }
}

/* callback for theme list */
static void _etk_prefs_theme_row_selected_cb(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
   Etk_Widget *child = NULL;
   Etk_Tree *tree;
   char *icol_string;
   Etk_Widget *preview;
     
   tree = ETK_TREE(object);
   preview = data;

   etk_tree_row_fields_get(row, etk_tree_nth_col_get(tree, 0), &icol_string, NULL, NULL, NULL);
   child = _etk_prefs_theme_preview_get(icol_string);
   etk_container_add(ETK_CONTAINER(preview), child);
   etk_widget_show_all(child);
}

/* callback for engines list */
static void _etk_prefs_engine_row_selected_cb(Etk_Object *object, Etk_Tree_Row *row)
{
   Etk_Tree *tree;
   char *icol_string;
     
   tree = ETK_TREE(object);

   etk_tree_row_fields_get(row, etk_tree_nth_col_get(tree, 0), &icol_string, NULL, NULL, NULL);
   free(_etk_prefs_engine);
   _etk_prefs_engine = strdup(icol_string);
}

/* theme preview widget */
static Etk_Widget *_etk_prefs_theme_preview_get(const char *theme)
{
   char file[PATH_MAX];
   static Etk_Widget *box = NULL;
   Etk_Widget *widget;
   Etk_Widget *vbox;
   Etk_Widget *frame;
   
   snprintf(file, sizeof(file), PACKAGE_DATA_DIR"/themes/%s.edj", theme);
   if (!ecore_file_exists(file))
   {
      char *home;
      
      home = getenv("HOME");
      if (!home)
	return NULL;
      
      snprintf(file, sizeof(file), "%s/.e/etk/themes/%s.edj", home, theme);
      if (!ecore_file_exists(file))
	return NULL;
   }
   
   free(_etk_prefs_widget_theme);
   _etk_prefs_widget_theme = strdup(theme);
   
   if (box)
   {
      etk_widget_theme_file_set(box, file);
      return box;
   }
   
   box = etk_vbox_new(ETK_FALSE, 0);   
   etk_widget_theme_file_set(box, file);
   
   frame = etk_frame_new(_("Buttons"));
   etk_box_append(ETK_BOX(box), frame, ETK_BOX_START, ETK_BOX_NONE, 0);
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox);
   widget = etk_button_new_with_label(_("Regular Button"));
   etk_box_append(ETK_BOX(vbox), widget, ETK_BOX_START, ETK_BOX_NONE, 0);   
   widget = etk_check_button_new_with_label(_("Check Button"));
   etk_box_append(ETK_BOX(vbox), widget, ETK_BOX_START, ETK_BOX_NONE, 0);   
   widget = etk_radio_button_new_with_label(_("Radio Button"), NULL);
   etk_box_append(ETK_BOX(vbox), widget, ETK_BOX_START, ETK_BOX_NONE, 0);   
   
   frame = etk_frame_new(_("Text"));
   etk_box_append(ETK_BOX(box), frame, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox);
   widget = etk_entry_new();
   etk_entry_text_set(ETK_ENTRY(widget), _("Sample text..."));
   etk_box_append(ETK_BOX(vbox), widget, ETK_BOX_START, ETK_BOX_NONE, 0);
   widget = etk_text_view_new();
   etk_textblock_text_set(etk_text_view_textblock_get(ETK_TEXT_VIEW(widget)),
			  _("Multi-line text widget!\nHow about that! (="),
			  ETK_TRUE);
   etk_widget_size_request_set(widget, 320, 50);
   etk_box_append(ETK_BOX(vbox), widget, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
         
   return box;
}

/* response callback for dialog action area */
static void _etk_prefs_response_cb(Etk_Object *object, int response_id, void *data)
{
   switch(response_id)
   {
      case ETK_RESPONSE_OK:
      _etk_prefs_apply();
      etk_config_save();
      etk_main_quit();
      break;

      case ETK_RESPONSE_APPLY:
      _etk_prefs_apply();      
      etk_config_save();
      break;
      
      case ETK_RESPONSE_CLOSE:
      etk_main_quit();
      break;
   }
}

/* destroy callback */
static void _etk_prefs_quit_cb(void *data)
{
   etk_main_quit();
}

/* apply things */
static void _etk_prefs_apply()
{
   if (_etk_prefs_widget_theme)
   {
     etk_config_wm_theme_set(_etk_prefs_widget_theme);
     etk_config_widget_theme_set(_etk_prefs_widget_theme);
   }

   if (_etk_prefs_engine)
   {
     etk_config_engine_set(_etk_prefs_engine);
   }
}
