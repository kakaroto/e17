#include <Elementary.h>

#include "../mess_header.h"

static void
_discard(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   evas_object_del(data);
}

static void
_save(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   evas_object_del(data);
}

static void
_cancel(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   evas_object_del(data);
}

void
ui_alert_need_saving(void (*done)(void *data), void *data)
{
   (void) done;
   Evas_Object *inwin, *bx, *hbx, *btn, *lbl;
   inwin = elm_win_inwin_add(elm_object_top_widget_get(data));
   evas_object_show(inwin);

   bx = elm_box_add(inwin);
   elm_win_inwin_content_set(inwin, bx);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, 0.0);
   evas_object_show(bx);

   lbl = elm_label_add(inwin);
   elm_object_text_set(lbl,
         _("<align=center>Would you like to save changes to document?<br>"
         "Any unsaved changes will be lost."));
   evas_object_size_hint_weight_set(lbl, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(lbl, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx, lbl);
   evas_object_show(lbl);

   hbx = elm_box_add(inwin);
   elm_box_horizontal_set(hbx, EINA_TRUE);
   evas_object_size_hint_weight_set(hbx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(hbx, EVAS_HINT_FILL, 0.0);
   elm_box_pack_end(bx, hbx);
   evas_object_show(hbx);

   btn = elm_button_add(inwin);
   elm_object_text_set(btn, _("Save"));
   elm_box_pack_end(hbx, btn);
   evas_object_show(btn);
   evas_object_smart_callback_add(btn, "clicked", _save, inwin);

   btn = elm_button_add(inwin);
   elm_object_text_set(btn, _("Discard"));
   elm_box_pack_end(hbx, btn);
   evas_object_show(btn);
   evas_object_smart_callback_add(btn, "clicked", _discard, inwin);

   btn = elm_button_add(inwin);
   elm_object_text_set(btn, _("Cancel"));
   elm_box_pack_end(hbx, btn);
   evas_object_show(btn);
   evas_object_smart_callback_add(btn, "clicked", _cancel, inwin);
}
