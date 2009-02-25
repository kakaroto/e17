#include "stickies.h"

#define STICKY_ONLY 0
#define STICKIES_ALL 1

#define NOT_DEFAULT 0
#define MAKE_DEFAULT 1

extern E_Stickies *ss;

static int _e_theme_apply = STICKY_ONLY;
static int _e_theme_default = NOT_DEFAULT;
static Etk_Widget *win;
static Etk_Widget *preview; 
static Etk_Widget *thumbs;

static void _e_theme_stickies_all_cb(Etk_Object *object, void *data);
static void _e_theme_sticky_only_cb(Etk_Object *object, void *data);
static void _e_theme_make_default_cb(Etk_Object *object, void *data);
static void _e_theme_ok_cb(Etk_Object *object, void *data);
static void _e_theme_apply_cb(Etk_Object *object, void *data);
static void _e_theme_cancel_cb(Etk_Object *object, void *data);
static void _e_theme_chooser_item_selected_cb(Etk_Object *object, Etk_Tree_Row *row, void *data);
static void _e_theme_apply_now(E_Sticky *s);
    
void
_e_theme_chooser_show(E_Sticky *s)
{
   Eina_List *themes;
   char *theme;
   char theme_file[PATH_MAX];
   
   Etk_Widget *ok_button;
   Etk_Widget *apply_button;
   Etk_Widget *cancel_button;
   Etk_Widget *preview_hbox;
   Etk_Widget *button_hbox;
   Etk_Widget *option_vbox;
   Etk_Widget *frame;
   Etk_Widget *button;
   Etk_Widget *vbox;
   Etk_Tree_Col *col1;
   Etk_Tree_Row *row;
   Etk_Tree_Model *model;
   
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Estickies - Theme Chooser");
   
   /* main vbox */
   vbox = etk_vbox_new(ETK_FALSE, 0);
   
   /* hbox to hold tree and preview */
   preview_hbox = etk_hbox_new(ETK_FALSE, 5);
   etk_box_append(ETK_BOX(vbox), preview_hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   /* the preview, an image */
   preview = etk_image_new();
   etk_widget_theme_group_set(preview, "tree/grid");
   etk_widget_size_request_set(preview, 320, 240);
      
   /* tree to hold the thumbs */
   thumbs = etk_tree_new();
   etk_widget_size_request_set(thumbs, 180, 240);
   etk_tree_mode_set(ETK_TREE(thumbs), ETK_TREE_MODE_LIST);
   etk_tree_multiple_select_set(ETK_TREE(thumbs), ETK_FALSE);
   col1 = etk_tree_col_new(ETK_TREE(thumbs), "Themes", 150, 0.0);
   model = etk_tree_model_image_new();
   etk_tree_model_image_width_set(model, 80, 0.5);
   etk_tree_col_model_add(col1, model);
   etk_tree_col_model_add(col1, etk_tree_model_text_new());
   etk_tree_rows_height_set(ETK_TREE(thumbs), 60);   
   etk_tree_headers_visible_set(ETK_TREE(thumbs), ETK_FALSE);
   etk_signal_connect("row_selected", ETK_OBJECT(thumbs), ETK_CALLBACK(_e_theme_chooser_item_selected_cb), s);
   etk_tree_build(ETK_TREE(thumbs));   
   
   /* scan for themes and add them to the list */
   themes = ecore_file_ls(PACKAGE_DATA_DIR"/themes");
   EINA_LIST_FREE(themes, theme)
     {
	char *theme_no_ext;
	static int i = 0;
	
	theme_no_ext = ecore_file_strip_ext(theme);
	snprintf(theme_file, sizeof(theme_file), PACKAGE_DATA_DIR"/themes/%s", theme);
	row = etk_tree_row_append(ETK_TREE(thumbs), NULL, col1, theme_file, "preview", theme_no_ext, NULL);
	if(i == 0)
	  etk_tree_row_select(row);
	++i;
	free(theme_no_ext);
	free(theme);
     }
   
   /* pack tree + preview widget */
   etk_box_append(ETK_BOX(preview_hbox), thumbs, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   etk_box_append(ETK_BOX(preview_hbox), preview, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   /* box to store the check buttons */
   frame = etk_frame_new("Options");
   option_vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), option_vbox);
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   /* check buttons for various options */
   button = etk_radio_button_new_with_label("Apply to this sticky only", NULL);
   etk_box_append(ETK_BOX(option_vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_e_theme_sticky_only_cb), NULL);
   
   button = etk_radio_button_new_with_label_from_widget("Apply to all stickies",
						       ETK_RADIO_BUTTON(button));
   etk_box_append(ETK_BOX(option_vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_e_theme_stickies_all_cb), NULL);   
   
   button = etk_check_button_new_with_label("Make this my default theme");
   etk_box_append(ETK_BOX(option_vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_signal_connect("toggled", ETK_OBJECT(button), ETK_CALLBACK(_e_theme_make_default_cb), NULL);
   
   /* box to store Ok / Apply / Cancel */
   button_hbox = etk_hbox_new(ETK_FALSE, 0);
   
   /* the buttons themselves */
   ok_button = etk_button_new_with_label("Ok");
   etk_signal_connect("clicked", ETK_OBJECT(ok_button), 
		      ETK_CALLBACK(_e_theme_ok_cb), s);
   apply_button = etk_button_new_with_label("Apply");
   etk_signal_connect("clicked", ETK_OBJECT(apply_button), 
		      ETK_CALLBACK(_e_theme_apply_cb), s);
   cancel_button = etk_button_new_with_label("Close");
   etk_signal_connect("clicked", ETK_OBJECT(cancel_button), 
		      ETK_CALLBACK(_e_theme_cancel_cb), s);
   
   etk_box_append(ETK_BOX(button_hbox), ok_button, 
		      ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_box_append(ETK_BOX(button_hbox), apply_button, 
		      ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_box_append(ETK_BOX(button_hbox), cancel_button, 
		      ETK_BOX_START, ETK_BOX_NONE, 0);   
   
   etk_box_append(ETK_BOX(vbox), button_hbox, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   etk_container_add(ETK_CONTAINER(win), vbox);
   etk_container_border_width_set(ETK_CONTAINER(win), 5);
   etk_widget_show_all(win);
}

static void 
_e_theme_chooser_item_selected_cb(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
   E_Sticky *s;
   Etk_Tree *tree;
   char *icol_string;
   
   s = data;
   tree = ETK_TREE(object);
   
   etk_tree_row_fields_get(row, etk_tree_nth_col_get(tree, 0), &icol_string, NULL, NULL, NULL);
   etk_image_set_from_edje(ETK_IMAGE(preview), icol_string, "preview");   
}

static void
_e_theme_sticky_only_cb(Etk_Object *object, void *data)
{
   _e_theme_apply = STICKY_ONLY;
}

static void
_e_theme_stickies_all_cb(Etk_Object *object, void *data)
{
   _e_theme_apply = STICKIES_ALL;
}

static void
_e_theme_make_default_cb(Etk_Object *object, void *data)
{
   _e_theme_default = etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(object));
}

static void 
_e_theme_ok_cb(Etk_Object *object, void *data)
{ 
   E_Sticky *s;
   
   s = data;
   _e_theme_apply_now(s);
   etk_object_destroy(ETK_OBJECT(win));
}

static void 
_e_theme_apply_cb(Etk_Object *object, void *data)
{ 
   E_Sticky *s;
   
   s = data;   
   _e_theme_apply_now(s);
}

static void 
_e_theme_cancel_cb(Etk_Object *object, void *data)
{
   etk_object_destroy(ETK_OBJECT(win));   
}

static void
_e_theme_apply_now(E_Sticky *s)
{
   Etk_Tree_Row *row;
   char *icol_string;
   
   if(!_e_sticky_exists(s))
     return;
   
   row = etk_tree_selected_row_get(ETK_TREE(thumbs));
   if(!row)
     return;
   
   etk_tree_row_fields_get(row, etk_tree_nth_col_get(ETK_TREE(thumbs), 0), 
			   &icol_string, NULL, NULL, NULL);   
   
   if(_e_theme_apply == STICKY_ONLY)
     _e_sticky_theme_apply(s, ecore_file_file_get(icol_string));
   else if(_e_theme_apply == STICKIES_ALL)
     _e_sticky_theme_apply_all(ecore_file_file_get(icol_string));

   if(_e_theme_default)
     {
	E_FREE(ss->theme);
	ss->theme = strdup(ecore_file_file_get(icol_string));	
     }   
}
