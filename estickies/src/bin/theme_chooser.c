#include "stickies.h"

#define STICKY_ONLY 0
#define STICKIES_ALL 1

#define NOT_DEFAULT 0
#define MAKE_DEFAULT 1

extern E_Stickies *ss;

static int _e_theme_apply = STICKY_ONLY;
static int _e_theme_default = NOT_DEFAULT;
static Evas_Object *win, *preview, *thumbs;
char buf[PATH_MAX];

static void _e_theme_stickies_all_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_theme_sticky_only_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_theme_make_default_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_theme_ok_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_theme_apply_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_theme_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_theme_chooser_item_selected_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_theme_apply_now(E_Sticky *s);

Eina_Bool
_e_sticky_exists(E_Sticky *s)
{
   if(eina_list_data_find(ss->stickies, s))
     return EINA_TRUE;
   return EINA_FALSE;
}
    
void
_e_theme_chooser_show(E_Sticky *s)
{
   Eina_List *themes;
   char *theme;
   
   Evas_Object *background, *ok_button, *apply_button, *cancel_button, *button_hbox, *option_vbox, *frame, *button, *rdg, *vbox, *hbox, *ic;
   Elm_List_Item *row;

   win = elm_win_add(NULL, "estickies-themechooser", ELM_WIN_BASIC);
   elm_win_title_set(win, "Estickies - Theme Chooser");
   evas_object_smart_callback_add(win, "delete-request", _e_theme_cancel_cb, NULL);
   evas_object_show(win);

   background = elm_bg_add(win);
   evas_object_size_hint_weight_set(background, 1.0, 1.0);
   elm_win_resize_object_add(win, background);
   evas_object_show(background);

   /* main vbox */
   vbox = elm_box_add(win);
   evas_object_size_hint_weight_set(vbox, 1.0, 1.0);
   elm_box_horizontal_set(vbox, 0);
   elm_win_resize_object_add(win, vbox);
   evas_object_show(vbox);

   /* hbox to hold tree and preview */
   hbox = elm_box_add(win);
   elm_box_horizontal_set(hbox, 1);
   evas_object_size_hint_weight_set(hbox, 1.0, 1.0);
   evas_object_size_hint_align_set(hbox, -1.0, -1.0);
   elm_box_pack_end(vbox, hbox);
   evas_object_show(hbox);
   
   /* the preview, an image */
   preview = elm_image_add(win);
   snprintf(buf, sizeof(buf), "%s/images/preview_bg.png", PACKAGE_DATA_DIR);
   elm_image_file_set(preview, buf, NULL);
   elm_image_smooth_set(preview, 1);
   elm_box_pack_end(hbox, preview);
   evas_object_size_hint_weight_set(preview, 1.0, 1.0);
   evas_object_size_hint_align_set(preview, -1.0, -1.0);
   evas_object_show(preview);
      
   /* tree to hold the thumbs */
   thumbs = elm_list_add(win);
   evas_object_size_hint_weight_set(thumbs, 1.0, 1.0);
   evas_object_size_hint_align_set(thumbs, -1.0, -1.0);
   elm_list_multi_select_set(thumbs, 0);

   /* scan for themes and add them to the list */
   snprintf(buf, sizeof(buf), "%s/themes", PACKAGE_DATA_DIR);
   themes = ecore_file_ls(buf);
   EINA_LIST_FREE(themes, theme)
     {
	char *theme_no_ext;
	static int i = 0;
	
	theme_no_ext = ecore_file_strip_ext(theme);

	ic = elm_icon_add(win);
	snprintf(buf, sizeof(buf), "%s/themes/%s", PACKAGE_DATA_DIR, theme);
	evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
	elm_icon_file_set(ic, buf, "preview");

	row = elm_list_item_append(thumbs, theme_no_ext, ic, NULL, _e_theme_chooser_item_selected_cb, s);
	if(i == 0)
	  elm_list_item_selected_set(row, 1);
	++i;
	E_FREE(theme_no_ext);
	E_FREE(theme);
     }

   elm_list_go(thumbs);
   elm_box_pack_start(hbox, thumbs);
   evas_object_show(thumbs);

   /* box to store the check buttons */
   frame = elm_frame_add(win);
   elm_frame_label_set(frame, "Options");
   evas_object_size_hint_weight_set(frame, 1.0, 0.0);
   evas_object_size_hint_align_set(frame, -1.0, 0.0);
   elm_box_pack_end(vbox, frame);
   evas_object_show(frame);

   option_vbox = elm_box_add(win);
   elm_box_horizontal_set(option_vbox, 0);
   elm_frame_content_set(frame, option_vbox);
   evas_object_show(option_vbox);
   
   /* check buttons for various options */
   button = elm_radio_add(win);
   elm_radio_label_set(button, "Apply to this sticky only");
   elm_radio_state_value_set(button, 1);
   evas_object_smart_callback_add(button, "changed", _e_theme_sticky_only_cb, NULL);
   elm_box_pack_end(option_vbox, button);
   rdg = button;
   evas_object_show(button);

   button = elm_radio_add(win);
   elm_radio_label_set(button, "Apply to all stickies");
   elm_radio_state_value_set(button, 0);
   elm_radio_group_add(button, rdg);
   evas_object_smart_callback_add(button, "changed", _e_theme_stickies_all_cb, NULL);
   elm_box_pack_end(option_vbox, button);
   evas_object_show(button);

   button = elm_check_add(win);
   elm_check_label_set(button, "Make this my default theme");
   elm_check_state_set(button, 0);
   evas_object_smart_callback_add(button, "changed", _e_theme_make_default_cb, NULL);
   elm_box_pack_end(option_vbox, button);
   evas_object_show(button);

   /* box to store Ok / Apply / Cancel */
   button_hbox = elm_box_add(win);
   elm_box_horizontal_set(button_hbox, 1);
   elm_box_pack_end(vbox, button_hbox);
   evas_object_show(button_hbox);
   
   /* the buttons themselves */
   ok_button = elm_button_add(win);
   elm_button_label_set(ok_button, "OK");
   evas_object_smart_callback_add(ok_button, "clicked", _e_theme_ok_cb, s);
   elm_box_pack_end(button_hbox, ok_button);
   evas_object_show(ok_button);

   apply_button = elm_button_add(win);
   elm_button_label_set(apply_button, "Apply");
   evas_object_smart_callback_add(apply_button, "clicked", _e_theme_apply_cb, s);
   elm_box_pack_end(button_hbox, apply_button);
   evas_object_show(apply_button);

   cancel_button = elm_button_add(win);
   elm_button_label_set(cancel_button, "Close");
   evas_object_smart_callback_add(cancel_button, "clicked", _e_theme_cancel_cb, s);
   elm_box_pack_end(button_hbox, cancel_button);
   evas_object_show(cancel_button);

   evas_object_resize(win, 450, 410);
}

