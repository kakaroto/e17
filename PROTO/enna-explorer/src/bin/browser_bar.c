#include <Elementary.h>

#include "file.h"
#include "enna_config.h"

static void
_up_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *o_layout = data;
   evas_object_smart_callback_call(o_layout, "up,clicked", NULL);
}

static void
_browse_icons_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *o_layout = data;
    Evas_Object *o_seg = evas_object_data_get(o_layout, "browse");
    Elm_Segment_Item *it = elm_segment_control_item_selected_get(o_seg);

    switch(elm_segment_control_item_index_get(it))
      {
       case 0:
          evas_object_smart_callback_call(o_layout, "next,clicked", NULL);
          break;
       case 1:
          evas_object_smart_callback_call(o_layout, "previous,clicked", NULL);
          break;
       default:
          return;
      }
    elm_segment_control_item_selected_set(it, EINA_FALSE);

}

static void
_view_mode_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
    Evas_Object *o_layout = data;
    Evas_Object *o_seg = evas_object_data_get(o_layout, "mode");
    Elm_Segment_Item *it = elm_segment_control_item_selected_get(o_seg);

    switch(elm_segment_control_item_index_get(it))
      {
       case 0:
          evas_object_smart_callback_call(o_layout, "mode,changed", "list");
          break;
       case 1:
          evas_object_smart_callback_call(o_layout, "mode,changed", "grid");
          break;
       default:
          return;
      }
}

Evas_Object *
enna_browser_bar_add(Evas_Object *parent, Enna_File *file)
{
   Evas_Object *o_layout;
   Evas_Object *o_ic;
   Evas_Object *o_seg;
   Evas_Object *o_box;
   Evas_Object *o_entry;

   o_layout = elm_layout_add(parent);
   elm_layout_file_set(o_layout, enna_config_theme_get(), "enna/browser/header");
   evas_object_show(o_layout);

   o_box = elm_box_add(o_layout);
   elm_box_horizontal_set(o_box, EINA_TRUE);
   evas_object_show(o_box);
   evas_object_size_hint_align_set(o_box, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(o_box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_box_padding_set(o_box, 8, 8);

   o_seg = elm_button_add(o_layout);
   o_ic = elm_icon_add(o_layout);
   evas_object_show(o_ic);
   elm_icon_file_set(o_ic, enna_config_theme_get(), "icon/arrow_left");
   elm_button_icon_set(o_seg, o_ic);
   elm_object_style_set(o_seg, "black");
   evas_object_show(o_seg);
   elm_box_pack_end(o_box, o_seg);
   evas_object_smart_callback_add(o_seg, "clicked", _up_clicked_cb, o_layout);

   o_seg = elm_segment_control_add(o_layout);
   o_ic = elm_icon_add(o_layout);
   evas_object_show(o_ic);
   elm_icon_file_set(o_ic, enna_config_theme_get(), "icon/arrow_left");
   elm_segment_control_item_add(o_seg, o_ic, NULL);
   o_ic = elm_icon_add(o_layout);
   evas_object_show(o_ic);
   elm_icon_file_set(o_ic, enna_config_theme_get(), "icon/arrow_right");
   elm_segment_control_item_add(o_seg, o_ic, NULL);
   evas_object_show(o_seg);
   evas_object_smart_callback_add(o_seg, "changed", _browse_icons_changed_cb, o_layout);
   evas_object_data_set(o_layout, "browse", o_seg);
   elm_box_pack_end(o_box, o_seg);

   o_entry = elm_entry_add(o_layout);
   elm_entry_single_line_set(o_entry, EINA_TRUE);
   if (file) elm_object_text_set(o_entry, file->mrl);
   evas_object_size_hint_weight_set(o_entry, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(o_entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(o_entry);
   elm_box_pack_end(o_box, o_entry);
   evas_object_data_set(o_layout, "entry", o_entry);

   o_seg = elm_segment_control_add(o_layout);
   o_ic = elm_icon_add(o_layout);
   evas_object_show(o_ic);
   elm_icon_file_set(o_ic, enna_config_theme_get(), "icon/view_list");
   elm_segment_control_item_add(o_seg, o_ic, NULL);
   o_ic = elm_icon_add(o_layout);
   evas_object_show(o_ic);
   elm_icon_file_set(o_ic, enna_config_theme_get(), "icon/view_grid");
   elm_segment_control_item_add(o_seg, o_ic, NULL);
   evas_object_show(o_seg);
   elm_box_pack_end(o_box, o_seg);
   evas_object_smart_callback_add(o_seg, "changed", _view_mode_changed_cb, o_layout);
   evas_object_data_set(o_layout, "mode", o_seg);

   o_seg = elm_button_add(o_layout);
   o_ic = elm_icon_add(o_layout);
   evas_object_show(o_ic);
   elm_icon_file_set(o_ic, enna_config_theme_get(), "icon/search");
   elm_button_icon_set(o_seg, o_ic);
   elm_object_style_set(o_seg, "black");
   evas_object_show(o_seg);
   elm_box_pack_end(o_box, o_seg);


   o_seg = elm_button_add(o_layout);
   o_ic = elm_icon_add(o_layout);
   evas_object_show(o_ic);
   elm_icon_file_set(o_ic, enna_config_theme_get(), "icon/settings");
   elm_button_icon_set(o_seg, o_ic);
   elm_object_style_set(o_seg, "black");
   evas_object_show(o_seg);
   elm_box_pack_end(o_box, o_seg);


   elm_layout_content_set(o_layout, "enna.swallow.content", o_box);

   return o_layout;
}

void
enna_browser_bar_file_set(Evas_Object *obj, Enna_File *file)
{
   Evas_Object *o_entry = evas_object_data_get(obj, "entry");

   elm_object_text_set(o_entry, file->mrl);
}