static void 
_e_theme_chooser_item_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
   E_Sticky *s;
   Elm_List_Item *item;
   char *icol_string;

   s = data;
   item = elm_list_selected_item_get(obj);

   icol_string = elm_list_item_label_get(item);
   snprintf(buf, sizeof(buf), "%s/themes/%s.edj", PACKAGE_DATA_DIR, icol_string);
   elm_image_file_set(preview, buf, "preview");
   evas_object_show(preview);
}

static void
_e_theme_sticky_only_cb(void *data, Evas_Object *obj, void *event_info)
{
   _e_theme_apply = STICKY_ONLY;
}

static void
_e_theme_stickies_all_cb(void *data, Evas_Object *obj, void *event_info)
{
   _e_theme_apply = STICKIES_ALL;
}

static void
_e_theme_make_default_cb(void *data, Evas_Object *obj, void *event_info)
{
   if (elm_check_state_get(obj))
     _e_theme_default = MAKE_DEFAULT;
}

static void 
_e_theme_ok_cb(void *data, Evas_Object *obj, void *event_info)
{ 
   E_Sticky *s;
   
   s = data;
   _e_theme_apply_now(s);
   evas_object_del(win);
}

static void 
_e_theme_apply_cb(void *data, Evas_Object *obj, void *event_info)
{ 
   E_Sticky *s;
   
   s = data;   
   _e_theme_apply_now(s);
}

static void 
_e_theme_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   evas_object_del(win);
}

static void
_e_theme_apply_now(E_Sticky *s)
{
   Elm_List_Item *item;
   char *icol_string;

   if(!_e_sticky_exists(s))
     return;

   item = elm_list_selected_item_get(thumbs);
   if(!item)
     return;

   icol_string = elm_list_item_label_get(item);

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
